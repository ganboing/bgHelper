#include <Windows.h>
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "ProcessSuspenderChild.h"

extern "C"{
	NTSTATUS NTAPI NtResumeProcess(HANDLE ProcessHandle);
	NTSTATUS NTAPI NtSuspendProcess(HANDLE ProcessHandle);
}

GEN_WINAPI_EH_RESULT(NULL, OpenProcess);
GEN_WINAPI_EH_RESULT(NULL, OpenThread);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_STATUS(0, NtSuspendProcess);
GEN_WINAPI_EH_STATUS(0, NtResumeProcess);
GEN_WINAPI_EH_RESULT((DWORD)-1, ResumeThread);
GEN_WINAPI_EH_RESULT((DWORD)-1, SuspendThread);

namespace ProcSuspender{

	ProcSuspenderChild::ProcSuspenderChild(DWORD _pid, HANDLE _mapping):
		pid(_pid),
		proc(EH_OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid)),
		mapping(_mapping),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SuspenderSharedArea))),
		area(*(SuspenderSharedArea*)view.get())
	{}

	ProcSuspenderChild::~ProcSuspenderChild()
	{}

	void ProcSuspenderChild::Run(){
		printf("ProcSuspender for %d started\n", pid);
		for (;;)
		{
			DWORD tid = area.tid.load();
			if (tid == DWORD(0) - 1){
				break;
			}
			if (tid){
				ManagedHANDLE thread(OpenThread(THREAD_SUSPEND_RESUME, FALSE, tid));
				printf("suspending pid=%d, by tid=%d\n", pid, tid);
				EH_NtSuspendProcess(proc.get());
				area.suspend.store(true);
				EH_ResumeThread(thread.get());
				while (area.suspend.load()){
					Sleep(1);
				}
				EH_SuspendThread(thread.get());
				EH_NtResumeProcess(proc.get());
				area.tid.store(0);
				printf("resuming pid=%d, by tid=%d\n", pid, tid);
			}
			else{
				Sleep(1);
			}
		}
		printf("ProcSuspender for %d stopped\n", pid);
	}
}