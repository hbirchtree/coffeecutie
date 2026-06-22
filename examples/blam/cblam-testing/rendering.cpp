#include "rendering.h"

#include <blam/volta/blam_bitm.h>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_urls.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_tag_classes.h>
#include <blam/volta/blam_tag_index.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/image/ktx_load.h>
#include <glw/texture_formats.h>
#include <glw/texture_formats_desc.h>
#include <peripherals/constants.h>
#include <peripherals/stl/iterator_slice.h>
#include <peripherals/stl/tuple_hash.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>

#include "caching.h"
#include "caching_item.h"
#include "coffee/graphics/apis/gleam/rhi_texture.h"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include "materials.h"
#include "peripherals/concepts/graphics_api.h"
#include "selected_version.h"

using namespace libc_types::size_literals;
using namespace std::string_view_literals;
using namespace Coffee::resource_literals;

using semantic::RSCA;
using typing::pixels::CompFmt;
using typing::pixels::PixDesc;

namespace detail {

inline std::tuple<pix_fmt, comp_flags> get_bitm_hash(BitmapItem const& bitm)
{
    return std::make_tuple(bitm.image.fmt.pixfmt, bitm.image.fmt.cmpflg);
}

template<typename MapType>
inline void assign_map(MapType& map, BitmapItem const* bitm)
{
    map.atlas_scale  = {0};
    map.atlas_offset = {0};
    map.layer        = -1;

    if(!bitm)
        return;

    map.atlas_scale  = bitm->image.scale;
    map.atlas_offset = bitm->image.offset;
    map.layer        = bitm->image.layer;
}

} // namespace detail

struct alignas(16) PerInstanceData
{
    Matf4 transform;
    i32   bone_base{-1};
    i32   pad[3]{};
};

static_assert(sizeof(PerInstanceData) == 80);

template<typename Version>
struct MeshRenderer
    : compo::RestrictedSubsystem<
          MeshRenderer<Version>,
          MeshRendererManifest<Version>>
{
    using type        = MeshRenderer;
    using Proxy       = compo::proxy_of<MeshRendererManifest<Version>>;
    using draw_data_t = gfx::draw_command::data_t;

    struct Pass
    {
        Pass()
        {
            draws.emplace_back();
        }

        gfx::draw_command                     command;
        std::vector<std::vector<draw_data_t>> draws;

        gfx::buffer_slice_t               material_buffer;
        gfx::buffer_slice_t               transparent_buffer;
        gfx::buffer_slice_t               matrix_buffer;
        Span<materials::shader_data>      material_mapping;
        Span<materials::transparent_data> transparent_mapping;
        Span<PerInstanceData>             matrix_mapping;

        std::string name;

        // Flat world-space centers parallel to draws (across all buckets).
        // Only populated for transparent passes; used for depth sorting.
        std::vector<Vecf3> sort_centers;

        model_tracker_t insert_draw(draw_data_t const& draw)
        {
            size_t num_draws = 0;
            for(auto const& draw : draws.back())
                num_draws += draw.instances.count;

            if(num_draws >= 128 || (num_draws + draw.instances.count) > 128)
                draws.emplace_back();

            auto& bucket_ = draws.back();
            auto  it      = std::find_if(
                bucket_.begin(), bucket_.end(), [&draw](draw_data_t const& d) {
                    return d.elements.offset == draw.elements.offset;
                });
            if(it != bucket_.end())
            {
                return model_tracker_t{
                    .bucket   = static_cast<u16>(draws.size() - 1),
                    .draw     = static_cast<u16>(it - bucket_.begin()),
                    .instance = static_cast<u16>(it->instances.count++),
                    .enabled  = true,
                };
            } else
            {
                bucket_.push_back(draw);
                return model_tracker_t{
                    .bucket   = static_cast<u16>(draws.size() - 1),
                    .draw     = static_cast<u16>(bucket_.size() - 1),
                    .instance = 0,
                    .enabled  = true,
                };
            }
        }

        // Transparent passes: one draw per item (no instancing), records
        // center.
        model_tracker_t insert_sortable(draw_data_t draw, Vecf3 const& center)
        {
            draw.instances.count = 1;
            if(draws.back().size() >= 128)
                draws.emplace_back();
            auto& bucket_ = draws.back();
            bucket_.push_back(draw);
            sort_centers.push_back(center);
            return model_tracker_t{
                .bucket   = static_cast<u16>(draws.size() - 1),
                .draw     = static_cast<u16>(bucket_.size() - 1),
                .instance = 0,
                .enabled  = true,
            };
        }

        // Sort transparent draws back-to-front by distance from cam.
        // Call after update_materials (material slots unchanged by reorder).
        void sort_by_depth(Vecf3 const& cam)
        {
            if(sort_centers.empty())
                return;

            std::vector<std::pair<Vecf3, draw_data_t>> flat;
            flat.reserve(sort_centers.size());
            size_t ci = 0;
            for(auto& bucket : draws)
                for(auto& d : bucket)
                    flat.push_back({sort_centers[ci++], d});

            /* stable: parts at equal distance (e.g. all sky model parts,
             * which share one transform) keep their creation order. */
            std::stable_sort(
                flat.begin(), flat.end(), [&](auto const& a, auto const& b) {
                    return glm::distance2(a.first, cam) >
                           glm::distance2(b.first, cam);
                });

            draws.clear();
            draws.emplace_back();
            sort_centers.clear();
            for(auto& [c, d] : flat)
            {
                if(draws.back().size() >= 128)
                    draws.emplace_back();
                draws.back().push_back(d);
                sort_centers.push_back(c);
            }
        }

        inline void clear()
        {
            draws.clear();
            draws.emplace_back();
            sort_centers.clear();
        }

        inline materials::shader_data& material_of(size_t idx)
        {
            if(idx >= material_mapping.size())
                Throw(std::out_of_range("material index out of range"));
            return material_mapping[idx];
        }

        inline materials::transparent_data& transparent_of(size_t idx)
        {
            if(idx >= transparent_mapping.size())
                Throw(std::out_of_range("transparent index out of range"));
            return transparent_mapping[idx];
        }

        inline size_t required_storage() const
        {
            u32 total = 0;
            for(auto const& bucket : draws)
                for(auto const& draw : bucket)
                    total += draw.instances.count;
            return total * sizeof(materials::shader_data);
        }

        inline size_t required_matrix_storage() const
        {
            return (required_storage() / sizeof(materials::shader_data)) *
                   sizeof(PerInstanceData);
        }

        inline size_t required_transparent_storage() const
        {
            return (required_storage() / sizeof(materials::shader_data)) *
                   sizeof(materials::transparent_data);
        }
    };

    time_point last_update{};

    struct cached_player_t
    {
        u32   seat_idx;
        Matf4 matrix;
        Vecf3 position;
    };

    gfx::api*            m_api;
    BlamResources&       m_resources;
    RenderingParameters& m_render_params;
    int m_render_flags{0x0};

    ShaderCache<Version>& shader_cache;
    BitmapCache<Version>& bitm_cache;
    BSPCache<Version>&    bsp_cache;

    std::vector<cached_player_t> m_players;

    std::array<Pass, Pass_Count> m_bsp;
    std::array<Pass, Pass_Count> m_model;

    const bool supports_splitscreen = !compile_info::platform::is_emscripten;

    std::shared_ptr<gfx::texture_2d_t> meow_tex;
    std::shared_ptr<gfx::sampler_t>    meow_sampler;

    MeshRenderer(
        gfx::api*             api,
        BlamResources&        resources,
        RenderingParameters&  render,
        ShaderCache<Version>& shader_cache,
        BitmapCache<Version>& bitm_cache,
        BSPCache<Version>&    bsp_cache)
        : m_api(api)
        , m_resources(resources)
        , m_render_params(render)
        , shader_cache(shader_cache)
        , bitm_cache(bitm_cache)
        , bsp_cache(bsp_cache)
    {
        this->priority = 3072;

        u32 i = 0;
        for(auto& pass : m_bsp)
        {
            pass.command.program = resources.bsp_pipeline;

            pass.name = fmt::format(
                "BSP::{}", magic_enum::enum_name(static_cast<Passes>(i++)));
        }
        i = 0;
        for(auto& pass : m_model)
        {
            pass.command.program = resources.model_pipeline;

            pass.name = fmt::format(
                "MOD::{}", magic_enum::enum_name(static_cast<Passes>(i++)));
        }
    }

    bool use_legacy_rendering() const
    {
        return m_api->api_version() == std::make_tuple<u32, u32>(2, 0);
    }

    BSPItem const* get_bsp(generation_idx_t bsp)
    {
        if(!bsp.valid())
            return nullptr;
        auto it = bsp_cache.find(bsp);
        if(it == bsp_cache.m_cache.end())
            return nullptr;
        return &it->second;
    }

    size_t align_for_gpu_padding(size_t size) const
    {
        u32 padding = m_api->limits().buffers.ubo_alignment;
        if((size % padding) == 0)
            return size;
        u32 mask           = padding - 1;
        u32 unaligned_size = size & mask;
        u32 added_padding  = padding - unaligned_size;
        return size + added_padding;
    }

    void setup_textures(std::vector<gfx::sampler_definition_t>& samplers)
    {
        samplers.clear();

        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"lightmaps"sv, 4},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGB565))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc1"sv, 0},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC1))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc2"sv, 1},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC2))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc3"sv, 2},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC3))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgb565"sv, 3},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGB565))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_r8"sv, 5},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::R8))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rg8"sv, 6},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RG8))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgba4"sv, 7},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGBA4))
                    .sampler});

        if(m_api->limits().textures.texture_units <= 8)
            return;

        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgba8"sv, 8},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGBA8))
                    .sampler});
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x320
        if(std::get<0>(m_api->api_version()) == 2)
            return;
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_bc1"sv, 9},
                bitm_cache
                    .template get_bucket<gfx::texture_cube_array_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC1),
                        blam::bitm::type_t::tex_cube)
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_rgb565"sv, 10},
                bitm_cache
                    .template get_bucket<gfx::texture_cube_array_t>(
                        PixDesc(pix_fmt::RGB565), blam::bitm::type_t::tex_cube)
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_rgba8"sv, 11},
                bitm_cache
                    .template get_bucket<gfx::texture_cube_array_t>(
                        PixDesc(pix_fmt::RGBA8), blam::bitm::type_t::tex_cube)
                    .sampler});
