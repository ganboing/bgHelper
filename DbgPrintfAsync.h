#pragma once
#include <atomic>
#include <thread>
#include "TaskQueue.hpp"
#include "ConcurrentStor.h"

#pragma warning(push)
#pragma warning(disable: 4510)
#pragma warning(disable: 4512)
#pragma warning(disable: 4610)

struct FMT{
	const char* const format;
	inline FMT(const char* _f) : format(_f){}
};

namespace DBG_PRINTF_ASYNC{

	static const size_t BUFF_LINES = 512;
	static const size_t MAX_DBG_STR_LEN = 512;
	static const size_t OBJ_SIZE = sizeof(uintptr_t)*2 + sizeof(double);

	class DBG_ASYNC;
	struct DBG_ASYNC_FMT{
		DBG_ASYNC& printer;
		const FMT fmt;
		template<class T>
		DBG_ASYNC& operator<<(T);
	};

	class DBG_ASYNC{
		typedef char ObjBuff[OBJ_SIZE];
		typedef char VStr[MAX_DBG_STR_LEN];
		ConcurrentStor<VStr> Vstrs;
		TaskQueue<ObjBuff, BUFF_LINES> BufferQueue;
		std::atomic<bool> IsShutdown;
		std::thread DbgPrintThread;
	public:
		typedef decltype(Vstrs) VStrsTy;
		typedef decltype(BufferQueue) BufferQueueTy;
		DBG_ASYNC();
		~DBG_ASYNC();
		inline DBG_ASYNC_FMT operator<< (FMT fmt) { return DBG_ASYNC_FMT{ *this, fmt }; }
		template<class T>
		inline DBG_ASYNC& operator << (T arg){
			print(arg);
			return *this;
		}
		void print(char c, const char* = "%c");
		void print(bool b);
		void print(unsigned short su, const char* = "%hu");
		inline void print(short s, const char* fmt = "%hd"){ print((unsigned short)s, fmt); }
		void print(unsigned int iu, const char* = "%u");
		inline void print(int i, const char* fmt = "%d"){ print((unsigned int)i, fmt); }
		void print(unsigned long lu, const char* = "%lu");
		inline void print(long l, const char* fmt = "%ld"){ print((unsigned long)l, fmt); }
		void print(unsigned long long llu, const char* = "%llu");
		inline void print(long long ll, const char* fmt = "%lld"){ print((unsigned long long)ll, fmt); }
		inline void print(const volatile void* p, const char* fmt = "%p"){ print((uintptr_t)p, fmt); }
		void print(double d, const char* = "%f");
		inline void print(const char* str, const char* fmt = "%s"){ print((uintptr_t)str, fmt); }
		void print(const volatile char* str, const char* = "%s");
	};

	template<class T>
	DBG_ASYNC& DBG_ASYNC_FMT::operator<< (T arg){
		printer.print(arg, fmt.format);
		return printer;
	}
}

typedef DBG_PRINTF_ASYNC::DBG_ASYNC DBG_ASYNC;
#pragma warning(pop)

extern DBG_ASYNC DBGa;
