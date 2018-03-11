#pragma once

#include <coffee/core/CFiles>
#include <coffee/image/cimage.h>
#include <coffee/core/types/edef/graphicsenum.h>

namespace Coffee{
namespace RHI{

template<typename GFX, typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadCompressedTexture(
        typename GFX::S_2D& surface,
        Resource&& tex_rsc,
        PixFmt fmt, CompFlags flags
        )
{
    using C = CompFlags;

    struct ImgDesc {
        void* img_data;
        szptr data_size;
        u32 width;
        u32 height;
        PixCmp components;
    } data;

    Bytes img_data = C_OCAST<Bytes>(tex_rsc);

    if(img_data.size < 8)
        return false;

    switch(fmt)
    {
    case PixFmt::S3TC:
    {
        switch(flags)
        {
        case C::S3TC_1:
        {
            data.components = PixCmp::RGB;
            data.data_size = 8;
            break;
        }
        case C::S3TC_5:
        {
            data.components = PixCmp::RGBA;
            data.data_size = 16;
            break;
        }
        default:
            break;
        }

        u32* pix_size = C_RCAST<u32*>(img_data.data);
        data.width = pix_size[0];
        data.height = pix_size[1];

        data.data_size *= (pix_size[0] * pix_size[1] / 16);

        break;
    }
    default:
        return false;
    }

    /* Ensure that the upload won't be bad */
    if(img_data.size < data.data_size)
        return false;

    /* TODO: Implement support for mipmap uploads */

    u32 compressionFlags = C_CAST<u32>(flags);
    compressionFlags <<= 10;

    CSize tex_size = {data.width,  data.height};

    surface = GFX::S_2D(fmt, 1, compressionFlags);

    surface.allocate(tex_size, fmt);

    surface.upload(BitFmt::Byte, fmt, tex_size,
                   img_data);

    return true;
}

template<typename GFX, typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadTexture(
        typename GFX::S_2D& surface,
        Resource&& tex_rsc
        )
{
    bool status = true;

    Stb::Img tex_src;
    if(Stb::LoadData(&tex_src, C_OCAST<Bytes>(tex_rsc)))
    {
        surface.allocate(tex_src.size, PixCmp::RGBA);
        surface.upload(
                    BitFormat::UByte, PixCmp::RGBA,
                    tex_src.size, tex_src.data, {0,0}, 0);
        Stb::ImageFree(&tex_src);
    }else
        status = false;

    return status;
}

template<typename GFX,typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadShader(
        typename GFX::SHD& shader,
        Resource&& data,
        ShaderStage stage
        )
{
    bool status = shader.compile(stage, C_OCAST<Bytes>(data));

    return status;
}

template<typename GFX, typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadPipeline(
        typename GFX::PIP& pip,
        Resource&& vert_file,
        Resource&& frag_file
        )
{
    typename GFX::SHD vert;
    typename GFX::SHD frag;

    if(!LoadShader<GFX>(vert, std::move(vert_file), ShaderStage::Vertex))
        return false;
    if(!LoadShader<GFX>(frag, std::move(frag_file), ShaderStage::Fragment))
        return false;

    auto& vert_ref = pip.storeShader(std::move(vert));
    auto& frag_ref = pip.storeShader(std::move(frag));

    if(!pip.attach(vert_ref, ShaderStage::Vertex))
        return false;
    if(!pip.attach(frag_ref, ShaderStage::Fragment))
        return false;

    bool status = pip.assemble();

    if(!status)
    {
        vert.dealloc();
        frag.dealloc();
    }

    return true;
}

}
}
