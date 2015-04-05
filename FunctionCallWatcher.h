#pragma once

#include <Windows.h>
#include <WinResMgr.h>

void InstallHWExec(ULONG_PTR target, ULONG_PTR redir);
void UninstallHWExec(ULONG_PTR target);
LONG CALLBACK HWBreakPointHandler(PEXCEPTION_POINTERS);

GEN_WINAPI_EH_RESULT(NULL, AddVectoredExceptionHandler);

template <typename T>
struct FuncWatcher;

template <typename Ret, typename ...Args>
struct FuncWatcher<Ret __stdcall(Args...)>{

	typedef Ret __stdcall Fty(Args...);

	template< Fty *& Original>
	struct Redir{
		static Ret __stdcall Impl(Args... args){
			return Original(args...);
		}
		static Ret __stdcall Hooker(Args...);
		static void Install(void* Target){
			Original = (Fty*)Target;
			InstallHWExec((ULONG_PTR)Original, (ULONG_PTR)Hooker);
		}

		static void Uninstall(){
			UninstallHWExec((ULONG_PTR)Original);
		}
	};
};

typedef BOOL WINAPI DLLMAIN_t(
	_In_  HINSTANCE hinstDLL,
	_In_  DWORD fdwReason,
	_In_  LPVOID lpvReserved
	);
typedef DWORD WINAPI EXEMAIN_t(void);