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
#include <sstream>
#include "NewApi.h"
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
		switch (packet.type){
		case PacketType::CHAR:
			DbgPintfA(packet.format, packet.data.c);
			return;
		case PacketType::INT:
			DbgPintfA(packet.format, packet.data.i);
			return;
		case PacketType::SHORT:
			DbgPintfA(packet.format, packet.data.s);
			return;
		case PacketType::LONG:
			DbgPintfA(packet.format, packet.data.l);
			return;
		case PacketType::LONGLONG:
			DbgPintfA(packet.format, packet.data.ll);
			return;
		case PacketType::DOUBLE:
			DbgPintfA(packet.format, packet.data.d);
			return;
		case PacketType::STRING:
			DbgPintfA(packet.format, area.Vstrs[packet.data.istr].str);
			area.Vstrs.Free(&area.Vstrs[packet.data.istr]);
			return;
		case PacketType::WSTRING:
			OutputDebugStringW(area.Vstrs[packet.data.istr].wstr);
			area.Vstrs.Free(&area.Vstrs[packet.data.istr]);
			return;
		}
	}

	void DbgPrintServer::Run(){
		DbgPintfA("DbgServer for %p started\n", (ULONG_PTR)pid);
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
		DbgPintfA("DbgServer for %p stopped\n", (ULONG_PTR)pid);
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
					PrinterMappingFormat, PrinterMappingName, (ULONG_PTR)pid
				)
			)
		),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DbgPntSharedArea))),
		area(*(DbgPntSharedArea*)view.get())
	{}

	DbgPrintServer::~DbgPrintServer()
	{}
}
