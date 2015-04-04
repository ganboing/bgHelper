#ifdef NDEBUG
#undef NDEBUG
#include "newstd.h"
#define NDEBUG
#else
#include "newstd.h"
#endif
#include <Windows.h>
#include <ntstatus.h>
#include <cstdio>
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "DbgPrintServer.h"

GEN_WINAPI_EH_RESULT(NULL, CreateFileMappingW);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile); 
GEN_WINAPI_EH_RESULT(NULL, OpenProcess);
GEN_WINAPI_EH_RESULT(WAIT_FAILED, WaitForSingleObject);
GEN_WINAPI_EH_RESULT(NULL, OpenFileMappingW);

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
				auto status = EH_WaitForSingleObject(proc.get(), 1);
				if (status == WAIT_OBJECT_0){
					break;
				}
				if (status != WAIT_TIMEOUT){
					DbgRaiseAssertionFailure();
				}
				continue;
			}
			Print(*nextpacket);
			area.PacketQueue.Dequeue();
		}
		printf("DbgServer for %d stopped\n", pid);
	}

	DbgPrintServer::DbgPrintServer(DWORD _pid) :
		pid(_pid),
		proc(EH_OpenProcess(SYNCHRONIZE, FALSE, pid)),
		mapping(
			EH_OpenFileMappingW(
				FILE_MAP_ALL_ACCESS,
				FALSE,
				my_snwprintf(
					(wchar_t*)alloca(PrinterMappingNameLen * sizeof(wchar_t)),
					PrinterMappingNameLen, 
					L"%s %p", PrinterMappingName, (ULONG_PTR)pid
				)
			)
		),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DbgPntSharedArea))),
		area(*(DbgPntSharedArea*)view.get())
	{}

	DbgPrintServer::~DbgPrintServer()
	{}
}
