#ifndef CBLAM_BITM_DECODERS
#define CBLAM_BITM_DECODERS

#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief Convert RGBA struct to integer format containing the RGBA bytes
 * \param c Color structure to be converted
 * \return An RGBA integer
 */
inline static uint32 blam_rgba_to_int(const blam_rgba &c)
{
    return c.i;
}

/*!
 * \brief Decoder for the A8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8(uint32 d, uint16 s,byte b)
{
    return b << 24;
}
/*!
 * \brief Decoder for the Y8 and P8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_p8_y8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = col.g = col.b = b;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the AY8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_ay8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.r = col.g = col.b = col.a = b;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the A8Y8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8y8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = s & 0xFF;
    col.r = col.g = col.b = s >> 8;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the R5G6B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_r5g6b5(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.r = (((s >> 11) & 0x1F) * 0xFF) / 31;
    c.g = (((s >>  5) & 0x3F) * 0xFF) / 63;
    c.b = (((s      ) & 0x1F) * 0xFF) / 31;
    c.a = 255;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the A1R5G5B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a1r5g5b5(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.a = (  s >> 15)         * 0xFF;
    c.r = (((s >> 10) & 0x1F) * 0xFF) / 31;
    c.g = (((s >>  5) & 0x3F) * 0xFF) / 31;
    c.b = (((s      ) & 0x1F) * 0xFF) / 31;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the A4R4G4B4 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a4r4g4b4(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.a = (( s >> 12)         * 0xFF) / 15;
    c.r = (((s >>  8) & 0x0F) * 0xFF) / 15;
    c.g = (((s >>  4) & 0x0F) * 0xFF) / 15;
    c.b = (((s >>  0) & 0x0F) * 0xFF) / 15;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the ARGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8r8g8b8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = (d >> 24);
    col.r = (d >> 16) & 0xFF;
    col.g = (d >>  8) & 0xFF;
    col.b = (d      ) & 0xFF;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the XRGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_x8r8g8b8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = (d >> 16) & 0xFF;
    col.g = (d >> 8) & 0xFF;
    col.b = (d) & 0xFF;
    return blam_rgba_to_int(col);
}


}
}

#endif
