#include <coffee/CCore>
#include <coffee/core/plat/application_start.h>

#include <coffee/misc/emu/gb/cgameboy.h>
#include <coffee/misc/emu/gb/operators.h>

using namespace Coffee;
using namespace Coffee::CMisc::CGameBoy;

int32 coffee_main(int32 argc, byte_t** argv)
{
    _cgb_core core;
    _cgb_mmu mmu;

    c_memclear(&core,sizeof(core));
    c_memclear(&mmu,sizeof(mmu));

    core_install_bios("bios.gb",&core,&mmu);
    core_load_rom("game.gb",&core,&mmu);

    gb_operator_map operators;

    core_emulate(&core,&mmu,operators);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
