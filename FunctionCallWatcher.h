#pragma once

#include "ArchDbg.h"
#include "ApiEHWrapper.h"
#include "ApiWrapper.h"
#include <algorithm>

template <typename T>
struct FuncWatcher{
	template<T* New, T*& Orig>
	struct Redir{
		static LONG CALLBACK handler(_In_  PEXCEPTION_POINTERS ExceptionInfo){
			if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP ||
				ExceptionInfo->ExceptionRecord->ExceptionAddress != Orig){
				return EXCEPTION_CONTINUE_SEARCH;
			}
			ExceptionInfo->ContextRecord->Eip = (ULONG_PTR)New;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		static void Install(void* Target){
			Orig = (T*)Target;
			if (!AddVectoredExceptionHandler(TRUE, handler)){
				DbgRaiseAssertionFailure();
			}

			CONTEXT context{ CONTEXT_DEBUG_REGISTERS };
			if (!GetThreadContext(GetCurrentThread(), &context)){
				DbgRaiseAssertionFailure();
			}
			DWORD* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3};
			auto DRe = DRs + _countof(DRs), DRi = std::find_if(DRs, DRe, [](DWORD* v){return *v == NULL; });
			if (DRi == DRe){
				DbgRaiseAssertionFailure();
			}
			**DRi = (ULONG_PTR)Orig;
			SetHWBreakPoint(context.Dr7, DRi - DRs, 0, 0);
			if (!SetThreadContext(GetCurrentThread(), &context)){
				DbgRaiseAssertionFailure();
			}
		}

		static void Uninstall(){
			CONTEXT context{ CONTEXT_DEBUG_REGISTERS };
			if (!GetThreadContext(GetCurrentThread(), &context)){
				DbgRaiseAssertionFailure();
			}
			DWORD* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3 };
			auto DRe = DRs + _countof(DRs), DRi = std::find_if(DRs, DRe, [](DWORD* v){return *v == (ULONG_PTR)Orig; });
			if (DRi == DRe){
				DbgRaiseAssertionFailure();
			}
			**DRi = NULL;
			UnsetHWBreakPoint(context.Dr7, DRi - DRs);
			if (!SetThreadContext(GetCurrentThread(), &context)){
				DbgRaiseAssertionFailure();
			}

			if (!RemoveVectoredExceptionHandler(handler)){
				DbgRaiseAssertionFailure();
			}
		}
	};
};