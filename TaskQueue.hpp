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
	::std::atomic_size_t p_commit;
	::std::atomic_size_t p_free;
	/*
		[--free--|--commit--|--reserved--]
		         |          |            |
				p_f        p_c          p_r
	*/
	void UpdateOnce() //CS without protection!!!
	{
		size_t p_c = p_commit.load();
		size_t p_r_old = p_reserve.load();
		if (p_c != p_r_old)
		{
			if (flags[p_c].load())
			{
				p_commit.store((p_c + 1) % S);
			}
		}
	}
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
		for (;;)
		{
			size_t p_r_old = p_reserve.load();
			size_t p_r_new;
			p_r_new = (p_r_old + 1) % S;
			if (p_r_new == p_free.load())
			{
				_mm_pause();
				continue;
			}
			while (!p_reserve.compare_exchange_strong(p_r_old, p_r_new));
			return EleSmartPtr(data + p_r_old, Committer{ this });
		}
	}
	T* Pick()
	{
		UpdateOnce();
		size_t p_f = p_free.load(); 
		if (p_f == p_commit.load())
		{
			return nullptr;
		}
		return data + p_f;
	}
private:
	void Enqueue(T* newdata)
	{
		flags[newdata - data].store(true);
	}
public:
	void Dequeue()
	{
		size_t p_f = p_free.load();
		flags[p_f].store(false);
		p_free.store((p_f + 1) % S);
	}
};