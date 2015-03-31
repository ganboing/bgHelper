#pragma once
#include <Windows.h>
#include <SubAuth.h>

#pragma comment(lib, "NonExportApis\\ntdll.lib")

#define LDR_DLL_NOTIFICATION_REASON_LOADED (1U)
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED (2U)

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
    ULONG Flags;                    //Reserved.
    PUNICODE_STRING FullDllName;   //The full path name of the DLL module.
    PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
    PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
    ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
	LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
	LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef VOID CALLBACK LDR_DLL_NOTIFICATION_FUNCTION(
	_In_      ULONG NotificationReason,
	_In_      PLDR_DLL_NOTIFICATION_DATA NotificationData,
	_In_opt_  PVOID Context
	);

typedef LDR_DLL_NOTIFICATION_FUNCTION *PLDR_DLL_NOTIFICATION_FUNCTION;

extern "C"{

	NTSTATUS NTAPI LdrRegisterDllNotification(
		_In_      ULONG Flags,
		_In_      PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
		_In_opt_  PVOID Context,
		_Out_     PVOID *Cookie
		);
	NTSTATUS NTAPI LdrUnregisterDllNotification(
		_In_  PVOID Cookie
		);
}
