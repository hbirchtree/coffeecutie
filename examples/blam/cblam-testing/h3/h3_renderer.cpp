#include "h3_scene.h"

#include "data.h"
#include "player_controller.h"
#include "render/cascaded_shadows.h"
#include "resource_creation.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>

namespace h3 {

using H3RendererManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo, const Model>,
    compo::type_list_t<const H3Scene, const BlamResources, const RenderingParameters, gfx::system>,
    compo::empty_list_t>;

/* Submits every material of the scene; no visibility yet */
struct H3Renderer : compo::RestrictedSubsystem<H3Renderer, H3RendererManifest>
{
    using Proxy = compo::proxy_of<H3RendererManifest>;
    using type  = H3Renderer;

    H3Renderer(compo::EntityContainer& e)
        : m_container(e)
    {
        /* Same slot as the Halo 2 renderer; only one of them has a scene */
        priority = 3080;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const& now)
    {
        auto const& scene = p.template subsystem<H3Scene>();
        if(!scene.loaded)
            return;
        ensure_gpu();
        if(scene.generation != m_generation)
        {
            m_vao->set_buffer(gfx::buffers::vertex, scene.vertex_buf, 0);
            m_vao->set_buffer(gfx::buffers::element, scene.element_buf);
            m_generation = scene.generation;
        }
        PlayerCamera const* camera{};
        for(auto e : p.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = e.components();
            if(info.seat_idx != 0)
                continue;
            camera = &cam;
            break;
        }
        if(!camera)
            return;

        auto&        resources = p.template subsystem<BlamResources>();
        /* The debug views and switches of the rendering panel */
        auto const& params     = p.template subsystem<RenderingParameters>();
        i32 const   debug_view = params.only_diffuse      ? 1
                                 : params.only_normals    ? 2
                                 : params.only_normalmaps ? 3
                                 : params.only_lightmaps  ? 4
                                 : params.only_reflections ? 5
                                                           : 0;
        i32 const debug_off = (params.render_fog ? 0 : 1) | (params.render_reflection ? 0 : 2) |
                              (params.render_lightmaps ? 0 : 4);
        i32 light_count = 0;
        f32 const    exposure  = adapt_exposure(scene, camera->camera.position, now);
        auto const   cubes        = nearest_cubemaps(scene, camera->camera.position);
        auto const&  dynamic_cube = cubes.first;
        Vecf3 const  offset{};
        f32 const    scale = 1.f;
        Vecf2 const  unit{1.f, 1.f};
        if(m_start == time_point{})
            m_start = now;
        f32 const seconds = std::chrono::duration<f32>(now - m_start).count();
        render_shadows(p, scene, *camera);
        Model first_person{};
        place_first_person(*camera, camera->camera.position, Attachment{}, first_person);
        auto const& csm_atlas = m_shadows.valid() ? m_shadows.sampler() : m_white_sampler;
        /* Opaque first, then water over a copy of the scene, then alpha
         * blended, then additive */
        for(int pass = 0; pass < 3; pass++)
        {
            if(pass == 1)
                draw_water(scene, resources, *camera, exposure, seconds);
            for(auto const& mat : scene.materials)
            {
                int const mat_pass = mat.additive ? 2 : mat.blended ? 1 : 0;
                if(mat_pass != pass || (mat.draws.empty() && mat.object_draws.empty()) ||
                   !mat.visible || mat.variant == Variant::water)
                    continue;
                bool const terrain = mat.variant == Variant::terrain;
                /* Terrain scales each layer in the fragment program */
                Vecf2 const uv_scale = terrain ? unit : mat.uv_scale;
                Matf4       model_matrix(1.f);
                auto vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"}, semantic::SpanOne<const Matf4>(camera->matrix)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"uv_scale"}, semantic::SpanOne<const Vecf2>(uv_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
                    gfx::uniform_pair<const f32>{
                        {"model_scale"}, semantic::SpanOne<const f32>(scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail_scale"}, semantic::SpanOne<const Vecf2>(mat.detail_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail2_scale"}, semantic::SpanOne<const Vecf2>(mat.detail2_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"illum_scale"}, semantic::SpanOne<const Vecf2>(mat.illum_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"mask_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"bump_scale"}, semantic::SpanOne<const Vecf2>(mat.bump_scale)},
                    gfx::uniform_pair<Matf4 const>{
                        {"model"}, semantic::SpanOne<const Matf4>(model_matrix)});
                i32 alpha_test = !mat.alpha_test ? 0 : mat.alpha_test_map.sampler ? 2 : 1;
                i32 blended    = pass;
                auto view = gfx::view_state{
                    .view = typing::vector_types::Veci4{
                        0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
                    .depth = gfx::depth_state{.range = typing::vector_types::Vecd2{0.0, 1.0}, .reversed = true},
                };
                auto submit_draws = [&](auto& frag_u, auto& samplers, auto const& draws) {
                    return m_api->submit(
                        {
                            .program       = m_programs[static_cast<size_t>(mat.variant)],
                            .vertices      = m_vao,
                            .render_target = resources.offscreen,
                            .call          = {.indexed = true, .instanced = true},
                            .data          = draws,
                        },
                        vert_u,
                        frag_u,
                        samplers,
                        view,
                        gfx::depth_extended_state{.depth_write = pass == 0},
                        gfx::blend_state{.additive = mat.additive});
                };
                static std::vector<ObjectDraws> const no_objects;
                /* The level, then each object where its Model is */
                auto submit = [&](auto& frag_u, auto& samplers) {
                    std::optional<std::tuple<gfx::error, std::string_view>> out;
                    model_matrix = Matf4(1.f);
                    /* The level by the lights nearest the camera, objects by theirs */
                    light_count = pack_lights(p, scene, camera->camera.position);
                    if(!mat.draws.empty())
                        out = submit_draws(frag_u, samplers, mat.draws);
                    for(auto const& object : params.render_scenery ? mat.object_draws : no_objects)
                    {
                        /* In front of everything, so it never reaches into walls */
                        if(object.entity == first_person_entity)
                        {
                            model_matrix     = first_person.transform;
                            light_count      = pack_lights(p, scene, camera->camera.position);
                            view.depth->range = typing::vector_types::Vecd2{first_person_depth, 1.0};
                            if(auto res = submit_draws(frag_u, samplers, object.draws); !out)
                                out = res;
                            view.depth->range = typing::vector_types::Vecd2{0.0, 1.0};
                            continue;
                        }
                        auto const* placed = p.template get<Model>(object.entity);
                        if(!placed)
                            continue;
                        model_matrix = placed->transform;
                        light_count  = pack_lights(p, scene, placed->position);
                        if(auto res = submit_draws(frag_u, samplers, object.draws); !out)
                            out = res;
                    }
                    return out;
                };
                auto or_white = [&](Texture const& t) {
                    return t.sampler ? t.sampler : m_white_sampler;
                };
                auto const& lightprobe = scene.lightprobe_sampler ? scene.lightprobe_sampler
                                                                  : m_no_lightprobe_sampler;
                Vecf3 const eye   = camera->camera.position;
                i32 const   model = static_cast<i32>(mat.model);
                i32 const   black_point = mat.black_point ? 1 : 0;
                using stage_t = typing::graphics::ShaderStage;
                std::optional<std::tuple<gfx::error, std::string_view>> res;
                if(terrain)
                {
                    std::array<Vecf4, 4> layer_scale;
                    Vecf4 enabled{0.f}, detailed{0.f}, base_curve{2.f}, detail_curve{2.f};
                    for(size_t k = 0; k < 4; k++)
                    {
                        layer_scale[k]  = mat.layers[k].scale;
                        enabled[k]      = mat.layers[k].enabled ? 1.f : 0.f;
                        detailed[k]     = mat.layers[k].detailed ? 1.f : 0.f;
                        base_curve[k]   = mat.layers[k].base_curve;
                        detail_curve[k] = mat.layers[k].detail_curve;
                    }
                    auto frag_u = gfx::make_uniform_list(
                        stage_t::Fragment,
                        gfx::uniform_pair<const i32>{
                            {"debug_view"}, semantic::SpanOne<const i32>(debug_view)},
                        gfx::uniform_pair<const i32>{
                            {"debug_off"}, semantic::SpanOne<const i32>(debug_off)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"simple_lights"},
                            semantic::Span<const Vecf4>(m_lights.data(), m_lights.size())},
                        gfx::uniform_pair<const i32>{
                            {"simple_light_count"}, semantic::SpanOne<const i32>(light_count)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"layer_scale"},
                            semantic::Span<const Vecf4>(layer_scale.data(), layer_scale.size())},
                        gfx::uniform_pair<Vecf2 const>{
                            {"blend_scale"}, semantic::SpanOne<const Vecf2>(mat.blend_scale)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"layer_enabled"}, semantic::SpanOne<const Vecf4>(enabled)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"layer_detailed"}, semantic::SpanOne<const Vecf4>(detailed)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"layer_base_curve"}, semantic::SpanOne<const Vecf4>(base_curve)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"layer_detail_curve"}, semantic::SpanOne<const Vecf4>(detail_curve)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"lightprobe_scales"},
                            semantic::Span<const Vecf4>(scene.lightprobe_scales.data(), scene.lightprobe_scales.size())},
                        gfx::uniform_pair<const f32>{
                            {"exposure"}, semantic::SpanOne<const f32>(exposure)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"intensity_scales"},
                            semantic::Span<const Vecf4>(scene.intensity_scales.data(), scene.intensity_scales.size())},
                        gfx::uniform_pair<Matf4 const>{
                            {"csm_matrices"}, semantic::Span<const Matf4>(m_shadow_sampling.matrices)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_texel"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.texel)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_params"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.params)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_tiles"}, semantic::Span<const Vecf4>(m_shadow_sampling.tiles)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_bias"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.bias)},
                        gfx::uniform_pair<Matf4 const>{
                            {"csm_occluder"}, semantic::SpanOne<const Matf4>(m_shadow_sampling.occluder)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_occluder_info"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.occluder_info)},
                        gfx::uniform_pair<Vecf3 const>{
                            {"sun_direction"}, semantic::SpanOne<const Vecf3>(scene.sun_direction)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"atmosphere"},
                            semantic::Span<const Vecf4>(scene.atmosphere.data(), scene.atmosphere.size())},
                        gfx::uniform_pair<Vecf3 const>{
                            {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                    auto const& l = mat.layers;
                    auto samplers = gfx::make_sampler_list(
                        gfx::sampler_definition_t{stage_t::Fragment, {"base_m0", 0}, or_white(l[0].base)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"base_m1", 1}, or_white(l[1].base)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"base_m2", 2}, or_white(l[2].base)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"base_m3", 3}, or_white(l[3].base)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail_m0", 4}, or_white(l[0].detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail_m1", 5}, or_white(l[1].detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail_m2", 6}, or_white(l[2].detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail_m3", 7}, or_white(l[3].detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"blend_map", 8}, or_white(mat.blend)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"lightprobe", 9}, lightprobe},
                        gfx::sampler_definition_t{stage_t::Fragment, {"csm_atlas", 10}, csm_atlas});
                    res = submit(frag_u, samplers);
                } else
                {
                    auto frag_u = gfx::make_uniform_list(
                        stage_t::Fragment,
                        gfx::uniform_pair<const i32>{
                            {"debug_view"}, semantic::SpanOne<const i32>(debug_view)},
                        gfx::uniform_pair<const i32>{
                            {"debug_off"}, semantic::SpanOne<const i32>(debug_off)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"simple_lights"},
                            semantic::Span<const Vecf4>(m_lights.data(), m_lights.size())},
                        gfx::uniform_pair<const i32>{
                            {"simple_light_count"}, semantic::SpanOne<const i32>(light_count)},
                        gfx::uniform_pair<const i32>{
                            {"alpha_test"}, semantic::SpanOne<const i32>(alpha_test)},
                        gfx::uniform_pair<const f32>{
                            {"env_dynamic_blend"}, semantic::SpanOne<const f32>(cubes.blend)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"albedo_tint"}, semantic::SpanOne<const Vecf4>(mat.tint)},
                        gfx::uniform_pair<const i32>{
                            {"blended"}, semantic::SpanOne<const i32>(blended)},
                        gfx::uniform_pair<Vecf3 const>{
                            {"curves"}, semantic::SpanOne<const Vecf3>(mat.curves)},
                        gfx::uniform_pair<const i32>{
                            {"bump_mode"}, semantic::SpanOne<const i32>(mat.bump_mode)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"bump_detail_ratio"},
                            semantic::SpanOne<const Vecf2>(mat.bump_detail_ratio)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"lightprobe_scales"},
                            semantic::Span<const Vecf4>(scene.lightprobe_scales.data(), scene.lightprobe_scales.size())},
                        gfx::uniform_pair<const f32>{
                            {"exposure"}, semantic::SpanOne<const f32>(exposure)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"intensity_scales"},
                            semantic::Span<const Vecf4>(scene.intensity_scales.data(), scene.intensity_scales.size())},
                        gfx::uniform_pair<Matf4 const>{
                            {"csm_matrices"}, semantic::Span<const Matf4>(m_shadow_sampling.matrices)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_texel"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.texel)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_params"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.params)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_tiles"}, semantic::Span<const Vecf4>(m_shadow_sampling.tiles)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_bias"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.bias)},
                        gfx::uniform_pair<Matf4 const>{
                            {"csm_occluder"}, semantic::SpanOne<const Matf4>(m_shadow_sampling.occluder)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"csm_occluder_info"}, semantic::SpanOne<const Vecf4>(m_shadow_sampling.occluder_info)},
                        gfx::uniform_pair<Vecf3 const>{
                            {"sun_direction"}, semantic::SpanOne<const Vecf3>(scene.sun_direction)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"atmosphere"},
                            semantic::Span<const Vecf4>(scene.atmosphere.data(), scene.atmosphere.size())},
                        gfx::uniform_pair<Vecf3 const>{
                            {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)},
                        gfx::uniform_pair<const i32>{
                            {"material_model"}, semantic::SpanOne<const i32>(model)},
                        gfx::uniform_pair<const i32>{
                            {"specular_mask_mode"}, semantic::SpanOne<const i32>(mat.specular_mask)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"spec_coefficients"},
                            semantic::SpanOne<const Vecf4>(mat.specular.coefficients)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"spec_lobe"}, semantic::SpanOne<const Vecf4>(mat.specular.lobe)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"spec_tint0"}, semantic::SpanOne<const Vecf4>(mat.specular.tint0)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"spec_tint1"}, semantic::SpanOne<const Vecf4>(mat.specular.tint1)},
                        gfx::uniform_pair<const i32>{
                            {"self_illum_mode"}, semantic::SpanOne<const i32>(mat.self_illum)},
                        gfx::uniform_pair<const i32>{
                            {"black_point"}, semantic::SpanOne<const i32>(black_point)},
                        gfx::uniform_pair<const i32>{
                            {"env_mode"}, semantic::SpanOne<const i32>(mat.env_mode)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"env_tint"}, semantic::SpanOne<const Vecf4>(mat.env_tint)},
                        gfx::uniform_pair<const f32>{
                            {"env_curve"}, semantic::SpanOne<const f32>(mat.environment_map.curve)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"detail3_ratio"}, semantic::SpanOne<const Vecf2>(mat.detail3_ratio)},
                        gfx::uniform_pair<const f32>{
                            {"detail3_curve"}, semantic::SpanOne<const f32>(mat.detail3_curve)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"illum_color"}, semantic::SpanOne<const Vecf4>(mat.illum_color)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"illum_channels"},
                            semantic::Span<const Vecf4>(mat.illum_channels.data(), 3)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"illum_detail_ratio"},
                            semantic::SpanOne<const Vecf2>(mat.illum_detail_ratio)},
                        gfx::uniform_pair<const f32>{
                            {"illum_curve"}, semantic::SpanOne<const f32>(mat.illum_curve)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"alpha_test_ratio"}, semantic::SpanOne<const Vecf2>(mat.alpha_test_ratio)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"illum_layers"}, semantic::SpanOne<const Vecf4>(mat.illum_layers)},
                        gfx::uniform_pair<Vecf4 const>{
                            {"illum_layer_xform"}, semantic::SpanOne<const Vecf4>(mat.illum_layer_xform)});
                    auto samplers = gfx::make_sampler_list(
                        gfx::sampler_definition_t{stage_t::Fragment, {"diffuse", 0}, or_white(mat.diffuse)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail", 1}, or_white(mat.detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail2", 2}, or_white(mat.detail2)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"bump_map", 3}, or_white(mat.bump)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"bump_detail_map", 4}, or_white(mat.bump_detail)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"alpha_test_map", 5}, or_white(mat.alpha_test_map)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"lightprobe", 9}, lightprobe},
                        gfx::sampler_definition_t{stage_t::Fragment, {"csm_atlas", 7}, csm_atlas},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"specular_mask_map", 10}, or_white(mat.specular_mask_map)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"material_map", 11}, or_white(mat.material_map)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"illum_map", 12}, or_white(mat.illum)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"illum_detail_map", 13}, or_white(mat.illum_detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail3", 14}, or_white(mat.detail3)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"env_dynamic", 15}, dynamic_cube},
                        gfx::sampler_definition_t{stage_t::Fragment, {"env_dynamic2", 6}, cubes.second},
                        gfx::sampler_definition_t{
                            stage_t::Fragment,
                            {"env_map", 16},
                            mat.environment_map.sampler ? mat.environment_map.sampler : m_black_cube_sampler});
                    res = submit(frag_u, samplers);
                }
                if(res.has_value() && !m_draw_error_logged)
                {
                    cWarning("H3: failed to draw: {}", std::get<1>(*res));
                    m_draw_error_logged = true;
                }
            }
        }
    }

  private:
    /* Moving objects cast into the cascades; the level's own shadows, and
     * those of what never moves, are baked into its lightmaps */
    void render_shadows(Proxy& p, H3Scene const& scene, PlayerCamera const& camera)
    {
        auto casts = [](Material const& mat) {
            return mat.visible && !mat.additive && !mat.blended && !mat.object_draws.empty();
        };
        if(!m_shadow_program || std::none_of(scene.materials.begin(), scene.materials.end(), casts))
        {
            m_shadow_sampling = {};
            return;
        }
        if(!m_shadows.valid())
            m_shadows.alloc(*m_api, {.shared_cascades = 0});
        m_shadows.update_view(
            0,
            blam_render::CascadedShadowMap::frustum_from(glm::inverse(camera.matrix), camera.camera.position),
            scene.sun_direction);
        m_shadow_sampling = m_shadows.sampling(0);
        m_shadows.clear();
        Matf4 light(1.f), model(1.f);
        auto  vert_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair<Matf4 const>{{"light", 0}, semantic::SpanOne<const Matf4>(light)},
            gfx::uniform_pair<Matf4 const>{{"model", 4}, semantic::SpanOne<const Matf4>(model)});
        for(u32 c = 0; c < m_shadows.tiles(); c++)
        {
            light = m_shadows.light_matrix(c);
            for(auto const& mat : scene.materials)
            {
                if(!casts(mat))
                    continue;
                for(auto const& object : mat.object_draws)
                {
                    auto const* placed = p.template get<Model>(object.entity);
                    if(!placed)
                        continue;
                    model = placed->transform;
                    m_api->submit(
                        {
                            .program       = m_shadow_program,
                            .vertices      = m_vao,
                            .render_target = m_shadows.target(),
                            .call          = {.indexed = true, .instanced = true},
                            .data          = object.draws,
                        },
                        vert_u,
                        gfx::view_state{.view = m_shadows.viewport(c), .depth = gfx::depth_state{}});
                }
            }
        }
    }

    /* The lights nearest the camera, as simple_lights.fx reads them; lights
     * on moving objects follow their Model */
    i32 pack_lights(Proxy& p, H3Scene const& scene, Vecf3 const& eye)
    {
        struct candidate_t
        {
            DynamicLight const* light;
            Vecf3               position, direction;
            f32                 distance;
        };
        std::vector<candidate_t> near;
        for(auto const& light : scene.lights)
        {
            Vecf3 position = light.position, direction = light.direction;
            if(light.entity)
            {
                auto const* model = p.template get<Model>(light.entity);
                if(!model)
                    continue;
                position  = Vecf3(model->transform * Vecf4(light.position, 1.f));
                direction = glm::normalize(Matf3(model->transform) * light.direction);
            }
            near.push_back({&light, position, direction, glm::distance(position, eye) - light.radius});
        }
        size_t const count = std::min(near.size(), max_lights);
        std::partial_sort(near.begin(), near.begin() + count, near.end(), [](auto const& a, auto const& b) {
            return a.distance < b.distance;
        });
        m_lights.fill(Vecf4(0.f));
        for(size_t i = 0; i < count; i++)
        {
            auto const& [light, position, direction, _] = near[i];
            m_lights[i * 5]     = Vecf4(position, light->size);
            /* Shaded towards the light: the spot faces the other way */
            m_lights[i * 5 + 1] = Vecf4(-direction, light->sphere);
            m_lights[i * 5 + 2] = Vecf4(light->color, light->smooth);
            m_lights[i * 5 + 3] = light->falloff;
            m_lights[i * 5 + 4] = Vecf4(light->radius * light->radius, 0.f, 0.f, 0.f);
        }
        return static_cast<i32>(count);
    }

    /* Water (water_shading.fx) reads the scene behind it: a copy of the
     * colour and of how far away each pixel is, made once per frame */
    void draw_water(
        H3Scene const&       scene,
        BlamResources const& resources,
        PlayerCamera const&  camera,
        f32                  exposure,
        f32                  seconds)
    {
        using stage_t = typing::graphics::ShaderStage;
        bool any      = false;
        for(auto const& mat : scene.materials)
            any = any || (mat.variant == Variant::water && mat.visible && !mat.draws.empty());
        if(!any || !resources.color || !resources.depth)
            return;
        auto const size = resources.offscreen_size;
        if(!m_copy_rt || m_copy_size != size)
        {
            m_copy_size = size;
            m_copy      = m_api->alloc_texture(
                gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA16F), 1);
            m_copy->alloc(size_3d<i32>{size.x, size.y, 1}.convert<u32>());
            m_copy_rt = m_api->alloc_rendertarget();
            m_copy_rt->alloc();
            m_copy_rt->attach(gfx::render_targets::attachment::color, *m_copy, 0);
            m_copy_rt->resize({0, 0, size.x, size.y});
            m_copy_sampler = m_copy->sampler();
            m_copy_sampler->alloc();
            m_copy_sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
            m_scene_color_sampler = resources.color->sampler();
            m_scene_color_sampler->alloc();
            m_scene_depth_sampler = resources.depth->sampler();
            m_scene_depth_sampler->alloc();
            m_scene_depth_sampler->set_filtering(typing::Filtering::Nearest, typing::Filtering::Nearest);
        }
        auto viewport = gfx::view_state{
            .view = typing::vector_types::Veci4{0, 0, size.x, size.y},
        };
        Vecf4 const quad_rect{1.f, 1.f, 0.f, 0.f};
        Matf4 const inverse_camera = glm::inverse(camera.matrix);
        Vecf3 const eye            = camera.camera.position;
        i32         zero_to_one    = 0;
