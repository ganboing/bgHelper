#pragma once
#include <cstdint>
#include <Windows.h>
#include <newstd.h>

namespace DBG_PRINT_REMOTE_NAMESPACE{

	static const size_t MAX_DBG_STR_LEN = 512;
	static const DWORD RAISE_CODE = EncodeInt{'P', 'N', 'T', 0xe0}.i;

#pragma pack(push, 1)

	enum class PacketType{
		STRING,
		CHAR,
		SHORT,
		INT,
		LONG,
		LONGLONG,
		DOUBLE,
	};

	union BasicTypeU{
		unsigned char c;
		unsigned short s;
		unsigned int i;
		unsigned long l;
		unsigned long long ll;
		double d;
	};

	typedef char MaxPacketChar[sizeof(uintptr_t) * EXCEPTION_MAXIMUM_PARAMETERS];
	typedef ULONG_PTR MaxPacketPtr[EXCEPTION_MAXIMUM_PARAMETERS];
	typedef char MaxLoad[sizeof(MaxPacketChar) - sizeof(PacketType)];
	typedef MaxLoad StrLoad;
	typedef char BasicFormat[sizeof(MaxLoad) - sizeof(BasicTypeU)];

	struct BasicLoad{
		BasicTypeU integer;
		BasicFormat format;
	};

	union PacketLoad{
		BasicLoad basic;
		StrLoad str;
	};

	union Packet{
		struct{
			PacketType type;
			PacketLoad load;
		};
		MaxPacketPtr buff_p;
		MaxPacketChar buff_b;
	};
	static_assert(sizeof(MaxPacketPtr) == sizeof(Packet), "check Packet size");

#pragma pack(pop)
}