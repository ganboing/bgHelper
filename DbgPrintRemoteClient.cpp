#include <Windows.h>
#include "DbgPrintRemoteClient.h"
#include "DbgPrintRemoteCommon.h"
#include "newstd.h"

namespace DBG_PRINT_REMOTE_NAMESPACE{

	template<typename T>
	void SendBasicWithFormat(PacketType type, T data, volatile const char* fmt){
		Packet p;
		p.type = type;
		memcpy(&p.load.basic.integer, &data, sizeof(T));
		size_t i = my_strncpy(p.load.basic.format, fmt);
		RaiseException(
			RAISE_CODE,
			0,
			(DWORD)get_upper_cnt<ULONG_PTR>(p.load.basic.format + i - p.buff_b),
			p.buff_p
			);
	}

	void SendStr(const volatile char* str){
		if (!str){
			str = "<<<null>>>";
		}
		Packet p;
		p.type = PacketType::STRING;
		while (*str){
			size_t i = my_strncpy(p.load.str, str);
			RaiseException(
				RAISE_CODE,
				0,
				(DWORD)get_upper_cnt<ULONG_PTR>(p.load.str + i - p.buff_b),
				p.buff_p
				);
		}
	}

	void DBG_PRINT_REMOTE::print(bool b){
		if (b){
			SendStr("true");
		}
		else{
			SendStr("false");
		}
	}

	void DBG_PRINT_REMOTE::print(const volatile char* str){
		SendStr(str);
	}

	void DBG_PRINT_REMOTE::print(unsigned char cu, const char* fmt){
		SendBasicWithFormat(PacketType::CHAR, cu, fmt);
	}

	void DBG_PRINT_REMOTE::print(unsigned short su, const char* fmt){
		SendBasicWithFormat(PacketType::SHORT, su, fmt);
	}

	void DBG_PRINT_REMOTE::print(unsigned int iu, const char* fmt){
		SendBasicWithFormat(PacketType::INT, iu, fmt);
	}

	void DBG_PRINT_REMOTE::print(unsigned long lu, const char* fmt){
		SendBasicWithFormat(PacketType::LONG, lu, fmt);
	}

	void DBG_PRINT_REMOTE::print(unsigned long long llu, const char* fmt){
		SendBasicWithFormat(PacketType::LONGLONG, llu, fmt);
	}

	void DBG_PRINT_REMOTE::print(double d, const char* fmt){
		SendBasicWithFormat(PacketType::DOUBLE, d, fmt);
	}
}

DBG_PRINT_REMOTE DBGr;