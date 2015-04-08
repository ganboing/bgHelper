#pragma once

#include <Windows.h>
#include <WinResMgr.h>

void InstallHWExec(ULONG_PTR target, ULONG_PTR redir);
void UninstallHWExec(ULONG_PTR target);
LONG CALLBACK HWBreakPointHandler(PEXCEPTION_POINTERS);

GEN_WINAPI_EH_RESULT(NULL, AddVectoredExceptionHandler);

template < typename Fty, Fty*& Impl, Fty& Redir>
struct FuncRedir{

	static void Install(){
		InstallHWExec((ULONG_PTR)Impl, (ULONG_PTR)Redir);
	}

	static void Uninstall(){
		UninstallHWExec((ULONG_PTR)Impl);
	}

	static void Unset(){
		if (!Impl){
			DbgRaiseAssertionFailure();
		}
		Impl = NULL;
	}
	static void Set(PVOID impl){
		if (Impl){
			DbgRaiseAssertionFailure();
		}
		Impl = (Fty*)impl;
	}
};

typedef DWORD WINAPI EXEMAIN(void);
typedef BOOL WINAPI DLLMAIN(HINSTANCE, DWORD, LPVOID);