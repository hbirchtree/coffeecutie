#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include "rhi_buffer.h"
#include "rhi_debug.h"
#include "rhi_draw_command.h"
#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_rendertarget.h"
#include "rhi_texture.h"
#include "rhi_uniforms.h"
#include "rhi_vertex.h"

#include <peripherals/concepts/graphics_api.h>
#include <peripherals/concepts/span.h>
#include <peripherals/stl/types.h>

#include <glw/extensions/EXT_multi_draw_arrays.h>
#include <glw/extensions/KHR_debug.h>

namespace gleam {
using stl_types::Optional;
using stl_types::String;
using stl_types::Tup;

struct api
{
    using texture_type      = texture_t;
    using buffer_type       = buffer_t;
    using rendertarget_type = rendertarget_t;
    using program_type      = program_t;
    using vertex_type =
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif

    template<class T>
    requires Buffer<buffer_t, buffer_slice_t>
    inline auto alloc_buffer(T, semantic::RSCA access)
    {
        return MkShared<buffer_t>(m_features.buffer, T::value, access);
    }

    inline auto alloc_program()
    {
        return MkShared<program_t>(m_features.program, std::ref(*m_debug));
    }

    inline auto alloc_rendertarget()
    {
        return MkShared<rendertarget_t>(
            m_features.rendertarget, std::ref(m_rendertargetCurrency));
    }

    template<typename... Args>
    inline auto alloc_shader(Args&&... args)
    {
        return MkShared<shader_t>(std::forward<Args>(args)...);
    }

    template<class T>
    requires Texture<texture_2d_t>
    inline auto alloc_texture(
        T,
        typing::pixels::PixDesc data_type,
        u32                     mipmaps,
        textures::property      properties = textures::property::none)
    {
        if constexpr(T::value == textures::type::d2)
        {
            return MkShared<texture_2d_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        } else if constexpr(T::value == textures::type::d2_array)
        {
            return MkShared<texture_2da_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        } else if constexpr(T::value == textures::type::d3)
        {
            return MkShared<texture_3d_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        } else if constexpr(T::value == textures::type::cube)
        {
            return MkShared<texture_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        } else if constexpr(T::value == textures::type::cube_array)
        {
            return MkShared<texture_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        }

        Throw(unimplemented_path("how did you get here"));
    }

    template<typename Dummy = void>
    requires VertexArray<vertex_type, buffer_type>
    inline auto alloc_vertex_array()
    {
        return MkShared<vertex_type>(
            std::ref(m_features.vertex), std::ref(*m_debug));
    }

    template<class T>
    requires
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        Query<query_t> &&
#endif
#if GLEAM_MAX_VERSION_ES >= 0x200 && GL_EXT_disjoint_timer_query
        Query<ext_disjoint_timer::query_t> &&
#endif
        Query<null_query_t>
    inline auto alloc_query(T type)
    {
        if constexpr(T::value == queries::type::fragments)
        {
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            return MkShared<query_t>(m_features.query, T::value);
#else
            return MkShared<null_query_t>(m_features.query, T::value);
#endif
        }
#if 0
        if constexpr(T::value == queries::type::time)
            return MkShared<query_t>(m_features.query, T::value);
        if constexpr(T::value == queries::type::timestamp)
            return MkShared<timestamp_query>(m_features.query, T::value);
#endif
        return nullptr;
    }

    template<class Dummy = void>
    requires RenderTarget<rendertarget_type, texture_type>
    inline auto default_rendertarget()
    {
        if(!m_framebuffer)
        {
            m_framebuffer = stl_types::MkShared<rendertarget_type>(
                m_features.rendertarget, std::ref(m_rendertargetCurrency));
            
            m_framebuffer->internal_bind(group::framebuffer_target::framebuffer);
            auto status = static_cast<group::framebuffer_status>(cmd::check_framebuffer_status(group::framebuffer_target::framebuffer));
            if (status != group::framebuffer_status::framebuffer_complete)
                Throw(undefined_behavior("invalid framebuffer"));
        }
        return m_framebuffer;
    }

    inline auto& debug()
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(!m_debug)
            m_debug = stl_types::MkUq<debug::api>();
        return *m_debug;
#else
        static debug::null_api nothing;
        return nothing;
#endif
    }

    template<typename... UList>
    Optional<Tup<error, String>> submit(
        draw_command const& command, UList&&... uniforms);

    using extensions_set = stl_types::Set<String>;
    struct load_options_t
    {
        Optional<u32>            api_version;
        Optional<api_type_t>     api_type;
        Optional<extensions_set> api_extensions;
        Optional<features>       api_features;
    };

    static Tup<features, api_type_t, u32> query_native_api_features(
        extensions_set const& extensions, load_options_t options = {});
    static Tup<u32, u32>  query_native_version();
    static extensions_set query_native_extensions();

    STATICINLINE u32 version_tuple_to_u32(Tup<u32, u32> const& ver)
    {
        auto [major, minor] = ver;
        return ((major << 8) & 0xF00) | ((minor << 4) & 0x0F0);
    }

    std::string_view    api_name();
    api_type_t          api_type();
    Tup<u32, u32>       api_version();
    String              shaderlang_name();
    api_type_t          shaderlang_type();
    Tup<u32, u32>       shaderlang_version();
    Tup<String, String> device();
    Optional<String>    device_driver();

