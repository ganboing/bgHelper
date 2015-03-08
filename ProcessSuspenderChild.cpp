#include <Windows.h>
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "ProcessSuspenderCommon.h"

extern "C"{
	NTSTATUS NTAPI NtResumeProcess(HANDLE ProcessHandle);
	NTSTATUS NTAPI NtSuspendProcess(HANDLE ProcessHandle);
}

GEN_WINAPI_EH_RESULT(NULL, OpenProcess);
GEN_WINAPI_EH_RESULT(NULL, OpenThread);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(0, DuplicateHandle);
GEN_WINAPI_EH_STATUS(0, NtSuspendProcess);
GEN_WINAPI_EH_STATUS(0, NtResumeProcess);
GEN_WINAPI_EH_RESULT((DWORD)-1, ResumeThread);
GEN_WINAPI_EH_RESULT((DWORD)-1, SuspendThread);


void ProcSuspenderLoop(DWORD proc, HANDLE target_mapping){
	HANDLE my_mapping;
	ManagedHANDLE hproc(EH_OpenProcess(PROCESS_DUP_HANDLE | PROCESS_SUSPEND_RESUME, FALSE, proc));
	EH_DuplicateHandle(hproc.get(), target_mapping, GetCurrentProcess(), &my_mapping, 0, FALSE, DUPLICATE_SAME_ACCESS);
	ManagedHANDLE hmapping(my_mapping);
	ManagedView view(EH_MapViewOfFile(my_mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SuspenderSharedArea)));
	auto& area = *(SuspenderSharedArea*)view.get();
	for (;;)
	{
		DWORD tid = area.tid.load();
		if (tid == DWORD(0) - 1){
			break;
		}
		if (tid){
			ManagedHANDLE thread(OpenThread(THREAD_SUSPEND_RESUME, FALSE, tid));
			EH_NtSuspendProcess(hproc.get());
			area.suspend.store(true);
			EH_ResumeThread(thread.get());
			while (area.suspend.load()){
				Sleep(1);
			}
			EH_SuspendThread(thread.get());
			EH_NtResumeProcess(hproc.get());
			area.tid.store(0);
		}
		else{
			Sleep(1);
		}
	}
}