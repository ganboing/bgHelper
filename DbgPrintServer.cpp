#include <rpc.h>
#include <ApiEHWrapper.h>
#include <cstdio>
#include <memory>
#include "DbgPrintServer.h"

GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);

namespace DbgPnt{
	
	void DbgPrintServer::Print(const Packet& packet){
		char buff[MAX_DBG_STR_LEN];
		switch (packet.type){
		case PacketType::CHAR:
			_snprintf(buff, _countof(buff), packet.format, packet.data.c);
			break;
		case PacketType::INT:
			_snprintf(buff, _countof(buff), packet.format, packet.data.i);
			break;
		case PacketType::SHORT:
			_snprintf(buff, _countof(buff), packet.format, packet.data.s);
			break;
		case PacketType::LONG:
			_snprintf(buff, _countof(buff), packet.format, packet.data.l);
			break;
		case PacketType::LONGLONG:
			_snprintf(buff, _countof(buff), packet.format, packet.data.ll);
			break;
		case PacketType::DOUBLE:
			_snprintf(buff, _countof(buff), packet.format, packet.data.d);
			break;
		case PacketType::STRING:
			_snprintf(buff, _countof(buff), packet.format, area.Vstrs[packet.data.istr].str);
			area.Vstrs.Free(&area.Vstrs[packet.data.istr]);
			break;
		case PacketType::WSTRING:
			OutputDebugStringW(area.Vstrs[packet.data.istr].wstr);
			area.Vstrs.Free(&area.Vstrs[packet.data.istr]);
			return;
		}
		OutputDebugStringA(buff);
	}

	void DbgPrintServer::Run(){
		printf("DbgServer for %d started\n", pid);
		for (;;){
			auto nextpacket = area.PacketQueue.Pick();
			if (!nextpacket){
				if (area.IsShutdown.load()){
					break;
				}
				else{
					Sleep(1);
					continue;
				}
			}
			Print(*nextpacket);
			area.PacketQueue.Dequeue();
		}
		printf("DbgServer for %d stopped\n", pid);
	}

	DbgPrintServer::DbgPrintServer(DWORD _pid, HANDLE _mapping) :
		pid(_pid),
		mapping(_mapping),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DbgPntSharedArea))),
		area(*(DbgPntSharedArea*)view.get())
	{}

	DbgPrintServer::~DbgPrintServer()
	{}
}
