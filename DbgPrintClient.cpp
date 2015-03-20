#include <Windows.h>
#include <memory>
#include "ApiWrapper.h"
#include "ApiEHWrapper.h"
#include "DbgPrintClient.h"
#include "GlobalIniter.hpp"
#include "newstd.h"

GEN_WINAPI_EH_RESULT(NULL, MapViewOfFile);
GEN_WINAPI_EH_RESULT(NULL, CreateFileMapping);

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

	DbgPrintClient::DbgPrintClient() :
		//mapping(InitSharedMapping()), 
		mapping(EH_CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DbgPntSharedArea), NULL)),
		view(new ((uintptr_t)mapping.get())DbgPntSharedArea),
		area(*view.get())
	{}

	DbgPrintClient::~DbgPrintClient(){
		area.IsShutdown.store(true);
	}

	PacketQueueTy::EleSmartPtr DbgPrintClient::GetPacketSlot(const volatile char* fmt){
		auto p = area.PacketQueue.Reserve();
		_my_strncpy(p->format, fmt);
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
		_my_strncpy(vstr->str, str);
		p->data.istr = vstr - &area.Vstrs[0];
	}
	
	void DbgPrintClient::print(const volatile UNICODE_STRING& wstr){
		auto p = GetPacketSlot("");
		p->type = PacketType::WSTRING;
		auto vstr = area.Vstrs.Alloc();
		_my_strncpy(vstr->wstr, wstr.Buffer, wstr.Length);
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

//GLOBAL_INITER_IMPL(DbgPrintClient, DBGr)
