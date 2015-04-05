#include <Windows.h>
#include <memory>
#ifdef NDEBUG
#undef NDEBUG
#include "newstd.h"
#define NDEBUG
#else
#include "newstd.h"
#endif
#include "ProcessSuspenderParent.h"
#include "WinResMgr.h"
#include "ApiEHWrapper.h"

GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(NULL, CreateFileMappingW);
GEN_WINAPI_EH_RESULT(FALSE, CreateProcessW);

namespace ProcSuspender{

	void* SuspenderSharedArea::operator new  (size_t count, uintptr_t mapping_handle){
		return EH_MapViewOfFile((HANDLE)mapping_handle, FILE_MAP_ALL_ACCESS, 0, 0, count);
	}
	void SuspenderSharedArea::operator delete  (void* ptr){
		UnmapViewOfFile(ptr);
	}
	void SuspenderSharedArea::operator delete  (void* ptr, uintptr_t mapping_handle){
		UnmapViewOfFile(ptr);
	}
	ProcSuspenderParent::ProcSuspenderParent() :
		mapping(
			EH_CreateFileMappingW(
				INVALID_HANDLE_VALUE,
				NULL, 
				PAGE_READWRITE, 
				0, 
				sizeof(SuspenderSharedArea),
				my_snwprintf(
					(wchar_t*)alloca(ProcSusMappingLen * sizeof(wchar_t)),
					ProcSusMappingLen,
					ProcsusMappingFormat, ProcSusMapping, (ULONG_PTR)GetCurrentProcessId()
				)
			)
		),
		area(new ((uintptr_t)mapping.get()) SuspenderSharedArea){

		wchar_t cmdline[_countof(ProcSusCmdLine)];
		std::copy(ProcSusCmdLine, ProcSusCmdLine + _countof(ProcSusCmdLine), cmdline);
		STARTUPINFOW startup{ sizeof(startup) };
		PROCESS_INFORMATION info;
		EH_CreateProcessW(NULL, cmdline, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup, &info);
		ManagedHANDLE(info.hProcess);
		ManagedHANDLE(info.hThread);
	}
	ProcSuspenderParent::~ProcSuspenderParent()
	{}
	void ProcSuspenderParent::SuspendSelf(){
		DWORD tid = GetCurrentThreadId();
		DWORD tid_old;
		do{
			tid_old = 0;
		} while (!area->tid.compare_exchange_strong(tid_old, tid));
		while (!area->suspend.load());
	}

	void ProcSuspenderParent::ResumeSelf(){
		DWORD tid = GetCurrentThreadId();
		area->suspend.store(false);
		while (area->tid.load() == tid);
	}

}

ProcSuspenderParent& SusPender(){
	static ProcSuspenderParent suspender;
	return suspender;
}

