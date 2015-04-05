#pragma once
#include <Windows.h>
#include <cstdint>
#include <winternl.h>
#include "DbgPrintCommon.h"
#include "WinResMgr.h"

#pragma warning(push)
#pragma warning(disable: 4510)
#pragma warning(disable: 4512)
#pragma warning(disable: 4610)

namespace DbgPnt{

	struct FMT{
		const char* const format;
		inline FMT(const char* _f) : format(_f){}
	};

	class DbgPrintClient;
	struct DbgPrintFMT{
		DbgPrintClient& printer;
		const FMT fmt;
		template<class T>
		DbgPrintClient& operator<<(T&&);
	};

	class DbgPrintClient{
	public:
		ManagedHANDLE const mapping;
	private:
		ManagedAreaView const view;
		DbgPntSharedArea& area;
		PacketQueueTy::EleSmartPtr GetPacketSlot(const volatile char*);
	public:
		DbgPrintClient();
		~DbgPrintClient();
		inline DbgPrintFMT operator<< (FMT fmt) { return DbgPrintFMT{ *this, fmt }; }
		template<class T>
		inline DbgPrintClient& operator << (T&& arg){
			print(std::forward<T>(arg));
			return *this;
		}
		void print(unsigned char cu, const char* = "%c");
		inline void print(signed char cs, const char* fmt = "%c"){ print((unsigned char)cs, fmt); }
		inline void print(char c, const char* fmt = "%c"){ print((unsigned char)c, fmt); }
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
		void print(const volatile char* str, const char* = "%s");
		void print(const volatile UNICODE_STRING& str);
	};

	template<class T>
	DbgPrintClient& DbgPrintFMT::operator<< (T&& arg){
		printer.print(std::forward<T>(arg), fmt.format);
		return printer;
	}
}

typedef DbgPnt::DbgPrintClient DbgPrintClient;
typedef DbgPnt::FMT FMT;
#pragma warning(pop)

extern DbgPrintClient& DBGr();
