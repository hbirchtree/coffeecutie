#ifndef COFFEE_BLAM_VOLTA_BITM_SOFT_H
#define COFFEE_BLAM_VOLTA_BITM_SOFT_H

#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief Decode Blam bitmap into RGBA8 format
 * \param img
 * \param map Pointer to "bitmaps.map" data or the map data, depending on version of map format
 * \return A pointer to data which may be loaded into an OpenGL RGBA8 texture
 */
extern uint32* coffee_bitm_soft_decode_image(
        const bitm_image_t* img,
        const void* map);

/*!
 * \brief Decoder which can be told how to decode each bit.
 * \param img
 * \param map
 * \param process This specifies how to process each pixel, from either of the inputs (uint32,uint16,byte) to uint32
 * \return
 */
extern uint32* coffee_bitm_soft_decode_micro(
        const bitm_image_t* img,
        const void* map,
        BlamBitmProcess process);

/*!
 * \brief Debug function for dumping bitmap to file
 * \param img
 * \param bitmfile
 * \param filename
 */
extern void coffee_bitm_dump(
        const bitm_image_t* img,
        const void* bitmfile,
        cstring filename);

}
}

#endif
