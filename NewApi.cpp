#include "NonExportApis/ntdll/ntdll.h"
#include "ApiEHWrapper.h"
#include "ApiWrapper.h"
#include "WinResMgr.h"

GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(0, GetMappedFileName);

DWORD WINAPI GetFileNameFromHandle(HANDLE hFile, LPTSTR lpName, DWORD nSize)
{
	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0){
		throw DWORD(-1);
	}

	// Create a file mapping object.
	ManagedHANDLE hFileMap(CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL));
	if (!hFileMap.get()){
		return 0;
	}

	// Create a file mapping to get the file name.
	ManagedView pMem(MapViewOfFile(hFileMap.get(), FILE_MAP_READ, 0, 0, 1));
	if (!pMem.get()){
		return 0;
	}

	return GetMappedFileName(GetCurrentProcess(), pMem.get(), lpName, nSize);
}

PVOID WINAPI GetModuleEntryPoint(PVOID hModule){
	auto imageBase = (PBYTE)hModule;

	auto pDosHeader = (PIMAGE_DOS_HEADER)imageBase;
	auto pNtHeaders = (PIMAGE_NT_HEADERS)(imageBase + pDosHeader->e_lfanew);
	return imageBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint;
}

PVOID WINAPI GetModuleEntryPointByName(LPCTSTR lpName){
	auto module = GetModuleHandle(lpName);
	if (!module){
		return NULL;
	}

	return GetModuleEntryPoint(module);
}

GEN_WINAPI_EH_STATUS(STATUS_SUCCESS, NtQueryInformationProcess);

DWORD WINAPI GetParentProcessId(){
	PROCESS_BASIC_INFORMATION_IMPL pbi;
	ULONG ret;
	if (NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &pbi, sizeof(pbi), &ret)
		!= STATUS_SUCCESS){
		DbgRaiseAssertionFailure();
	}
	if (ret != sizeof(pbi)){
		DbgRaiseAssertionFailure();
	}
	return (DWORD)pbi.InheritedFromUniqueProcessId;
}

LPTSTR WINAPI NtStatusToString(NTSTATUS Err){
	LPTSTR str;
	auto ret =
		FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		Err,
		LANG_SYSTEM_DEFAULT,
		(LPTSTR)&str,
		0,
		NULL);
	if (ret){
		return str;
	}
	else{
		return NULL;
	}
}

VOID WINAPI DbgPintfA(LPCSTR format, ...){
	va_list va;
	va_start(va, format);
	auto len = _vsnprintf(NULL, 0, format, va);
	auto buffer = (char*)alloca((len + 1)*sizeof(char));
	auto ret = _vsnprintf(buffer, len + 1, format, va);
	if (ret != len){
		DbgRaiseAssertionFailure();
	}
	OutputDebugStringA(buffer);
	va_end(va);
}

VOID WINAPI DbgPintfW(LPCWSTR format, ...){
	va_list va;
	va_start(va, format);
	auto len = _vsnwprintf(NULL, 0, format, va);
	auto buffer = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
	auto ret = _vsnwprintf(buffer, len + 1, format, va);
	if (ret != len){
		DbgRaiseAssertionFailure();
	}
	OutputDebugStringW(buffer);
	va_end(va);
}