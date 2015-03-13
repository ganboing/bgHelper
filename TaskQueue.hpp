#pragma once
#include <atomic>
#include <algorithm>
#include <memory>
#include <intrin.h>

template<class T, size_t S>
class TaskQueue{
private:
	T data[S];
	::std::atomic<bool> flags[S];
	::std::atomic_size_t p_reserve;
	::std::atomic_size_t p_free;
	/*
		[--free--|-----01010101010101----]
		         | <-commit + reserved-> |
				p_f                     p_r
	*/
public:
#pragma warning(push)
#pragma warning(disable:4510)
#pragma warning(disable:4512)
#pragma warning(disable:4610)
	struct Committer{
		TaskQueue* const parent;
		void operator() (T* p){
			if (p){
				parent->Enqueue(p);
			}
		}
	};
#pragma warning(pop)
	typedef std::unique_ptr<T, Committer> EleSmartPtr;
	EleSmartPtr Reserve()
	{
		size_t p_r_old = p_reserve.load();
		for(;;) {
			size_t p_r_new;
			p_r_new = (p_r_old + 1) % S;
			if (p_r_new == p_free.load()){
				_mm_pause();
				continue;
			}
			if (p_reserve.compare_exchange_strong(p_r_old, p_r_new)){
				return EleSmartPtr(data + p_r_old, Committer{ this });
			}
		}
	}
	T* Pick(){
		size_t p_f = p_free.load(); 
		if (p_f == p_reserve.load()){
			return nullptr;
		}
		if (!flags[p_f].load()){
			return nullptr;
		}
		return data + p_f;
	}
private:
	void Enqueue(T* newdata){
		flags[newdata - data].store(true);
	}
public:
	void Dequeue(){
		size_t p_f = p_free.load();
		flags[p_f].store(false);
		p_free.store((p_f + 1) % S);
	}
};