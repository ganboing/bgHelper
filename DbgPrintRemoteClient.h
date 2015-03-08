#pragma once
#include <cstdint>

#pragma warning(push)
#pragma warning(disable: 4510)
#pragma warning(disable: 4512)
#pragma warning(disable: 4610)

namespace DBG_PRINT_REMOTE_NAMESPACE{

	struct FMT{
		const char* const format;
		inline FMT(const char* _f) : format(_f){}
	};

	class DBG_PRINT_REMOTE;
	struct DBG_PRINT_REMOTE_FMT{
		DBG_PRINT_REMOTE& printer;
		const FMT fmt;
		template<class T>
		DBG_PRINT_REMOTE& operator<<(T);
	};

	class DBG_PRINT_REMOTE{
	public:
		inline DBG_PRINT_REMOTE_FMT operator<< (FMT fmt) { return DBG_PRINT_REMOTE_FMT{ *this, fmt }; }
		template<class T>
		inline DBG_PRINT_REMOTE& operator << (T arg){
			print(arg);
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
		void print(const volatile char* str);
	};

	template<class T>
	DBG_PRINT_REMOTE& DBG_PRINT_REMOTE_FMT::operator<< (T arg){
		printer.print(arg, fmt.format);
		return printer;
	}
}

typedef DBG_PRINT_REMOTE_NAMESPACE::DBG_PRINT_REMOTE DBG_PRINT_REMOTE;
typedef DBG_PRINT_REMOTE_NAMESPACE::FMT FMT;
#pragma warning(pop)

extern DBG_PRINT_REMOTE DBGr;
