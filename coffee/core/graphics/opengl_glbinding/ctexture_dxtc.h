#ifndef CTEXTURE_DXTC
#define CTEXTURE_DXTC

#include "coffee/core/base/cfiles.h"
#include "coffee/core/types/composite_types.h"
#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CDXTCHeader
{
    CDXTCHeader();

    CSize  resolution; /*!< Size of the largest mipmap */
    const void* data; /*!< Pointer to DXTC data */
    uint16 mipmaps; /*!< Mipmap levels present in the texture */
    uint16 blockSize; /*!< 8 for DXT1, 16 for DXT2 through DXT5 */
    CTexIntFormat internalFormat; /*!< Specifies version of DXTC*/
};

/*!
 * \brief Loads DXTC data and returns a texture object pointing to it.
 * \param rsc A header describing the DXTC data
 * \return Always a texture object, validity depends on the input data, has same amount of mipmap levels as input data specifies. Texture object is deleted with delete operator.
 */
extern CTexture* coffee_graphics_tex_dxtc_load(const CDXTCHeader *rsc);

/*!
 * \brief Dump an S3TC-compressed texture to file
 * \param tex Texture that will be dumped, will regardlessly yield compressed data
 * \param res Resource to dump the data to
 */
extern void coffee_graphics_tex_dxtc_dump(const CTexture* tex, CResources::CResource* res);

}
}

#endif
