#include "ctexture_pbo.h"

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_pbo_upload(
        CTexture *texture, CBuffer *buffer,
        CTexFormat const& fmt, CTextureRegion const& region,
        CDataType const& type, CGint const& level)
{
    buffer->type = CBufferType::PixelUnpack;

    CTextureData data;

    data.data = nullptr;
    data.format = fmt;
    data.size.w = region.w;
    data.size.h = region.h;
    data.size.d = region.d;
    data.datatype = type;

    coffee_graphics_bind(buffer);
    coffee_graphics_tex_store(texture,&data,level);
    coffee_graphics_unbind(buffer);
}

void coffee_graphics_tex_pbo_download(CTexture *texture, CBuffer *buffer)
{

}

}
}
