#include <Windows.h>
#include <string>

DWORD WINAPI GetFileNameFromHandle(HANDLE hFile, LPTSTR lpName, DWORD nSize);
PVOID WINAPI GetModuleEntryPointByName(LPCTSTR lpName);
PVOID WINAPI GetModuleEntryPoint(PVOID hModule);
void* GetStackStor();
