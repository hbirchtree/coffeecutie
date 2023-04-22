#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"

template<typename V>
using MeshRendererManifest = Components::SubsystemManifest<
    type_list_t<BspReference, SubModel, Model, MeshTrackingData, DebugDraw>,
    type_list_t<
        BitmapCache<V>,
        ModelCache<V>,
        BSPCache<V>,
        ShaderCache<V>,
        BlamFiles,
        RenderingParameters>,
    empty_list_t>;

using ScreenClearManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<gfx::system, BlamResources, PostProcessParameters>,
    type_list_t<
        comp_app::Windowing,
        comp_app::DisplayInfo,
        comp_app::GraphicsFramebuffer>>;

struct ScreenClear
    : compo::RestrictedSubsystem<ScreenClear, ScreenClearManifest>
{
    using type  = ScreenClear;
    using Proxy = compo::proxy_of<ScreenClearManifest>;

    ScreenClear()
    {
        priority = 1536;
    }

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

void alloc_renderer(compo::EntityContainer& container);