#endif
        for(auto& sampler : samplers)
        {
            std::get<2>(sampler)->set_lod_bias(m_render_params.tex_res);
        }
    }

    auto get_view_state(u32 player_idx)
    {
        using typing::vector_types::Vecd2;
        using typing::vector_types::Veci4;

        const auto depth = gfx::depth_state{
            .range    = Vecd2{0.0, 1.0},
            .reversed = true,
        };

        if(m_api->default_rendertarget() == m_resources.offscreen)
            return gfx::view_state{
                .depth = gfx::depth_state{
                    .range    = depth.range,
                    .reversed = true,
                }};

        auto view = Veci4{
            0,
            0,
            m_resources.offscreen_size.x,
            m_resources.offscreen_size.y,
        };

        switch(m_players.size())
        {
        case 1:
            view.y = m_resources.offscreen_size.y;
            view.z = m_resources.offscreen_size.x;
            view.w = m_resources.offscreen_size.y;
            break;
        case 2:
            view.y = (m_resources.offscreen_size.y / 2) * (player_idx + 1);
            view.z = m_resources.offscreen_size.x;
            view.w = m_resources.offscreen_size.y / 2;
            break;
        case 3:
        case 4:
            view.x = (m_resources.offscreen_size.x / 2) * (player_idx % 2);
            view.y = (m_resources.offscreen_size.y / 2) * (player_idx / 2 + 1);
            view.z = m_resources.offscreen_size.x / 2;
            view.w = m_resources.offscreen_size.y / 2;
            break;
        }
        view.y = m_resources.offscreen_size.y - view.y;

        return gfx::view_state{
#if !defined(COFFEE_EMSCRIPTEN)
            .view = view,
#endif
            .depth = depth,
        };
    }

    gfx::uniform_pair<const int> get_renderflag_uniform()
    {
        m_render_flags =
            (m_render_params.render_fog ? 0x1 : 0) |
            (m_render_params.render_lightmaps ? 0x2 : 0) |
            (m_render_params.render_reflection ? 0x4 : 0) |
            (m_render_params.render_model_bones ? 0x8 : 0) |
            (m_render_params.only_normals ? 0x10 : 0) |
            (m_render_params.only_normalmaps ? 0x20 : 0) |
            (m_render_params.only_lightmaps ? 0x40 : 0) |
            (m_render_params.only_reflections ? 0x80 : 0) |
            (m_render_params.only_multipurpose ? 0x100 : 0) |
            (m_render_params.only_multipurpose2 ? 0x200 : 0) |
            (m_render_params.only_diffuse ? 0x400 : 0);
        return gfx::uniform_pair{
            {"render_flags"sv, 31},
            semantic::SpanOne<const int>(m_render_flags),
        };
    }

    template<typename... Args>
    void render_pass(Proxy&, u32 idx, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        if(!supports_splitscreen && idx != 0)
            return;
        if(idx >= m_players.size())
            return;

        auto        _ = m_api->debug().scope(pass.name);
        ProfContext __;

        auto const& player = m_players[idx];

        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1},
                semantic::SpanOne<const Matf4>(player.matrix)},
            get_renderflag_uniform());
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne(player.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            },
            get_renderflag_uniform());
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer,
                sizeof(PerInstanceData),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::shader_data),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"BoneMatrices"sv, 3},
                m_resources.bone_matrix_buf->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"TransparentProperties"sv, 4},
                pass.transparent_buffer,
                sizeof(materials::transparent_data),
            });

        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& draw : pass.draws)
        {
            auto res = m_api->submit(
                {
                    .program       = pass.command.program,
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                fragment_u,
                buffers,
                get_view_state(idx),
                samplers,
                std::forward<Args&&>(extra)...);
            if(res)
                cFatal("submit error: {}", std::get<1>(*res));
        }
    }

    template<typename... Args>
    void render_bsp_pass(
        Proxy&, u32 idx, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        if(!supports_splitscreen && idx != 0)
            return;
        if(idx >= m_players.size())
            return;

        auto        _ = m_api->debug().scope(pass.name);
        ProfContext __;

        auto const& player = m_players[idx];

        /* Step 1: Set up shared uniform state + buffers */
        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1}, semantic::SpanOne(player.matrix)},
            get_renderflag_uniform());
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne<const Vecf3>(player.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            },
            get_renderflag_uniform());
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::shader_data),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"TransparentProperties"sv, 4},
                pass.transparent_buffer,
                sizeof(materials::transparent_data),
            });

        /* Step 2: Set up all the textures */
        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& draw : pass.draws)
        {
            /* Step 3: DRAW */
            auto res = m_api->submit(
                {
                    .program       = pass.command.program,
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                fragment_u,
                buffers,
                get_view_state(idx),
                gfx::cull_state{
                    .front_face = true,
                },
                samplers,
                std::forward<Args&&>(extra)...);
            if(res)
                cFatal("submit error: {}", std::get<1>(*res));
        }
    }

    void render_debug_lines(Proxy& e)
    {
        if(m_api->workarounds().bugs.adreno)
            return;

        DebugMarkers* markers;
        e.subsystem(markers);
        if(!markers->available())
            return;

        std::vector<gfx::draw_command::data_t> groups;
        RenderingParameters*                   params;
        e.subsystem(params);

        for(auto& ent : e.select(ObjectBsp))
        {
            if(!params->debug_portals && !params->debug_clusters)
                break;
            auto           ref  = e.template ref<Proxy>(ent);
            BspReference&  bsp_ = ref.template get<BspReference>();
            BSPItem const* bsp  = get_bsp(bsp_.bsp);

            if(params->debug_portals)
            {
                groups.insert(
                    groups.end(), bsp->portals.begin(), bsp->portals.end());
            } else if(params->debug_clusters)
            {
                u32 cur = params->current_bsp_cluster;
                if(cur < bsp->clusters.size())
                {
                    for(auto const& sub : bsp->clusters[cur].sub)
                        if(sub.debug_color_idx < bsp->portals.size())
                            groups.push_back(bsp->portals[sub.debug_color_idx]);
                }
            }
        }
        for(auto& ent : e.template select<DebugDraw>())
        {
            if(!params->debug_markers)
                continue;
            if(!params->debug_triggers && ent.tags & ObjectTriggerVolume)
                continue;
            auto             ref  = e.template ref<Proxy>(ent);
            DebugDraw const& draw = ref.template get<DebugDraw>();
            groups.push_back(draw.data);
            groups.back().instances.offset = draw.color_ptr;
        }

        Matf4 debug_matrix =
            m_players.empty() ? glm::identity<Matf4>() : m_players[0].matrix;
        m_api->submit(
            {
                .program       = m_resources.debug_lines_pipeline,
                .vertices      = m_resources.debug_attr,
                .render_target = m_resources.offscreen,
                .call =
                    gfx::draw_command::call_spec_t{
                        .indexed = false,
                        .mode    = gfx::drawing::primitive::line_strip,
                    },
                .data = groups,
            },
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv, 0}, semantic::SpanOne(debug_matrix)}),
            get_view_state(0));
    }

    void start_restricted(Proxy& p, time_point const& time)
    {
        ProfContext _;

        /* Collect active local players sorted by seat_idx */
        m_players.clear();
        for(auto& entity : p.template select<PlayerCamera>())
        {
            auto* cam  = p.template get<PlayerCamera>(entity.id);
            auto* info = p.template get<PlayerInfo>(entity.id);
            if(!info || info->is_remote() || !cam->is_active())
                continue;
            m_players.push_back({
                .seat_idx = info->seat_idx,
                .matrix   = cam->matrix,
                .position = cam->camera->position,
            });
        }
        std::sort(
            m_players.begin(),
            m_players.end(),
            [](auto const& a, auto const& b) {
                return a.seat_idx < b.seat_idx;
            });

        // Performance is terrible on Emscripten when updating every frame
        // We need a more efficient way to update the buffer in that case
        // Also we don't use this data when rendering using legacy codepath
        // We create our own batching there based on different rules
        bool invalidated = true; //! compile_info::platform::is_emscripten;
        if((time - last_update > std::chrono::seconds(10) || invalidated) &&
            !use_legacy_rendering())
        {
            f32 t = std::fmod(stl_types::Chrono::to_f32(time), 3600.f);
            {
                ModelCache<Version>* model_cache;
                p.subsystem(model_cache);
                model_cache->tick_animations(t);
            }
            generate_draws(p);
            update_materials(p, time);
            last_update = time;
            if(m_api->feature_info().program.buffer_binding)
            {
                m_resources.material_store->unmap();
                m_resources.transparent_store->unmap();
            }
        }

        RenderingParameters const* rendering_props;
        p.subsystem(rendering_props);

        if(rendering_props->debug_clear)
            m_resources.offscreen->clear(
                Vecf4(rendering_props->clear_color, 1.f));
        else
            m_resources.offscreen->clear(Vecf4(0, 0, 0, 1));

        // Check if shaders are compiled
        do {
            LoadingStatus* loading_state;
            p.subsystem(loading_state);

            auto bsp_state = m_resources.bsp_pipeline->check_async_ready();
            auto mod_state = m_resources.model_pipeline
                ? m_resources.model_pipeline->check_async_ready()
                : stl_types::result<bool, gfx::program_t::compile_error_t>(true);

            if(!bsp_state.has_value() || !mod_state.has_value())
                return;
            loading_state->loaded_shaders = bsp_state.value() && mod_state.value() 
                ? LoadingStatus::loaded 
                : LoadingStatus::in_progress;
            loading_state->check_all_loaded(true);
            if(loading_state->loading)
                return;
        } while(false);

        /* to_f32 gives seconds-since-epoch (~1.7e9). At that magnitude
         * float32 precision is ~128 s, so adjacent frames are identical
         * and UV animations freeze. Wrap to a shorter cycle. */
        f32 t = std::fmod(stl_types::Chrono::to_f32(time), 3600.f);

        if(use_legacy_rendering())
        {
            legacy_render(p, t);
            return;
        }

        auto blend_for_pass = [](Passes pass) -> gfx::blend_state {
            switch(pass)
            {
            case Pass_SkyAdditive:
            case Pass_Additive:
                return {.additive = true};
            case Pass_SkyMultiply:
            case Pass_Multiply:
                return {.multiply = true};
            case Pass_Max:
                return {.maximum = true};
            default:
                return {};
            }
        };

        /* Primary player is always the first one (seat_idx == 0) */
        u32 primary_player = 0;

        // Sky passes — drawn first so opaque geometry overwrites them
        // naturally. No depth write; depth test passes everywhere (empty
        // buffer).
        {
            gfx::depth_extended_state sky_depth{.depth_write = false};
            for(i32 pi = Pass_SkyOpaque; pi <= Pass_LastSky; ++pi)
            {
                auto pass  = static_cast<Passes>(pi);
                auto blend = blend_for_pass(pass);
                render_pass(
                    p, primary_player, t, m_model[pass], blend, sky_depth);
            }
        }

        // Opaque world geometry — all players, depth write + stencil write.
        gfx::stencil_state opaque_stencil{
            .depth_pass = gfx::stencil_state::operation_t::write,
            .mask       = 0x1,
            .reference  = 0x1,
        };
        for(i32 pi = Pass_Opaque; pi <= Pass_LastOpaque; ++pi)
        {
            auto pass = static_cast<Passes>(pi);
            for(auto i : stl_types::range<u32>(m_players.size()))
            {
                render_bsp_pass(p, i, t, m_bsp[pass], opaque_stencil);
                render_pass(p, i, t, m_model[pass], opaque_stencil);
            }
        }

        // Special case for 3 players; black out the 4th quadrant
        if(m_players.size() == 3 && !compile_info::platform::is_emscripten)
        {
            ScreenClear* clear;
            p.subsystem(clear);

            while(!meow_tex)
            {
                auto pause_tex =
                    ktx::load_from("textures/meow.0.etc2"_rsc.data());
                if(!pause_tex)
                {
                    cWarning("Failed to load blanking texture");
                    break;
                }
                auto        meow_data = std::move(pause_tex.value());
                auto const& meow_size = meow_data.mips.at(0).size;
                meow_tex              = m_api->alloc_texture(
                    gfx::textures::d2,
                    CompFmt(
                        comp_app::pix_fmt::ETC2, typing::pixels::pix_flags::RG),
                    1);
                meow_tex->alloc(size_3d<u32>{meow_size.x, meow_size.y, 1u});
                meow_tex->upload(
                    meow_data.mips.at(0).data,
                    Veci2{},
                    Veci2{meow_size.x, meow_size.y});
                meow_sampler = meow_tex->sampler();
                meow_sampler->alloc();
                meow_sampler->set_edge_policy(
                    1, typing::WrapPolicy::MirrorClamp);
                meow_tex->set_swizzle(
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::alpha);
                break;
            }

            auto size =
                p.template service<comp_app::GraphicsFramebuffer>()->size();
            clear->extra_quads.push_back({
                .position    = Vecf2(size.w / 2.f, 0),
                .size        = Vecf2(size.w / 2.f, size.h / 2.f),
                .atlas_scale = Vecf2{1.f, -1.f},
                .sampler     = meow_sampler,
            });
        }

        Vecf3 const& cam_pos =
            m_players.empty() ? Vecf3{0} : m_players[primary_player].position;
        for(i32 pi = Pass_LastOpaque + 1; pi < Pass_Count; ++pi)
        {
            m_model[static_cast<Passes>(pi)].sort_by_depth(cam_pos);
            m_bsp[static_cast<Passes>(pi)].sort_by_depth(cam_pos);
        }

        // Transparent world geometry — primary player, no depth write.
        gfx::depth_extended_state transparent_depth{.depth_write = false};
        for(i32 pi = Pass_LastOpaque + 1; pi < Pass_Count; ++pi)
        {
            auto pass  = static_cast<Passes>(pi);
            auto blend = blend_for_pass(pass);
            render_pass(
                p, primary_player, t, m_model[pass], blend, transparent_depth);
            render_bsp_pass(
                p, primary_player, t, m_bsp[pass], blend, transparent_depth);
        }

        render_debug_lines(p);

        m_resources.model_matrix_store->next();
        m_resources.transparent_store->next();
        m_resources.material_store->next();
        m_resources.bone_matrix_buf->next();
    }

    struct LegacyBatch
    {
        gfx::draw_command::call_spec_t         call;
        std::vector<gfx::draw_command::data_t> data;
        std::shared_ptr<gfx::texture_t>        lightmap;
        std::shared_ptr<gfx::sampler_t>        lightmap_sampler;
        std::vector<gfx::texture_t*>           base_map;
        std::shared_ptr<gfx::sampler_t>        base_sampler;
        std::vector<Vecf2>                     base_map_scale;
        std::vector<gfx::texture_t*>           micro_map;
        std::shared_ptr<gfx::sampler_t>        micro_sampler;
        std::vector<Vecf2>                     micro_map_scale;
    };

    void legacy_render(Proxy& p, f32 t)
    {
        using typing::vector_types::Vecd2;

        RenderingParameters const* rendering_props;
        p.subsystem(rendering_props);
        
        ProfContext _;
        /* Batch per lightmap PAGE, not per lightmap tag. One lightmap tag
         * (section.lightmap_) holds many pages, one per lightmap_idx, each
         * a distinct layer/subtexture. Keying by tag collapsed every page
         * onto the last-seen subtexture, so the whole BSP sampled a single
         * page (correct gradient where it happened to fit, garbage —
         * orange checker — elsewhere). The lightmap generation id encodes
         * the specific page. */
        std::map<cache_id_t, LegacyBatch> batches;
        for(auto const& ent : p.select(ObjectBsp))
        {
            auto                ref     = p.template ref<Proxy>(ent);
            BspReference const& bsp_ref = ref.template get<BspReference>();
            if(!bsp_ref.visible)
                continue;
            if(!bsp_ref.shader.valid() || !bsp_ref.lightmap.valid())
                continue;
            ShaderItem const& shader = shader_cache.find(bsp_ref.shader)->second;
            if(shader.tag_class != blam::tag_class_t::senv)
                continue;
            BitmapItem const& lightm = bitm_cache.find(bsp_ref.lightmap)->second;
            blam::shader::shader_env const* senv =
                reinterpret_cast<blam::shader::shader_env const*>(shader.header);
            LegacyBatch& batch = batches[bsp_ref.lightmap.i];
            auto light_bucket = bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    lightm.image.fmt);

            auto setup_texture = [&](
                generation_idx_t ref,
                std::vector<gfx::texture_t*>& array,
                std::shared_ptr<gfx::sampler_t>& sampler)
            { 
                if(!ref.valid())
                {
                    array.push_back(nullptr);
                    return;
                }
                BitmapItem const& bitm = bitm_cache.find(ref)->second;
                auto bucket = bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                        bitm.image.fmt);
                array.push_back(bucket.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(bitm.image.layer).get());
                sampler = bucket.sampler;
            };
            
            // Texture setup
            batch.lightmap = light_bucket.template texture_as<gfx::compat::texture_2da_t>()
                .subtexture(lightm.image.layer);
            batch.lightmap_sampler = light_bucket.sampler;
            
            setup_texture(shader.senv.base_bitm, batch.base_map, batch.base_sampler);
            setup_texture(shader.senv.micro_bitm, batch.micro_map, batch.micro_sampler);

            // Draw call setup
            batch.call = bsp_ref.draw.call;
            batch.data.push_back(bsp_ref.draw.data.front());
        }

        Coffee::Profiler::PushContext("legacy_render: Opaque BSP");
        for(auto const& [_, light_group] : batches)
        {
            Vecf2 base_map_scale{1, 1};
            Vecf2 micro_map_scale{8, 8};
            auto vertex_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv},
                    semantic::SpanOne<const Matf4>(m_players[0].matrix)
                });
            auto fragment_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"base_map_scale"sv},
                    semantic::SpanOne<const Vecf2>(base_map_scale)
                },
                gfx::uniform_pair{
                    {"micro_map_scale"sv},
                    semantic::SpanOne<const Vecf2>(micro_map_scale)
                });
            light_group.lightmap_sampler->rebind(light_group.lightmap);
            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"lightmap"sv, 0},
                    light_group.lightmap_sampler
                });
            auto texture_lists = gfx::make_instance_textures(
                gfx::instance_texture_t{
                    .stage = typing::graphics::ShaderStage::Fragment,
                    .uniform = gfx::uniform_key{"base_map"sv, 1},
                    .sampler = light_group.base_sampler,
                    .textures = light_group.base_map,
                },
                gfx::instance_texture_t{
                    .stage = typing::graphics::ShaderStage::Fragment,
                    .uniform = gfx::uniform_key{"micro_map"sv, 2},
                    .sampler = light_group.micro_sampler,
                    .textures = light_group.micro_map,
                });
            m_api->submit({
                    .program = m_resources.bsp_pipeline,
                    .vertices = m_resources.bsp_attr,
                    .render_target = m_resources.offscreen,
                    .call = gfx::draw_command::call_spec_t{
                        .indexed = true,
                        .mode = gfx::drawing::primitive::triangle,
                    },
                    .data = light_group.data,
                },
                vertex_u,
                fragment_u,
                samplers,
                texture_lists,
                gfx::cull_state{.front_face = true},
                gfx::view_state{
                    .depth = gfx::depth_state{
                        .range    = Vecd2{0.0, 1.0},
                        .reversed = true,
                    },
                });
        }
        Coffee::Profiler::PopContext();

        Coffee::Profiler::PushContext("legacy_render: Water BSP");
        for(auto const& ent : p.select(ObjectBsp))
        {
            auto                ref     = p.template ref<Proxy>(ent);
            BspReference const& bsp_ref = ref.template get<BspReference>();
            /* Don't apply the occluder's per-surface visibility here: large
             * water planes get assigned to a cluster the occluder culls even
             * when the surface is on screen (the seabed under them stays
             * visible). swat is only a handful of surfaces, and the GPU still
             * frustum-culls, so just draw them all. */
            if(!bsp_ref.shader.valid())
                continue;
            auto sh_it = shader_cache.find(bsp_ref.shader);
            if(sh_it == shader_cache.end())
                continue;
            if(sh_it->second.tag_class != blam::tag_class_t::swat)
                continue;
            generation_idx_t base_id = sh_it->second.swat.base;
            if(!base_id.valid())
                continue;
            auto b_it = bitm_cache.find(base_id);
            if(b_it == bitm_cache.end())
                continue;
            BitmapItem const& b = b_it->second;
            auto base_tex =
                bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    b.image.fmt);
            base_tex.sampler->rebind(
                base_tex.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(b.image.layer));

            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"base"sv, 0},
                base_tex.sampler});

            /* Ripple layer (swat bump/ripple map) */
            i32 has_ripple = 0;
            if(auto rid = sh_it->second.swat.bump; rid.valid())
            {
                if(auto r_it = bitm_cache.find(rid); r_it != bitm_cache.end())
                {
                    BitmapItem const& rb = r_it->second;
                    auto rtex = bitm_cache.template get_bucket<
                        gfx::compat::texture_2da_t>(rb.image.fmt);
                    rtex.sampler->rebind(
                        rtex.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(rb.image.layer));
                    samplers.push_back(gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"ripple"sv, 1},
                        rtex.sampler});
                    has_ripple = 1;
                }
            }

            /* Lightmap modulation */
            i32   has_light = 0;
            Vecf2 light_scale{1, 1}, light_offset{0, 0};
            if(bsp_ref.lightmap.valid())
            {
                if(auto l_it = bitm_cache.find(bsp_ref.lightmap);
                   l_it != bitm_cache.end())
                {
                    BitmapItem const& lm = l_it->second;
                    auto ltex = bitm_cache.template get_bucket<
                        gfx::compat::texture_2da_t>(lm.image.fmt);
                    ltex.sampler->rebind(
                        ltex.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(lm.image.layer));
                    samplers.push_back(gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"lightmap"sv, 2},
                        ltex.sampler});
                    light_scale  = lm.image.scale;
                    light_offset = lm.image.offset;
                    has_light    = 1;
                }
            }

            auto vtx_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv},
                    semantic::SpanOne<const Matf4>(m_players[0].matrix)});
            auto frg_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{{"time"sv}, semantic::SpanOne<const f32>(t)},
                gfx::uniform_pair{
                    {"has_ripple"sv}, semantic::SpanOne<const i32>(has_ripple)},
                gfx::uniform_pair{
                    {"has_light"sv}, semantic::SpanOne<const i32>(has_light)},
                gfx::uniform_pair{
                    {"light_scale"sv},
                    semantic::SpanOne<const Vecf2>(light_scale)},
                gfx::uniform_pair{
                    {"light_offset"sv},
                    semantic::SpanOne<const Vecf2>(light_offset)});
            m_api->submit(
                {
                    .program       = m_resources.water_pipeline,
                    .vertices      = m_resources.bsp_attr,
                    .render_target = m_resources.offscreen,
                    .call =
                        gfx::draw_command::call_spec_t{
                            .indexed = true,
                            .mode    = gfx::drawing::primitive::triangle,
                        },
                    .data = bsp_ref.draw.data,
                },
                vtx_u,
                frg_u,
                samplers,
                gfx::blend_state{}, /* standard src-alpha transparency */
                gfx::view_state{
                    .depth = gfx::depth_state{
                        .range    = Vecd2{0.0, 1.0},
                        .reversed = true,
                    },
                });
        }
        Coffee::Profiler::PopContext();

        if(!rendering_props->render_scenery)
            return;

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        Coffee::Profiler::PushContext("legacy_render: Scenery");
        Matf4 const& camera = m_players[0].matrix;
        for(auto const& ent : p.select(ObjectMod2))
        {
            auto            ref = p.template ref<Proxy>(ent);
            SubModel const& sm  = ref.template get<SubModel>();
            Model const&    mod =
                p.template ref<Proxy>(sm.parent).template get<Model>();
            if(!mod.visible || !sm.shader.valid())
                continue;

            auto shader_it = shader_cache.find(sm.shader);
            if(shader_it == shader_cache.end())
                continue;
            ShaderItem const& shitem = shader_it->second;
            auto model_it = model_cache->find(mod.model);
            if(model_it == model_cache->end())
                continue;
            ModelItem<Version> const& mitem = model_it->second;

            auto vtx_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv, 1}, semantic::SpanOne<const Matf4>(camera)},
                gfx::uniform_pair{
                    {"model"sv, 2},
                    semantic::SpanOne<const Matf4>(mod.transform)});

            /* schi/scex (incl. the sky dome) — simplified multi-map combiner. */
            if(shitem.tag_class == blam::tag_class_t::schi ||
               shitem.tag_class == blam::tag_class_t::scex)
            {
                auto const& maps = shitem.tag_class == blam::tag_class_t::schi
                                       ? shitem.schi.maps
                                       : shitem.scex.maps;
                static constexpr std::array<std::string_view, 4> map_names = {
                    {"map0"sv, "map1"sv, "map2"sv, "map3"sv}};
                std::vector<gfx::sampler_definition_t> samplers;
                Vecf2                                  scale0{1, 1};
                i32                                    count = 0;
                for(i32 i = 0; i < 4; i++)
                {
                    if(!maps[i].valid())
                        break; /* chicago maps are contiguous from 0 */
                    auto it = bitm_cache.find(maps[i]);
                    if(it == bitm_cache.end())
                        break;
                    BitmapItem const& b = it->second;
                    auto bkt = bitm_cache.template get_bucket<
                        gfx::compat::texture_2da_t>(b.image.fmt);
                    auto sub =
                        bkt.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(b.image.layer);
                    bkt.sampler->rebind(sub);
                    samplers.push_back(gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {map_names[i], i},
                        bkt.sampler});
                    if(i == 0)
                        scale0 = b.image.scale;
                    count++;
                }
                if(count == 0)
                    continue;
                auto frg_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair{
                        {"map_count"sv, 4},
                        semantic::SpanOne<const i32>(count)},
                    gfx::uniform_pair{
                        {"base_map_scale"sv, 5},
                        semantic::SpanOne<const Vecf2>(scale0)});
                m_api->submit(
                    {
                        .program       = m_resources.chicago_pipeline,
                        .vertices      = m_resources.model_attr,
                        .render_target = m_resources.offscreen,
                        .call =
                            gfx::draw_command::call_spec_t{
                                .indexed = true,
                                .mode = gfx::drawing::primitive::triangle_strip,
                            },
                        .data = sm.draw.data,
                    },
                    vtx_u,
                    frg_u,
                    samplers,
                    gfx::cull_state{.front_face = true},
                    gfx::view_state{
                        .depth = gfx::depth_state{
                            .range          = Vecd2{0.0, 1.0},
                            .reversed       = true,
                            .strict_greater = true,
                        },
                    });
                continue;
            }

            generation_idx_t base_bitm = shitem.soso.base_bitm;
            if(!base_bitm.valid())
                continue;
            auto bitm_it = bitm_cache.find(base_bitm);
            if(bitm_it == bitm_cache.end())
                continue;

            BitmapItem const& base = bitm_it->second;
            auto base_tex =
                bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    base.image.fmt);
            auto base_sub =
                base_tex.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(base.image.layer);
            base_tex.sampler->rebind(base_sub);

            Vecf2 base_scale = base.image.scale * mitem.header->uvscale;
            auto  frg_u      = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"base_map_scale"sv, 3},
                    semantic::SpanOne<const Vecf2>(base_scale)});

            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"diffuse"sv, 0},
                base_tex.sampler});

            m_api->submit(
                {
                    .program       = m_resources.model_pipeline,
                    .vertices      = m_resources.model_attr,
                    .render_target = m_resources.offscreen,
                    .call =
                        gfx::draw_command::call_spec_t{
                            .indexed = true,
                            .mode = gfx::drawing::primitive::triangle_strip,
                        },
                    .data = sm.draw.data,
                },
                vtx_u,
                frg_u,
                samplers,
                gfx::cull_state{.front_face = true},
                gfx::view_state{
                    .depth = gfx::depth_state{
                        .range          = Vecd2{0.0, 1.0},
                        .reversed       = true,
                        .strict_greater = true,
                    },
                });
        }
        Coffee::Profiler::PopContext();
    }

    void end_restricted(Proxy& /*p*/, time_point const& /*time*/)
    {
    }

    void generate_static_draws(Proxy& p, size_t& materials_ptr, size_t& transparent_ptr)
    {
        ProfContext _;
        /* First go through al lthe BSPs, will at the same time count the amount
         * of material instances we need for the BSP passes */
        std::map<Passes, i32> instance_offsets;
        for(Pass& pass : m_bsp)
            pass.clear();

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;

            Pass& wf              = m_bsp[bsp.current_pass];
            i32&  instance_offset = instance_offsets[bsp.current_pass];
            wf.command.vertices   = m_resources.bsp_attr;
            wf.command.call       = {
                      .indexed   = true,
                      .instanced = true,
                      .mode      = gfx::drawing::primitive::triangle,
            };
            bsp.draw.data.front().instances.offset = instance_offset;
            instance_offset += bsp.draw.data.front().instances.count;
            if(bsp.current_pass > Pass_LastOpaque)
                wf.insert_sortable(bsp.draw.data.front(), bsp.sort_center);
            else
                wf.draws[0].push_back(bsp.draw.data.front());
        }

        if(!m_api->feature_info().program.buffer_binding)
            return;

        /* Allocate the material instances from the material pool */
        for(Pass& pass : m_bsp)
        {
            auto transparent_size = align_for_gpu_padding(pass.required_transparent_storage());
            auto material_size    = align_for_gpu_padding(pass.required_storage());
            pass.material_buffer =
                m_resources.material_store->slice(materials_ptr, material_size);
            pass.material_mapping =
                pass.material_buffer.template buffer_cast<materials::shader_data>();
            pass.transparent_buffer =
                m_resources.transparent_store->slice(transparent_ptr, transparent_size);
            pass.transparent_mapping =
                pass.transparent_buffer.template buffer_cast<materials::transparent_data>();
            materials_ptr   += material_size;
            transparent_ptr += transparent_size;
        }

        /* Write the static material information, animations are updated later
         */
        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;
            populate_bsp_material(bsp, bsp.draw.data.front().instances.offset);
        }
    }

    void generate_draws(Proxy& p)
    {
        ProfContext _;

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        size_t materials_ptr   = 0;
        size_t transparent_ptr = 0;
        generate_static_draws(p, materials_ptr, transparent_ptr);

        if(use_legacy_rendering())
            return;

        for(Pass& pass : m_model)
            pass.clear();

        for(compo::Entity const& ent : p.select(ObjectMod2))
        {
            auto              ref    = p.template ref<Proxy>(ent);
            SubModel&         model  = ref.template get<SubModel>();
            MeshTrackingData& track  = ref.template get<MeshTrackingData>();
            auto              parent = p.template ref<Proxy>(model.parent);
            Model&            mod    = parent.template get<Model>();

            if(!mod.visible || (!rendering_params->render_scenery &&
                                (ent.tags & ObjectSkybox) == 0))
            {
                track.model_id = {};
                continue;
            }

            Pass& wf            = m_model[model.current_pass];
            wf.command.vertices = m_resources.model_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::triangle_strip,
            };
            if(model.current_pass > Pass_LastOpaque)
            {
                Vecf3 center = Vecf3(mod.transform[3]);
                track.model_id =
                    wf.insert_sortable(model.draw.data.front(), center);
            } else
                track.model_id = wf.insert_draw(model.draw.data.front());
        }
        Coffee::Profiler::PopContext();

        for(Pass& pass : m_model)
        {
            i32 instance_offset = 0;
            for(auto& bucket : pass.draws)
                for(draw_data_t& draw : bucket)
                {
                    draw.instances.offset = instance_offset;
                    instance_offset += draw.instances.count;
                }
        }

        if(!m_api->feature_info().program.buffer_binding)
            return;

        size_t matrix_ptr = 0;
        for(Pass& pass : m_model)
        {
            auto material_size    = align_for_gpu_padding(pass.required_storage());
            auto matrix_size      = align_for_gpu_padding(pass.required_matrix_storage());
            auto transparent_size = align_for_gpu_padding(pass.required_transparent_storage());
            pass.material_buffer =
                m_resources.material_store->slice(materials_ptr, material_size);
            pass.matrix_buffer =
                m_resources.model_matrix_store->slice(matrix_ptr, matrix_size);
            pass.material_mapping =
                pass.material_buffer.template buffer_cast<materials::shader_data>();
            pass.matrix_mapping =
                pass.matrix_buffer.template buffer_cast<PerInstanceData>();
            pass.transparent_buffer =
                m_resources.transparent_store->slice(transparent_ptr, transparent_size);
            pass.transparent_mapping =
                pass.transparent_buffer.template buffer_cast<materials::transparent_data>();
            matrix_ptr       += matrix_size;
            materials_ptr    += material_size;
            transparent_ptr  += transparent_size;
        }

        // Reset per-frame bone_base so each model uploads once per frame
        for(auto& [id, item] : model_cache->m_cache)
            item.bone_base = -1;
        std::vector<Matf4> bone_upload;
        size_t             bone_write_ptr = 0;

        for(auto& ent : p.select(ObjectMod2))
        {
            if(!rendering_params->render_scenery &&
               (ent.tags & ObjectSkybox) == 0)
                continue;
            auto            ref    = p.template ref<Proxy>(ent);
            SubModel const& smodel = ref.template get<SubModel>();
            Model const&    model =
                p.template ref<Proxy>(smodel.parent).template get<Model>();
            MeshTrackingData const& track =
                ref.template get<MeshTrackingData>();

            if(!track.model_id.enabled)
                continue;

            Pass&              pass = m_model[smodel.current_pass];
            draw_data_t const& draw =
                pass.draws[track.model_id.bucket].at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;

            ModelItem<Version>& cache_item =
                model_cache->find(model.model)->second;
            if(!cache_item.bone_matrices.empty() && cache_item.bone_base < 0)
            {
                cache_item.bone_base = static_cast<i32>(bone_write_ptr);
                bone_write_ptr += cache_item.bone_matrices.size();
                bone_upload.insert(
                    bone_upload.end(),
                    cache_item.bone_matrices.begin(),
                    cache_item.bone_matrices.end());
            }

            auto& pid     = pass.matrix_mapping[instance_id];
            pid.transform = model.transform;
            pid.bone_base = cache_item.bone_base;
            populate_mod2_material(
                smodel,
                cache_item,
                model_context(model.origin_object),
                instance_id);
        }

        if(!bone_upload.empty())
            m_resources.bone_matrix_buf->update(
                0, Span<const Matf4>(bone_upload));

        m_resources.model_matrix_store->unmap();
    }

    void update_materials(Proxy& p, time_point const& time)
    {
        if(!m_api->feature_info().program.buffer_binding)
            return;

        for(Pass& pass : m_bsp)
        {
            pass.material_mapping =
                pass.material_buffer
                    .template buffer_cast<materials::shader_data>();
        }
        for(Pass& pass : m_model)
        {
            pass.material_mapping =
                pass.material_buffer
                    .template buffer_cast<materials::shader_data>();
        }

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        for(auto& ent : p.select(ObjectMod2))
        {
            if(!rendering_params->render_scenery &&
               (ent.tags & ObjectSkybox) == 0)
                continue;
            auto              ref    = p.template ref<Proxy>(ent);
            SubModel&         smodel = ref.template get<SubModel>();
            MeshTrackingData& track  = ref.template get<MeshTrackingData>();
            Pass&             pass   = m_model[smodel.current_pass];
            auto&             bucket = pass.draws[track.model_id.bucket];
            if(bucket.empty())
                continue;
            draw_data_t const& draw = bucket.at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;
            update_animations(
                material_of(smodel, instance_id), smodel.shader, time);
            if(static_cast<size_t>(instance_id) <
               pass.transparent_mapping.size())
                shader_cache.update_transparent_animations(
                    pass.transparent_of(instance_id), smodel.shader, time);
        }

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;

            i32 instance_offset = bsp.draw.data.front().instances.offset;
            update_animations(
                material_of(bsp, instance_offset), bsp.shader, time);
            Pass& pass = m_bsp[bsp.current_pass];
            if(instance_offset >= 0 &&
               static_cast<size_t>(instance_offset) <
                   pass.transparent_mapping.size())
                shader_cache.update_transparent_animations(
                    pass.transparent_of(instance_offset), bsp.shader, time);
        }
    }

    materials::shader_data& material_of(SubModel& sub, size_t i)
    {
        Pass& pass = m_model[sub.current_pass];
        return pass.material_of(i);
    }

    materials::shader_data& material_of(BspReference& bsp, size_t i)
    {
        Pass& pass = m_bsp[bsp.current_pass];
        return pass.material_of(i);
    }

    void populate_bsp_material(BspReference& ref, size_t i = 0)
    {
        Pass&                   pass     = m_bsp[ref.current_pass];
        materials::shader_data& material = pass.material_of(i);
        shader_cache.populate_material(material, ref.shader, Vecf2{1, 1});
        bitm_cache.assign_atlas_data(material.lightmap, ref.lightmap);
        if(material.material.material == materials::id::sotr &&
           i < pass.transparent_mapping.size())
            shader_cache.populate_transparent_material(
                pass.transparent_of(i), ref.shader);
    }

    std::optional<ShaderCache<halo_version>::material_context> model_context(
        blam::tag_t const* tag)
    {
        if(!m_render_params.color_changing)
            return std::nullopt;
        switch(tag->tag_class())
        {
        case blam::tag_class_t::bipd:
        case blam::tag_class_t::vehi:
        case blam::tag_class_t::scen:
            return tag->template data<blam::scn::unit>(shader_cache.magic)
                .value();
        default:
            return std::nullopt;
        }
    }

    void populate_mod2_material(
        SubModel const&                                            sub,
        ModelItem<Version> const&                                  model,
        std::optional<ShaderCache<halo_version>::material_context> context,
        size_t                                                     i = 0)
    {
        Pass&                   pass     = m_model[sub.current_pass];
        materials::shader_data& material = pass.material_of(i);
        shader_cache.populate_material(
            material, sub.shader, model.header->uvscale, context);
        if(material.material.material == materials::id::sotr &&
           i < pass.transparent_mapping.size())
            shader_cache.populate_transparent_material(
                pass.transparent_of(i), sub.shader);
    }

    void update_animations(
        materials::shader_data& material,
        generation_idx_t const& shader,
        time_point const&       time)
    {
        shader_cache.update_uv_animations(material, shader, time);
    }
};

