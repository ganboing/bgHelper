#include <cstdint>
#include <Windows.h>

enum MY_EXCEPTIONS : DWORD{

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
	ULONG_PTR: sizeof(ULONG_PTR) * 8 - 16;
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
	ULONG_PTR: sizeof(ULONG_PTR) * 8 - 32;
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

static void UnsetHWBreakPoint(DWORD& _dr7, size_t ith)
{
	Dr7_t dr7{ _dr7 };
	switch (ith)
	{
	case 0:
		dr7.L0 = 0;
		break;
	case 1:
		dr7.L1 = 0;
		break;
	case 2:
		dr7.L2 = 0;
		break;
	case 3:
		dr7.L3 = 0;
		break;
	default:
		__assume(0);
	}
	_dr7 = dr7.DR7;
}

static inline void SetHWBreakPoint(DWORD& _dr7, size_t ith, ULONG_PTR attr, ULONG_PTR len)
{
	Dr7_t dr7{ _dr7 };
	switch (ith)
	{
	case 0:
		dr7.L0 = 1;
		dr7.RW0 = attr;
		dr7.LEN0 = len;
		break;
	case 1:
		dr7.L1 = 1;
		dr7.RW1 = attr;
		dr7.LEN1 = len;
		break;
	case 2:
		dr7.L2 = 1;
		dr7.RW2 = attr;
		dr7.LEN2 = len;
		break;
	case 3:
		dr7.L3 = 1;
		dr7.RW3 = attr;
		dr7.LEN3 = len;
		break;
	default:
		__assume(0);
	}
	_dr7 = dr7.DR7;
}

static inline void ClearHWBreakPointStatus(DWORD& _dr6, size_t ith){
	Dr6_t dr6{ _dr6 };
	switch (ith)
	{
	case 0:
		dr6.B0 = 0;
		break;
	case 1:
		dr6.B1 = 0;
		break;
	case 2:
		dr6.B2 = 0;
		break;
	case 3:
		dr6.B3 = 0;
		break;
	default:
		__assume(0);
	}
	_dr6 = dr6.DR6;
}

static inline bool CheckHWBreakPointStatus(const DWORD& _dr6, size_t ith){
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