#pragma once
#include <Windows.h>
#include <cstdint>
#include <newstd.h>
#include <TaskQueue.hpp>
#include <ConcurrentStor.h>

#ifdef _WIN64
#define DBG_PNT_SERVER_NAME "DBG_PNT_x64"
#else
#define DBG_PNT_SERVER_NAME "DBG_PNT_x32"
#endif

namespace DbgPnt{
	extern "C"{
		unsigned __int3264 DbgPntCreate(IN RPC_BINDING_HANDLE);
	}

	static const size_t BUFF_LINES = 512;
	static const size_t MAX_DBG_STR_LEN = 512;

	union PacketDataU{
		unsigned char c;
		unsigned short s;
		unsigned int i;
		unsigned long l;
		unsigned long long ll;
		double d;
		size_t istr;
	};

	enum class PacketType{
		STRING,
		CHAR,
		SHORT,
		INT,
		LONG,
		LONGLONG,
		DOUBLE,
	};

	struct Packet{
		PacketType type;
		PacketDataU data;
		char format[sizeof(uintptr_t) * 2];
	};
	typedef char VStr[MAX_DBG_STR_LEN];
	typedef ConcurrentStor<VStr> VStrsTy;
	typedef TaskQueue<Packet, BUFF_LINES> PacketQueueTy;

	struct DbgPntSharedArea{
		void* operator new  (size_t count);
		void* operator new  (size_t count, uintptr_t mapping_handle); 
		void operator delete  (void* ptr);
		void operator delete  (void* ptr, uintptr_t mapping_handle);
		VStrsTy Vstrs;
		PacketQueueTy PacketQueue;
		std::atomic<bool> IsShutdown;
	};
}
