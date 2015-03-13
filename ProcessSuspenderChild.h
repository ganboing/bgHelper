#include <Windows.h>
#include "ApiEHWrapper.h"
#include "WinResMgr.h"
#include "ProcessSuspenderCommon.h"

namespace ProcSuspender{

	class ProcSuspenderChild{
		DWORD const pid;
		ManagedHANDLE const proc;
		ManagedHANDLE const mapping;
		ManagedView const view;
		SuspenderSharedArea& area;
	public:
		ProcSuspenderChild(DWORD pid, HANDLE mapping);
		~ProcSuspenderChild();
		inline static DWORD WINAPI ThreadProc(LPVOID p){
			((ProcSuspenderChild*)p)->Run();
			return 0;
		}
		void Run();
	};
}

typedef ProcSuspender::ProcSuspenderChild ProcSuspenderChild;