void ScreenClear::start_restricted(Proxy& e, const time_point&)
{
    auto fb = e.subsystem<gfx::system>().default_rendertarget();
    //        fb->clear(Vecf4{Vecf3{0.1f}, 1.f}, 1.0, 0);

    e.subsystem<BlamResources>().offscreen->clear(0.0);
}

void ScreenClear::end_restricted(Proxy& e, const time_point&)
{
    auto& api         = e.subsystem<gfx::system>();
    auto& resources   = e.subsystem<BlamResources>();
    auto& postprocess = e.subsystem<PostProcessParameters>();

    auto _ = api.debug().scope();

    if(!quad_program)
        load_resources(api, e.subsystem<BlamResources>());

    if(api.default_rendertarget() == resources.offscreen)
        return;

    f32 display_scale = postprocess.scale;

    Matf4 transform = glm::scale(
        glm::translate(glm::identity<Matf4>(), Vecf3{-1, -1, 0}),
        Vecf3{2, 2, 1});
    transform = glm::scale(transform, Vecf3{display_scale, display_scale, 1.f});
    Vecf2 uvscale{1.f, 1.f};
    Vecf2 offset{0, 0};

    auto params_v = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Vertex,
        gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)});
    auto params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(postprocess.gamma)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{
            {"exposure"sv}, semantic::SpanOne(postprocess.exposure)});

    // clang-format off
    api.submit(gfx::draw_command{
                    .program = quad_program,
                    .vertices = quad_vao,
                    .call = {
                         .indexed = false,
                         .mode = gfx::drawing::primitive::triangle_fan,
                    },
                    .data = {{ .arrays = { .count = 4, }}},
                },
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"source"sv},
                    offscreen_sampler,
                }),
                params_v,
                std::move(params_f));
    // clang-format on

    comp_app::interfaces::GraphicsFramebuffer* framebuffer =
        e.service<comp_app::GraphicsFramebuffer>();

    Vecf2 item_scale{2.f / framebuffer->size().w, 2.f / framebuffer->size().h};
    f32   one = 1.f;

    params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(one)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{{"exposure"sv}, semantic::SpanOne(one)});

    for(screen_quad_t const& draw : extra_quads)
    {
        if(draw.sampler.expired())
            continue;

        Vecf3 translation(Vecf2(draw.position) * item_scale - 1.f, 0.f);
        Vecf3 scale(
            draw.size.x * item_scale.x, draw.size.y * item_scale.y, 1.f);

        uvscale = draw.atlas_scale;
        offset  = draw.atlas_offset;

        transform = glm::scale(
            glm::translate(glm::identity<Matf4>(), translation), scale);
        // clang-format off
        api.submit(gfx::draw_command{
                    .program = quad_program,
                    .vertices = quad_vao,
                    .call = {
                         .indexed = false,
                         .mode = gfx::drawing::primitive::triangle_fan,
                    },
                    .data = {{ .arrays = { .count = 4 }}},
                },
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"source"sv},
                    draw.sampler.lock()
                }),
                params_v,
                params_f,
                gfx::blend_state{});
        // clang-format on
    }

    extra_quads.clear();
}

