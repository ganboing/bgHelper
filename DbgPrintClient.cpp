#include <Windows.h>
#include <ApiWrapper.h>
#include <ApiEHWrapper.h>
#include <memory>
#include "DbgPrintClient.h"
#include "newstd.h"

GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);

namespace DbgPnt{

	HANDLE InitSharedMapping();

	DbgPrintClient::DbgPrintClient() :
		mapping(InitSharedMapping()),
		view(EH_MapViewOfFile(mapping.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DbgPntSharedArea))),
		area(*(DbgPntSharedArea*)view.get())
	{}

	DbgPrintClient::~DbgPrintClient(){
		area.IsShutdown.store(true);
	}

	PacketQueueTy::EleSmartPtr DbgPrintClient::GetPacketSlot(const volatile char* fmt){
		auto p = area.PacketQueue.Reserve();
		my_strncpy(p->format, fmt);
		return p;
	}

	void DbgPrintClient::print(bool b){
		if (b){
			print("true");
		}
		else{
			print("false");
		}
	}

	void DbgPrintClient::print(const volatile char* str, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::STRING;
		auto vstr = area.Vstrs.Alloc();
		my_strncpy(*vstr, str);
		p->data.istr = vstr - &area.Vstrs[0];
	}

	void DbgPrintClient::print(unsigned char cu, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::CHAR;
		p->data.c = cu;
	}

	void DbgPrintClient::print(unsigned short su, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::SHORT;
		p->data.s = su;
	}

	void DbgPrintClient::print(unsigned int iu, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::INT;
		p->data.i = iu;
	}

	void DbgPrintClient::print(unsigned long lu, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::LONG;
		p->data.l = lu;
	}

	void DbgPrintClient::print(unsigned long long llu, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::LONGLONG;
		p->data.ll = llu;
	}

	void DbgPrintClient::print(double d, const char* fmt){
		auto p = GetPacketSlot(fmt);
		p->type = PacketType::DOUBLE;
		p->data.d = d;
	}
}

DbgPrintClient DBGr;

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