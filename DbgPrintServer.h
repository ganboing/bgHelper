#include <DbgPrintCommon.h>
#include <WinResMgr.h>

namespace DbgPnt{
	
	class DbgPrintServer{
		typedef std::unique_ptr<DbgPntSharedArea> ManagedAreaView;
	public:
		DWORD const pid;
		ManagedHANDLE const thread;
		ManagedHANDLE const mapping;
		HANDLE const mapping_client;
		ManagedAreaView const view;
		DbgPntSharedArea& area;

		DbgPrintServer(DWORD pid);
		~DbgPrintServer();
		void Run(); 
		void Print(const Packet& packet);
	};
}

//void DbgPrintRemote(const EXCEPTION_RECORD&);