#ifndef CTEXTURE_DXTC
#define CTEXTURE_DXTC

#include "glbinding.h"
#include "coffee/cfiles.h"
#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CDXTCHeader
{
    uint16 mipmaps      = 0;
    uint16 components   = 0;
    uint16 blockSize    = 0; /*!< 8 for DXT1, 16 for DXT2 through DXT5 */
    GLenum internalFormat = GL_NONE;
    CSize  resolution;

    szptr           memsize = 0;
    const void*    data     = nullptr;
};

extern CTexture* coffee_graphics_tex_dxtc_load(const CDXTCHeader *rsc);

}
}

#endif
