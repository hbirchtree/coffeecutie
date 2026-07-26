#pragma once

/* gexxo::api - GameCube (GX) implementation mirroring gleam::api's surface so
 * GLeamBaseTest_RHI can typedef between the two. Milestone: load() brings up GX
 * and default_rendertarget()->clear() clears the screen. submit() is a stub for
 * the user to implement (GX draw differs substantially from GL); program/shader
 * allocation is intentionally out of scope. */

#include "rhi_resources.h"

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

namespace comp_app::interfaces {
struct AppInfo;
}

namespace gexxo {

struct load_options_t
{
};

struct api
{
    using texture_type      = texture_t;
    using buffer_type       = buffer_t;
    using rendertarget_type = rendertarget_t;
    using vertex_type       = vertex_array_t;
    using sampler_type      = sampler_t;
    using query_type        = query_t;
    using draw_command_type = draw_command;

    api() noexcept             = default;
    api(api&&)                 = default;
    api& operator=(api&&)      = default;
    api(api const&)            = delete;
    api& operator=(api const&) = delete;

    /* --- Resource allocation (mirrors gleam::api) ------------------------- */

    template<typename T>
    inline auto alloc_buffer(T, semantic::RSCA access)
    {
        return std::make_shared<buffer_t>(T::value, access);
    }

    template<typename T>
    inline auto alloc_texture(
        T,
        typing::pixels::PixDesc data_type,
        u32                     mipmaps,
        textures::property /*properties*/ = textures::property::none)
    {
        return std::make_shared<texture_2d_t>(T::value, data_type, mipmaps);
    }

    inline auto alloc_vertex_array()
    {
        return std::make_shared<vertex_array_t>();
    }

    template<typename T>
    inline auto alloc_query(T type)
    {
        return std::make_shared<query_t>(type.value);
    }

    inline auto alloc_rendertarget()
    {
        return std::make_shared<rendertarget_t>();
    }

    inline auto alloc_program()
    {
        return std::make_shared<program_t>();
    }

    inline auto default_rendertarget()
    {
        if(!m_framebuffer)
            m_framebuffer = std::make_shared<rendertarget_t>();
        return m_framebuffer;
    }

    /* --- Lifecycle -------------------------------------------------------- */

    std::optional<error> load(load_options_t options = {});
    void                 unload();

    /* --- Introspection (mirrors gleam::api) ------------------------------- */

    static std::tuple<u32, u32> query_native_version();
    static extensions_set       query_native_extensions();
    static api_type_t           query_native_api();

    std::string_view                     api_name();
    api_type_t                           api_type();
    std::tuple<u32, u32>                 api_version();
    std::string                          shaderlang_name();
    api_type_t                           shaderlang_type();
    std::tuple<u32, u32>                 shaderlang_version();
    std::tuple<std::string, std::string> device();
    std::optional<std::string>           device_driver();
    extensions_set                       extensions();

    void collect_info(comp_app::interfaces::AppInfo& appInfo);

    /* --- Draw submission -------------------------------------------------- */

    template<typename... StateList>
    inline std::optional<std::tuple<error, std::string_view>> submit(
        draw_command const& command, StateList&&... state_changes);

  private:
    std::shared_ptr<rendertarget_t> m_framebuffer;

    /* Cache of recorded display lists, keyed by a hash of the (static) draw
     * state. submit() records a list the first time it sees a given draw and
     * replays it thereafter; see detail::hash_draw in rhi_draw.h. */
    std::unordered_map<u64, display_list_t> m_display_lists;
};

} // namespace gexxo

#include "rhi_draw.h"
