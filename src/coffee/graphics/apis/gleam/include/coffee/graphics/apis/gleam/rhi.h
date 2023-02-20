#pragma once

#include <algorithm>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/concepts/span.h>
#include <peripherals/stl/all_of.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/stl/for_each.h>
#include <peripherals/stl/types.h>

#include "rhi_features.h"
#include "rhi_versioning.h"

#include "rhi_buffer.h"
#include "rhi_context.h"
#include "rhi_debug.h"
#include "rhi_draw_command.h"
#include "rhi_limits.h"
#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_rendertarget.h"
#include "rhi_texture.h"
#include "rhi_vertex.h"

#include <coffee/comp_app/services.h>

#include <glw/extensions/EXT_multi_draw_arrays.h>
#include <glw/extensions/KHR_debug.h>

namespace gleam {
using stl_types::Optional;
using stl_types::String;
using stl_types::Tup;

struct api
{
    api() noexcept;
    api(api&&);
    api(api const&) = delete;

    using texture_type      = texture_t;
    using buffer_type       = buffer_t;
    using rendertarget_type = rendertarget_t;
    using program_type      = program_t;
    using vertex_type =
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 \
    || defined(GL_OES_vertex_array_object)
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif

    template<class T>
    requires Buffer<buffer_t, buffer_slice_t>
    inline auto alloc_buffer(T, semantic::RSCA access)
    {
        return MkShared<buffer_t>(m_features.buffer, m_workarounds, T::value, access);
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

    template<typename T>
    inline auto alloc_shader(
        T const& data, shader_t::constants_t const& constants = {})
    {
        return MkShared<shader_t>(std::ref(data), constants);
    }
    template<typename T>
    inline auto alloc_shader(
        T&& data, shader_t::constants_t const& constants = {})
    {
        return MkShared<shader_t>(std::move(data), constants);
    }

    template<class T>
    requires Texture<texture_2d_t>
    inline auto alloc_texture(
        T,
        typing::pixels::PixDesc data_type,
        u32                     mipmaps,
        textures::property      properties = textures::property::none)
    {
        using namespace semantic::concepts::graphics::textures;
        static_assert(
            stl_types::is_any_of_v<
                T,
                decltype(d2),
                decltype(d2_array),
                decltype(d3),
                decltype(cube_array)>,
            "invalid texture type");
        [[maybe_unused]] const auto no_3d_textures
            = m_api_version == 0x200 && m_api_type == api_type_t::es;
        [[maybe_unused]] const auto no_cube_arrays = false;

        if constexpr(T::value == textures::type::d2)
        {
            return MkShared<texture_2d_t>(
                m_features.texture,
                std::ref(m_texture_decode_queue),
                std::ref(*m_debug),
                T::value,
                data_type,
                mipmaps,
                properties);
        }
#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
        else if constexpr(T::value == textures::type::d2_array)
        {
            if(no_3d_textures)
                return std::shared_ptr<texture_2da_t>();
            return MkShared<texture_2da_t>(
                m_features.texture,
                std::ref(m_texture_decode_queue),
                std::ref(*m_debug),
                T::value,
                data_type,
                mipmaps,
                properties);
        } else if constexpr(T::value == textures::type::d3)
        {
            if(no_3d_textures)
                return std::shared_ptr<texture_3d_t>();
            return MkShared<texture_3d_t>(
                m_features.texture,
                std::ref(m_texture_decode_queue),
                std::ref(*m_debug),
                T::value,
                data_type,
                mipmaps,
                properties);
        }
#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x320
        else if constexpr(T::value == textures::type::cube_array)
        {
            if(no_3d_textures)
                return std::shared_ptr<texture_cube_array_t>();
            return MkShared<texture_cube_array_t>(
                m_features.texture,
                std::ref(m_texture_decode_queue),
                std::ref(*m_debug),
                T::value,
                data_type,
                mipmaps,
                properties);
        }
#endif
#endif
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
    inline auto alloc_query([[maybe_unused]] T type)
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

            if constexpr(compile_info::debug_mode)
            {
                [[maybe_unused]] u32 current_read = 0, current_draw = 0;
                current_read = m_rendertargetCurrency.read;
                current_draw = m_rendertargetCurrency.draw;

                m_framebuffer->internal_bind(
                    group::framebuffer_target::framebuffer);
                auto status = static_cast<group::framebuffer_status>(
                    cmd::check_framebuffer_status(
                        group::framebuffer_target::framebuffer));
                if(status != group::framebuffer_status::framebuffer_complete)
                    Throw(undefined_behavior("invalid framebuffer"));

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
                if(m_features.rendertarget.readdraw_buffers)
                {
                    m_rendertargetCurrency.update(
                        current_read,
                        group::framebuffer_target::read_framebuffer);
                    m_rendertargetCurrency.update(
                        current_draw,
                        group::framebuffer_target::draw_framebuffer);
                    cmd::bind_framebuffer(
                        group::framebuffer_target::read_framebuffer,
                        current_read);
                    cmd::bind_framebuffer(
                        group::framebuffer_target::draw_framebuffer,
                        current_draw);
                } else
#endif
                {
                    m_rendertargetCurrency.update(
                        current_draw, group::framebuffer_target::framebuffer);
                    cmd::bind_framebuffer(
                        group::framebuffer_target::framebuffer, current_draw);
                }
            }
        }
        return m_framebuffer;
    }