void ScreenClear::load_resources(gleam::system& api, BlamResources& resources)
{
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;

    struct vertex_t
    {
        vecb2 pos;
        vecb2 tex;
    };

    quad_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    quad_vbo->alloc();
    std::array<vecb4, 6> vbo = {{
        vecb4{0, 0, 0, 0},
        vecb4{127, 0, 127, 0},
        vecb4{127, 127, 127, 127},
        vecb4{0, 127, 0, 127},

    }};
    quad_vbo->commit(vbo);
    quad_vao = api.alloc_vertex_array();
    quad_vao->alloc();
    quad_vao->add(
        gfx::vertex_attribute::from_member(
            &vertex_t::pos, gfx::vertex_float_type));
    auto tex = gfx::vertex_attribute::from_member(
        &vertex_t::tex, gfx::vertex_float_type);
    tex.index = 1;
    quad_vao->add(tex);
    quad_vao->set_buffer(gfx::buffers::vertex, quad_vbo, 0);
    quad_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    quad_vao->force_attribute_names();

    constexpr std::string_view vertex_shader   = R"(#version 100
precision highp float;
attribute vec2 pos;
attribute vec2 tex;
varying vec2 in_tex;
uniform mat4 transform;
void main()
{
    in_tex = tex;
    gl_Position = transform * vec4(pos.x, pos.y, 0.0, 1.0);
}
)";
    constexpr std::string_view fragment_shader = R"(#version 100