#if GLEAM_MAX_VERSION >= 0x450
        zero_to_one = m_api->feature_info().program.dsa ? 1 : 0;
#endif
        auto copy_v = gfx::make_uniform_list(
            stage_t::Vertex,
            gfx::uniform_pair<Vecf4 const>{{"quad_rect"}, semantic::SpanOne<const Vecf4>(quad_rect)});
        auto copy_f = gfx::make_uniform_list(
            stage_t::Fragment,
            gfx::uniform_pair<Matf4 const>{
                {"inverse_camera"}, semantic::SpanOne<const Matf4>(inverse_camera)},
            gfx::uniform_pair<Vecf3 const>{{"eye"}, semantic::SpanOne<const Vecf3>(eye)},
            gfx::uniform_pair<const i32>{
                {"depth_zero_to_one"}, semantic::SpanOne<const i32>(zero_to_one)});
        auto copy_s = gfx::make_sampler_list(
            gfx::sampler_definition_t{stage_t::Fragment, {"scene_color", 0}, m_scene_color_sampler},
            gfx::sampler_definition_t{stage_t::Fragment, {"scene_depth", 1}, m_scene_depth_sampler});
        auto copy_res = m_api->submit(
            {
                .program       = m_copy_program,
                .vertices      = m_vao,
                .render_target = m_copy_rt,
                .call          = {.indexed = false, .mode = gfx::drawing::primitive::triangle},
                .data          = {{.arrays = {.count = 3}}},
            },
            copy_v,
            copy_f,
            copy_s,
            viewport);
        if(copy_res.has_value() && !m_draw_error_logged)
        {
            cWarning("H3: scene copy failed: {}", std::get<1>(*copy_res));
            m_draw_error_logged = true;
        }

        Vecf3 const offset{};
        f32 const   scale = 1.f;
        Matf4 const identity(1.f);
        Vecf2 const unit{1.f, 1.f};
        Vecf2 const target{static_cast<f32>(size.x), static_cast<f32>(size.y)};
        auto lightprobe = scene.lightprobe_sampler ? scene.lightprobe_sampler : m_no_lightprobe_sampler;
        for(auto const& mat : scene.materials)
        {
            if(mat.variant != Variant::water || !mat.visible || mat.draws.empty())
                continue;
            auto const& w      = mat.water;
            auto        vert_u = gfx::make_uniform_list(
                stage_t::Vertex,
                gfx::uniform_pair<Matf4 const>{{"camera"}, semantic::SpanOne<const Matf4>(camera.matrix)},
                gfx::uniform_pair<Vecf2 const>{{"uv_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                gfx::uniform_pair<Vecf3 const>{{"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
                gfx::uniform_pair<const f32>{{"model_scale"}, semantic::SpanOne<const f32>(scale)},
                gfx::uniform_pair<Matf4 const>{{"model"}, semantic::SpanOne<const Matf4>(identity)});
            auto frag_u = gfx::make_uniform_list(
                stage_t::Fragment,
                gfx::uniform_pair<Vecf3 const>{{"camera_position"}, semantic::SpanOne<const Vecf3>(eye)},
                gfx::uniform_pair<Vecf4 const>{
                    {"atmosphere"},
                    semantic::Span<const Vecf4>(scene.atmosphere.data(), scene.atmosphere.size())},
                gfx::uniform_pair<Vecf4 const>{
                    {"lightprobe_scales"},
                    semantic::Span<const Vecf4>(scene.lightprobe_scales.data(), scene.lightprobe_scales.size())},
                gfx::uniform_pair<const f32>{{"exposure"}, semantic::SpanOne<const f32>(exposure)},
                gfx::uniform_pair<const f32>{{"time"}, semantic::SpanOne<const f32>(seconds)},
                gfx::uniform_pair<Vecf4 const>{{"slope_xform"}, semantic::SpanOne<const Vecf4>(w.slope_xform)},
                gfx::uniform_pair<Vecf4 const>{{"shape_xform"}, semantic::SpanOne<const Vecf4>(w.shape_xform)},
                gfx::uniform_pair<Vecf4 const>{{"slope"}, semantic::SpanOne<const Vecf4>(w.slope)},
                gfx::uniform_pair<Vecf4 const>{{"heights"}, semantic::SpanOne<const Vecf4>(w.heights)},
                gfx::uniform_pair<Vecf4 const>{{"shading"}, semantic::SpanOne<const Vecf4>(w.shading)},
                gfx::uniform_pair<Vecf4 const>{{"refraction"}, semantic::SpanOne<const Vecf4>(w.refraction)},
                gfx::uniform_pair<Vecf4 const>{{"misc"}, semantic::SpanOne<const Vecf4>(w.misc)},
                gfx::uniform_pair<Vecf4 const>{{"foam_xform"}, semantic::SpanOne<const Vecf4>(w.foam_xform)},
                gfx::uniform_pair<Vecf4 const>{
                    {"foam_detail_xform"}, semantic::SpanOne<const Vecf4>(w.foam_detail_xform)},
                gfx::uniform_pair<Vecf4 const>{{"foam"}, semantic::SpanOne<const Vecf4>(w.foam_params)},
                gfx::uniform_pair<Vecf4 const>{
                    {"displacement_range"}, semantic::SpanOne<const Vecf4>(w.displacement_range)},
                gfx::uniform_pair<Vecf4 const>{{"water_diffuse"}, semantic::SpanOne<const Vecf4>(w.diffuse)},
                gfx::uniform_pair<Vecf2 const>{{"viewport"}, semantic::SpanOne<const Vecf2>(target)},
                gfx::uniform_pair<Vecf2 const>{{"water_curves"}, semantic::SpanOne<const Vecf2>(w.curves)});
            auto samplers = gfx::make_sampler_list(
                gfx::sampler_definition_t{
                    stage_t::Fragment, {"water_color", 0}, w.color.sampler ? w.color.sampler : m_white_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment,
                    {"water_reflection", 1},
                    w.reflection.sampler ? w.reflection.sampler : m_black_cube_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment,
                    {"water_slopes", 2},
                    w.slopes_sampler ? w.slopes_sampler : m_no_lightprobe_sampler},
                gfx::sampler_definition_t{stage_t::Fragment, {"scene_copy", 3}, m_copy_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment, {"foam_texture", 4}, w.foam.sampler ? w.foam.sampler : m_white_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment,
                    {"foam_detail", 5},
                    w.foam_detail.sampler ? w.foam_detail.sampler : m_white_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment, {"global_shape", 6}, w.shape.sampler ? w.shape.sampler : m_white_sampler},
                gfx::sampler_definition_t{
                    stage_t::Fragment,
                    {"wave_displacement", 7},
                    w.displacement_sampler ? w.displacement_sampler : m_no_lightprobe_sampler},
                gfx::sampler_definition_t{stage_t::Fragment, {"lightprobe", 9}, lightprobe});
            auto res = m_api->submit(
                {
                    .program       = m_programs[static_cast<size_t>(Variant::water)],
                    .vertices      = m_vao,
                    .render_target = resources.offscreen,
                    .call          = {.indexed = true, .instanced = true},
                    .data          = mat.draws,
                },
                vert_u,
                frag_u,
                samplers,
                gfx::view_state{
                    .view  = typing::vector_types::Veci4{0, 0, size.x, size.y},
                    .depth = gfx::depth_state{.reversed = true},
                },
                gfx::depth_extended_state{.depth_write = true});
            if(res.has_value() && !m_draw_error_logged)
            {
                cWarning("H3: water draw failed: {}", std::get<1>(*res));
                m_draw_error_logged = true;
            }
        }
    }

    /* The cubemap captured nearest the camera stands in for Halo 3's
     * per-object pair of dynamic cubemaps */
    /* The two cubemaps captured nearest `eye` and how far towards the second
     * to blend, so reflections fade between them instead of switching */
    struct cubemap_pair_t
    {
        std::shared_ptr<gfx::sampler_t> first, second;
        f32                              blend{0.f};
    };

    cubemap_pair_t nearest_cubemaps(H3Scene const& scene, Vecf3 const& eye) const
    {
        f32 best[2] = {std::numeric_limits<f32>::max(), std::numeric_limits<f32>::max()};
        u32 found[2] = {~0u, ~0u};
        for(auto const& probe : scene.cubemap_probes)
        {
            if(probe.cubemap >= scene.cubemaps.size())
                continue;
            f32 const d = glm::distance(probe.position, eye);
            if(d < best[0])
            {
                if(found[0] != probe.cubemap)
                    best[1] = best[0], found[1] = found[0];
                best[0] = d, found[0] = probe.cubemap;
            } else if(d < best[1] && probe.cubemap != found[0])
                best[1] = d, found[1] = probe.cubemap;
        }
        if(found[0] == ~0u)
            return {m_black_cube_sampler, m_black_cube_sampler, 0.f};
        auto const& first = scene.cubemaps[found[0]].sampler;
        if(found[1] == ~0u)
            return {first, first, 0.f};
        return {first, scene.cubemaps[found[1]].sampler, best[0] / std::max(best[0] + best[1], 1e-4f)};
    }

    /* Auto exposure: eases towards the camera's target over a second or
     * two. The target is the baked light around the camera, a few metres
     * across so single lightmap texels don't make it flicker, and is
     * refreshed a few times a second as it scans the lightmapped triangles */
    f32 adapt_exposure(H3Scene const& scene, Vecf3 const& eye, time_point const& now)
    {
        if(!scene.auto_exposure.enabled)
            return scene.exposure;
        if(m_exposure_generation != scene.generation)
        {
            m_exposure_generation = scene.generation;
            m_stops               = std::log2(scene.exposure);
            m_target_stops.reset();
            m_exposure_time = now;
            m_target_time   = {};
        }
        if(now - m_target_time >= std::chrono::milliseconds(250))
        {
            m_target_time = now;
            f32 luminance = 0.f;
            u32 samples   = 0;
            for(Vecf2 const offset : {Vecf2(0.f), Vecf2(2.f, 0.f), Vecf2(-2.f, 0.f), Vecf2(0.f, 2.f), Vecf2(0.f, -2.f)})
                if(auto light = scene.light_below(eye + Vecf3(offset, 0.f)))
                {
                    luminance += glm::dot(
                        diffuse_radiance(*light, Vecf3(0.f, 0.f, 1.f)), Vecf3(0.2126f, 0.7152f, 0.0722f));
                    samples++;
                }
            if(samples)
                m_target_stops = scene.auto_exposure.stops_for(luminance / static_cast<f32>(samples));
        }
        f32 const dt = std::clamp(std::chrono::duration<f32>(now - m_exposure_time).count(), 0.f, .25f);
        m_exposure_time = now;
        if(m_target_stops)
            m_stops += (*m_target_stops - m_stops) * (1.f - std::exp(-dt / exposure_time_constant));
        return std::exp2(m_stops);
    }

    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api     = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;

        m_vao = api.alloc_vertex_array();
        m_vao->alloc();
        m_vao->add(gfx::vertex_attribute{
            .index = 0,
            .value = {.offset = 0, .stride = sizeof(Vertex), .count = 3},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 1,
            .value = {.offset = offsetof(Vertex, texcoord), .stride = sizeof(Vertex), .count = 2},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 2,
            .value = {.offset = offsetof(Vertex, normal), .stride = sizeof(Vertex), .count = 3},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 3,
            .value = {.offset = offsetof(Vertex, lightmap), .stride = sizeof(Vertex), .count = 3},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 4,
            .value =
                {.offset = offsetof(Vertex, light),
                 .stride = sizeof(Vertex),
                 .count  = 4,
                 .type   = semantic::type_t::u8,
                 .flags  = gfx::vertex_attribute::attribute_flags::packed |
                          gfx::vertex_attribute::attribute_flags::normalized},
        });
        m_vao->set_attribute_names({
            {"position", 0},
            {"texcoord", 1},
            {"normal", 2},
            {"lightmap", 3},
            {"vertex_light", 4},
        });

        /* Stand-in for render methods without a usable base map */
        m_white = api.alloc_texture(
            gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_white->alloc(size_3d<u32>{2, 2, 1});
        m_white->upload(std::vector<u8>(16, 0xFF), Veci2{0, 0}, Veci2{2, 2}, 0);
        m_white_sampler = m_white->sampler();
        m_white_sampler->alloc();
        m_no_lightprobe = std::make_shared<gfx::compat::texture_2da_t>(
            &api, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_no_lightprobe->alloc(size_3d<u32>{2, 2, 1});
        m_no_lightprobe->upload(std::vector<u8>(16, 0xFF), Veci3{0, 0, 0}, size_3d<i32>{2, 2, 1});
        m_no_lightprobe_sampler = m_no_lightprobe->sampler();
        m_no_lightprobe_sampler->alloc();
        m_black_cube = api.alloc_texture(
            gfx::textures::cube, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_black_cube->alloc(size_3d<u32>{1, 1, 1});
        std::array<u8, 4>                        black{0, 0, 0, 0};
        std::array<semantic::Span<const u8>, 6> faces;
        faces.fill(semantic::Span<const u8>(black.data(), black.size()));
        m_black_cube->upload(faces, Veci2{0, 0}, Veci2{1, 1}, 0);
        m_black_cube_sampler = m_black_cube->sampler();
        m_black_cube_sampler->alloc();

        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h3_quad",
                .fragment_file = "h3_scene_copy",
                .shader        = m_copy_program,
            });
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "shadow_depth",
                .fragment_file = "shadow_depth",
                .shader        = m_shadow_program,
            });
        for(size_t i = 0; i < m_programs.size(); i++)
            create_program(
                api,
                shader_pair_t{
                    .vertex_file   = "h3",
                    .fragment_file = variant_files[i],
                    .shader        = m_programs[i],
                });
    }

    compo::EntityContainer& m_container;
    gfx::api*               m_api{nullptr};
    u32                     m_generation{0};

    std::shared_ptr<gfx::vertex_array_t>        m_vao;
    std::array<std::shared_ptr<gfx::program_t>, static_cast<size_t>(Variant::count)>
        m_programs;
    std::shared_ptr<gfx::texture_2d_t>          m_white;
    std::shared_ptr<gfx::sampler_t>             m_white_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> m_no_lightprobe;
    std::shared_ptr<gfx::sampler_t>             m_no_lightprobe_sampler;
    std::shared_ptr<gfx::texture_cube_t>        m_black_cube;
    std::shared_ptr<gfx::program_t>             m_copy_program;
    std::shared_ptr<gfx::program_t>             m_shadow_program;
    blam_render::CascadedShadowMap              m_shadows;
    /* The near slice of the depth range the first-person models draw into */
    static constexpr double first_person_depth = 0.98;
    blam_render::CascadedShadowMap::sampling_t  m_shadow_sampling{};
    std::shared_ptr<gfx::texture_2d_t>          m_copy;
    std::shared_ptr<gfx::rendertarget_t>        m_copy_rt;
    std::shared_ptr<gfx::sampler_t>             m_copy_sampler;
    std::shared_ptr<gfx::sampler_t>             m_scene_color_sampler;
    std::shared_ptr<gfx::sampler_t>             m_scene_depth_sampler;
    Veci2                                       m_copy_size{};
    time_point                                  m_start{};
    std::shared_ptr<gfx::sampler_t>             m_black_cube_sampler;
    f32                                         m_stops{0.f};
    std::optional<f32>                          m_target_stops;
    time_point                                  m_exposure_time{};
    time_point                                  m_target_time{};
    static constexpr f32                        exposure_time_constant = 1.5f; /*!< Seconds */
    u32                                         m_exposure_generation{~0u};
    bool                                        m_draw_error_logged{false};
    static constexpr size_t                     max_lights = 8;
    std::array<Vecf4, max_lights * 5>           m_lights{};
};

void alloc_h3_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H3Renderer>(std::ref(e));
}

} // namespace h3
