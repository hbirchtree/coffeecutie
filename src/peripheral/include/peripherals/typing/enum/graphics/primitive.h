#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {
namespace primitive {

enum class base
{
    Triangle,
    Point,
    Line,
    Patch,
};

enum class creation
{
    Explicit,
    Strip,
    Fan,
    Loop,
    Adjacency,

    MaxValue,
};

C_FLAGS(creation, libc_types::u32);

} // namespace primitive
} // namespace graphics
} // namespace typing