    inline auto& debug()
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(!m_debug)
            m_debug = std::make_unique<debug::api>(std::ref(m_features.debug));
        return *m_debug;
#else
        static debug::null_api nothing;
        return nothing;
#endif
    }

    inline auto& context_robustness()
    {
        return m_context_state;
    }

    inline const auto& feature_info() const
    {
        return m_features;
    }

    template<typename... UList>
    Optional<Tup<error, std::string_view>> submit(
        draw_command const& command, UList&&... uniforms);

    using extensions_set = stl_types::Set<String>;
    struct load_options_t
    {
        Optional<u32>            api_version;
        Optional<api_type_t>     api_type;
        Optional<extensions_set> api_extensions{};
        Optional<features>       api_features{};
        Optional<workarounds>    api_workarounds{};
    };

    static const load_options_t default_options;

    static Tup<features, api_type_t, u32> query_native_api_features(
        extensions_set const& extensions, load_options_t options = default_options);
    static api_type_t     query_native_api();
    static Tup<u32, u32>  query_native_version();
    static Tup<u32, u32>  query_compiled_version();
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

    void collect_info(comp_app::interfaces::AppInfo& appInfo);

    Optional<error> load(load_options_t options = default_options);
    void            unload();

    enum class queues
    {
        main,
        texture_decode,
    };
    template<queues Q>
    auto& queue()
    {
        if constexpr(Q == queues::texture_decode)
            return m_texture_decode_queue;
        if constexpr(Q == queues::main)
            return m_main_queue;
    }

  private:
    static bool supports_extension(
        stl_types::Set<String> const& extensions, String const& ext);
    bool supports_extension(String const& ext);

    stl_types::ShPtr<rendertarget_type> m_framebuffer;
    stl_types::UqPtr<
        std::conditional<debug::api_available, debug::api, debug::null_api>::
            type>
                 m_debug;
    context::api m_context_state;

    rendertarget_currency m_rendertargetCurrency;
    features              m_features;
    api_limits            m_limits;
    workarounds           m_workarounds;
    extensions_set        m_extensions;
    api_type_t            m_api_type{api_type_t::none};
    u32                   m_api_version{0};
    rq::runtime_queue*    m_main_queue{nullptr};
    rq::runtime_queue*    m_texture_decode_queue{nullptr};
};

inline void test_t2d()
{
    api  ap;
    auto a = ap.alloc_texture(
        textures::d2,
        typing::pixels::PixDesc(typing::pixels::PixFmt::RGBA8),
        3);
    a->upload(null_span{}, Veci2{}, Veci2{}, 0);
    a->view(textures::d2, {});
}

inline void test_buffer()
{
    using semantic::RSCA;
    api  ap;
    auto a
        = ap.alloc_buffer(buffers::vertex, RSCA::ReadWrite | RSCA::Persistent);
}

inline void test_query()
{
    api  ap;
    [[maybe_unused]] auto a = ap.alloc_query(queries::time);
}

} // namespace gleam