precision highp float;
precision highp sampler2D;
varying vec2 in_tex;
uniform sampler2D source;
uniform float gamma;
uniform float exposure;
uniform vec2 offset;
uniform vec2 scale;
void main()
{
    vec4 color = texture2D(source, offset + in_tex * scale).rgba;
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
    color.rgb = pow(exposure * color.rgb, vec3(1.0 / gamma));
    gl_FragColor = color;
}
)";

    quad_program = api.alloc_program();
    quad_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    quad_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(fragment_shader)));
    if(auto res = quad_program->compile(); res.has_error())
        cDebug("Error compiling quad shader: {0}", res.error());

    if(resources.color)
    {
        offscreen_sampler = resources.color->sampler();
        offscreen_sampler->alloc();
    }
}

void LoadingScreen::start_restricted(Proxy&, const time_point&)
{
}

void LoadingScreen::end_restricted(Proxy& e, const time_point& time)
{
    using namespace std::chrono_literals;

    gfx::system*   api;
    ScreenClear*   screen_clear;
    LoadingStatus* status;
    e.subsystem(api);
    e.subsystem(screen_clear);
    e.subsystem(status);

    if(status->progress < 0)
    {
        if(!was_loading)
            return;
        if(loading_screen_gone_time.has_value() &&
           time > *loading_screen_gone_time)
        {
            was_loading = false;
            loading_screen_gone_time.reset();
            return;
        }
        if(!loading_screen_gone_time.has_value())
            loading_screen_gone_time = time + 2s;
    }

    if(!loading_program)
        load_resources(*api);

    auto _ = api->debug().scope();

    auto screen_aspect =
        e.service<comp_app::GraphicsFramebuffer>()->size().aspect();
    f32 timef = std::chrono::duration_cast<stl_types::Chrono::seconds_f32>(
                    time.time_since_epoch())
                    .count();

    Matf4 transform = glm::rotate(
        glm::scale(
            glm::identity<Matf4>(),
            Vecf3{
                2 / (screen_aspect < 1 ? screen_aspect : 1),
                2 * (screen_aspect > 1 ? screen_aspect : 1),
                1}),
        0.f /*std::fmod(timef, glm::pi<f32>() * 2)*/,
        Vecf3{0, 0, 1});
    Vecf2 bg_offset{0, std::fmod(timef, stl_types::math::pi_f * 4.f)};

    f32 time_left =
        loading_screen_gone_time.has_value()
            ? std::chrono::duration_cast<stl_types::Chrono::seconds_f32>(
                  *loading_screen_gone_time - time)
                  .count()
            : 2.f;
    f32 curtain_fade = time_left / 2.f;

    auto res = api->submit(
        gfx::draw_command{
            .program  = loading_bg_program,
            .vertices = quad_vao,
            .call =
                {
                    .indexed = false,
                    .mode    = gfx::drawing::primitive::triangle_fan,
                },
            .data = {{.arrays = {.count = 34, .offset = 4}}},
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)},
            gfx::uniform_pair{{"tex_offset"sv}, semantic::SpanOne(bg_offset)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"curtain_fade"sv}, semantic::SpanOne(curtain_fade)}),
        gfx::make_sampler_list(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"noise"},
                loading_sampler,
            }),
        gfx::blend_state{});

    if(status->progress < 0)
        return;

    was_loading = true;

    transform = glm::translate(
        // glm::scale(glm::identity<Matf4>(), glm::vec3(0.2f)),
        glm::scale(
            glm::identity<Matf4>(), glm::vec3(0.2f / screen_aspect, 0.2f, 1)),
        glm::vec3(1 / (0.2f / screen_aspect) - 1, -5, 0));
    timef     = std::fmod(timef, 10.f) / 10.f;
    f32 start = timef;
    f32 end   = std::fmod(timef + 0.2f, 1.f);

    res = api->submit(
        gfx::draw_command{
            .program  = loading_program,
            .vertices = quad_vao,
            .call =
                {
                    .indexed = false,
                    .mode    = gfx::drawing::primitive::triangle_fan,
                },
            .data = {{.arrays = {.count = 4}}},
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{{"range_start"sv}, semantic::SpanOne(start)},
            gfx::uniform_pair{{"range_end"sv}, semantic::SpanOne(end)}),
        gfx::blend_state{});
    if(res)
    {
        auto [err, msg] = *res;
        cWarning(
            "Failed to draw loading screen: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }
}

void LoadingScreen::load_resources(gleam::system& api)
{
    using namespace gleam::literals;
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;

    ProfContext _;

    struct vertex_t
    {
        vecb2 pos;
        vecb2 tex;
    };

    quad_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    quad_vbo->alloc();
    std::vector<vecb4> vbo = {
        // Full-screen quad
        vecb4{0, 0, 0, 0},
        vecb4{127, 0, 127, 0},
        vecb4{127, 127, 127, 127},
        vecb4{0, 127, 0, 127},

        // Zip-zoomy loading background
        vecb4{0, 0, 0, 0},
        // The rest of the vertices are generated
    };
    for(auto i : range<>(32))
    {
        f32            x_1 = stl_types::math::pi_f * 2.f * (i / 32.f);
        libc_types::i8 phi = glm::cos(x_1) * 127, theta = glm::sin(x_1) * 127;
        vbo.push_back(vecb4{phi, theta, 127 * (((i % 2) == 0) ? 1 : -1), 127});
    }
    vbo.push_back(vecb4{glm::cos(0) * 127, glm::sin(0) * 127, 127, 127});

    quad_vbo->commit(vbo);
    quad_vao = api.alloc_vertex_array();
    quad_vao->alloc();
    quad_vao->add(
        gfx::vertex_attribute::from_member(
            &vertex_t::pos, gfx::vertex_float_type));
    auto tex = gfx::vertex_attribute::from_member(
        &vertex_t::tex, gfx::vertex_float_type);
    tex.index = 1;
    quad_vao->add(tex);
    quad_vao->set_buffer(gfx::buffers::vertex, quad_vbo, 0);
    quad_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    quad_vao->force_attribute_names();

    // 32x32 noise texture :)
    constexpr std::string_view noise_tex =
        "M,#QAI+#;GJK?(BYBY?(E:'2FJ;7EJ+3@(R]9W.D86V><W^PAY/$H*S=F:76J+3E"
        "E*#1G*C9J+3EO\\O\\K;GJI+#AN,3UP,S]HJ[?A9'\"AI+#FZ?8K[OLEZ/4AY/$D)S-"
        "N\\?XE:'2B97&H*S=JK;GH:W>FJ;7FZ?8EZ/4AI+#?XN\\BI;'A9'\"B97&@(R]B97&"
        ">X>X@X^`F:76LK[OJ+3EIK+CM<'RO,CYL;WNE*#1EZ/4ML+SPL[_J+3EG:G:J+3E"
        "M\\/TG:G:E:'2HZ_@K;GJH*S=EJ+3E*#1E:'2F:76E*#1H*S=E:'2BY?(?XN\\@X^`"
        ">H:W?(BYCYO,J+3EK;GJJ+3EI+#AI[/DI+#ABI;'DI[/ML+SQ-$!K+CIF:76E*#1"
        "D)S-C9G*EJ+3F:76G*C9DY_0DI[/F:76HZ_@L;WNJ[?HL[_PK+CIFJ;7BI;'@8V^"
        "?XN\\AY/$F:76JK;GKKKKG:G:E*#1CIK+AI+#=8&R?XN\\H*S=M\\/TJ+3EF:76A9'\""
        "A)#!DI[/I+#AI;'BIK+CG*C9G:G:I+#ALK[OM\\/TL;WNK[OLL[_PI;'BG:G:AY/$"
        "AY/$DY_0EJ+3G:G:G*C9CIK+C9G*BI;'>H:W<GZO>86VD)S-HZ_@GJK;AI+#<W^P"
        "F*35FZ?8I+#AI+#AK+CII[/DL[_PM,#QL[_PJK;GH*S=H:W>I;'BFJ;7EJ+3AY/$"
        "C)C)DY_0FJ;7I;'BG:G:C)C)AY/$C9G*@8V^A9'\"B)3%G:G:J+3EHJ[?AI+#>X>X"
        "KKKKFZ?8G*C9GZO<I+#AHJ[?J[?HI[/DH*S=I+#AG:G:FJ;7D)S-CYO,FJ;7DY_0"
        "EZ/4D9W.EZ/4H*S=H:W>E:'2B97&C)C)A9'\"E:'2E:'2FZ?8EZ/4D)S-@(R]A9'\""
        "GJK;CIK+EJ+3F:76J+3EIK+CIK+CH:W>FJ;7I;'BHZ_@G*C9BI;'B)3%B97&A)#!"
        "B97&B)3%EZ/4GJK;I+#AH:W>D9W.@8V^?(BYC9G*E*#1E:'2E:'2C9G*@(R]B)3%"
        "H*S=BI;'B97&C9G*G:G:H*S=KKKKKKKKG*C9EJ+3EJ+3G:G:F:76G:G:D9W.C9G*"
        "DI[/EJ+3I[/DF:76EJ+3E*#1EJ+3A9'\"A)#!D9W.E:'2F:76IK+CG:G:E:'2I+#A"
        "IK+CE*#1C9G*C)C)GZO<GJK;J[?HI[/DEZ/4BI;'A)#!BY?(FZ?8J+3EG:G:FJ;7"
        "G*C9HZ_@K;GJCYO,@(R]@X^`D9W.C)C)D9W.E:'2DI[/G*C9J;7FJ;7FHZ_@K[OL"
        "BY?(D)S-F*35BI;'DI[/GJK;LK[OJ;7FD9W.A9'\"<GZO<'RM@8V^C9G*BI;'CYO,"
        "GJK;K;GJKKKKB97&>86VA)#!DY_0EZ/4FJ;7GJK;F:76I;'BJ;7FI+#AJ;7FL;WN"
        "=X.TC9G*DI[/B97&BY?(F:76IK+CFJ;7BY?(CIK+=(\"Q:G:G=(\"Q>H:W@(R]B)3%"
        "DY_0G:G:FJ;7?XN\\?(BYE*#1HZ_@H:W>FZ?8HJ[?G:G:HJ[?G:G:E*#1E:'2G:G:"
        ";'BIBY?(DY_0G:G:G:G:H:W>GZO<EZ/4DY_0FJ;7?8FZ;7FJ;WNL>(2UAY/$CYO,"
        "F:76HZ_@H*S=@HZ_=8&R@8V^CIK+CIK+E*#1H*S=J;7FL;WNJ[?HG:G:E*#1F*35"
        ";GJKE:'2I;'BM<'RK;GJH:W>H:W>HJ[?G:G:EJ+3@8V^>86V=X.T@8V^C)C)F:76"
        "K;GJML+SI;'BA)#!;GJK<'RM?HJ[@(R]C)C)G*C9G*C9K+CIN\\?XL[_PH*S=HZ_@"
        "<'RME*#1K;GJN<7VL+SMI+#AHZ_@GZO<EJ+3C9G*BY?(DY_0D9W.FJ;7EZ/4E:'2"
        "G:G:J;7FEZ/4A9'\"<7VN:W>H>X>X@8V^E*#1GJK;EZ/4J+3EML+SK;GJF*35EZ/4"
        "@(R]F:76K[OLNL;WML+SI[/DKKKKJ+3EF:76DI[/E*#1DY_0C9G*C9G*B97&CYO,"
        "C9G*B97&?8FZ?8FZ?(BY?(BYD)S-D9W.HZ_@GZO<C9G*CYO,FJ;7G*C9CIK+F:76"
        "@8V^C)C)HZ_@IK+CHJ[?G*C9K;GJI[/DCYO,BI;'E*#1C)C)AI+#?XN\\@X^`E*#1"
        "E*#1A9'\"?(BYAI+#C)C)D9W.IK+CI;'BK[OLHZ_@CYO,A9'\"A)#!AY/$A9'\"FZ?8"
        "<7VN@X^`I+#AI[/DHZ_@E*#1EZ/4CYO,A9'\"CYO,I;'BDI[/C)C)@(R]A)#!BI;'"
        "B97&AY/$AY/$FZ?8F*35F*35I[/DK+CIJK;GDY_0@X^`@8V^=8&R=(\"Q<W^P@X^`"
        "7VN<>(2UGJK;J+3EI;'BDI[/BI;'AI+#B97&F:76K+CIEZ/4DI[/BI;'E*#1C)C)"
        "AY/$B)3%D)S-J[?HIK+CG*C9FZ?8I[/DJ[?HD9W.@(R]>H:W<W^P>H:W?8FZAI+#"
        "6&25;7FJC)C)F*35H:W>D)S-B)3%BI;'E*#1F:76IK+CE:'2F:76D)S-DI[/BI;'"
        "BY?(E:'2FZ?8J+3EHZ_@I+#AFJ;7HJ[?LK[OF:76AY/$@(R]A9'\"EJ+3DI[/AI+#"
        "7&B9=8&RBY?(C)C)D)S-CIK+C)C)B)3%D9W.D9W.CIK+AY/$DY_0EZ/4E*#1EZ/4"
        "GJK;J[?HK;GJHZ_@FJ;7GJK;FZ?8HJ[?L;WNG*C9D)S-AI+#BY?(EJ+3D9W.AI+#"
        "<'RMAY/$DY_0DY_0DY_0EZ/4EJ+3E*#1FZ?8D)S-B97&CIK+FZ?8I+#AEJ+3EJ+3"
        "F*35G*C9H*S=D)S-@HZ_AI+#CIK+GZO<IK+CG*C9FZ?8E*#1F:76FJ;7F:76DY_0"
        "A9'\"D9W.GZO<J;7FJ;7FJK;GI[/DEZ/4CYO,B97&F:76I;'BJK;GI[/DD9W.DY_0"
        "EZ/4DI[/E:'2D)S-?XN\\>H:W>(2UA9'\"DI[/FZ?8FZ?8DY_0D)S-EJ+3FZ?8IK+C"
        "EZ/4D9W.D9W.G*C9GZO<G*C9G:G:BY?(B)3%CIK+JK;GIK+CE:'2BY?(>H:W@8V^"
        "B)3%C9G*H:W>J+3EE*#1AY/$=H*S=H*S?XN\\BY?(DI[/B)3%AY/$C)C)F:76GZO<"
        ">H:W?(BYAI+#FJ;7GZO<CYO,CIK+?HJ[?(BYB97&IK+CF*35@HZ_?HJ[=X.TAY/$"
        "CIK+EJ+3JK;GK[OLGZO<B97&=(\"Q<7VN=X.T?HJ[@(R]@8V^CYO,E:'2JK;GJK;G"
        ":'2E=X.TBY?(I+#AIK+CBI;'A)#!@HZ_A9'\"CYO,H*S=DI[/>X>X>(2U?(BYDY_0"
        "EZ/4HZ_@I[/DI[/DHJ[?DI[/?8FZ<7VN;WNL=8&R>H:WAI+#EZ/4GJK;L;WNO,CY"
        "=8&R?XN\\A)#!CYO,D)S-@8V^AY/$DI[/E:'2E:'2C)C)@(R]>86V>86V@HZ_G*C9"
        "HJ[?K+CII[/DH*S=H:W>DY_0@X^`>86V>X>X@X^`BI;'E*#1EZ/4I+#AK[OLN\\?X"
        "BY?(CIK+BY?(BY?(BY?(?XN\\@X^`C)C)DI[/CIK+@HZ_AI+#D9W.CIK+A9'\"DI[/"
        "HZ_@K;GJI[/DEZ/4CYO,BI;'?HJ[?HJ[AY/$C)C)C)C)EJ+3CYO,F*35FJ;7GJK;"
        "@X^`B)3%A)#!AI+#@(R]?(BYAY/$B)3%@HZ_@8V^BI;'G:G:K+CIJ;7FDI[/CIK+"
        "G*C9F:76DI[/C)C)A)#!AI+#@X^`BY?(G:G:H:W>DI[/CIK+C)C)GZO<FZ?8G*C9"
        ";GJK>86V?8FZ?XN\\?HJ[@(R]CYO,CIK+?XN\\@X^`FZ?8JK;GK[OLK[OLEZ/4E*#1"
        "J+3EGZO<EZ/4FJ;7@(R]<GZO<7VN>(2UA9'\"C9G*CIK+D9W.CYO,G*C9GZO<HZ_@"
        "6&25;'BI@HZ_?(BY>(2U@(R]G*C9E:'2AI+#D9W.N,3UP<W^ML+SJ;7FG:G:GZO<"
        "M,#QJK;GG:G:H*S=?XN\\:W>H<7VN>X>X?HJ[C)C)GZO<IK+CH*S=H:W>HJ[?H*S="
        "3UN,8V^@>H:W;7FJ9G*C=H*SF:76H:W>E:'2FJ;7P,S]OLK[I;'BF:76FZ?8F:76"
        "J[?HJ+3EHZ_@IK+CA9'\"=H*SBI;'EZ/4B)3%A)#!F*35J;7FH*S=I;'BH*S=D9W.";

    constexpr std::string_view vertex_shader = R"(#version 100
precision highp float;
attribute vec2 pos;
attribute vec2 tex;
varying vec2 in_tex;
varying vec2 in_pos;
uniform mat4 transform;
uniform vec2 tex_offset;
void main()
{
    in_pos = pos.xy;
    in_tex = tex - (tex_offset / 4.0);
    gl_Position = transform * vec4(pos.x, pos.y, 0.3, 1.0);
}
)";

    constexpr std::string_view fragment_shader = R"(#version 100
precision highp float;

varying vec2 in_tex;
uniform float range_start;

void main()
{
    float dist_from_center = length(in_tex * 1.3 - 0.65);
    if(dist_from_center > 0.6)
        discard;
    highp float alpha = sin(abs(dist_from_center) * 10.0 + 3.34) * 1.4 - 0.4;
    vec2 p1 = in_tex * 1.3 - 0.65;
    float p2_rad = range_start * 31.4;
    vec2 p2 = vec2(sin(p2_rad), cos(p2_rad));
    alpha = clamp(alpha, 0.0, 1.0) * (length(p2 - p1) / 4.0);
    alpha = pow(alpha * 2.5, 4.0);
    gl_FragColor = vec4(vec3(1.0), alpha);
})";

    constexpr std::string_view background_fragment_shader = R"(#version 100
