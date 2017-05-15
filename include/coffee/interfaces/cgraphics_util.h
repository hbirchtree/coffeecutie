#pragma once

#include <coffee/core/CFiles>
#include <coffee/image/cimage.h>
#include <coffee/core/types/edef/graphicsenum.h>

namespace Coffee{
namespace RHI{

template<typename GFX>
FORCEDINLINE bool LoadTexture(typename GFX::S_2D& surface, cstring file, ResourceAccess access)
{
    using RSC = CResources::Resource;

    bool status = true;

    RSC tex_rsc(file, access);
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
FORCEDINLINE bool LoadShader(typename GFX::SHD& shader, cstring file,
                             ResourceAccess access, ShaderStage stage)
{
    using RSC = CResources::Resource;

    RSC data(file, access);

    if(!CResources::FileMap(data, ResourceAccess::ReadOnly))
        return false;

    bool status = shader.compile(stage, FileGetDescriptor(data));

    CResources::FileUnmap(data);

    return status;
}

template<typename GFX>
FORCEDINLINE bool LoadPipeline(typename GFX::PIP& pip, cstring vert_file,
                               cstring frag_file, ResourceAccess access)
{
    typename GFX::SHD vert;
    typename GFX::SHD frag;

    if(!LoadShader<GFX>(vert, vert_file, access, ShaderStage::Vertex))
        return false;
    if(!LoadShader<GFX>(frag, frag_file, access, ShaderStage::Fragment))
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
