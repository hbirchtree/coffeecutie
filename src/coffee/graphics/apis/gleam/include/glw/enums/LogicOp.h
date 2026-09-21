#pragma once

#include "common.h"

namespace gl::group {

// LogicOp
enum class logic_op : u32
{
    and_          = 0x1501, // GL_AND
    and_inverted  = 0x1504, // GL_AND_INVERTED
    and_reverse   = 0x1502, // GL_AND_REVERSE
    clear         = 0x1500, // GL_CLEAR
    copy          = 0x1503, // GL_COPY
    copy_inverted = 0x150C, // GL_COPY_INVERTED
    equiv         = 0x1509, // GL_EQUIV
    invert        = 0x150A, // GL_INVERT
    nand          = 0x150E, // GL_NAND
    noop          = 0x1505, // GL_NOOP
    nor           = 0x1508, // GL_NOR
    or_           = 0x1507, // GL_OR
    or_inverted   = 0x150D, // GL_OR_INVERTED
    or_reverse    = 0x150B, // GL_OR_REVERSE
    set           = 0x150F, // GL_SET
    xor_          = 0x1506, // GL_XOR
}; // enum class logic_op

} // namespace gl::group
