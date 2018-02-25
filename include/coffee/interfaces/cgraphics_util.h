#pragma once

#include <coffee/core/CFiles>
#include <coffee/image/cimage.h>
#include <coffee/core/types/edef/graphicsenum.h>

namespace Coffee{
namespace RHI{

template<typename GFX, typename Resource,
         typename std::enable_if<
             std::is_base_of<ByteProvider, Resource>::value
             >::type* = nullptr>
FORCEDINLINE bool LoadTexture(typename GFX::S_2D& surface,
                              Resource&& tex_rsc)
{
    bool status = true;

    Stb::Img tex_src;
    if(Stb::LoadData(&tex_src, C_OCAST<Bytes>(tex_rsc)))
    {
        surface.allocate(tex_src.size, PixCmp::RGBA);
        surface.upload(BitFormat::UByte, PixCmp::RGBA, tex_src.size, tex_src.data,
        {0,0}, 0);
        Stb::ImageFree(&tex_src);
    }else
        status = false;

    return status;
}

template<typename GFX,typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadShader(typename GFX::SHD& shader, Resource&& data,
                             ShaderStage stage)
{
    bool status = shader.compile(stage, C_OCAST<Bytes>(data));

    return status;
}

template<typename GFX, typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadPipeline(typename GFX::PIP& pip,
                               Resource&& vert_file,
                               Resource&& frag_file)
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
