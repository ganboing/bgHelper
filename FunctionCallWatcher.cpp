#include "FunctionCallWatcher.h"

GEN_WINAPI_EH_RESULT(NULL, AddVectoredExceptionHandler);

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
	if (FunctionWatches[DRi - DRs]){
		DbgRaiseAssertionFailure();
	}

	**DRi = target;
	FunctionWatches[DRi - DRs] = target;
	FunctionRedirs[DRi - DRs] = redir;
	SetHWBreakPoint(context.Dr7, DRi - DRs, 0, 0);

	if (!SetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
}
void UninstallHWExec(ULONG_PTR target){

	auto i = std::find(FunctionWatches, FunctionWatches + _countof(FunctionWatches),
		target);
	if (i == FunctionWatches + _countof(FunctionWatches)){
		DbgRaiseAssertionFailure();
	}
	*i = NULL;

	CONTEXT context{ CONTEXT_DEBUG_REGISTERS };
	if (!GetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
	decltype(context.Dr0)* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3 };
	if (*DRs[i - FunctionWatches] != target){
		DbgRaiseAssertionFailure();
	}
	*DRs[i - FunctionWatches] = NULL;
	UnsetHWBreakPoint(context.Dr7, i - FunctionWatches);

	if (!SetThreadContext(GetCurrentThread(), &context)){
		DbgRaiseAssertionFailure();
	}
}

static LONG CALLBACK handler(_In_  PEXCEPTION_POINTERS ExceptionInfo){
	if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP){
		return EXCEPTION_CONTINUE_SEARCH;
	}
	auto i = std::find(FunctionWatches, FunctionWatches + _countof(FunctionWatches),
		(ULONG_PTR)ExceptionInfo->ExceptionRecord->ExceptionAddress);
	if (i == FunctionWatches + _countof(FunctionWatches)){
		return EXCEPTION_CONTINUE_SEARCH;
	}
	//check DR6
	if (!CheckHWBreakPointStatus(ExceptionInfo->ContextRecord->Dr6, i - FunctionWatches)){
		DbgRaiseAssertionFailure();
	}
	ClearHWBreakPointStatus(ExceptionInfo->ContextRecord->Dr6, i - FunctionWatches);
	//check DRx
	auto& context = *ExceptionInfo->ContextRecord;
	decltype(context.Dr0)* DRs[] = { &context.Dr0, &context.Dr1, &context.Dr2, &context.Dr3 };
	if (*DRs[i - FunctionWatches] == (ULONG_PTR)ExceptionInfo->ExceptionRecord->ExceptionAddress){
		DbgRaiseAssertionFailure();
	}

	ExceptionInfo->ContextRecord->Eip = FunctionRedirs[i - FunctionWatches];
	return EXCEPTION_CONTINUE_EXECUTION;
}

static std::unique_ptr<VOID, WINAPI_CALLABLE(RemoveVectoredExceptionHandler)>
_h(EH_AddVectoredExceptionHandler(TRUE, &handler));
