#pragma once

#include "cblam_structures.h"

namespace blam {
namespace hsc {

enum class opcode_t : u16
{
    noop = 0x0,
};

struct script_header
{
    u16 unknown[7];
};

struct opcode_layout
{
    u16      salt;
    opcode_t opcode;
};

struct script_ref
{
    bl_string name;

    /* Questionmark? */
    inline semantic::mem_chunk<u32> opcodes(u32 count) const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);
        return semantic::mem_chunk<u32>::From(
            base_ptr + sizeof(bl_string), count);
    }
};

} // namespace hsc
} // namespace blam
