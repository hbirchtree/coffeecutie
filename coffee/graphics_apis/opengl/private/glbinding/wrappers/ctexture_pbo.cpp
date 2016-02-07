#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture_pbo.h>
#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_pbo_upload(
        CTexture& texture, CBuffer &buffer,
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

    coffee_graphics_bind(buffer);

    switch(texture.size.dimensions())
    {
    case 1:
        coffee_graphics_tex_store_1d(texture,data,level);
        break;
    case 2:
        coffee_graphics_tex_store_2d(texture,data,level);
        break;
    case 3:
        coffee_graphics_tex_store_3d(texture,data,level);
        break;
    }

    coffee_graphics_unbind(buffer);

    buffer.type = oldType;
}

void coffee_graphics_tex_pbo_download(CTexture const& texture, CBuffer &buffer)
{
    CBufferType oldType = buffer.type;
    buffer.type = CBufferType::PixelPack;

    coffee_graphics_bind(buffer);
    coffee_graphics_tex_readtexels(texture,0,CTexFormat::RGBA,CDataType::UByte,nullptr);
    coffee_graphics_unbind(buffer);

    buffer.type = oldType;
}

}
}
