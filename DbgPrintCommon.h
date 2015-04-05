#pragma once
#include <Windows.h>
#include <memory>
#include <cstdint>
#include <TaskQueue.hpp>
#include <ConcurrentStor.h>

namespace DbgPnt{

	static const size_t BUFF_LINES = 512;
	static const size_t MAX_DBG_STR_LEN = 512;

	static const wchar_t PrinterCmdLine[] = L"DbgPrinter.exe";
	static const wchar_t PrinterMappingName[] = L"Dbg_Printer_Mapping_for_proc_";
	static const wchar_t PrinterMappingFormat[] = L"%s%p";
	static const size_t PrinterMappingNameLen = _countof(PrinterMappingName) + sizeof(HANDLE) * 2;

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
		WSTRING,
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
	union VStr{
		char str[MAX_DBG_STR_LEN];
		wchar_t wstr[MAX_DBG_STR_LEN / sizeof(wchar_t)];
	};
	typedef ConcurrentStor<VStr, BUFF_LINES> VStrsTy;
	typedef TaskQueue<Packet, BUFF_LINES> PacketQueueTy;

	struct DbgPntSharedArea{
		void* operator new  (size_t count);
		void* operator new  (size_t count, uintptr_t mapping_handle); 
		void operator delete  (void* ptr);
		void operator delete  (void* ptr, uintptr_t mapping_handle);
		VStrsTy Vstrs;
		PacketQueueTy PacketQueue;
	};
	typedef std::unique_ptr<DbgPntSharedArea> ManagedAreaView;
}
