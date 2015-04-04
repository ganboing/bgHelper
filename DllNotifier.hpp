#pragma once
#include "NonExportApis/ntdll/ntdll.h"
#include "ApiEHWrapper.h"

GEN_WINAPI_EH_STATUS(STATUS_SUCCESS, LdrRegisterDllNotification);

template<LDR_DLL_NOTIFICATION_FUNCTION F>
class DllNotify{
	PVOID cookie;
public:
	DllNotify(PVOID Context){
		EH_LdrRegisterDllNotification(0, &F, Context, &cookie);
	}
	~DllNotify(){
		LdrUnregisterDllNotification(cookie);
	}
};