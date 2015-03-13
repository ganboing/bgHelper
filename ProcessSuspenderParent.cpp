#include <Windows.h>
#include <memory>
#include "ProcessSuspenderParent.h"
#include "WinResMgr.h"
#include "ApiEHWrapper.h"

GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);

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
		mapping(EH_CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SuspenderSharedArea), NULL)),
		area(new ((uintptr_t)mapping.get()) SuspenderSharedArea)
	{}
	ProcSuspenderParent::~ProcSuspenderParent(){
		area->tid.store(DWORD(0) - 1);
	}
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
