#pragma once
#include <ProcessSuspenderCommon.h>
#include <WinResMgr.h>

namespace ProcSuspender{
	class ProcSuspenderParent{
	public:
		ManagedHANDLE const mapping;
	private:
		std::unique_ptr<SuspenderSharedArea> const area;
	public:
		ProcSuspenderParent();
		~ProcSuspenderParent();
		void SuspendSelf();
		void ResumeSelf();
	};
}

typedef ProcSuspender::ProcSuspenderParent ProcSuspenderParent;
ProcSuspenderParent& SusPender();