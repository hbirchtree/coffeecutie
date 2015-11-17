#ifndef COFFEE_MISC_EMU_GAMEBOY_STRUCTURES_H
#define COFFEE_MISC_EMU_GAMEBOY_STRUCTURES_H

#include <coffee/core/CBase>
#include <coffee/core/CTypes>

namespace Coffee{
namespace CMisc{
namespace CGameBoy{

typedef uint32 addr_t;
typedef uint16 reg8_t;
typedef uint16 reg16_t;

struct _cgb_clk
{
    uint32 m;
    uint32 t;
};

enum class _cgb_regflag : uint8
{
    clear = 0x0,
    crry = 0x10,
    hcrry = 0x20,
    op = 0x40,
    zero = 0x80,
};

C_FLAGS(_cgb_regflag,uint8)

struct _cgb_registers
{
    reg8_t a,b,c,d,e,h,l; /*!< These are 8-bit registers, uint16 to avoid unnoticed overflow*/
    _cgb_regflag f;
    reg16_t pc,sp; /*!< These are 16-bit registers*/
    _cgb_clk lclk;
};

struct _cgb_core{
    _cgb_clk clk;
    _cgb_registers reg;
};

struct _cgb_bios
{
    szptr size;
    byte* data;
    bool in_bios;
};

struct _cgb_rom
{
    szptr size;
    byte* data;
};

struct _cgb_gpu
{
    byte* memory;
    byte* object_attribute_mem;
};

struct _cgb_memory_unit
{
    byte* eram;
    byte* wram;
    byte* zram;
};

/*!
 * \brief Memory management unit, more used as pure memory container
 */
struct _cgb_mmu
{
    _cgb_memory_unit mem;
    _cgb_rom rom;
    _cgb_bios bios;
    _cgb_gpu gpu;
};

/*!
 * \brief The fundamental GameBoy core operator
 */
//using gb_operator = void(*)(_cgb_core*,_cgb_mmu*);
using gb_operator = std::function<void(_cgb_core*,_cgb_mmu*)>;
/*!
 * \brief A map from opcode to function pointers
 */
using gb_operator_map = std::map<uint8,gb_operator>;

}
}
}

#endif
