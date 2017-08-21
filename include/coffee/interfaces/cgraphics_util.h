#pragma once

#include <coffee/core/CFiles>
#include <coffee/image/cimage.h>
#include <coffee/core/types/edef/graphicsenum.h>

namespace Coffee{
namespace RHI{

template<typename GFX>
FORCEDINLINE bool LoadTexture(typename GFX::S_2D& surface, Resource&& tex_rsc)
{
    bool status = true;

    if(CResources::FileMap(tex_rsc, ResourceAccess::ReadOnly))
    {
        Stb::Img tex_src;
        if(Stb::LoadData(&tex_src, &tex_rsc))
        {
            surface.allocate(tex_src.size, PixCmp::RGBA);
            surface.upload(BitFormat::UByte, PixCmp::RGBA, tex_src.size, tex_src.data,
            {0,0}, 0);
            Stb::ImageFree(&tex_src);
        }else
            status = false;
        CResources::FileUnmap(tex_rsc);
    }else
        status = false;
    return status;
}

template<typename GFX>
FORCEDINLINE bool LoadShader(typename GFX::SHD& shader, Resource&& data,
                             ShaderStage stage)
{
    if(!CResources::FileMap(data, ResourceAccess::ReadOnly))
        return false;

    bool status = shader.compile(stage, FileGetDescriptor(data));

    CResources::FileUnmap(data);

    return status;
}

template<typename GFX>
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

    if(!pip.attach(vert, ShaderStage::Vertex))
        return false;
    if(!pip.attach(frag, ShaderStage::Fragment))
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
