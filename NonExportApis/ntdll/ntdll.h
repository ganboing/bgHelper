#pragma once

#include <ntstatus.h>
#include <Windows.h>
#include <winternl.h>
#include "../../UndocImplChecker.h"

#pragma comment(lib, "NonExportApis\\ntdll.lib")

#define LDR_DLL_NOTIFICATION_REASON_LOADED (1U)
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED (2U)

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
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
	NTSTATUS WINAPI NtQueryInformationProcess(
		_In_       HANDLE ProcessHandle,
		_In_       PROCESSINFOCLASS ProcessInformationClass,
		_Out_      PVOID ProcessInformation,
		_In_       ULONG ProcessInformationLength,
		_Out_opt_  PULONG ReturnLength
		);
	NTSTATUS NTAPI NtResumeProcess(
		_In_	HANDLE ProcessHandle
		);
	NTSTATUS NTAPI NtSuspendProcess(
		_In_	HANDLE ProcessHandle
		);
}

typedef struct _LDR_DATA_TABLE_ENTRY_IMPL {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	WORD LoadCount;
	WORD TlsIndex;
	union{
		LIST_ENTRY HashLinks;
		struct{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	_ACTIVATION_CONTEXT * EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
}LDR_DATA_TABLE_ENTRY_IMPL, *PLDR_DATA_TABLE_ENTRY_IMPL;

CHECK_UNDOC_STRUCT(LDR_DATA_TABLE_ENTRY, FullDllName);
CHECK_UNDOC_STRUCT(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
CHECK_UNDOC_STRUCT(LDR_DATA_TABLE_ENTRY, DllBase);
CHECK_UNDOC_STRUCT(LDR_DATA_TABLE_ENTRY, CheckSum);
CHECK_UNDOC_STRUCT(LDR_DATA_TABLE_ENTRY, TimeDateStamp);


typedef struct _PROCESS_BASIC_INFORMATION_IMPL {
	DWORD ExitStatus;
	PPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION_IMPL, *PPROCESS_BASIC_INFORMATION_IMPL;

CHECK_UNDOC_STRUCT(PROCESS_BASIC_INFORMATION, PebBaseAddress);
CHECK_UNDOC_STRUCT(PROCESS_BASIC_INFORMATION, UniqueProcessId);