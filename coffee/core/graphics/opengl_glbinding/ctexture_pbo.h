#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_PBO_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_PBO_H

#include "cbuffer.h"
#include "ctexture_types.h"
#include "crenderfence.h"

namespace Coffee{
namespace CGraphicsWrappers{

extern void coffee_graphics_tex_pbo_upload(
        CTexture* texture, CBuffer* buffer,
        CTexIntFormat const& fmt, CTextureRegion const& region,
        CDataType const& type, CGint const& level);

extern void coffee_graphics_tex_pbo_download(
        CTexture* texture, CBuffer* buffer);

}
}

#endif
