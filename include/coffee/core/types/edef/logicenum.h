#pragma once

#include "enumfun.h"
#include "../tdef/integertypes.h"

#undef Always

namespace Coffee{

enum class Operator
{
    Add,
    Sub,
    RevSub,
    Min,
    Max,
};

enum class LogicOp
{
    CLEAR0 = 0x1,
    CLEAR1 = 0x2,

    AND = 0x4,
    NAND = 0x8,
    OR = 0x10,
    XOR = 0x20,
    NOR = 0x40,

    SRC_INVERSE = 0x80, /*Invert source*/
    DST_INVERSE = 0x100, /*Invert destination*/

    COPY = 0x200, /* Copy source */
    NOOP = 0x400, /* Copy destination */
};
C_FLAGS(LogicOp,uint32);

enum class ValueComparison
{
    Never,
    Less,
    LEqual,
    Equal,
    NEqual,
    Greater,
    GEqual,
    Always,
};

enum class ValueHandling
{
    Zero,
    Keep,
    Replace,
    Increment,
    Decrement,
    WrapIncrement,
    WrapDecrement,
    Invert,
};

}
