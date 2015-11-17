#ifndef COFFEE_MISC_EMU_GAMEBOY_H
#define COFFEE_MISC_EMU_GAMEBOY_H

#include "cgameboy_structures.h"
#include <coffee/core/Base>

namespace Coffee{
namespace CMisc{
namespace CGameBoy{

static uint8 mmu_read_byte(
        const _cgb_core* core,
        _cgb_mmu* mmu, const addr_t& addr)
{
    switch(addr&0xF000)
    {
        //BIOS
    case 0x0000:{
        if(mmu->bios.in_bios)
        {
            if(addr < 0x0100)
                return mmu->bios.data[addr];
            else if(core->reg.pc == 0x0100)
                mmu->bios.in_bios = 0;
        }
        return mmu->rom.data[addr];
        break;
    }
        //ROM0
    case 0x1000:
    case 0x2000:
    case 0x3000:
        //ROM1 (16k)
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:{
        return mmu->rom.data[addr];
    }
        //Graphics, VRAM (8k)
    case 0x8000:
    case 0x9000:{
        return mmu->gpu.memory[addr&0x1FFF];
    }
        //External RAM (8k)
    case 0xA000:
    case 0xB000:{
        return mmu->mem.eram[addr&0x1FFF];
    }
        //Working RAM (8k)
    case 0xC000:
    case 0xD000:
        //Working RAM shadow
    case 0xE000:{
        return mmu->mem.wram[addr&0x1FFF];
    }
    case 0xF000:{
        switch(addr&0x0F00)
        {
        //Working RAM shadow
        case 0x000:
        case 0x100:
        case 0x200:
        case 0x300:
        case 0x400:
        case 0x500:
        case 0x600:
        case 0x700:
        case 0x900:
        case 0xA00:
        case 0xB00:
        case 0xC00:
        case 0xD00:
            return mmu->mem.wram[addr&0x1FFF];
        case 0xE00:{
            if(addr < 0xFEA0)
                return mmu->gpu.object_attribute_mem[addr&0xFF];
            else
                return 0;
        }
        case 0xF00:{
            if(addr >= 0xFF80)
                return mmu->mem.zram[addr&0x7F];
            else
                //I/O control handling
                return 0;
        }
        }
    }
    }
    return 0;
}

static uint16 mmu_read_word(
        const _cgb_core* core,
        _cgb_mmu* mmu, const addr_t& addr)
{
    return mmu_read_byte(core,mmu,addr) + (mmu_read_byte(core,mmu,addr+1) << 8);
}

static void mmu_write_byte(
        const _cgb_core* core,
        _cgb_mmu* mmu, const addr_t& addr, const uint8& val)
{
}

static void mmu_write_word(
        const _cgb_core* core,
        _cgb_mmu* mmu, const addr_t& addr, const uint16& val)
{
    mmu_write_byte(core,mmu,addr,val);
    mmu_write_byte(core,mmu,addr+1,(val>>8));
}

static void core_reset(_cgb_core* core, _cgb_mmu* mmu)
{
    c_memclear(&core->reg,sizeof(core->reg));
    c_memclear(&core->clk,sizeof(core->clk));

    c_free(mmu->gpu.memory);
    c_free(mmu->gpu.object_attribute_mem);

    c_free(mmu->mem.eram);
    c_free(mmu->mem.wram);
    c_free(mmu->mem.zram);

    c_free(mmu->rom.data);

    mmu->gpu.memory = (byte*)c_alloc(8*1024);

    mmu->mem.eram = (byte*)c_alloc(8*1024);
    mmu->mem.wram = (byte*)c_alloc(8*1024);
    mmu->mem.zram = (byte*)c_alloc(128);

    mmu->bios.in_bios = 1;
    c_free(mmu->bios.data);
    mmu->bios.size = 0;
    c_free(mmu->rom.data);
    mmu->rom.size = 0;
}

static void core_install_bios(
        cstring file, _cgb_core* core, _cgb_mmu* mmu)
{
    core_reset(core,mmu);

    CResources::CResource bios_file(file);
    CResources::coffee_file_pull(&bios_file);

    mmu->bios.data = (byte*)bios_file.data;
    mmu->bios.size = bios_file.size;
}

static void core_load_rom(cstring file,
                          _cgb_core*,
                          _cgb_mmu* mmu)
{
    CResources::CResource rom_file(file);
    CResources::coffee_file_pull(&rom_file);

    mmu->rom.data = (byte*)rom_file.data;
    mmu->rom.size = rom_file.size;
}

static void core_emulate(
        _cgb_core* core,
        _cgb_mmu* mmu,
        gb_operator_map& operators)
{
    uint8 op = mmu_read_byte(core,mmu,core->reg.pc++);

    if(operators[op])
        operators[op](core,mmu);
    else
        cDebug("Missing operator: 0x%x",op);

    core->clk.m += core->reg.lclk.m;
    core->clk.t += core->reg.lclk.t;
}

}
}
}

#endif
