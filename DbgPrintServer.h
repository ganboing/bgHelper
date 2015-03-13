#include <DbgPrintCommon.h>
#include <WinResMgr.h>

namespace DbgPnt{
	
	class DbgPrintServer{
	public:
		DWORD const pid;
		ManagedHANDLE const mapping;
		ManagedView const view;
		DbgPntSharedArea& area;

		DbgPrintServer(DWORD pid, HANDLE mapping);
		~DbgPrintServer();
		inline static DWORD WINAPI ThreadProc(LPVOID p){
			((DbgPrintServer*)p)->Run();
			return 0;
		}
		void Run(); 
		void Print(const Packet& packet);
	};
}

typedef DbgPnt::DbgPrintServer DbgPrintServer;

//void DbgPrintRemote(const EXCEPTION_RECORD&);