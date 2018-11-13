#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace typing {

enum class LogicOp : libc_types::u32
{
    CLEAR0 = 0x1,
    CLEAR1 = 0x2,

    AND  = 0x4,
    NAND = 0x8,
    OR   = 0x10,
    XOR  = 0x20,
    NOR  = 0x40,

    SRC_INVERSE = 0x80,  /*Invert source*/
    DST_INVERSE = 0x100, /*Invert destination*/

    COPY = 0x200, /* Copy source */
    NOOP = 0x400, /* Copy destination */
};
C_FLAGS(LogicOp, libc_types::u32);

} // namespace typing
