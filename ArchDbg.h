#include <cstdint>
#include <Windows.h>

#pragma warning(push)
#pragma warning( disable : 4201 )

#pragma pack(push, 1)

typedef union{
	struct{
		uint32_t B0 : 1;
		uint32_t B1 : 1;
		uint32_t B2 : 1;
		uint32_t B3 : 1;
	uint32_t: 9;
		uint32_t BD : 1;
		uint32_t BS : 1;
		uint32_t BT : 1;
		uint32_t RTM : 1;
	uint32_t: 15;
	};
	uint32_t DR6;
}Dr6_t;

typedef union{
	struct{
		uint32_t L0 : 1;
		uint32_t G0 : 1;
		uint32_t L1 : 1;
		uint32_t G1 : 1;
		uint32_t L2 : 1;
		uint32_t G2 : 1;
		uint32_t L3 : 1;
		uint32_t G3 : 1;
		uint32_t LE : 1;
		uint32_t GE : 1;
	uint32_t: 1;
		uint32_t RTM : 1;
	uint32_t: 1;
		uint32_t GD : 1;
	uint32_t: 2;
		uint32_t RW0 : 2;
		uint32_t LEN0 : 2;
		uint32_t RW1 : 2;
		uint32_t LEN1 : 2;
		uint32_t RW2 : 2;
		uint32_t LEN2 : 2;
		uint32_t RW3 : 2;
		uint32_t LEN3 : 2;
	};
	uint32_t DR7;
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

static void UnsetBreakPoint(CONTEXT& Context, SIZE_T Dr7Slot)
{
	Dr7_t dr7;
	dr7.DR7 = Context.Dr7;
	switch (Dr7Slot)
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
	}
	Context.Dr7 = dr7.DR7;
}

static void SetReadBreakPoint(CONTEXT& Context, ULONG_PTR Addr, SIZE_T Dr7Slot)
{
	Dr7_t dr7;
	dr7.DR7 = Context.Dr7;
	switch (Dr7Slot)
	{
	case 0:
		dr7.L0 = 1;
		dr7.RW0 = 3;
		dr7.LEN0 = 3;
		Context.Dr0 = Addr;
		break;
	case 1:
		dr7.L1 = 1;
		dr7.RW1 = 3;
		dr7.LEN1 = 3;
		Context.Dr1 = Addr;
		break;
	case 2:
		dr7.L2 = 1;
		dr7.RW2 = 3;
		dr7.LEN2 = 3;
		Context.Dr2 = Addr;
		break;
	case 3:
		dr7.L3 = 1;
		dr7.RW3 = 3;
		dr7.LEN3 = 3;
		Context.Dr3 = Addr;
		break;
	}
	Context.Dr7 = dr7.DR7;
}