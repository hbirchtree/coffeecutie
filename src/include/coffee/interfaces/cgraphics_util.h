#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/types/edef/graphicsenum.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/cgraphics_api_basic.h>
#include <coffee/interfaces/file_resolver.h>

#include "cgraphics_pixops.h"

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
            break;
        case C::S3TC_3:
        case C::S3TC_5:
            pixflags        = PixFlg::RGBA;
            data.components = PixCmp::RGBA;
            break;
        default:
            break;
        }

        data.width  = pix->size.w;
        data.height = pix->size.h;

        break;
    }
    default:
        return std::make_tuple(Size(), CompFmt());
    }

    data.data_size = GetPixCompressedSize(
        fmt, data.components, pixflags, flags, {data.width, data.height});

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

    u32  mip = 0;
    auto it  = urls.begin();
    while((it = std::find_if(it, urls.end(), pred)) != urls.end())
    {
        LoadCompressedTexture<GFX>(
            surface, rr.resolveResource(*it), layer, mip++);
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

template<typename GFX>
struct shader_param_view
{
    using Pipeline  = typename GFX::PIP;
    using UniState  = typename GFX::USTATE;
    using ProgState = typename GFX::PSTATE;

    using const_desc    = typename GFX::UNIFDESC;
    using const_value   = typename GFX::UNIFVAL;
    using sampler_value = typename GFX::UNIFSMP;
    using param_desc    = typename GFX::PPARAM;

    using constant_desc_t  = Vector<const_desc>;
    using constant_desc_it = typename constant_desc_t::iterator;

    using param_desc_t  = Vector<param_desc>;
    using param_desc_it = typename param_desc_t::iterator;

    static Function<bool(const_desc&)> constant_by_name(
        CString const& param_name)
    {
        return [=](const_desc& desc) { return desc.name == param_name; };
    }

    shader_param_view(Pipeline& pip) : m_pipeline(pip)
    {
    }

    /*!
     * \brief Retrieve pipeline constants and parameters for iteration
     */
    void get_pipeline_params()
    {
        m_states.clear();

        m_constant_desc.clear();
        m_parameter_desc.clear();

        m_constant_data.clear();
        m_constant_values.clear();

        GFX::GetShaderUniformState(
            m_pipeline, &m_constant_desc, &m_parameter_desc);
    }

    constant_desc_it constants_begin()
    {
        return m_constant_desc.begin();
    }
    constant_desc_it constants_end()
    {
        return m_constant_desc.end();
    }

    param_desc_it params_begin()
    {
        return m_parameter_desc.begin();
    }
    param_desc_it params_end()
    {
        return m_parameter_desc.end();
    }

    quick_container<constant_desc_it> constants()
    {
        return quick_container<constant_desc_it>(
            [&]() { return constants_begin(); },
            [&]() { return constants_end(); });
    }

    quick_container<param_desc_it> params()
    {
        return quick_container<param_desc_it>(
            [&]() { return params_begin(); }, [&]() { return params_end(); });
    }

    param_desc_it get_attribute(CString const& attr_name)
    {
        return std::find_if(
            params_begin(), params_end(), [&](param_desc const& param) {
                return param.m_name == attr_name;
            });
    }

    /*!
     * \brief Set data for a given constant,
     *  user is responsible for setting the correct type of data.
     * \param desc
     * \param data
     */
    void set_constant(const_desc const& desc, Bytes&& data)
    {
        if(!(desc.m_flags & ShaderTypes::Uniform_v))
            return;

        m_constant_data.emplace(
            std::make_pair(const_desc_id{&desc}, std::move(data)));
        m_constant_values.emplace(
            std::make_pair(const_desc_id{&desc}, const_value()));
    }

    void set_sampler(const_desc const& desc, sampler_value&& data)
    {
        if(!(desc.m_flags & ShaderTypes::Sampler_v))
            return;

        m_sampler_handles.emplace(
            std::make_pair(const_desc_id{&desc}, std::move(data)));
    }

    Bytes& get_constant_data(CString const& variable)
    {
        for(auto& constant : m_constant_data)
            if(constant.first.desc->m_name == variable)
                return constant.second;

        throw undefined_behavior("constant not found");
    }

    /*!
     * \brief After data is set using set_* functions,
     *  this functions creates a USTATE object
     */
    void build_state()
    {
        for(auto& constant_data : m_constant_data)
        {
            auto& constant_value = m_constant_values[constant_data.first];
            auto  stage          = constant_data.first.desc->stages;

            if(!constant_data.second.data)
                continue;

            constant_value.data  = &constant_data.second;
            constant_value.flags = constant_data.first.desc->m_flags;

            m_states[stage].setUniform(
                *constant_data.first.desc, &constant_value);
        }

        for(auto& sampler_value : m_sampler_handles)
        {
            auto stage = sampler_value.first.desc->stages;

            m_states[stage].setSampler(
                *sampler_value.first.desc, &sampler_value.second);
        }
    }

    /*!
     * \brief Get a USTATE that can be used with the pipeline
     * \param stage
     * \return
     */
    ProgState& get_state()
    {
        ProgState& pstate = m_cached_state;
        pstate.clear();

        for(auto& e : m_states)
            pstate[e.first] = &e.second;

        return pstate;
    }

    /* Containers for direct shader state */
    constant_desc_t m_constant_desc;
    param_desc_t    m_parameter_desc;

    /* Meta-state for building USTATE */
    struct ConstantDescriptor
    {
        typename GFX::UNIFVAL value;
        Bytes                 data;
    };

    struct SamplerDescriptor
    {
        typename GFX::UNIFSMP value;
    };

    struct const_desc_id
    {
        const_desc const* desc;

        bool operator<(const_desc_id const& other) const
        {
            return desc->m_name < other.desc->m_name;
        }
    };

    Map<const_desc_id, Bytes>         m_constant_data;
    Map<const_desc_id, sampler_value> m_sampler_handles;
    Map<const_desc_id, const_value>   m_constant_values;
    Map<ShaderStage, UniState>        m_states;

  private:
    ProgState m_cached_state;
    Pipeline& m_pipeline;
};

} // namespace RHI
} // namespace Coffee