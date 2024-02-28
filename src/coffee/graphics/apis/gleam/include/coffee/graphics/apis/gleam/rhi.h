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
using std::optional;
using std::string;
using std::tuple;

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
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 || \
    defined(GL_OES_vertex_array_object)
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif

    template<class T>
    requires Buffer<buffer_t, buffer_slice_t>
    inline auto alloc_buffer(T, semantic::RSCA access)
    {
        return std::make_shared<buffer_t>(
            m_features.buffer,
            m_workarounds,
            std::ref(m_usage),
            T::value,
            access);
    }

    inline auto alloc_program()
    {
        return std::make_shared<program_t>(
            m_features.program, std::ref(*m_debug));
    }

    inline auto alloc_rendertarget()
    {
        return std::make_shared<rendertarget_t>(
            m_features.rendertarget, std::ref(m_rendertargetCurrency));
    }

    template<typename T>
    inline auto alloc_shader(
        T const& data, shader_t::constants_t const& constants = {})
    {
        return std::make_shared<shader_t>(std::ref(data), constants);
    }

    template<typename T>
    inline auto alloc_shader(
        T&& data, shader_t::constants_t const& constants = {})
    {
        return std::make_shared<shader_t>(std::move(data), constants);
    }

    template<typename T>
    inline auto alloc_shader(
        programs::shader_format_t    fmt,
        T&&                          data,
        shader_t::constants_t const& constants  = {},
        std::string_view             entrypoint = detail::default_entrypoint)
    {
        return std::make_shared<shader_t>(
            std::move(data), fmt, constants, entrypoint);
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
        [[maybe_unused]] const auto no_3d_textures =
            m_api_version == 0x200 && m_api_type == api_type_t::es;
        [[maybe_unused]] const auto no_cube_arrays = false;

        if constexpr(T::value == textures::type::d2)
        {
            return std::make_shared<texture_2d_t>(
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
            return std::make_shared<texture_2da_t>(
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
            return std::make_shared<texture_3d_t>(
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
            return std::make_shared<texture_cube_array_t>(
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
        return std::make_shared<vertex_type>(
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
            return std::make_shared<query_t>(m_features.query, T::value);
#else
            return std::make_shared<null_query_t>(m_features.query, T::value);
#endif
        }
#if 0
        if constexpr(T::value == queries::type::time)
            return std::make_shared<query_t>(m_features.query, T::value);
        if constexpr(T::value == queries::type::timestamp)
            return std::make_shared<timestamp_query>(m_features.query, T::value);
#endif
        return nullptr;
    }

    template<class Dummy = void>
    requires RenderTarget<rendertarget_type, texture_type>
    inline auto default_rendertarget()
    {
        if(!m_framebuffer)
        {
            m_framebuffer = std::make_shared<rendertarget_type>(
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
#if GLEAM_RHI_REAL_DEBUG_ENABLED
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

    inline const auto& limits() const
    {
        return m_limits;
    }

    inline const auto& workarounds() const
    {
        return m_workarounds;
    }

    inline gleam::usage& usage()
    {
        return m_usage;
    }

    template<typename... UList>
    optional<tuple<error, std::string_view>> submit(
        draw_command const& command, UList&&... uniforms);

    using extensions_set = std::set<string>;

    struct load_options_t
    {
        optional<u32>                api_version;
        optional<api_type_t>         api_type;
        optional<extensions_set>     api_extensions{};
        optional<features>           api_features{};
        optional<gleam::workarounds> api_workarounds{};
        optional<api_limits>         limits{};
        optional<size_t>             indirect_buffer_size{};
        optional<size_2d<u32>>       resolution{};

        optional<tuple<std::string_view, std::string_view>> device_info{};
        optional<std::string_view>                          version_string{};
        optional<std::string_view> glsl_version_string{};
    };

    static const load_options_t default_options;

    static tuple<features, api_type_t, u32> query_native_api_features(
        extensions_set const& extensions,
        load_options_t        options = default_options);
    static api_type_t      query_native_api();
    static tuple<u32, u32> query_native_version();
    static tuple<u32, u32> query_compiled_version();
    static extensions_set  query_native_extensions();

    STATICINLINE u32 version_tuple_to_u32(tuple<u32, u32> const& ver)
    {
        auto [major, minor] = ver;
        return ((major << 8) & 0xF00) | ((minor << 4) & 0x0F0);
    }

    std::string_view      api_name();
    api_type_t            api_type();
    tuple<u32, u32>       api_version();
    string                shaderlang_name();
    api_type_t            shaderlang_type();
    tuple<u32, u32>       shaderlang_version();
    tuple<string, string> device();
    optional<string>      device_driver();
    extensions_set        extensions();

    void collect_info(comp_app::interfaces::AppInfo& appInfo);

    optional<error> load(load_options_t options = default_options);
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

    using debug_api =
        std::conditional_t<debug::api_available, debug::api, debug::null_api>;

  private:
    static bool supports_extension(
        std::set<string> const& extensions, string const& ext);
    bool        supports_extension(string const& ext);
    static bool supports_render_format(
        features const& features, PixDesc const& fmt);

    std::shared_ptr<rendertarget_type> m_framebuffer;
    std::unique_ptr<debug_api>         m_debug;
    context::api                       m_context_state;

    rendertarget_currency m_rendertargetCurrency;
    features              m_features;
    api_limits            m_limits;
    gleam::workarounds    m_workarounds;
    extensions_set        m_extensions;
    api_type_t            m_api_type{api_type_t::none};
    u32                   m_api_version{0};
    rq::runtime_queue*    m_main_queue{nullptr};
    rq::runtime_queue*    m_texture_decode_queue{nullptr};
    gleam::usage          m_usage{};
#if GLEAM_MAX_VERSION_ES != 0x200
    std::unique_ptr<circular_buffer_t> m_indirect_buffer;
#endif
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
    auto a =
        ap.alloc_buffer(buffers::vertex, RSCA::ReadWrite | RSCA::Persistent);
}

inline void test_query()
{
    api                   ap;
    [[maybe_unused]] auto a = ap.alloc_query(queries::time);
}

} // namespace gleam
