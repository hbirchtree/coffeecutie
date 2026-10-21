#pragma once

#include <peripherals/libc/types.h>

#include <algorithm>

/*! GX vertex attribute quantisation, shared by gx-bsp and the transcoder */
namespace blam::gx {

/*! s16 with 15 fraction bits */
inline libc_types::i16 to_s16_frac15(libc_types::f32 v)
{
    return static_cast<libc_types::i16>(std::clamp(v * 32768.f, -32768.f, 32767.f));
}

/*! s8 with 6 fraction bits */
inline libc_types::i8 to_s8_frac6(libc_types::f32 v)
{
    return static_cast<libc_types::i8>(std::clamp(v * 64.f, -128.f, 127.f));
}

} // namespace blam::gx
