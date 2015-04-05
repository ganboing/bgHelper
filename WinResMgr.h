#pragma once
#include "ApiWrapper.h"
#include "ApiEHWrapper.h"
#include <memory>

typedef WINAPI_CALLABLE(CloseHandle) _HANDLE_Deleter;
typedef WINAPI_CALLABLE(UnmapViewOfFile) _SECTION_Unmapper;
typedef WINAPI_CALLABLE(RemoveVectoredExceptionHandler) _VEHUninstaller;
typedef WINAPI_CALLABLE(LocalFree) _HLocalDeleter;

//typedef ::std::unique_ptr<FILE, _HANDLE_Deleter> ManagedFile;
typedef ::std::unique_ptr<std::remove_pointer<HANDLE>::type, _HANDLE_Deleter> ManagedHANDLE;
typedef ::std::unique_ptr<VOID, _SECTION_Unmapper> ManagedView;
typedef ::std::unique_ptr<VOID, _VEHUninstaller> ManagedVEH;
typedef ::std::unique_ptr<std::remove_pointer<HLOCAL>::type, _HLocalDeleter> ManagedHLocal;
