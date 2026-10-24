#include "h2_scene.h"

#include "data.h"
#include "player_controller.h"
#include "render/cascaded_shadows.h"
#include "world/placement.h"
#include "world/player_spawn.h"
#include "world/world_gpu.h"
#include "resource_creation.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>

#include <array>
#include <chrono>
#include <cmath>
#include <limits>

namespace h2 {

using H2RendererManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo, const Model>,
    compo::type_list_t<
        const H2Scene,
        const H2Visibility,
        const BlamResources,
        const RenderingParameters,
        gfx::system>,
    compo::empty_list_t>;

/* Submits the scene's materials, filtered by the occluder's visible set */
struct H2Renderer : compo::RestrictedSubsystem<H2Renderer, H2RendererManifest>
{
    using Proxy = compo::proxy_of<H2RendererManifest>;
    using type  = H2Renderer;

    H2Renderer(compo::EntityContainer& e)
        : m_container(e)
    {
        /* After MeshRenderer (3072), same slot as the RS2 loader */
        priority = 3080;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const& now)
    {
        f32 const seconds = std::fmod(
            std::chrono::duration<f32>(now.time_since_epoch()).count(), 3600.f);
        m_seconds = seconds;
        auto const& scene = p.template subsystem<H2Scene>();
        auto const& vis   = p.template subsystem<H2Visibility>();
        auto const& debug = p.template subsystem<RenderingParameters>();
        if(!scene.loaded)
            return;
        ensure_gpu();
        if(scene.generation != m_generation)
        {
            m_vao->set_buffer(gfx::buffers::vertex, scene.vertex_buf, 0);
            m_vao->set_buffer(gfx::buffers::element, scene.element_buf);
            m_draws.assign(scene.materials.size(), {});
            m_generation = scene.generation;
        }
        PlayerCamera const* camera{};
        PlayerInfo const*   player{};
        for(auto e : p.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = e.components();
            if(!world::is_local_seat(info))
                continue;
            camera = &cam;
            player = &info;
            break;
        }
        if(!camera)
            return;
        /* View models only with a body: not flying (forge) or in a cutscene */
        m_first_person = player->mode.physics && !camera->cinematic;

        auto& resources = p.template subsystem<BlamResources>();
        ensure_depth_copy(resources);
        Matf4 const inverse_camera = glm::inverse(camera->matrix);
        Vecf2 const viewport{resources.offscreen_size};
        /* The water fan goes around the camera at sea level: the section's
         * planar fog plane when it lies flat, lifted a little so sand at the
         * waterline sinks under it and fades in */
        Vecf2 const water_eye{camera->camera.position};
        f32 const   sea_level = vis.planar_fog.plane.z > .9f && vis.planar_fog.plane.w > -1e8f
                                    ? vis.planar_fog.plane.w / vis.planar_fog.plane.z + .05f
                                    : 0.f;
        Vecf3 const sun_direction = scene.sun_direction;
        Vecf3 const sun_color =
            scene.sky_flares.empty() ? Vecf3{0.f} : scene.sky_flares[0].color;
        Vecf4 const sun_spot{scene.water.sun_spot, scene.water.sun_power};
        {
            Coffee::DProfContext _("H2 instances");
            update_instances(p, scene, *camera);
        }
        {
            Coffee::DProfContext _("H2 shadows");
            draw_shadows(p, scene, vis, debug, *camera, sun_direction);
        }
        static blam_render::CascadedShadowMap::sampling_t const no_shadows{};
        auto const& csm = debug.shadows ? m_shadows.sampling(0) : no_shadows;
        i32 const   render_flags = debug.render_flags();
        i32 const   depth_zero_to_one =
            gleam::detail::depth_zero_to_one(m_api->feature_info().program) ? 1 : 0;
        /* Below the horizon, where the sky model ends */
        resources.offscreen->clear(
            Vecf4(debug.debug_clear ? debug.clear_color : scene.sky_clear, 1.f));
        size_t draws_total = 0, draws_visible = 0;
        /* Sky first (never writes depth), opaque, then water blended over
         * everything */
        std::optional<Coffee::DProfContext> prof{std::in_place, "H2 passes"};
        for(int pass = 0; pass < 4; pass++)
        {
            /* Opaque depth done: what the sea measures its depth against */
            if(pass == 2)
                m_depth_target->copy(
                    *resources.offscreen,
                    {0, 0, static_cast<u32>(viewport.x), static_cast<u32>(viewport.y)},
                    {0, 0},
                    gfx::render_targets::attachment::depth,
                    gfx::render_targets::attachment::depth);
            if(pass == 1 && debug.render_fog)
                draw_background_fog(vis, camera->camera.position, inverse_camera, resources);
            for(auto const& [mi, mat] : stl_types::enumerate(scene.materials))
            {
                /* Sky, opaque, the sea, then everything blended over it */
                int const mat_pass = mat.sky     ? 0
                                     : mat.ocean ? 2
                                     : (mat.water || mat.additive || mat.blended) ? 3
                                                                                  : 1;
                /* Glass, decals: nothing sensible to draw them with yet */
                if(mat_pass != pass || mat.section_draws.empty() ||
                   !mat.texture)
                    continue;
                auto& draws = m_draws[mi];
                draws.clear();
                m_moving_draws.clear();
                m_animated.clear();
                for(auto const& d : mat.section_draws)
                    if(!vis.draw_visible(d.tag, scene.sections) ||
                       (d.tag.placed && !debug.render_scenery))
                        continue;
                    else if(d.tag.object >= 0)
                    {
                        if(auto slot = moving_slot(d.tag.object))
                        {
                            m_moving_draws.push_back(d.cmd);
                            m_moving_draws.back().instances.offset = *slot;
                        }
                    }
                    else if(d.tag.animated >= 0)
                    {
                        if(!d.tag.first_person || m_first_person)
                            m_animated.push_back(d);
                    }
                    else
                        draws.push_back(d.cmd);
                draws_total += mat.section_draws.size();
                draws_visible += draws.size() + m_moving_draws.size() + m_animated.size();
                if(draws.empty() && m_moving_draws.empty() && m_animated.empty())
                    continue;
                i32   skinned   = 0;
                i32 const billboard = mat.billboard ? 1 : 0;
                Vecf2 const water_fan{mat.water_fan ? 1.f : 0.f, sea_level};
                Vecf3 offset = mat.sky ? camera->camera.position : Vecf3{};
                Vecf3 const& eye = camera->camera.position;
                f32   scale  = mat.sky ? scene.sky_scale : 1.f;
                auto  vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"},
                        semantic::SpanOne<const Matf4>(camera->matrix)},
                    gfx::uniform_pair<const i32>{
                        {"skinned"}, semantic::SpanOne<const i32>(skinned)},
                    gfx::uniform_pair<const i32>{
                        {"billboard"}, semantic::SpanOne<const i32>(billboard)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"eye"}, semantic::SpanOne<const Vecf3>(eye)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"water_fan"}, semantic::SpanOne<const Vecf2>(water_fan)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"water_eye"}, semantic::SpanOne<const Vecf2>(water_eye)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"uv_scale"},
                        semantic::SpanOne<const Vecf2>(mat.uv_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
                    gfx::uniform_pair<const f32>{
                        {"model_scale"}, semantic::SpanOne<const f32>(scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail_scale"},
                        semantic::SpanOne<const Vecf2>(mat.detail_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail2_scale"},
                        semantic::SpanOne<const Vecf2>(mat.detail2_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"illum_scale"},
                        semantic::SpanOne<const Vecf2>(mat.illum_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"mask_scale"},
                        semantic::SpanOne<const Vecf2>(mat.mask_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"bump_scale"},
                        semantic::SpanOne<const Vecf2>(mat.bump_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                i32  alpha_test = mat.alpha_test ? 1 : 0;
                i32  bumped     = mat.bump.sampler ? 1 : 0;
                auto frag_u     = std::tuple_cat(gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair<const i32>{
                        {"alpha_test"},
                        semantic::SpanOne<const i32>(alpha_test)},
                    gfx::uniform_pair<const i32>{
                        {"bumped"}, semantic::SpanOne<const i32>(bumped)},
                    gfx::uniform_pair<const i32>{
                        {"render_flags"}, semantic::SpanOne<const i32>(render_flags)},
                    gfx::uniform_pair<const f32>{
                        {"time"}, semantic::SpanOne<const f32>(seconds)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"plasma_colors"},
                        semantic::Span<const Vecf3>(mat.plasma_colors)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"env_facing"},
                        semantic::SpanOne<const Vecf4>(mat.env_facing)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"env_glancing"},
                        semantic::SpanOne<const Vecf4>(mat.env_glancing)},
                    gfx::uniform_pair<Matf4 const>{
                        {"inverse_camera"},
                        semantic::SpanOne<const Matf4>(inverse_camera)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"viewport_size"}, semantic::SpanOne<const Vecf2>(viewport)},
                    gfx::uniform_pair<const i32>{
                        {"depth_zero_to_one"}, semantic::SpanOne<const i32>(depth_zero_to_one)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"sun_direction"}, semantic::SpanOne<const Vecf3>(sun_direction)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"sun_color"}, semantic::SpanOne<const Vecf3>(sun_color)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"sun_spot"}, semantic::SpanOne<const Vecf4>(sun_spot)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"illum_color"},
                        semantic::SpanOne<const Vecf3>(mat.illum_color)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"fog_color"}, semantic::SpanOne<const Vecf4>(vis.fog)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"fog_range"}, semantic::SpanOne<const Vecf2>(vis.fog_range)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"fog_eye"}, semantic::SpanOne<const Vecf3>(eye)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"planar_fog_plane"},
                        semantic::SpanOne<const Vecf4>(vis.planar_fog.plane)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"planar_fog_color"},
                        semantic::SpanOne<const Vecf4>(vis.planar_fog.color)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"planar_fog_opaque"},
                        semantic::SpanOne<const Vecf2>(vis.planar_fog.opaque)}),
                        csm.uniforms());
                auto lm_sampler = scene.lightmap_sampler
                                      ? scene.lightmap_sampler
                                      : m_fallback.white_array_sampler;
                auto samplers = gfx::make_sampler_list(
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"diffuse", 0},
                        mat.sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail", 1},
                        mat.detail.sampler ? mat.detail.sampler : m_fallback.white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail2", 2},
                        mat.detail2.sampler ? mat.detail2.sampler
                                            : m_fallback.white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"illum", 3},
                        mat.illum.sampler ? mat.illum.sampler : m_fallback.white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"lightmaps", 4},
                        lm_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"mask", 5},
                        mat.mask.sampler ? mat.mask.sampler : m_fallback.white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"bump", 6},
                        mat.bump.sampler ? mat.bump.sampler : m_fallback.white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"env", 7},
                        mat.env_sampler ? mat.env_sampler : m_fallback.black_cube_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"scene_depth", 8},
                        m_depth_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"csm_atlas", 9},
                        m_shadows.sampler(),
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"csm_occluder_atlas", 10},
                        m_shadows.occluder_sampler(),
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"csm_occluder_tiles", 11},
                        m_shadows.occluder_tiles_sampler(),
                    });
                /* Solid surfaces only show their front; the sky is seen from
                 * inside, and cut-outs and transparents from both sides */
                bool const cull = !mat.sky && !mat.water && !mat.ocean && !mat.additive &&
                                  !mat.blended && !mat.alpha_test && !mat.plasma;
                bool near   = false;
                auto submit = [&](std::vector<gfx::draw_command::data_t> const& data,
                                  gfx::buffer_list&                             instances) {
                    gfx::draw_command const command{
                        .program       = m_programs[static_cast<size_t>(mat.variant)],
                        .vertices      = m_vao,
                        .render_target = resources.offscreen,
                        .call          = {.indexed = true, .instanced = true},
                        .data          = data,
                    };
                    gfx::view_state view{
                        .view = typing::vector_types::Veci4{
                            0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
                        /* View models in the nearest slice: never inside walls */
                        .depth =
                            gfx::depth_state{
                                .range    = near ? typing::vector_types::Vecd2{first_person_depth, 1.0}
                                                : typing::vector_types::Vecd2{0.0, 1.0},
                                .reversed = true,
                            },
                    };
                    gfx::depth_extended_state depth{
                        .depth_write =
                            !mat.sky && !mat.water && !mat.additive && !mat.blended};
                    gfx::blend_state blend{.additive = mat.additive};
                    /* Only skinned draws pay for the bone palette */
                    auto send = [&](auto&&... extra) {
                        return cull ? m_api->submit(
                                          command,
                                          vert_u,
                                          frag_u,
                                          samplers,
                                          instances,
                                          view,
                                          depth,
                                          blend,
                                          gfx::cull_state{.front_face = false},
                                          extra...)
                                    : m_api->submit(
                                          command,
                                          vert_u,
                                          frag_u,
                                          samplers,
                                          instances,
                                          view,
                                          depth,
                                          blend,
                                          extra...);
                    };
                    auto res = skinned ? send(m_bones_list) : send();
                    if(res.has_value() && !m_draw_error_logged)
                    {
                        cWarning("H2: failed to draw: {}", std::get<1>(*res));
                        m_draw_error_logged = true;
                    }
                };
                if(!draws.empty())
                    submit(draws, m_static_list);
                if(!m_moving_draws.empty())
                    submit(m_moving_draws, m_instance_list);
                skinned = 1;
                std::stable_sort(m_animated.begin(), m_animated.end(), [](auto const& a, auto const& b) {
                    return a.tag.animated < b.tag.animated;
                });
                for(auto it = m_animated.begin(); it != m_animated.end();)
                {
                    auto const j    = static_cast<size_t>(it->tag.animated);
                    u32 const  slot = animated_slot(scene, j);
                    near            = it->tag.first_person;
                    m_single.clear();
                    for(; it != m_animated.end() && it->tag.animated == static_cast<i32>(j); ++it)
                    {
                        m_single.push_back(it->cmd);
                        m_single.back().instances.offset = slot;
                    }
                    m_bones = posed(scene, j);
                    submit(m_single, m_instance_list);
                    near = false;
                }
            }
        }
        prof.emplace("H2 flares");
        draw_flares(scene, *camera, resources);
        if(vis.camera_cluster != m_logged_cluster)
        {
            cDebug("H2: drew {} of {} ranges", draws_visible, draws_total);
            m_logged_cluster = vis.camera_cluster;
        }
    }

  private:
    /* Per-draw object transforms, H2Instances in h2.vert indexed by base
     * instance: 0 = identity, then the moving objects, then the animated */
    void update_instances(Proxy& p, H2Scene const& scene, PlayerCamera const& camera)
    {
        m_instances.assign(1, Matf4(1.f));
        m_moving_valid.assign(scene.moving_objects.size(), 0);
        for(auto const& [i, id] : stl_types::enumerate(scene.moving_objects))
        {
            auto const* model = p.template get<Model>(id);
            m_moving_valid[i] = model != nullptr;
            m_instances.push_back(model ? model->transform : Matf4(1.f));
        }
        for(auto const& object : scene.animated_objects)
            m_instances.push_back(object.transform);
        place_first_person(scene, camera);
        /* A bound range never shorter than the block (WebGL insists) */
        m_instances.resize(m_instances.size() + instance_block, Matf4(1.f));
        auto const bytes = m_instances.size() * sizeof(Matf4);
        m_instance_buf->commit(
            Span<const Matf4>(m_instances.data(), m_instances.size()));
        m_instance_list = gfx::make_buffer_list(gfx::buffer_definition_t{
            typing::graphics::ShaderStage::Vertex,
            {"H2Instances", 0},
            m_instance_buf->slice(0, bytes),
            sizeof(Matf4),
        });
        /* Structure reads the identity in slot 0; unstrided, so its batches
         * keep multi-draw */
        m_static_list = gfx::make_buffer_list(gfx::buffer_definition_t{
            typing::graphics::ShaderStage::Vertex,
            {"H2Instances", 0},
            m_instance_buf->slice(0, bytes),
            0,
        });
        m_posed.assign(scene.animated_objects.size(), 0);
        m_poses.resize(scene.animated_objects.size());
    }

    std::optional<u32> moving_slot(i32 object) const
    {
        auto const i = static_cast<size_t>(object);
        if(object < 0 || i >= m_moving_valid.size() || !m_moving_valid[i])
            return std::nullopt;
        return static_cast<u32>(1 + i);
    }

    /* View models hang from the eye (models +X forward, +Z up; camera -Z
     * forward, +Y up); the body stands with its camera marker there, turned
     * with the view */
    void place_first_person(H2Scene const& scene, PlayerCamera const& camera)
    {
        auto const& fp = scene.first_person;
        /* Where the view matrix was built, between physics ticks */
        Vecf3 const eye = camera.drawn_eye;
        Model       hands{};
        ::place_first_person(camera, eye, Attachment{}, hands);
        for(i32 index : {fp.hands, fp.weapon})
            if(index >= 0)
                m_instances[animated_slot(scene, static_cast<size_t>(index))] = hands.transform;
        if(fp.body < 0)
            return;
        Vecf3 const view = glm::transpose(Matf3(camera.rotation)) * Vecf3(0.f, 0.f, -1.f);
        Vecf3 forward{view.x, view.y, 0.f};
        forward = glm::length(forward) > 1e-4f ? glm::normalize(forward) : Vecf3{1.f, 0.f, 0.f};
        Vecf3 const up{0.f, 0.f, 1.f};
        m_instances[animated_slot(scene, static_cast<size_t>(fp.body))] =
            glm::translate(Matf4(1.f), eye) *
            Matf4(Matf3(forward, glm::cross(up, forward), up)) *
            glm::translate(Matf4(1.f), -fp.eye);
    }

    u32 animated_slot(H2Scene const& scene, size_t j) const
    {
        return static_cast<u32>(1 + scene.moving_objects.size() + j);
    }

    /* An animated object's skin matrices this frame, worked out once */
    std::array<Matf4, 64> const& posed(H2Scene const& scene, size_t j)
    {
        if(!m_posed[j])
        {
            auto const& object = scene.animated_objects[j];
            pose(scene.animations[object.animation], m_seconds);
            m_poses[j] = m_bones;
            m_posed[j] = 1;
        }
        return m_poses[j];
    }

    /* An animation's skin matrices at `seconds`, looping, between frames
     * blended */
    void pose(H2Scene::animation_t const& animation, f32 seconds)
    {
        if(animation.frames == 0)
            return;
        auto const [f0, f1, t] = world::loop_frame(seconds * animation.rate, animation.frames);
        u32 const n            = std::min<u32>(animation.nodes, m_bones.size());
        for(u32 i = 0; i < n; i++)
            m_bones[i] = animation.palette[f0 * animation.nodes + i] * (1.f - t) +
                         animation.palette[f1 * animation.nodes + i] * t;
    }

    /* Moving and animated objects cast into each cascade; the BSP and
     * static scenery have their shadows baked, and only keep the casters'
     * shadows off what they shade (the world map once, tiles near the
     * camera each frame) */
    void draw_shadows(
        Proxy&                     p,
        H2Scene const&             scene,
        H2Visibility const&        vis,
        RenderingParameters const& params,
        PlayerCamera const&        camera,
        Vecf3 const&               to_light)
    {
        using Shadows = blam_render::CascadedShadowMap;
        if(!params.shadows)
            return;
        Shadows::config_t const want{
            .view_cascades       = std::clamp<u32>(params.shadow_cascades, 1, 3),
            .resolution          = std::clamp<u32>(params.shadow_resolution, 256, 4096),
            .occluder_resolution = 4096,
            .occluder_tiles      = true,
            /* The sky's light does not always match the bake's sun: far
             * terrain (Coagulation's cliffs) is left to the lightmap */
            .occluder_reach = 12.f,
        };
        auto const& have = m_shadows.config();
        if(!m_shadows.valid() || have.view_cascades != want.view_cascades ||
           have.resolution != want.resolution)
            m_shadows.alloc(*m_api, want);
        m_shadows.tune(params.shadow_distance, params.shadow_split);
        m_shadows.update_view(
            0,
            Shadows::frustum_from(glm::inverse(camera.matrix), camera.camera.position),
            to_light);

        /* The active section's static draws, world space */
        m_static.clear();
        Vecf3 lo(std::numeric_limits<f32>::max()), hi(std::numeric_limits<f32>::lowest());
        for(auto const& mat : scene.materials)
        {
            /* What the opaque pass draws; cut-outs are left to the bake */
            if(mat.sky || mat.water || mat.ocean || mat.additive || mat.blended ||
               mat.alpha_test || !mat.texture)
                continue;
            for(auto const& d : mat.section_draws)
                if(d.tag.object < 0 && d.tag.animated < 0 && d.bounds.w >= 0.f && 
                   (d.tag.section < 0 || d.tag.section == vis.active))
                {
                    m_static.push_back({d.cmd, d.bounds});
                    lo = glm::min(lo, Vecf3(d.bounds) - d.bounds.w);
                    hi = glm::max(hi, Vecf3(d.bounds) + d.bounds.w);
                }
        }
        if(m_static.empty())
            lo = hi = camera.camera.position;
        Vecf3 const center = (lo + hi) * .5f;
        f32 const   radius = std::max(glm::distance(lo, hi) * .5f, params.shadow_distance);
        m_shadows.update_shared(center, radius, to_light);
        m_shadows.clear();

        auto const _ = m_api->debug().scope("H2 shadows");
        Matf4      light{1.f};
        i32        skinned = 0;
        f32 const  unit    = 1.f;
        Vecf3 const origin{0.f};
        Vecf2 const no_fan{0.f};
        auto       vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair<Matf4 const>{{"camera"}, semantic::SpanOne<const Matf4>(light)},
            gfx::uniform_pair<const f32>{{"model_scale"}, semantic::SpanOne<const f32>(unit)},
            gfx::uniform_pair<Vecf3 const>{
                {"model_offset"}, semantic::SpanOne<const Vecf3>(origin)},
            gfx::uniform_pair<Vecf2 const>{{"water_fan"}, semantic::SpanOne<const Vecf2>(no_fan)},
            gfx::uniform_pair<const i32>{{"skinned"}, semantic::SpanOne<const i32>(skinned)});
        auto depth = [&](std::shared_ptr<gfx::rendertarget_t> const& target,
                         typing::vector_types::Veci4 const&          view,
                         std::vector<gfx::draw_command::data_t> const& data) {
            if(data.empty())
                return;
            gfx::draw_command const command{
                .program       = m_shadow_program,
                .vertices      = m_vao,
                .render_target = target,
                .call          = {.indexed = true, .instanced = true},
                .data          = data,
            };
            gfx::view_state state{.view = view, .depth = gfx::depth_state{}};
            bool const placed = std::any_of(
                data.begin(), data.end(), [](auto const& d) { return d.instances.offset > 0; });
            auto& instances = placed ? m_instance_list : m_static_list;
            if(skinned)
                m_api->submit(command, vertex_u, instances, m_bones_list, state);
            else
                m_api->submit(command, vertex_u, instances, state);
        };

        /* Static geometry: the world map when the sun or section changes,
         * the tiles around the camera every frame */
        skinned = 0;
        if(m_shadows.update_occluder(center, radius, to_light))
        {
            light = m_shadows.occluder_matrix();
            m_draws_scratch.clear();
            for(auto const& d : m_static)
                m_draws_scratch.push_back(d.cmd);
            depth(m_shadows.occluder_target(), m_shadows.occluder_viewport(), m_draws_scratch);
        }
        for(u32 c = 0; c < m_shadows.config().view_cascades; c++)
        {
            u32 const tile = m_shadows.view_tile(0, c);
            light          = m_shadows.light_matrix(tile);
            m_draws_scratch.clear();
            for(auto const& d : m_static)
                if(m_shadows.covers(tile, Vecf3(d.bounds), d.bounds.w))
                    m_draws_scratch.push_back(d.cmd);
            depth(m_shadows.occluder_tiles_target(), m_shadows.viewport(tile), m_draws_scratch);
        }

        /* Casters, each where it stands */
        if(!params.render_scenery)
            return;
        /* The casters this frame, each with its transform and bounds */
        m_casters.clear();
        for(auto const& mat : scene.materials)
        {
            if(mat.sky || mat.water || mat.ocean || mat.additive || mat.blended)
                continue;
            for(auto const& d : mat.section_draws)
            {
                if(d.tag.first_person)
                    continue;
                Matf4 const* model = nullptr;
                caster_t     caster{.cmd = d.cmd};
                if(auto slot = moving_slot(d.tag.object))
                {
                    caster.cmd.instances.offset = *slot;
                    model                        = &m_instances[*slot];
                } else if(d.tag.animated >= 0 &&
                          static_cast<size_t>(d.tag.animated) < scene.animated_objects.size())
                {
                    caster.animated = d.tag.animated;
                    caster.cmd.instances.offset =
                        animated_slot(scene, static_cast<size_t>(d.tag.animated));
                    model = &scene.animated_objects[d.tag.animated].transform;
                } else
                    continue;
                caster.bounds = d.bounds.w >= 0.f ? world::place_sphere(*model, d.bounds)
                                                  : Vecf4(Vecf3((*model)[3]), -1.f);
                m_casters.push_back(caster);
            }
        }
        /* Rigid first, then the skinned grouped by object */
        std::stable_sort(m_casters.begin(), m_casters.end(), [](auto const& a, auto const& b) {
            return a.animated < b.animated;
        });
        m_skinned_from = std::partition_point(
            m_casters.begin(), m_casters.end(), [](auto const& c) { return c.animated < 0; });
        for(u32 tile = 0; tile < m_shadows.tiles(); tile++)
        {
            light = m_shadows.light_matrix(tile);
            /* Rigid casters in one go, skinned ones with their bones */
            skinned = 0;
            m_draws_scratch.clear();
            for(auto it = m_casters.begin(); it != m_skinned_from; ++it)
                if(it->bounds.w < 0.f || m_shadows.covers(tile, Vecf3(it->bounds), it->bounds.w))
                    m_draws_scratch.push_back(it->cmd);
            depth(m_shadows.target(), m_shadows.viewport(tile), m_draws_scratch);
            skinned = 1;
            /* One submit per object: its parts share the bones */
            for(auto it = m_skinned_from; it != m_casters.end();)
            {
                i32 const animated = it->animated;
                m_single.clear();
                for(; it != m_casters.end() && it->animated == animated; ++it)
                    if(it->bounds.w < 0.f || m_shadows.covers(tile, Vecf3(it->bounds), it->bounds.w))
                        m_single.push_back(it->cmd);
                if(m_single.empty())
                    continue;
                m_bones = posed(scene, static_cast<size_t>(animated));
                depth(m_shadows.target(), m_shadows.viewport(tile), m_single);
            }
        }
    }

    /* Background seen through the planar fog volume: nothing drawn there
     * fogs itself */
    void draw_background_fog(
        H2Visibility const&  vis,
        Vecf3 const&         eye,
        Matf4 const&         inverse_camera,
        BlamResources const& resources)
    {
        auto const& fog = vis.planar_fog;
        if(fog.plane.w < -1e8f || fog.color.a <= 0.f)
            return;
        m_api->submit(
            gfx::draw_command{
                .program       = m_fog_program,
                .vertices      = m_fog_vao,
                .render_target = resources.offscreen,
                .call =
                    {
                        .instanced = false,
                        .mode      = gfx::drawing::primitive::triangle,
                    },
                .data = {{.arrays = {.count = 3}}},
            },
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair<Matf4 const>{
                    {"inverse_camera"}, semantic::SpanOne<const Matf4>(inverse_camera)},
                gfx::uniform_pair<Vecf3 const>{
                    {"fog_eye"}, semantic::SpanOne<const Vecf3>(eye)},
                gfx::uniform_pair<Vecf4 const>{
                    {"planar_fog_plane"}, semantic::SpanOne<const Vecf4>(fog.plane)},
                gfx::uniform_pair<Vecf4 const>{
                    {"planar_fog_color"}, semantic::SpanOne<const Vecf4>(fog.color)},
                gfx::uniform_pair<Vecf2 const>{
                    {"planar_fog_opaque"}, semantic::SpanOne<const Vecf2>(fog.opaque)}),
            gfx::view_state{
                .view = typing::vector_types::Veci4{
                    0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
            },
            gfx::blend_state{});
    }

    struct FlareVertex
    {
        Vecf2 position;
        Vecf2 texcoord;
        Vecf4 color;
    };

    /* Sky lights' lens flares; the vertex shader dims them by how much of
     * the light the depth buffer shows covered */
    void draw_flares(
        H2Scene const&      scene,
        PlayerCamera const& camera,
        BlamResources const& resources)
    {
        f32 const aspect = static_cast<f32>(resources.offscreen_size.x) /
                           static_cast<f32>(std::max(1, resources.offscreen_size.y));
        for(auto const& flare : scene.sky_flares)
        {
            Vecf4 const clip = camera.matrix *
                               Vecf4(camera.camera.position + flare.direction * 50.f, 1.f);
            if(clip.w <= 0.f)
                continue;
            Vecf2 const ndc = Vecf2(clip) / clip.w;
            if(std::abs(ndc.x) > 1.f || std::abs(ndc.y) > 1.f)
                continue;
            for(auto const& s : flare.sprites)
            {
                Vecf2 const at   = ndc * (1.f - 2.f * s.position);
                Vecf2 const half = Vecf2{s.radius / aspect, s.radius};
                Vecf4 const tint{s.color, 1.f};
                FlareVertex tl{at + Vecf2{-half.x, half.y}, {0.f, 0.f}, tint};
                FlareVertex tr{at + half, {1.f, 0.f}, tint};
                FlareVertex bl{at - half, {0.f, 1.f}, tint};
                FlareVertex br{at + Vecf2{half.x, -half.y}, {1.f, 1.f}, tint};
                m_flare_vertices.insert(m_flare_vertices.end(), {tl, bl, br, tl, br, tr});
                m_flare_samplers.push_back(s.image.sampler);
                m_flare_light.push_back(ndc * 0.5f + 0.5f);
            }
        }
        if(m_flare_vertices.empty())
            return;
        m_flare_buf->commit(
            Span<const FlareVertex>(m_flare_vertices.data(), m_flare_vertices.size()));
        for(size_t i = 0; i < m_flare_samplers.size(); i++)
            m_api->submit(
                gfx::draw_command{
                    .program       = m_flare_program,
                    .vertices      = m_flare_vao,
                    .render_target = resources.offscreen,
                    .call =
                        {
                            .instanced = false,
                            .mode      = gfx::drawing::primitive::triangle,
                        },
                    .data = {{.arrays = {
                                  .count  = 6,
                                  .offset = static_cast<u32>(i * 6)}}},
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Vecf2 const>{
                        {"light_uv"},
                        semantic::SpanOne<const Vecf2>(m_flare_light[i])}),
                gfx::make_sampler_list(
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"image", 0},
                        m_flare_samplers[i],
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Vertex,
                        {"scene_depth", 1},
                        m_depth_sampler,
                    }),
                gfx::view_state{
                    .view = typing::vector_types::Veci4{
                        0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
                },
                gfx::blend_state{.additive = true});
        m_flare_vertices.clear();
        m_flare_samplers.clear();
        m_flare_light.clear();
    }

    /* The offscreen depth, read by the flares and the water */
    /* What the sea and flares read of the scene's depth: a copy, since
     * sampling the attachment being drawn to is a feedback loop (ANGLE
     * rejects the draw) */
    void ensure_depth_copy(BlamResources const& resources)
    {
        Veci2 const size = resources.offscreen_size;
        if(m_depth_copy && m_depth_copy_size == size)
            return;
        if(!m_depth_target)
        {
            m_depth_target = m_api->alloc_rendertarget();
            m_depth_target->alloc();
        }
        m_depth_copy =
            m_api->alloc_texture(gfx::textures::d2, resources.depth->m_format, 1);
        m_depth_copy->alloc(size_3d<i32>{size.x, size.y}.convert<u32>());
        m_depth_target->attach(gfx::render_targets::attachment::depth, *m_depth_copy, 0);
        m_depth_target->resize({0, 0, size.x, size.y});
        m_depth_sampler = m_depth_copy->sampler();
        m_depth_sampler->alloc();
        m_depth_sampler->set_filtering(
            typing::Filtering::Nearest, typing::Filtering::Nearest);
        m_depth_copy_size = size;
    }

    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;

        m_vao = world::alloc_vertex_layout(api, true);
        m_instance_buf = api.alloc_buffer(
            gfx::buffers::constants, semantic::RSCA::WriteOnly | semantic::RSCA::Streaming);
        m_instance_buf->alloc();

        /* Stand-ins for materials without a usable texture */
        m_fallback.alloc(api, 255);

        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h2_flare",
                .fragment_file = "h2_ui",
                .shader        = m_flare_program,
            });
        m_flare_vao = api.alloc_vertex_array();
        m_flare_vao->alloc();
        m_flare_vao->add(gfx::vertex_attribute{
            .index = 0,
            .value = {.offset = 0, .stride = sizeof(FlareVertex), .count = 2},
        });
        m_flare_vao->add(gfx::vertex_attribute{
            .index = 1,
            .value = {.offset = offsetof(FlareVertex, texcoord), .stride = sizeof(FlareVertex), .count = 2},
        });
        m_flare_vao->add(gfx::vertex_attribute{
            .index = 2,
            .value = {.offset = offsetof(FlareVertex, color), .stride = sizeof(FlareVertex), .count = 4},
        });
        m_flare_vao->set_attribute_names({{"position", 0}, {"texcoord", 1}, {"color", 2}});
        m_flare_buf = api.alloc_buffer(
            gfx::buffers::vertex, semantic::RSCA::WriteOnly | semantic::RSCA::Streaming);
        m_flare_buf->alloc();
        m_flare_vao->set_buffer(gfx::buffers::vertex, m_flare_buf, 0);

        /* The scene's own vertex stage, so skinned objects cast as drawn */
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h2",
                .fragment_file = "shadow_depth",
                .shader        = m_shadow_program,
            });

        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h2_fog",
                .fragment_file = "h2_fog",
                .shader        = m_fog_program,
            });
        m_fog_vao = api.alloc_vertex_array();
        m_fog_vao->alloc();
        m_fog_vao->add(gfx::vertex_attribute{
            .index = 0,
            .value = {.offset = 0, .stride = sizeof(Vecf2), .count = 2},
        });
        m_fog_vao->set_attribute_names({{"position", 0}});
        m_fog_buf = api.alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        m_fog_buf->alloc();
        {
            std::array<Vecf2, 3> const screen{{{-1.f, -1.f}, {3.f, -1.f}, {-1.f, 3.f}}};
            m_fog_buf->commit(Span<const Vecf2>(screen.data(), screen.size()));
        }
        m_fog_vao->set_buffer(gfx::buffers::vertex, m_fog_buf, 0);

        for(size_t i = 0; i < m_programs.size(); i++)
            create_program(
                api,
                shader_pair_t{
                    .vertex_file   = "h2",
                    .fragment_file = variant_files[i],
                    .shader        = m_programs[i],
                });
    }

    compo::EntityContainer& m_container;
    gfx::api*               m_api{nullptr};
    u32                     m_generation{0};
    u32                     m_logged_cluster{std::numeric_limits<u32>::max()};

    std::shared_ptr<gfx::vertex_array_t> m_vao;
    std::array<std::shared_ptr<gfx::program_t>, static_cast<size_t>(Variant::count)>
        m_programs;
    world::FallbackTextures                     m_fallback;
    /* Per material: this frame's visible ranges */
    std::vector<std::vector<gfx::draw_command::data_t>> m_draws;
    std::vector<Material::draw_t>                       m_animated;
    std::array<Matf4, 64>                               m_bones{};
    std::vector<gfx::draw_command::data_t>              m_single;
    bool m_draw_error_logged{false};

    std::shared_ptr<gfx::program_t>              m_flare_program;
    std::shared_ptr<gfx::vertex_array_t>         m_flare_vao;
    std::shared_ptr<gfx::buffer_t>               m_flare_buf;
    std::vector<FlareVertex>                     m_flare_vertices;
    std::vector<std::shared_ptr<gfx::sampler_t>> m_flare_samplers;
    std::vector<Vecf2>                           m_flare_light;
    std::shared_ptr<gfx::rendertarget_t>         m_depth_target;
    std::shared_ptr<gfx::texture_2d_t>           m_depth_copy;
    Veci2                                        m_depth_copy_size{};
    std::shared_ptr<gfx::sampler_t>              m_depth_sampler;

    std::shared_ptr<gfx::program_t>      m_fog_program;
    std::shared_ptr<gfx::vertex_array_t> m_fog_vao;
    std::shared_ptr<gfx::buffer_t>       m_fog_buf;

    blam_render::CascadedShadowMap  m_shadows;
    std::shared_ptr<gfx::program_t> m_shadow_program;
    struct static_draw_t
    {
        gfx::draw_command::data_t cmd;
        Vecf4                     bounds;
    };
    std::vector<static_draw_t>             m_static;
    static constexpr double first_person_depth = world::first_person_depth;
    bool                 m_first_person{false};

    struct caster_t
    {
        gfx::draw_command::data_t cmd;
        Vecf4                     bounds{0.f, 0.f, 0.f, -1.f};
        i32                       animated{-1};
    };
    std::vector<caster_t>                  m_casters;
    std::vector<caster_t>::iterator        m_skinned_from;
    std::vector<gfx::draw_command::data_t> m_moving_draws;
    std::vector<Matf4>                     m_instances;
    std::vector<u8>                        m_moving_valid;
    std::vector<u8>                        m_posed;
    std::vector<std::array<Matf4, 64>>     m_poses;
    std::shared_ptr<gfx::buffer_t>         m_instance_buf;
    static constexpr size_t                instance_block = 64; /*!< H2Instances::data */
    gfx::buffer_list                       m_instance_list;
    gfx::buffer_list                       m_static_list;
    decltype(gfx::make_uniform_list(
        typing::graphics::ShaderStage::Vertex,
        gfx::uniform_pair<Matf4 const>{{"bones"}, semantic::Span<const Matf4>()})) m_bones_list =
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair<Matf4 const>{
                {"bones"}, semantic::Span<const Matf4>(m_bones.data(), m_bones.size())});
    std::vector<gfx::draw_command::data_t> m_draws_scratch;
    f32                                    m_seconds{0.f};
};

void alloc_h2_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H2Renderer>(std::ref(e));
}

} // namespace h2