    Optional<error> load(load_options_t options = {});

  private:
    static bool supports_extension(
        stl_types::Set<String> const& extensions, String const& ext);
    bool supports_extension(String const& ext);

    stl_types::ShPtr<rendertarget_type> m_framebuffer;
    stl_types::UqPtr<
        std::conditional<debug::api_available, debug::api, debug::null_api>::
            type>
        m_debug;

    rendertarget_currency m_rendertargetCurrency;
    features              m_features;
    extensions_set        m_extensions;
    api_type_t            m_api_type{api_type_t::none};
    u32                   m_api_version{0};
};

namespace detail {

inline Optional<error> evaluate_draw_state(draw_command const& command)
{
    auto const& call = command.call;
    auto const& data = command.data;
    if(call.indexed)
    {
        i32 current_binding{0};
        cmd::get_integerv(
            group::get_prop::element_array_buffer_binding,
            SpanOne(current_binding));
        if(current_binding == 0)
            return error::draw_no_element_buffer;
        if(data.elements.count == 0)
            return error::draw_no_elements;
    } else
    {
        if(data.arrays.count == 0)
            return error::draw_no_arrays;
    }

    return std::nullopt;
}

inline void unsupported_drawcall()
{
    Throw(unimplemented_path("unsupported draw call"));
}

} // namespace detail

template<typename... UList>
inline Optional<Tup<error, String>> api::submit(
    draw_command const& command, UList&&... uniforms)
{
    auto _ = debug().scope(__PRETTY_FUNCTION__);

    auto const& call    = command.call;
    auto const& data    = command.data;
    auto        program = command.program.lock();

    if constexpr(compile_info::debug_mode)
    {
        auto log = detail::program_log(program->m_handle);
        if(auto error = detail::evaluate_draw_state(command); error.has_value())
            return std::make_tuple(*error, String());
    }

    auto                      vao = command.vertices.lock();
    std::shared_ptr<buffer_t> element_buf;
#if GLEAM_MAX_VERSION_ES != 0x200
    if(m_features.vertex.vertex_arrays && m_features.vertex.attribute_binding)
        cmd::bind_vertex_array(vao->m_handle);
    else
#endif
    {
        using buffer_target = group::buffer_target_arb;

        for(auto const& attrib : vao->m_attribute_names)
            cmd::bind_attrib_location(
                program->m_handle, attrib.second, attrib.first);
        for(auto i : stl_types::range<u32>(16))
            cmd::disable_vertex_attrib_array(i);
        for(auto const& attrib : vao->m_attributes)
        {
            cmd::enable_vertex_attrib_array(attrib.index);
            cmd::bind_buffer(
                buffer_target::array_buffer,
                vao->m_buffers.at(attrib.buffer.id).lock()->m_handle);
            detail::vertex_setup_attribute(attrib);
        }

        element_buf = vao->m_element_buffer.lock();
        cmd::bind_buffer(
            buffer_target::element_array_buffer, element_buf->m_handle);
    }

    cmd::use_program(program->m_handle);

    detail::apply_uniforms<UList...>(*program, std::move(uniforms)...);
    if(auto r = detail::apply_samplers(*program, command.samplers);
       r.has_error())
    {
        Throw(undefined_behavior("error when binding samplers"));
    }

    if(call.indexed)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(call.instanced)
        {
            if(data.instances.offset == 0 && data.arrays.offset == 0)
                cmd::draw_elements_instanced(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count);
            else
#if GLEAM_MAX_VERSION_ES == 0
            {
                cmd::draw_elements_instanced_base_instance(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.instances.offset);
            }
#else
                detail::unsupported_drawcall();
#endif
        } else
#endif
            cmd::draw_elements(
                convert::to(call.mode),
                data.elements.count,
                convert::to<group::draw_elements_type>(data.elements.type),
                data.elements.offset);
    } else
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(call.instanced)
        {
            if(data.instances.offset == 0)
                cmd::draw_arrays_instanced(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count);
            else
#if GLEAM_MAX_VERSION_ES == 0
            {
                cmd::draw_arrays_instanced_base_instance(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count,
                    data.instances.offset);
            }
#else
                detail::unsupported_drawcall();
#endif
        } else
#endif
        {
            cmd::draw_arrays(
                convert::to(call.mode), data.arrays.offset, data.arrays.count);
        }
    }

    if(element_buf)
    {
        cmd::bind_buffer(
            group::buffer_target_arb::element_array_buffer,
            element_buf->m_handle);
    }

    return std::nullopt;
}

inline void test_t2d()
{
    api  ap;
    auto a = ap.alloc_texture(
        textures::d2,
        typing::pixels::PixDesc(typing::pixels::PixFmt::RGBA8),
        3);
    a->view(textures::d2, {}, Vecui3{}, size_2d<u32>());
}

inline void test_buffer()
{
    using semantic::RSCA;
    api  ap;
    auto a =
        ap.alloc_buffer(buffers::vertex, RSCA::ReadWrite | RSCA::Persistent);
}

inline void test_query()
{
    api  ap;
    auto a = ap.alloc_query(queries::time);
}

} // namespace gleam
