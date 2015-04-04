#include <Windows.h>

DWORD WINAPI GetFileNameFromHandle(HANDLE hFile, LPTSTR lpName, DWORD nSize);
PVOID WINAPI GetModuleEntryPointByName(LPCTSTR lpName);
PVOID WINAPI GetModuleEntryPoint(PVOID hModule);
DWORD WINAPI GetParentProcessId();
