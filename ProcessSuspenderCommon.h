#include <atomic>
#include <Windows.h>

namespace ProcSuspender{

	static const wchar_t ProcSusCmdLine[] = L"ProcessSuspender.exe";
	static const wchar_t ProcSusMapping[] = L"ProcSuspender_Area_for";
	static const size_t ProcSusMappingLen = _countof(ProcSusMapping) + 1 + sizeof(HANDLE) * 2;

	struct SuspenderSharedArea{
		void* operator new  (size_t count);
		void* operator new  (size_t count, uintptr_t mapping_handle);
		void operator delete  (void* ptr);
		void operator delete  (void* ptr, uintptr_t mapping_handle);
		std::atomic<DWORD> tid;
		std::atomic<bool> suspend;
	};
}