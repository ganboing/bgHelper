#ifdef NDEBUG
#undef NDEBUG
#include "newstd.h"
#define NDEBUG
#else
#include "newstd.h"
#endif
#include "NonExportApis/ntdll/ntdll.h"
#include "NewApi.h"
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "ProcessSuspenderChild.h"

GEN_WINAPI_EH_RESULT(NULL, OpenProcess);
GEN_WINAPI_EH_RESULT(NULL, OpenThread);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(NULL, OpenFileMappingW);
GEN_WINAPI_EH_STATUS(STATUS_SUCCESS, NtSuspendProcess);
GEN_WINAPI_EH_STATUS(STATUS_SUCCESS, NtResumeProcess);
GEN_WINAPI_EH_RESULT((DWORD)-1, ResumeThread);
GEN_WINAPI_EH_RESULT((DWORD)-1, SuspendThread);
GEN_WINAPI_EH_RESULT(WAIT_FAILED, WaitForSingleObject);

namespace ProcSuspender{

	ProcSuspenderChild::ProcSuspenderChild(DWORD _pid):
		pid(_pid),
		proc(EH_OpenProcess(PROCESS_SUSPEND_RESUME | SYNCHRONIZE, FALSE, pid)),
		mapping(
			EH_OpenFileMappingW(
				FILE_MAP_ALL_ACCESS, 
				FALSE, 
				my_snwprintf(
					(wchar_t*)alloca(ProcSusMappingLen * sizeof(wchar_t)),
					ProcSusMappingLen,
					ProcsusMappingFormat, ProcSusMapping, (ULONG_PTR)pid
				)
			)
		),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SuspenderSharedArea))),
		area(*(SuspenderSharedArea*)view.get())
	{}

	ProcSuspenderChild::~ProcSuspenderChild()
	{}

	void ProcSuspenderChild::Run(){
		DbgPintfA("ProcSuspender for %p started\n", (ULONG_PTR)pid);
		for (;;)
		{
			DWORD tid = area.tid.load();
			if (tid){
				ManagedHANDLE thread(EH_OpenThread(THREAD_SUSPEND_RESUME, FALSE, tid));
				DbgPintfA("suspending pid=%p, by tid=%p\n", (ULONG_PTR)pid, (ULONG_PTR)tid);
				EH_NtSuspendProcess(proc.get());
				area.suspend.store(true);
				EH_ResumeThread(thread.get());
				while (area.suspend.load()){
					Sleep(1);
				}
				EH_SuspendThread(thread.get());
				EH_NtResumeProcess(proc.get());
				area.tid.store(0);
				DbgPintfA("resuming pid=%p, by tid=%p\n", (ULONG_PTR)pid, (ULONG_PTR)tid);
			}
			else{
				auto status = EH_WaitForSingleObject(proc.get(), 1);
				if (status == WAIT_OBJECT_0){
					break;
				}
				if (status != WAIT_TIMEOUT){
					DbgRaiseAssertionFailure();
				}
			}
		}
		DbgPintfA("ProcSuspender for %p stopped\n", (ULONG_PTR)pid);
	}
}