#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

/*!
 * Packed vertex fields the Xbox formats share. Normals are signed
 * normalized bitfields, x in the low bits.
 */
namespace blam::vertex_pack {

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using typing::vector_types::Vecf3;

/*! A signed normalized bitfield of `width` bits */
inline f32 snorm_bits(u32 bits, u32 width)
{
    i32 value = static_cast<i32>(bits);
    if(value & (1 << (width - 1)))
        value -= 1 << width;
    return static_cast<f32>(value) / static_cast<f32>((1 << (width - 1)) - 1);
}

/*! 11:11:10, as Halo 1 and 2 pack normals */
inline Vecf3 unpack_11_11_10(u32 v)
{
    return {
        snorm_bits(v & 0x7FF, 11),
        snorm_bits((v >> 11) & 0x7FF, 11),
        snorm_bits((v >> 22) & 0x3FF, 10)};
}

/*! 10:11:11 (D3DDECLTYPE_DHEN3N) */
inline Vecf3 unpack_10_11_11(u32 v)
{
    return {
        snorm_bits(v & 0x3FF, 10),
        snorm_bits((v >> 10) & 0x7FF, 11),
        snorm_bits((v >> 21) & 0x7FF, 11)};
}

} // namespace blam::vertex_pack
