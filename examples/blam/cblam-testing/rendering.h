#pragma once

#include "bitmap_cache.h"
#include "blam_files.h"
#include "caching.h"
#include "coffee/graphics/apis/gleam/rhi_buffer.h"
#include "coffee/graphics/apis/gleam/rhi_draw_command.h"
#include "coffee/graphics/apis/gleam/rhi_program.h"
#include "coffee/graphics/apis/gleam/rhi_rendertarget.h"
#include "coffee/graphics/apis/gleam/rhi_texture.h"
#include "coffee/graphics/apis/gleam/rhi_vertex.h"
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

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& e, time_point const&);

    void end_restricted(Proxy& e, time_point const&);

    void load_resources(gfx::system& api, BlamResources& resources);

    std::shared_ptr<gfx::buffer_t>       quad_vbo;
    std::shared_ptr<gfx::vertex_array_t> quad_vao;
    std::shared_ptr<gfx::program_t>      quad_program;
    std::shared_ptr<gfx::sampler_t>      offscreen_sampler;

    static constexpr std::size_t blur_max_levels = 4;
    std::array<std::shared_ptr<gfx::texture_t>, blur_max_levels> blur_chain;
    std::array<std::shared_ptr<gfx::sampler_t>, blur_max_levels>
                                    blur_chain_samplers;
    std::shared_ptr<gfx::program_t> blur_down_program;
    std::shared_ptr<gfx::program_t> blur_up_program;

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
        const Model,
        const MeshTrackingData,
        const DebugDraw,
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

template<typename V>
using LegacyMeshRendererManifest = compo::SubsystemManifest<
    type_list_t<
        const BspReference,
        const SubModel,
        const Model,
        const Visibility,
        const PlayerInfo,
        const PlayerCamera,
        const DrawState>,
    type_list_t<
        BitmapCache<V>,
        const BlamResources,
        gfx::system,
        LoadingStatus,
        ModelCache<V>,
        const RenderingParameters,
        ShaderCache<V>>,
    empty_list_t>;

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
        priority = 801;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void end_restricted(Proxy& e, time_point const& time);

    void load_resources(gfx::system& api);

    void render_ring_texture(gfx::system& api);

    std::shared_ptr<gfx::buffer_t>       quad_vbo;
    std::shared_ptr<gfx::vertex_array_t> quad_vao;
    std::shared_ptr<gfx::program_t>      loading_program;

    std::shared_ptr<gfx::buffer_t>       loading_ebo;
    std::shared_ptr<gfx::buffer_t>       loading_vbo;
    std::shared_ptr<gfx::vertex_array_t> loading_vao;
    std::shared_ptr<gfx::program_t>      ring_program;
    std::shared_ptr<gfx::texture_2d_t>   machine_tex;
    std::shared_ptr<gfx::texture_2d_t>   simplex_noise_tex;
    std::shared_ptr<gfx::sampler_t>      simplex_noise_sampler;

    enum model_name_t
    {
        model_ring_exterior,
        model_ring_interior,
        model_threshold,
    };

    std::map<model_name_t, gfx::draw_command::data_t> loading_draws;
    std::vector<std::shared_ptr<gfx::texture_t>>      loading_textures;

    std::shared_ptr<gfx::texture_2d_t>   ring_color;
    std::shared_ptr<gfx::sampler_t>      ring_color_sampler;
    std::shared_ptr<gfx::rendertarget_t> ring_rt;
    std::shared_ptr<gfx::program_t>      zoom_program;
    bool                                 m_ring_rendered{false};

    std::optional<compo::time_point> loading_screen_gone_time;
    bool                             was_loading{false};
    libc_types::u32                  frames_since_loaded{0};
    static constexpr libc_types::u32 scene_frames_before_fade{2};
};

void alloc_renderer(compo::EntityContainer& container);
