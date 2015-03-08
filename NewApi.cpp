#include <Windows.h>
#include <winternl.h>
#include <Psapi.h>
#include <string>
#include "ApiWrapper.h"
#include "WinResMgr.h"

#define BUFSIZE 512

std::string GetFileNameFromHandle(HANDLE hFile)
{
	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0){
		throw DWORD(-1);
	}

	// Create a file mapping object.
	ManagedHANDLE hFileMap(EH_CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 1, NULL));

	// Create a file mapping to get the file name.
	ManagedView pMem(EH_MapViewOfFile(hFileMap.get(), FILE_MAP_READ, 0, 0, 1));

	char pszFilename[MAX_PATH];
	EH_GetMappedFileNameA(GetCurrentProcess(),pMem.get(),pszFilename,MAX_PATH);
	return std::string(pszFilename);
}

void* GetStackStor()
{
	_NT_TIB* pTib = (_NT_TIB*)NtCurrentTeb();
	return pTib->StackLimit;
}
