#include <Windows.h>
#include "ProcessSuspenderCommon.h"
#include "WinResMgr.h"
#include "ApiEHWrapper.h"
#include <memory>

GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);
GEN_WINAPI_EH_RESULT(0, CreateProcess);

void* SuspenderSharedArea::operator new  (size_t count, uintptr_t mapping_handle){
	return EH_MapViewOfFile((HANDLE)mapping_handle, FILE_MAP_ALL_ACCESS, 0, 0, count);
}
void SuspenderSharedArea::operator delete  (void* ptr){
	UnmapViewOfFile(ptr);
}
void SuspenderSharedArea::operator delete  (void* ptr, uintptr_t mapping_handle){
	UnmapViewOfFile(ptr);
}

HANDLE CreateSuspender(HANDLE mapping){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char cmdline[MAX_PATH];
	_snprintf(cmdline, MAX_PATH, "\"ProcessSuspender.exe\" %x %p", GetCurrentProcessId(), mapping);
	EH_CreateProcess(NULL, cmdline, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	ManagedHANDLE thread(pi.hThread);
	return pi.hProcess;
}

struct _ProcSuspenderIniter{
	ManagedHANDLE const mapping;
	std::unique_ptr<SuspenderSharedArea> const area;
	ManagedHANDLE const process;
	_ProcSuspenderIniter() : 
		mapping(EH_CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SuspenderSharedArea), NULL)),
		area(new ((uintptr_t)mapping.get()) SuspenderSharedArea),
		process(CreateSuspender(mapping.get()))
	{}
	~_ProcSuspenderIniter()
	{
		area->tid.store(DWORD(0) - 1);
	}
};

_ProcSuspenderIniter ProcessSuspender;

void SuspendSelf(){
	DWORD tid = GetCurrentThreadId(); 
	DWORD tid_old;
	do{
		tid_old = 0;
	} while (!ProcessSuspender.area->tid.compare_exchange_strong(tid_old, tid));
	while (!ProcessSuspender.area->suspend.load());
}

void ResumeSelf(){
	DWORD tid = GetCurrentThreadId();
	ProcessSuspender.area->suspend.store(false);
	while (ProcessSuspender.area->tid.load() == tid);
}