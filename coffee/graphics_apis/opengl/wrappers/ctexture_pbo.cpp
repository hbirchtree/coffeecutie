#include "ctexture_pbo.h"

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_pbo_upload(
        CTexture const& texture, CBuffer &buffer,
        CTexFormat const& fmt, CDataType const& type,
        CGint const& level)
{
    CBufferType oldType = buffer.type;
    buffer.type = CBufferType::PixelUnpack;

    CTextureData data;

    data.data = nullptr;
    data.format = fmt;
    data.size = texture.size;
    data.datatype = type;

    coffee_graphics_bind(&buffer);
    coffee_graphics_tex_store(&texture,&data,level);
    coffee_graphics_unbind(&buffer);

    buffer.type = oldType;
}

void coffee_graphics_tex_pbo_download(CTexture const& texture, CBuffer &buffer)
{
    CBufferType oldType = buffer.type;
    buffer.type = CBufferType::PixelPack;

    coffee_graphics_bind(&buffer);
    coffee_graphics_tex_readpixels(texture,0,CTexFormat::RGBA,CDataType::UByte,nullptr);
    coffee_graphics_unbind(&buffer);

    buffer.type = oldType;
}

}
}
