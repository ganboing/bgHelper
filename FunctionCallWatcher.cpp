#include "FunctionCallWatcher.h"

static ULONG_PTR FunctionWatches[4] = {};
static ULONG_PTR FunctionRedirs[4] = {};

void InstallHWExec(ULONG_PTR target, ULONG_PTR redir){
	CONTEXT context{ CONTEXT_DEBUG_REGISTERS };
	if (!GetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
	decltype(context.Dr0)* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3 };
	auto DRe = DRs + _countof(DRs), DRi = std::find_if(DRs, DRe, [](decltype(context.Dr0) *v){return *v == NULL; });
	if (DRi == DRe){
		DbgRaiseAssertionFailure();
	}
	auto ith = DRi - DRs;
	if (FunctionWatches[ith]){
		DbgRaiseAssertionFailure();
	}

	*DRs[ith] = target;
	FunctionWatches[ith] = target;
	FunctionRedirs[ith] = redir;
	SetHWBreakPointCtrl(context.Dr7, ith, {true, HWBP_EXEC, HWBP_LEN1});

	if (!SetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
}
void UninstallHWExec(ULONG_PTR target){

	auto FWe = FunctionWatches + _countof(FunctionWatches), FWi = std::find(FunctionWatches, FWe, target);
	if (FWi == FWe){
		DbgRaiseAssertionFailure();
	}
	auto ith = FWi - FunctionWatches;

	FunctionWatches[ith] = NULL;
	CONTEXT context{ CONTEXT_DEBUG_REGISTERS };
	if (!GetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
	decltype(context.Dr0)* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3 };
	if (*DRs[ith] != target){
		DbgRaiseAssertionFailure();
	}
	*DRs[ith] = NULL;
	SetHWBreakPointCtrl(context.Dr7, ith, { false });

	if (!SetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
}

LONG CALLBACK HWBreakPointHandler(_In_  PEXCEPTION_POINTERS ExceptionInfo){
	if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP){
		return EXCEPTION_CONTINUE_SEARCH;
	}
	auto addr = (ULONG_PTR)ExceptionInfo->ExceptionRecord->ExceptionAddress;
	auto& context = *ExceptionInfo->ContextRecord;
	if (addr != context.Eip){
		//check if eip == breakpoint address
		DbgRaiseAssertionFailure();
	}
	auto FWe = FunctionWatches + _countof(FunctionWatches), FWi = std::find(FunctionWatches, FWe, addr);
	if (FWi == FWe){
		//check if breakpoint is the watched function
		return EXCEPTION_CONTINUE_SEARCH;
	}
	auto ith = FWi - FunctionWatches;

	decltype(context.Dr0) DRs[] = { context.Dr0, context.Dr1, context.Dr2, context.Dr3 };
	if (DRs[ith] != *FWi){
		//check if the drx is valid
		DbgRaiseAssertionFailure();
	}
	auto ctrl = GetHWBreakPointCtrl(context.Dr7, ith);
	if (!ctrl.ENABLE || ctrl.RW != HWBP_EXEC || ctrl.LEN != HWBP_LEN1){
		//check if dr7 is valid
		DbgRaiseAssertionFailure();
	}
	SetHWBreakPointStatus(context.Dr6, ith, false);
	
	context.Eip = FunctionRedirs[ith];
	return EXCEPTION_CONTINUE_EXECUTION;
}
