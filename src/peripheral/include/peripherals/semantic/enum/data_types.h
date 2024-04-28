#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace semantic {

enum class type_t : libc_types::u8
{
    u8,
    i8,
    u16,
    i16,
    u32,
    i32,
    u64,
    i64,
    f32,
    f64,

    f11, /* pix_fmt::R10FG11FB11F */
    u11, /* A rare format I hope isn't widespread */
};
}
