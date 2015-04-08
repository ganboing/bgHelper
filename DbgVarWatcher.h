#pragma once
#include <Windows.h>
#include <ntstatus.h>
#include "ProcessSuspenderParent.h"
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "ArchDbg.h"
#include "NewApi.h"

typedef void DbgVarWatchCallback(uintptr_t offset, bool IsWrite, bool HasComplete);

template<typename Type, void* Impl, DbgVarWatchCallback Callback>
class DbgVarWatcher{
	static DWORD threadId;
	static ULONG_PTR dataAddr;
	static bool accessType;
	static const ManagedVEH ehHandle;
public:
	static char Impl[sizeof(Type)];
	static LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo){

		if (threadId){
			if (GetCurrentThreadId() != threadId){
				DbgRaiseAssertionFailure();
			}

			if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP){
				DbgRaiseAssertionFailure();
			}
			Eflags_t eflags{ ExceptionInfo->ContextRecord->EFlags };
			eflags.TF = 0;
			ExceptionInfo->ContextRecord->EFlags = eflags.EFLAGS;

			Callback(dataAddr - (uintptr_t)Redir, accessType, true);
			memcpy(Impl, Redir, sizeof(Type));

			DWORD oldProt;
			if (!VirtualProtect(Redir, sizeof(Type), PAGE_NOACCESS, &oldProt)){
				DbgRaiseAssertionFailure();
			}
			SusPender().ResumeSelf();

			threadId = 0;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		else{
			if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION){
				return EXCEPTION_CONTINUE_SEARCH;
			}
			if (ExceptionInfo->ExceptionRecord->NumberParameters != 2){
				DbgRaiseAssertionFailure();
			}
			auto type = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
			auto addr = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
			if (addr < (uintptr_t)Redir || addr >= (uintptr_t)Redir + sizeof(Type)){
				return EXCEPTION_CONTINUE_SEARCH;
			}
			if (type != !!type){
				return EXCEPTION_CONTINUE_SEARCH;
			}
			accessType = !!type;
			dataAddr = addr;
			threadId = GetCurrentThreadId();

			SusPender().SuspendSelf();
			DWORD oldProt;
			if (!VirtualProtect(Redir, sizeof(Type), PAGE_READWRITE, &oldProt)){
				DbgRaiseAssertionFailure();
			}

			Callback(dataAddr - (uintptr_t)Redir, accessType, false);
			memcpy(Redir, Imple, sizeof(Type));

			Eflags_t eflags{ ExceptionInfo->ContextRecord->EFlags };
			eflags.TF = 1;
			ExceptionInfo->ContextRecord->EFlags = eflags.EFLAGS;

		}
	}
};