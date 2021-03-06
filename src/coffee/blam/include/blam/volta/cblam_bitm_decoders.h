#pragma once

#include "cblam_structures.h"

namespace blam {
namespace bitm_decode {
namespace detail {
/*!
 * \brief Convert RGBA struct to integer format containing the RGBA bytes
 * \param c Color structure to be converted
 * \return An RGBA integer
 */
inline static u32 rgba_to_int(bl_rgba_t const& c)
{
    return c.i;
}

} // namespace detail

/*!
 * \brief Decoder for the A8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 a8(u32, u16, u8 b)
{
    return C_FCAST<u32>(b << 24);
}
/*!
 * \brief Decoder for the Y8 and P8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 p8_y8(u32, u16, u8 b)
{
    bl_rgba_t col;
    col.a = 0;
    col.r = col.g = col.b = b;
    return detail::rgba_to_int(col);
}
/*!
 * \brief Decoder for the AY8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 ay8(u32, u16, u8 b)
{
    bl_rgba_t col;
    col.r = col.g = col.b = col.a = b;
    return detail::rgba_to_int(col);
}
/*!
 * \brief Decoder for the A8Y8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 a8y8(u32, u16 s, u8)
{
    bl_rgba_t col;
    col.a = s & 0xFF;
    col.r = col.g = col.b = s >> 8;
    return detail::rgba_to_int(col);
}
/*!
 * \brief Decoder for the R5G6B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 r5g6b5(u32, u16 s, u8)
{
    bl_rgba_t c;
    c.r = (((s >> 11) & 0x1F) * 0xFF) / 31;
    c.g = (((s >> 5) & 0x3F) * 0xFF) / 63;
    c.b = (((s)&0x1F) * 0xFF) / 31;
    c.a = 255;
    return detail::rgba_to_int(c);
}
/*!
 * \brief Decoder for the A1R5G5B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 a1r5g5b5(u32, u16 s, u8)
{
    bl_rgba_t c;
    c.a = (s >> 15) * 0xFF;
    c.r = (((s >> 10) & 0x1F) * 0xFF) / 31;
    c.g = (((s >> 5) & 0x3F) * 0xFF) / 31;
    c.b = (((s)&0x1F) * 0xFF) / 31;
    return detail::rgba_to_int(c);
}
/*!
 * \brief Decoder for the A4R4G4B4 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 a4r4g4b4(u32, u16 s, u8)
{
    bl_rgba_t c;
    c.a = ((s >> 12) * 0xFF) / 15;
    c.r = (((s >> 8) & 0x0F) * 0xFF) / 15;
    c.g = (((s >> 4) & 0x0F) * 0xFF) / 15;
    c.b = (((s >> 0) & 0x0F) * 0xFF) / 15;
    return detail::rgba_to_int(c);
}
/*!
 * \brief Decoder for the ARGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 a8r8g8b8(u32 d, u16, u8)
{
    bl_rgba_t col;
    col.a = (d >> 24);
    col.r = (d >> 16) & 0xFF;
    col.g = (d >> 8) & 0xFF;
    col.b = (d)&0xFF;
    return detail::rgba_to_int(col);
}
/*!
 * \brief Decoder for the XRGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static u32 x8r8g8b8(u32 d, u16, u8)
{
    bl_rgba_t col;
    col.a = 0;
    col.r = (d >> 16) & 0xFF;
    col.g = (d >> 8) & 0xFF;
    col.b = (d)&0xFF;
    return detail::rgba_to_int(col);
}

} // namespace bitm_decode
} // namespace blam
