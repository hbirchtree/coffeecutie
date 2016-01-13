#ifndef CTEXTURE_DXTC
#define CTEXTURE_DXTC

#include <base/cfiles.h>
#include <types/composite_types.h>

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Loads DXTC data and returns a texture object pointing to it.
 * \param rsc A header describing the DXTC data
 * \return Always a texture object, validity depends on the input data, has same amount of mipmap levels as input data specifies. Texture object is deleted with delete operator.
 */
extern void coffee_graphics_tex_dxtc_load(
        CTexture& texture, const CGint &blocksize, c_cptr data);

/*!
 * \brief Dump an S3TC-compressed texture to file
 * \param tex Texture that will be dumped, will regardlessly yield compressed data
 * \param res Resource to dump the data to
 */
extern void coffee_graphics_tex_compressed_store(
        const CTexture& tex, CResources::CResource& res);

/*!
 * \brief Load a normal compressed image into memory
 * \param tex
 * \param level
 * \param fmt
 * \param pixelSize
 * \param data
 */
extern void coffee_graphics_tex_compressed_load(
        const CTexture& tex, const CGint& level,
        const CTexIntFormat& fmt, const CGint &pixelSize,
        c_cptr data);

}
}

#endif