precision highp float;

varying vec2 in_pos;
varying vec2 in_tex;

uniform sampler2D noise;
uniform float curtain_fade;

void main()
{
    float fade = pow(length(abs(in_pos)), 1.2);
    gl_FragColor = vec4(
        vec3(
            length(texture2D(noise, in_tex).rgb * fade) *
            3.0 *
            texture2D(noise, in_tex * vec2(-0.2, 0.05)).rgb
        ),
        curtain_fade
    );
})";

    loading_program = api.alloc_program();
    loading_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    loading_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(fragment_shader)));
    if(auto res = loading_program->compile(); res.has_error())
        cWarning("Error compiling loader shader: {}", res.error());

    loading_bg_program = api.alloc_program();
    loading_bg_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    loading_bg_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                background_fragment_shader)));
    if(auto res = loading_bg_program->compile(); res.has_error())
        cWarning("Error compiling loader bg shader: {}", res.error());

    loading_tex =
        api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::RGB8), 1);
    loading_tex->alloc(size_3d<u32>{32, 32, 1});
    loading_tex->upload(gsl::span(noise_tex), Veci2{}, size_2d<i32>{32, 32});
    loading_sampler = loading_tex->sampler();
    loading_sampler->alloc();
    loading_sampler->set_edge_policy(0, typing::WrapPolicy::MirrorRepeat);
    loading_sampler->set_edge_policy(1, typing::WrapPolicy::MirrorRepeat);
}

void alloc_renderer(EntityContainer& container)
{
    ProfContext _;
    auto& api = container.subsystem_cast<gfx::system>();

    container.register_subsystem_inplace<MeshRenderer<halo_version>>(
        &api,
        std::ref(container.subsystem_cast<BlamResources>()),
        std::ref(container.subsystem_cast<RenderingParameters>()),
        std::ref(container.subsystem_cast<ShaderCache<halo_version>>()),
        std::ref(container.subsystem_cast<BitmapCache<halo_version>>()),
        std::ref(container.subsystem_cast<BSPCache<halo_version>>()));

    container.register_subsystem_inplace<ScreenClear>();
    // TODO: Find out why this fails on OpenGL ES 2.0 emulation
    if(api.api_version() != std::make_tuple<u32, u32>(2, 0))
        container.register_subsystem_inplace<LoadingScreen>();
}
