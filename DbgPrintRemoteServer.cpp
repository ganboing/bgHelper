#include "DbgPrintRemoteCommon.h"
#include <cstdio>

using namespace DBG_PRINT_REMOTE_NAMESPACE;

void DbgPrintRemote(const EXCEPTION_RECORD& record){
	char buff[MAX_DBG_STR_LEN];
	if (record.ExceptionCode != RAISE_CODE){
		return;
	}
	Packet p;
	memcpy(p.buff_p, record.ExceptionInformation, sizeof(ULONG_PTR)*record.NumberParameters);
	switch (p.type){
	case PacketType::STRING:
		_snprintf(buff, MAX_DBG_STR_LEN, "%s", p.load.str);
		break;
	case PacketType::CHAR:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.c);
		break;
	case PacketType::SHORT:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.s);
		break;
	case PacketType::INT:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.i);
		break;
	case PacketType::LONG:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.l);
		break;
	case PacketType::LONGLONG:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.ll);
		break;
	case PacketType::DOUBLE:
		_snprintf(buff, MAX_DBG_STR_LEN, p.load.basic.format, p.load.basic.integer.d);
		break;
	}
	OutputDebugStringA(buff);
}
