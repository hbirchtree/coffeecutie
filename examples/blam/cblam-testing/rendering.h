#pragma once

#include "blam_files.h"
#include "caching.h"
#include "components.h"
#include "data.h"

#include <coffee/components/subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/image/ktx_load.h>
#include <peripherals/stl/type_list.h>

using ScreenClearManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<gfx::system, BlamResources, PostProcessParameters>,
    type_list_t<comp_app::GraphicsFramebuffer>>;

struct ScreenClear
    : compo::RestrictedSubsystem<ScreenClear, ScreenClearManifest>
{
    using type  = ScreenClear;
    using Proxy = compo::proxy_of<ScreenClearManifest>;

    ScreenClear()
    {
        priority = 899;
    }

    bool main_thread_only() const override { return true; }

    void start_restricted(Proxy& e, time_point const&);

    void end_restricted(Proxy& e, time_point const&);

    void load_resources(gfx::system& api, BlamResources& resources);

    std::shared_ptr<gfx::buffer_t>       quad_vbo;
    std::shared_ptr<gfx::vertex_array_t> quad_vao;
    std::shared_ptr<gfx::program_t>      quad_program;
    std::shared_ptr<gfx::sampler_t>      offscreen_sampler;

    struct screen_quad_t
    {
        Vecf2                         position; /* Position in display coords */
        Vecf2                         size;     /* Size in display coords */
        Vecf2                         atlas_offset{0.f, 0.f};
        Vecf2                         atlas_scale{1.f, 1.f};
        std::weak_ptr<gfx::sampler_t> sampler; /* Sampler to draw on the quad */
    };

    std::vector<screen_quad_t> extra_quads;
};

template<typename V>
struct DrawListBuilder;

template<typename V>
using MeshRendererManifest = compo::SubsystemManifest<
    type_list_t<
        const BspReference,
        const SubModel,
        const DrawState,
        Model,
        const MeshTrackingData,
        DebugDraw,
        const PlayerCamera,
        const PlayerInfo,
        const Visibility>,
    type_list_t<
        const DrawListBuilder<V>,
        BlamFiles<V>,
        BitmapCache<V>,
        ModelCache<V>,
        BSPCache<V>,
        ShaderCache<V>,
        RenderingParameters,
        ScreenClear,
        LoadingStatus,
        DebugMarkers,
        gfx::system>,
    type_list_t<comp_app::GraphicsFramebuffer>>;

using LoadingScreenManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<gfx::system, LoadingStatus, ScreenClear>,
    type_list_t<comp_app::DisplayInfo, comp_app::GraphicsFramebuffer>>;

struct LoadingScreen
    : compo::RestrictedSubsystem<LoadingScreen, LoadingScreenManifest>
{
    using type  = LoadingScreen;
    using Proxy = compo::proxy_of<LoadingScreenManifest>;

    LoadingScreen()
    {
        priority = 1535;
    }

    void end_restricted(Proxy& e, time_point const& time);

    void load_resources(gfx::system& api);

    std::shared_ptr<gfx::buffer_t>       quad_vbo;
    std::shared_ptr<gfx::vertex_array_t> quad_vao;
    std::shared_ptr<gfx::program_t>      loading_program;
    std::shared_ptr<gfx::program_t>      loading_bg_program;
    std::shared_ptr<gfx::texture_2d_t>   loading_tex;
    std::shared_ptr<gfx::sampler_t>      loading_sampler;

    std::optional<compo::time_point> loading_screen_gone_time;
    bool                             was_loading{false};
};

void alloc_renderer(compo::EntityContainer& container);
