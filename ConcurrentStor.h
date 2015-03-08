#include <atomic>
#include <intrin.h>


#ifdef _WIN64
#define __bsf__ _BitScanForward64
#else
#define __bsf__ _BitScanForward
#endif

template<typename T>
class ConcurrentStor{
	static const size_t cap = sizeof(uintptr_t) * 8;
	T data[cap];
	std::atomic<uintptr_t> book;
public:
/*
#pragma warning(push)
#pragma warning(disable:4510)
#pragma warning(disable:4512)
#pragma warning(disable:4610)
	struct Deleter{
		ConcurrentStor* const parent;
		void operator() (T* p){
			if (p){
				parent->Free(p);
			}
		}
	};
#pragma warning(pop)
	typedef std::unique_ptr<T, Deleter> EleSmartPtr;
	*/
	T& operator[](size_t idx){
		return data[idx];
	}
	ConcurrentStor() : book(uintptr_t(0) - 1){}
	T* Alloc(){
		for (;;)
		{
			uintptr_t book_old = book.load();
			uintptr_t book_new;
			unsigned long pos;
			if (!__bsf__(&pos, book_old)){
				_mm_pause();
				continue;
			}
			book_new = book_old & ~(uintptr_t(1) << pos);
			while (!book.compare_exchange_strong(book_old, book_new));
			return data + pos;
		}
	}
	void Free(T* ptr)
	{
		unsigned long pos = ptr - data;
		book.fetch_or(uintptr_t(1) << pos);
	}
};