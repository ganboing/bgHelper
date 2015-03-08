#include <atomic>
#include <Windows.h>

struct SuspenderSharedArea{
	void* operator new  (size_t count);
	void* operator new  (size_t count, uintptr_t mapping_handle);
	void operator delete  (void* ptr);
	void operator delete  (void* ptr, uintptr_t mapping_handle);
	std::atomic<DWORD> tid;
	std::atomic<bool> suspend;
};