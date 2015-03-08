#include <rpc.h>
#include <ApiEHWrapper.h>
#include <cstdio>
#include <memory>
#include "DbgPrintServer.h"

GEN_WINAPI_EH_RESULT(NULL, CreateThread);
GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);
GEN_WINAPI_EH_RESULT(NULL, OpenProcess);
GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(0, DuplicateHandle);
GEN_WINAPI_EH_STATUS(RPC_S_OK, I_RpcBindingInqLocalClientPID);

namespace DbgPnt{

	void* DbgPntSharedArea::operator new(size_t count, uintptr_t mapping){
		return EH_MapViewOfFile((HANDLE)mapping, FILE_MAP_ALL_ACCESS, 0, 0, count);
	}

	void DbgPntSharedArea::operator delete  (void* ptr){
		UnmapViewOfFile(ptr);
	}

	void DbgPntSharedArea::operator delete  (void* ptr, uintptr_t mapping_handle){
		UnmapViewOfFile(ptr);
	}

	HANDLE DuplicateToProcess(DWORD Pid, HANDLE Handle){
		HANDLE ret;
		HANDLE my = GetCurrentProcess();
		ManagedHANDLE other(EH_OpenProcess(PROCESS_DUP_HANDLE, FALSE, Pid));
		EH_DuplicateHandle(my, Handle, other.get(), &ret, 0, FALSE, DUPLICATE_SAME_ACCESS);
		return ret;
	}

	void DbgPrintServer::Print(const Packet& packet){
		char buff[MAX_DBG_STR_LEN];
		switch (packet.type){
		case PacketType::STRING:
			_snprintf(buff, _countof(buff), packet.format, area.Vstrs[packet.data.istr]);
			area.Vstrs.Free(&area.Vstrs[packet.data.istr]);
			break;
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
		}
		OutputDebugString(buff);
	}

	void DbgPrintServer::Run(){
		printf("DbgServer for %d started\n", pid);
		for (;;){
			auto nextpacket = area.PacketQueue.Pick();
			if (!nextpacket){
				if (area.IsShutdown.load()){
					printf("DbgServer for %d stopped\n", pid);
					break;
				}
				else{
					Sleep(20);
					continue;
				}
			}
			Print(*nextpacket);
			area.PacketQueue.Dequeue();
		}
	}

	DWORD WINAPI DbgPntThread(LPVOID _pDbgServer){
		std::unique_ptr<DbgPrintServer> pDbgServer((DbgPrintServer*)_pDbgServer);
		pDbgServer->Run();
		return 0;
	}


	DbgPrintServer::DbgPrintServer(DWORD _pid) :
		pid(_pid),
		thread(EH_CreateThread(NULL, 0, DbgPntThread, this, CREATE_SUSPENDED, NULL)),
		mapping(EH_CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DbgPntSharedArea), NULL)),
		mapping_client(DuplicateToProcess(pid, mapping.get())),
		view(new ((uintptr_t)mapping.get())DbgPntSharedArea),
		area(*view.get())
	{}

	DbgPrintServer::~DbgPrintServer()
	{}

	extern "C"{
		unsigned __int3264 DbgPntCreate(IN RPC_BINDING_HANDLE h1){
			DWORD pid;
			EH_I_RpcBindingInqLocalClientPID(h1, &pid);
			auto pDbgServer = new DbgPrintServer(pid);
			printf("new print serverfor process: %d created\n", pid);
			ResumeThread(pDbgServer->thread.get());
			return (uintptr_t)pDbgServer->mapping_client;
		}
	}
}

extern "C"
{
	void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
	{
		return malloc(len);
	}

	void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
	{
		return free(ptr);
	}
}