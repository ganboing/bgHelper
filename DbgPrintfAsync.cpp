#include <Windows.h>
#include <intrin.h>
#include <thread>
#include <cstdarg>
#include "DbgPrintfAsync.h"
#include "newstd.h"

#define ___STR(v) #v
#define TOSTR(v) ___STR(v)

#define MAX_DBG_STR 512

namespace DBG_PRINTF_ASYNC{

	struct DbgStrBase{
		const char* format;
		inline DbgStrBase(const char* _f) : format(_f){}
		inline virtual ~DbgStrBase() = 0{};
	};

	DBG_ASYNC::DBG_ASYNC():
		DbgPrintThread([&]()->void{
		for (;;){
			auto nextline = BufferQueue.Pick();
			if (!nextline){
				if (IsShutdown.load()){
					break;
				}
				else{
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					continue;
				}
			}
			auto lineobj = (DbgStrBase*)nextline;
			lineobj->~DbgStrBase();
			BufferQueue.Dequeue();
		}
	})
	{}

	DBG_ASYNC::~DBG_ASYNC()
	{
		IsShutdown.store(true);
		DbgPrintThread.join();
	}

	template<class T>
	struct DbgStrBasicType :DbgStrBase{
		T data;
		inline DbgStrBasicType(T _d, const char* _f) : DbgStrBase(_f), data(_d){
			static_assert(sizeof(*this) <= OBJ_SIZE, "check obj size");
		}
		inline virtual ~DbgStrBasicType(){
			char buff[MAX_DBG_STR];
			_snprintf(buff, MAX_DBG_STR, format, data);
			OutputDebugStringA(buff);
		}
	};

	template<>
	struct DbgStrBasicType<bool> :DbgStrBase{
		bool data;
		inline DbgStrBasicType(bool _d) :DbgStrBase(nullptr), data(_d){
			static_assert(sizeof(*this) <= OBJ_SIZE, "check obj size");
		}
		inline virtual ~DbgStrBasicType(){
			if (data){
				OutputDebugStringA("true");
			}
			else{
				OutputDebugStringA("false");
			}
		}
	};

	struct DbgLineVStr : DbgStrBase{
		DBG_ASYNC::VStrsTy::EleSmartPtr vstr;
		inline DbgLineVStr(DBG_ASYNC::VStrsTy::EleSmartPtr&& _p, const volatile char* str,
			const char* _f) :DbgStrBase(_f), vstr(move(_p)){
			static_assert(sizeof(*this) <= OBJ_SIZE, "check obj size");
			my_strncpy(*(vstr.get()), str);
		}
		inline virtual ~DbgLineVStr(){
			char buff[MAX_DBG_STR];
			_snprintf(buff, MAX_DBG_STR, format, vstr.get());
			OutputDebugStringA(buff);
		}
	};
	void DBG_ASYNC::print(bool b){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<bool>(b);
	}
	void DBG_ASYNC::print(char c, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<char>(c, format);
	}
	void DBG_ASYNC::print(unsigned short su, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<unsigned short>(su, format);
	}
	void DBG_ASYNC::print(unsigned int iu, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<unsigned int>(iu, format);
	}
	void DBG_ASYNC::print(unsigned long lu, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<unsigned long>(lu, format);
	}
	void DBG_ASYNC::print(unsigned long long llu, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<unsigned long long>(llu, format);
	}
	void DBG_ASYNC::print(double d, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgStrBasicType<double>(d, format);
	}
	void DBG_ASYNC::print(const volatile char* str, const char* format){
		auto pLine = BufferQueue.Reserve();
		new(pLine.get()) DbgLineVStr(Vstrs.Alloc(), str, format);
	}
};
