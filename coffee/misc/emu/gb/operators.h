#ifndef COFFEE_MISC_EMU_GAMEBOY_OPERATORS_H
#define COFFEE_MISC_EMU_GAMEBOY_OPERATORS_H

#define CGB_LDrr_XX(reg1,reg2) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        core->reg.reg1 = core->reg.reg2;\
        core->reg.m = 1;\
        core->reg.t = 4;\
    }

#define CGB_LDrHLm_X(reg1) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        core->reg.reg1 = mmu_read_byte(core,mmu,(core->reg.h << 8)+core->reg.l);\
        core->reg.m = 2;\
        core->reg.t = 8;\
    }

#define CGB_LDHLmr_X(reg1) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        mmu_write_byte(core,mmu,(core->reg.h << 8)+core->reg.l,core->reg.reg1);\
        core->reg.m = 2;\
        core->reg.t = 8;\
    }

#define CGB_LDXXnn(reg1,reg2) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        core->reg.reg2 = mmu_read_byte(core,mmu,core->reg.pc);\
        core->reg.reg1 = mmu_read_byte(core,mmu,core->reg.pc+1);\
        core->reg.pc += 2;\
        core->reg.m = 3;\
        core->reg.t = 12;\
    }

#define CGB_LDrn_X(reg1) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        core->reg.reg1=mmu_read_byte(core,mmu,core->reg.pc);\
        core->reg.pc++;\
        core->reg.m = 2;\
        core->reg.t = 8;\
    }

#define CGB_LDHLmn() [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        mmu_write_byte(core,mmu,(core->reg.h<<8)+core->reg.l,mmu_read_byte(core->reg.pc));\
        core->reg.pc++;\
        core->reg.m = 3;\
        core->reg.t = 12;\
    }

#define CGB_LDXXmX(reg1,reg2,reg3) [](_cgb_core* core, _cgb_mmu* mmu)\
    {\
        core->reg.reg1=mmu_read_byte(core,mmu,core->reg.pc);\
        core->reg.pc++;\
        core->reg.m = 2;\
        core->reg.t = 8;\
    }

#include "cgameboy_structures.h"

namespace Coffee{
namespace CMisc{
namespace CGameBoy{

static void core_noop(_cgb_core* core, _cgb_mmu*)
{
    core->reg.lclk.m = 1; core->reg.lclk.t = 4;
}

static void core_add(_cgb_core* core, _cgb_mmu*)
{
    core->reg.a += core->reg.e;
    core->reg.f = _cgb_regflag::clear;
    if(!(core->reg.a&255))
        core->reg.f |= _cgb_regflag::zero;
    if(core->reg.a>255)
        core->reg.f |= _cgb_regflag::crry;
    core->reg.a &= 255;
    core->reg.lclk.m = 1; core->reg.lclk.t = 4;
}

static void core_cmp(_cgb_core* core, _cgb_mmu*)
{
    uint16 i = core->reg.a;
    i -= core->reg.b;
    core->reg.f |= _cgb_regflag::op;
    if(!(i&255))
        core->reg.f |= _cgb_regflag::zero;
    if(i<0)
        core->reg.f |= _cgb_regflag::crry;
    core->reg.lclk.m = 1; core->reg.lclk.t = 4;
}

static void core_ld(_cgb_core* core, _cgb_mmu* mmu)
{
    uint16 addr = mmu_read_word(core,mmu,core->reg.pc);
    core->reg.pc += 2;
    core->reg.a = mmu_read_byte(core,mmu,addr);
    core->reg.lclk.m = 4; core->reg.lclk.t = 16;
}

static void core_push_bc(_cgb_core* core, _cgb_mmu* mmu)
{
    core->reg.sp--;
    mmu_write_byte(core,mmu,core->reg.sp,core->reg.b);
    core->reg.sp--;
    mmu_write_byte(core,mmu,core->reg.sp,core->reg.c);
    core->reg.lclk.m = 3; core->reg.lclk.t = 12;
}

static void core_pop_hl(_cgb_core* core, _cgb_mmu* mmu)
{
    core->reg.l = mmu_read_byte(core,mmu,core->reg.sp);
    core->reg.sp++;
    core->reg.h = mmu_read_byte(core,mmu,core->reg.sp);
    core->reg.sp++;
    core->reg.lclk.m = 3; core->reg.lclk.t = 12;
}

static gb_operator_map get_operators()
{
    gb_operator_map map;

    map[0x00] = core_noop;
    map[0x01] = [](_cgb_core* core, _cgb_mmu*){ };

    return map;
}

}
}
}

#endif
