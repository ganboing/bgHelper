#include <atomic>
#include <intrin.h>


#ifdef _WIN64
#define __bsf__ _BitScanForward64
#else
#define __bsf__ _BitScanForward
#endif

template<typename T, size_t S>
class ConcurrentStor{
	static const size_t unit = sizeof(uintptr_t) * 8;
	static const size_t mux = (S + unit - 1) / unit;
	static const size_t cap = mux * unit;
	T data[cap];
	std::atomic<uintptr_t> books[mux];
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
	ConcurrentStor(){
		for (size_t i = 0; i < mux; ++i){
			books[i].store(uintptr_t(0) - 1);
		}
	}
	T* Alloc(){
		for (size_t i = 0;; i = (i + 1) % mux)
		{
			uintptr_t book_old = books[i].load();
			uintptr_t book_new;
			unsigned long pos;
			if (!__bsf__(&pos, book_old)){
				_mm_pause();
				continue;
			}
			book_new = book_old & ~(uintptr_t(1) << pos);
			if (books[i].compare_exchange_strong(book_old, book_new)){
				return data + i*unit + pos;
			}
		}
	}
	void Free(T* ptr)
	{
		size_t pos = ptr - data;
		size_t ith = pos / unit;
		unsigned long offset = pos % unit;
		books[ith].fetch_or(uintptr_t(1) << offset);
	}
};