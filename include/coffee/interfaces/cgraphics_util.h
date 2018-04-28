#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/types/edef/graphicsenum.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/cgraphics_api_basic.h>
#include <coffee/interfaces/file_resolver.h>

namespace Coffee {
namespace RHI {

FORCEDINLINE Tuple<Size, CompFmt> UnpackCompressedTexture(Bytes const& img_data)
{
    using C = CompFlags;

    auto pix = C_RCAST<IMG::serial_image const*>(img_data.data);
    struct ImageData
    {
        szptr  data_size;
        i32    width, height;
        PixCmp components;

        u8 _pad[7];
    } data;

    auto fmt      = pix->fmt;
    auto flags    = pix->comp_fmt;
    auto pixflags = PixFlg::None;

    if(img_data.size < 8)
        return std::make_tuple(Size(), CompFmt());

    switch(fmt)
    {
    case PixFmt::S3TC:
    {
        switch(flags)
        {
        case C::S3TC_1:
            pixflags        = PixFlg::RGB;
            data.components = PixCmp::RGB;
            data.data_size  = 8;
            break;
        case C::S3TC_3:
        case C::S3TC_5:
            pixflags        = PixFlg::RGBA;
            data.components = PixCmp::RGBA;
            data.data_size  = 16;
            break;
        default:
            break;
        }

        data.data_size *= (pix->size.w * pix->size.h / 16);
        data.width  = pix->size.w;
        data.height = pix->size.h;

        break;
    }
    default:
        return std::make_tuple(Size(), CompFmt());
    }

    /* Ensure that the upload won't be bad */
    if(img_data.size < data.data_size)
        return std::make_tuple(Size(), CompFmt());

    u32 compressionFlags = C_CAST<u32>(flags);
    compressionFlags <<= 10;

    CSize tex_size = {data.width, data.height};

    return std::make_tuple(tex_size, CompFmt(fmt, pixflags, flags));
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadCompressedTexture(
    typename GFX::S_2D& surface, Bytes&& img_data, PixFmt fmt)
{
    if(img_data.size == 0)
        return false;

    Size    tex_size = {};
    CompFmt cfmt;

    std::tie(tex_size, cfmt) = UnpackCompressedTexture(img_data);

    surface = GFX::S_2D(fmt, 1, 0);

    surface.allocate(tex_size, fmt);

    Bytes image_data;
    image_data.data = &img_data.data[sizeof(IMG::serial_image)];
    image_data.size = img_data.size - sizeof(IMG::serial_image);

    surface.upload(cfmt, tex_size, image_data);

    return true;
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadCompressedTexture(
    typename GFX::S_2DA& surface, Bytes&& img_data, i32 layer = 0, u32 mip = 0)
{
    if(img_data.size == 0)
        return false;

    Size    tex_size = {};
    CompFmt cfmt;

    std::tie(tex_size, cfmt) = UnpackCompressedTexture(img_data);

    Bytes image_data;
    image_data.data = &img_data.data[sizeof(IMG::serial_image)];
    image_data.size = img_data.size - sizeof(IMG::serial_image);

    surface.upload(
        cfmt, {tex_size.w, tex_size.h, 1}, image_data, {0, 0, layer}, mip);

    return true;
}

template<
    typename GFX,
    typename Resource,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
/*!
 * \brief Variant of LoadCompressedTexture that loads mipmaps
 * \param surface
 * \param img_data
 * \param layer
 * \return
 */
FORCEDINLINE bool LoadCompressedTextureMipmap(
    typename GFX::S_2DA&              surface,
    ResourceResolver<Resource> const& rr,
    Function<bool(Url const&)> const& pred,
    Url const&                        baseUrl,
    i32                               layer = 0)
{
    Vector<Url> urls;
    if(!rr.resourceQuery(Path(baseUrl).removeExt(), urls))
        return false;

    u32 mip = 0;
    auto it = urls.begin();
    while((it = std::find_if(it, urls.end(), pred)) != urls.end())
    {
        LoadCompressedTexture<GFX>(
                    surface,
                    rr.resolveResource(*it),
                    layer,
                    mip++
                );
        it++;

        if(mip >= surface.mipmaps())
            break;
    }

    return true;
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadTexture(typename GFX::S_2D& surface, Bytes&& tex_rsc)
{
    bool status = true;

    Stb::Img tex_src;
    if(Stb::LoadData(&tex_src, tex_rsc))
    {
        surface.allocate(tex_src.size, PixCmp::RGBA);
        surface.upload(
            BitFormat::UByte,
            PixCmp::RGBA,
            tex_src.size,
            tex_src.data,
            {0, 0},
            0);
        Stb::ImageFree(&tex_src);
    } else
        status = false;

    return status;
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadTexture(
    typename GFX::S_2DA& surface, Bytes&& tex_rsc, i32 layer = 0)
{
    bool status = true;

    Stb::Img tex_src;
    if(Stb::LoadData(&tex_src, tex_rsc))
    {
        CSize3 tex_size = {tex_src.size.w, tex_src.size.h, 1};

        surface.upload(
            BitFormat::UByte,
            PixCmp::RGBA,
            tex_size,
            tex_src.data,
            {0, 0, layer},
            0);
        Stb::ImageFree(&tex_src);
    } else
        status = false;

    return status;
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadShader(
    typename GFX::SHD& shader, Bytes&& data, ShaderStage stage)
{
    bool status = shader.compile(stage, data);

    return status;
}

template<
    typename GFX,
    typename implements<GraphicsAPI_Base, GFX>::type* = nullptr>
FORCEDINLINE bool LoadPipeline(
    typename GFX::PIP& pip, Bytes&& vert_file, Bytes&& frag_file)
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

template<typename GFX>
FORCEDINLINE bool LoadPipeline(
    typename GFX::PIP&   pip,
    BytesResolver const& resolver,
    Url const&           vert_file,
    Url const&           frag_file)
{
    return LoadPipeline<GFX>(
        pip, resolver.resolver(vert_file), resolver.resolver(frag_file));
}

} // namespace RHI
} // namespace Coffee
