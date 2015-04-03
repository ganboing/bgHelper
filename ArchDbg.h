#pragma once
#include <cstdint>
#include <Windows.h>

enum HWBreakPointRWLEN : ULONG_PTR{
	HWBP_EXEC = 0,
	HWBP_WRITE = 1,
	HWBP_ACCESS = 3,
	HWBP_LEN1 = 0,
	HWBP_LEN2 = 1,
	HWBP_LEN8 = 2,
	HWBP_LEN4 = 3
};

struct HWBP_CTRL{
	bool ENABLE;
	ULONG_PTR RW;
	ULONG_PTR LEN;
};

#pragma warning(push)
#pragma warning( disable : 4201 )

#pragma pack(push, 1)

typedef union{
	ULONG_PTR DR6;
	struct{
		ULONG_PTR B0 : 1;
		ULONG_PTR B1 : 1;
		ULONG_PTR B2 : 1;
		ULONG_PTR B3 : 1;
	ULONG_PTR: 9;
		ULONG_PTR BD : 1;
		ULONG_PTR BS : 1;
		ULONG_PTR BT : 1;
	};
}Dr6_t;

typedef union{
	ULONG_PTR DR7;
	struct{
		ULONG_PTR L0 : 1;
		ULONG_PTR G0 : 1;
		ULONG_PTR L1 : 1;
		ULONG_PTR G1 : 1;
		ULONG_PTR L2 : 1;
		ULONG_PTR G2 : 1;
		ULONG_PTR L3 : 1;
		ULONG_PTR G3 : 1;
		ULONG_PTR LE : 1;
		ULONG_PTR GE : 1;
	ULONG_PTR: 3;
		ULONG_PTR GD : 1;
	ULONG_PTR: 2;
		ULONG_PTR RW0 : 2;
		ULONG_PTR LEN0 : 2;
		ULONG_PTR RW1 : 2;
		ULONG_PTR LEN1 : 2;
		ULONG_PTR RW2 : 2;
		ULONG_PTR LEN2 : 2;
		ULONG_PTR RW3 : 2;
		ULONG_PTR LEN3 : 2;
	};
}Dr7_t;

typedef struct {
	uint8_t opcode;
	union{
		struct {
			uint8_t sib;
			uintptr_t addr;
		}GvEv;
		struct {
			uintptr_t addr;
		}rAxOv;
	};
}MovRM32, *PMovRM32;

#pragma pack(pop)

#pragma warning(pop)

static inline void SetHWBreakPointCtrl(DWORD& _dr7, size_t ith, const HWBP_CTRL& ctrl)
{
	Dr7_t dr7{ _dr7 };
	switch (ith)
	{
	case 0:
		dr7.L0 = ctrl.ENABLE;
		dr7.RW0 = ctrl.RW;
		dr7.LEN0 = ctrl.LEN;
		break;
	case 1:
		dr7.L1 = ctrl.ENABLE;
		dr7.RW1 = ctrl.RW;
		dr7.LEN1 = ctrl.LEN;
		break;
	case 2:
		dr7.L2 = ctrl.ENABLE;
		dr7.RW2 = ctrl.RW;
		dr7.LEN2 = ctrl.LEN;
		break;
	case 3:
		dr7.L3 = ctrl.ENABLE;
		dr7.RW3 = ctrl.RW;
		dr7.LEN3 = ctrl.LEN;
		break;
	default:
		__assume(0);
	}
	_dr7 = dr7.DR7;
}

static inline HWBP_CTRL GetHWBreakPointCtrl(DWORD& _dr7, size_t ith)
{
	Dr7_t dr7{ _dr7 };
	switch (ith)
	{
	case 0:
		return{ dr7.L0, dr7.RW0, dr7.LEN0 };
	case 1:
		return{ dr7.L1, dr7.RW1, dr7.LEN1 };
	case 2:
		return{ dr7.L2, dr7.RW2, dr7.LEN2 };
	case 3:
		return{ dr7.L3, dr7.RW3, dr7.LEN3 };
	default:
		__assume(0);
	}
}


static inline void SetHWBreakPointStatus(DWORD& _dr6, size_t ith, bool status){
	Dr6_t dr6{ _dr6 };
	switch (ith)
	{
	case 0:
		dr6.B0 = status;
		break;
	case 1:
		dr6.B1 = status;
		break;
	case 2:
		dr6.B2 = status;
		break;
	case 3:
		dr6.B3 = status;
		break;
	default:
		__assume(0);
	}
	_dr6 = dr6.DR6;
}

static inline bool GetHWBreakPointStatus(const DWORD& _dr6, size_t ith){
	Dr6_t dr6{ _dr6 };
	switch (ith)
	{
	case 0:
		return !!dr6.B0;
	case 1:
		return !!dr6.B1;
	case 2:
		return !!dr6.B2;
	case 3:
		return !!dr6.B3;
	default:
		__assume(0);
	}
}