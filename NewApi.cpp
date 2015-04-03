#include <Windows.h>
#include <Psapi.h>
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

void* GetStackStor()
{
	_NT_TIB* pTib = (_NT_TIB*)NtCurrentTeb();
	return pTib->StackLimit;
}
