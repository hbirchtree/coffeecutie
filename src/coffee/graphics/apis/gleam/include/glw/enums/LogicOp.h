#pragma once

#include "common.h"

namespace gl::group {

// LogicOp
enum class logic_op : ::libc_types::u32
{
#ifdef GL_AND
    and_ = GL_AND,
#endif
#ifdef GL_AND_INVERTED
    and_inverted = GL_AND_INVERTED,
#endif
#ifdef GL_AND_REVERSE
    and_reverse = GL_AND_REVERSE,
#endif
#ifdef GL_CLEAR
    clear = GL_CLEAR,
#endif
#ifdef GL_COPY
    copy = GL_COPY,
#endif
#ifdef GL_COPY_INVERTED
    copy_inverted = GL_COPY_INVERTED,
#endif
#ifdef GL_EQUIV
    equiv = GL_EQUIV,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_NAND
    nand = GL_NAND,
#endif
#ifdef GL_NOOP
    noop = GL_NOOP,
#endif
#ifdef GL_NOR
    nor = GL_NOR,
#endif
#ifdef GL_OR
    or_ = GL_OR,
#endif
#ifdef GL_OR_INVERTED
    or_inverted = GL_OR_INVERTED,
#endif
#ifdef GL_OR_REVERSE
    or_reverse = GL_OR_REVERSE,
#endif
#ifdef GL_SET
    set = GL_SET,
#endif
#ifdef GL_XOR
    xor_ = GL_XOR,
#endif
}; // enum class logic_op

} // namespace gl::group
