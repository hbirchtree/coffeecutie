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
#include <glm/gtc/matrix_access.hpp>

#include <unordered_map>

namespace h3 {

using H3RendererManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo, const Model>,
    compo::type_list_t<
        const H3Scene,
        const BlamResources,
        const RenderingParameters,
        const PostProcessParameters,
        gfx::system>,
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
            m_first_person_vao->set_buffer(gfx::buffers::vertex, scene.first_person_buf, 0);
            m_first_person_vao->set_buffer(gfx::buffers::element, scene.element_buf);
            m_generation = scene.generation;
        }
        PlayerCamera const* camera{};
        /* The free (forge) camera has no body to hold anything */
        bool first_person_shown = false;
        bool first_person_camo  = false;
        for(auto e : p.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = e.components();
            if(info.seat_idx != 0)
                continue;
            camera             = &cam;
            first_person_shown = info.mode.physics;
            first_person_camo  = info.camouflaged;
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
        f32 const    dynamic_curve = scene.cubemaps.empty() ? 2.f : scene.cubemaps.front().curve;
        auto const&  dynamic_cube = cubes.first;
        Vecf3 const  offset{};
        f32 const    scale = 1.f;
        Vecf2 const  unit{1.f, 1.f};
        if(m_start == time_point{})
            m_start = now;
        f32 const seconds = std::chrono::duration<f32>(now - m_start).count();
        /* Timed per section; the queries cannot nest */
        auto const frame_scope = m_api->debug().scope("H3Renderer");
        {
            auto const _     = m_api->debug().scope("H3Renderer shadows");
            auto const timer = m_system->gpu_timer("H3Renderer shadows");
            render_shadows(p, scene, *camera);
        }
        gather_lights(p, scene);
        Model first_person{};
        /* Where the view is drawn from, between ticks when walking */
        place_first_person(*camera, camera->drawn_eye, Attachment{}, first_person);
        ease_first_person_light(scene, camera->drawn_eye, now);
        auto const& first_person_light = m_first_person_light;
        auto const& csm_atlas = m_shadows.valid() ? m_shadows.sampler() : m_white_sampler;
        /* Opaque first, then water over a copy of the scene, then alpha
         * blended, then additive */
        static constexpr std::array<char const*, 3> pass_names = {
            "H3Renderer opaque", "H3Renderer blended", "H3Renderer additive"};
        for(int pass = 0; pass < 3; pass++)
        {
            if(pass == 1 && params.render_scenery)
            {
                auto const _     = m_api->debug().scope("H3Renderer decorators");
                auto const timer = m_system->gpu_timer("H3Renderer decorators");
                draw_decorators(scene, resources, *camera, exposure);
            }
            if(pass == 1)
            {
                auto const _     = m_api->debug().scope("H3Renderer water");
                auto const timer = m_system->gpu_timer("H3Renderer water");
                draw_water(scene, resources, *camera, exposure, seconds);
            }
            auto const pass_scope = m_api->debug().scope(pass_names[static_cast<size_t>(pass)]);
            auto const pass_timer = m_system->gpu_timer(pass_names[static_cast<size_t>(pass)]);
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
                auto vertices     = m_vao;
                auto submit_draws = [&](auto& frag_u, auto& samplers, auto const& draws) {
                    return m_api->submit(
                        {
                            .program       = m_programs[static_cast<size_t>(mat.variant)],
                            .vertices      = vertices,
                            .render_target = resources.offscreen,
                            .call          = {.indexed = true, .instanced = true},
                            .data          = draws,
                        },
                        vert_u,
                        frag_u,
                        samplers,
                        view,
                        gfx::depth_extended_state{.depth_write = pass == 0},
                        gfx::blend_state{.additive = mat.additive},
                        gfx::cull_state{.front_face = cull_front});
                };
                static std::vector<ObjectDraws> const no_objects;
                std::array<Vecf4, 5>                  object_light{};
                i32                                   object_lit = 0;
                /* The level, then each object where its Model is */
                auto submit = [&](auto& frag_u, auto& samplers) {
                    std::optional<std::tuple<gfx::error, std::string_view>> out;
                    model_matrix = Matf4(1.f);
                    object_lit   = 0;
                    /* The level by the lights nearest the camera, objects by theirs */
                    light_count = pack_lights(0, camera->camera.position);
                    if(!mat.draws.empty())
                        out = submit_draws(frag_u, samplers, mat.draws);
                    for(auto const& object : params.render_scenery ? mat.object_draws : no_objects)
                    {
                        /* In front of everything, so it never reaches into walls */
                        if(object.entity == first_person_entity)
                        {
                            if(!first_person_shown || first_person_camo)
                                continue;
                            object_lit = first_person_light ? 1 : 0;
                            if(first_person_light)
                                object_light = *first_person_light;
                            model_matrix     = first_person.transform;
                            light_count      = pack_lights(first_person_entity, camera->camera.position);
                            view.depth->range = typing::vector_types::Vecd2{first_person_depth, 1.0};
                            vertices          = m_first_person_vao;
                            if(auto res = submit_draws(frag_u, samplers, object.draws); !out)
                                out = res;
                            vertices          = m_vao;
                            object_lit        = 0;
                            view.depth->range = typing::vector_types::Vecd2{0.0, 1.0};
                            continue;
                        }
                        auto const* placed = p.template get<Model>(object.entity);
                        if(!placed || placed->camouflaged ||
                           !in_view(camera->matrix, object.sphere(placed->transform)))
                            continue;
                        model_matrix = placed->transform;
                        light_count  = pack_lights(object.entity, placed->position);
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
                            {"object_light"}, semantic::Span<const Vecf4>(object_light.data(), object_light.size())},
                        gfx::uniform_pair<const i32>{{"object_lit"}, semantic::SpanOne<const i32>(object_lit)},
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
                        gfx::uniform_pair<const f32>{
                            {"env_dynamic_curve"}, semantic::SpanOne<const f32>(dynamic_curve)},
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
                            {"object_light"}, semantic::Span<const Vecf4>(object_light.data(), object_light.size())},
                        gfx::uniform_pair<const i32>{{"object_lit"}, semantic::SpanOne<const i32>(object_lit)},
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
                            mat.environment_map.sampler ? mat.environment_map.sampler : m_black_cube_sampler},
                        gfx::sampler_definition_t{
                            stage_t::Fragment,
                            {"object_lights", 18},
                            scene.object_lights_sampler ? scene.object_lights_sampler : m_white_sampler});
                    res = submit(frag_u, samplers);
                }
                if(res.has_value() && !m_draw_error_logged)
                {
                    cWarning("H3: failed to draw: {}", std::get<1>(*res));
                    m_draw_error_logged = true;
                }
            }
        }
        {
            auto const _     = m_api->debug().scope("H3Renderer camouflage");
            auto const timer = m_system->gpu_timer("H3Renderer camouflage");
            draw_camouflage(
                p,
                scene,
                resources,
                *camera,
                first_person,
                first_person_shown && first_person_camo,
                p.template subsystem<PostProcessParameters>().camo_scale);
        }
        if(scene.bloom.intensity > 0.f)
        {
            auto const _     = m_api->debug().scope("H3Renderer bloom");
            auto const timer = m_system->gpu_timer("H3Renderer bloom");
            draw_bloom(scene.bloom, resources);
        }
    }

  private:
    /* Bloom targets: what blooms at a quarter size, halved three times,
     * then the two sums on the way back up */
    struct BloomTarget
    {
        std::shared_ptr<gfx::texture_2d_t>   texture;
        std::shared_ptr<gfx::rendertarget_t> target;
        std::shared_ptr<gfx::sampler_t>      sampler;
        Veci2                                size{};
    };

    enum bloom_target_t
    {
        bloom_bright,
        bloom_small,
        bloom_medium,
        bloom_large,
        bloom_sum_medium,
        bloom_sum_small,
        bloom_target_count,
    };

    /* Halo 3's bloom: what is brighter than the bloom point, blurred at three
     * sizes and tinted per size, added over the exposed scene */
    void draw_bloom(Bloom const& bloom, BlamResources const& resources)
    {
        using stage_t = typing::graphics::ShaderStage;
        if(!resources.color)
            return;
        auto const size = resources.offscreen_size;
        if(m_bloom_size != size)
        {
            m_bloom_size = size;
            if(!m_scene_color_sampler)
            {
                m_scene_color_sampler = resources.color->sampler();
                m_scene_color_sampler->alloc();
            }
            m_scene_color_sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
            static constexpr std::array<i32, bloom_target_count> divisors = {4, 8, 16, 32, 16, 8};
            for(size_t i = 0; i < m_bloom.size(); i++)
            {
                auto& t   = m_bloom[i];
                t.size    = glm::max(size / divisors[i], Veci2(1));
                t.texture = m_api->alloc_texture(
                    gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA16F), 1);
                t.texture->alloc(size_3d<i32>{t.size.x, t.size.y, 1}.convert<u32>());
                t.target = m_api->alloc_rendertarget();
                t.target->alloc();
                t.target->attach(gfx::render_targets::attachment::color, *t.texture, 0);
                t.target->resize({0, 0, t.size.x, t.size.y});
                t.sampler = t.texture->sampler();
                t.sampler->alloc();
                t.sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
            }
        }
        Vecf4 const quad_rect{1.f, 1.f, 0.f, 0.f};
        auto        quad_v = gfx::make_uniform_list(
            stage_t::Vertex,
            gfx::uniform_pair<Vecf4 const>{{"quad_rect"}, semantic::SpanOne<const Vecf4>(quad_rect)});
        auto texel = [](Veci2 extent) { return Vecf2(1.f) / Vecf2(extent); };
        auto pass  = [&](auto const& program, auto& frag_u, auto& samplers, std::shared_ptr<gfx::rendertarget_t> target,
                        Veci2 extent, bool additive) {
            auto viewport = gfx::view_state{
                .view = typing::vector_types::Veci4{0, 0, extent.x, extent.y},
            };
            auto res = m_api->submit(
                {
                    .program       = program,
                    .vertices      = m_vao,
                    .render_target = target,
                    .call          = {.indexed = false, .mode = gfx::drawing::primitive::triangle},
                    .data          = {{.arrays = {.count = 3}}},
                },
                quad_v,
                frag_u,
                samplers,
                viewport,
                gfx::depth_extended_state{.depth_write = false},
                gfx::blend_state{.additive = additive});
            if(res.has_value() && !m_draw_error_logged)
            {
                cWarning("H3: bloom failed: {}", std::get<1>(*res));
                m_draw_error_logged = true;
            }
        };

        Vecf2 const curve{bloom.point, bloom.inherent};
        Vecf2 const scene_texel = texel(size);
        auto        bright_u    = gfx::make_uniform_list(
            stage_t::Fragment,
            gfx::uniform_pair<Vecf2 const>{{"source_texel"}, semantic::SpanOne<const Vecf2>(scene_texel)},
            gfx::uniform_pair<Vecf2 const>{{"bloom_curve"}, semantic::SpanOne<const Vecf2>(curve)});
        auto bright_s = gfx::make_sampler_list(
            gfx::sampler_definition_t{stage_t::Fragment, {"scene_color", 0}, m_scene_color_sampler});
        pass(m_bloom_bright_program, bright_u, bright_s, m_bloom[bloom_bright].target,
             m_bloom[bloom_bright].size, false);

        /* `source` into `target`: halved, or doubled with `scale` plus `tint` times `level` */
        auto blur = [&](size_t source, std::shared_ptr<gfx::rendertarget_t> target, Veci2 extent, i32 mode,
                        Vecf3 scale, Vecf3 tint, std::shared_ptr<gfx::sampler_t> level, bool additive) {
            Vecf2 const source_texel = texel(m_bloom[source].size);
            auto        frag_u       = gfx::make_uniform_list(
                stage_t::Fragment,
                gfx::uniform_pair<Vecf2 const>{{"source_texel"}, semantic::SpanOne<const Vecf2>(source_texel)},
                gfx::uniform_pair<const i32>{{"mode"}, semantic::SpanOne<const i32>(mode)},
                gfx::uniform_pair<Vecf3 const>{{"scale"}, semantic::SpanOne<const Vecf3>(scale)},
                gfx::uniform_pair<Vecf3 const>{{"tint"}, semantic::SpanOne<const Vecf3>(tint)});
            auto samplers = gfx::make_sampler_list(
                gfx::sampler_definition_t{stage_t::Fragment, {"source", 0}, m_bloom[source].sampler},
                gfx::sampler_definition_t{stage_t::Fragment, {"level", 1}, level});
            pass(m_bloom_program, frag_u, samplers, target, extent, additive);
        };
        auto to = [&](size_t i) { return std::make_pair(m_bloom[i].target, m_bloom[i].size); };
        Vecf3 const none(0.f), one(1.f);
        for(size_t i = bloom_small; i <= bloom_large; i++)
        {
            auto [target, extent] = to(i);
            blur(i - 1, target, extent, 0, one, none, m_white_sampler, false);
        }
        auto const& [large, medium, small] = bloom.colors;
        {
            auto [target, extent] = to(bloom_sum_medium);
            blur(bloom_large, target, extent, 1, large, medium, m_bloom[bloom_medium].sampler, false);
        }
        {
            auto [target, extent] = to(bloom_sum_small);
            blur(bloom_sum_medium, target, extent, 1, one, small, m_bloom[bloom_small].sampler, false);
        }
        blur(bloom_sum_small, resources.offscreen, size, 1, Vecf3(bloom.intensity), none, m_white_sampler, true);
    }

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
                    Vecf4 const sphere = object.sphere(placed->transform);
                    if(!m_shadows.covers(c, Vecf3(sphere), sphere.w))
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
    /* The first person's light: the lightmap under a few points around the
     * eye, looked up four times a second and eased towards, so walking in
     * and out of shade fades rather than steps */
    void ease_first_person_light(H3Scene const& scene, Vecf3 const& eye, time_point const& now)
    {
        bool const reload = m_first_person_generation != scene.generation;
        if(reload || now - m_first_person_sampled > std::chrono::milliseconds(250))
        {
            m_first_person_sampled = now;
            std::array<Vecf4, 5> sum{};
            u32                  samples = 0;
            for(Vecf2 const offset : {Vecf2(0.f), Vecf2(.5f, 0.f), Vecf2(-.5f, 0.f), Vecf2(0.f, .5f), Vecf2(0.f, -.5f)})
                if(auto light = scene.light_below(eye + Vecf3(offset, 0.f)))
                {
                    auto const row = h3::packed(*light);
                    for(size_t i = 0; i < sum.size(); i++)
                        sum[i] += row[i];
                    samples++;
                }
            if(samples)
                for(auto& v : sum)
                    v /= static_cast<f32>(samples);
            m_first_person_target = samples ? std::optional(sum) : std::nullopt;
            if(reload)
                m_first_person_light.reset();
            m_first_person_generation = scene.generation;
        }
        f32 const dt = std::clamp(std::chrono::duration<f32>(now - m_first_person_time).count(), 0.f, .25f);
        m_first_person_time = now;
        if(!m_first_person_target)
            return;
        if(!m_first_person_light)
        {
            m_first_person_light = m_first_person_target;
            return;
        }
        f32 const t = 1.f - std::exp(-dt / .3f);
        for(size_t i = 0; i < m_first_person_light->size(); i++)
            (*m_first_person_light)[i] = glm::mix((*m_first_person_light)[i], (*m_first_person_target)[i], t);
    }

    /* Whether a bounding sphere reaches into the view; the projection's far
     * plane is at infinity, so only its sides and the eye plane count */
    static bool in_view(Matf4 const& view_projection, Vecf4 const& sphere)
    {
        Vecf4 const row3 = glm::row(view_projection, 3);
        auto outside = [&](Vecf4 const& plane) {
            return glm::dot(Vecf3(plane), Vecf3(sphere)) + plane.w < -sphere.w * glm::length(Vecf3(plane));
        };
        for(int i = 0; i < 2; i++)
        {
            Vecf4 const row = glm::row(view_projection, i);
            if(outside(row3 + row) || outside(row3 - row))
                return false;
        }
        return !outside(row3);
    }

    /* The lights where their objects carry them this frame */
    void gather_lights(Proxy& p, H3Scene const& scene)
    {
        m_frame_lights.clear();
        m_packed.clear();
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
            m_frame_lights.push_back({&light, position, direction, 0.f});
        }
    }

    /* The lights nearest `eye`, as simple_lights.fx reads them; chosen once a
     * frame per `key` (an object, or 0 for the level) */
    i32 pack_lights(u64 key, Vecf3 const& eye)
    {
        auto [it, fresh] = m_packed.try_emplace(key);
        auto& packed     = it->second;
        if(fresh)
        {
            for(auto& c : m_frame_lights)
                c.distance = glm::distance(c.position, eye) - c.light->radius;
            size_t const count = std::min(m_frame_lights.size(), max_lights);
            std::partial_sort(
                m_frame_lights.begin(), m_frame_lights.begin() + count, m_frame_lights.end(),
                [](auto const& a, auto const& b) { return a.distance < b.distance; });
            packed.lights.fill(Vecf4(0.f));
            for(size_t i = 0; i < count; i++)
            {
                auto const& [light, position, direction, _] = m_frame_lights[i];
                packed.lights[i * 5] = Vecf4(position, light->size);
                /* Shaded towards the light: the spot faces the other way */
                packed.lights[i * 5 + 1] = Vecf4(-direction, light->sphere);
                packed.lights[i * 5 + 2] = Vecf4(light->color, light->smooth);
                packed.lights[i * 5 + 3] = light->falloff;
                packed.lights[i * 5 + 4] = Vecf4(light->radius * light->radius, 0.f, 0.f, 0.f);
            }
            packed.count = static_cast<i32>(count);
        }
        m_lights = packed.lights;
        return packed.count;
    }

    /* Water (water_shading.fx) reads the scene behind it: a copy of the
     * colour and of how far away each pixel is, made once per frame */
    /* Each decorator set's mesh at all of its instances, after the opaque
     * pass: alpha tested, lit by the light baked into each instance */
    void draw_decorators(H3Scene const& scene, BlamResources const& resources, PlayerCamera const& camera, f32 exposure)
    {
        if(scene.decorators.empty() || !scene.decorator_instances_sampler || !m_decorator_program)
            return;
        using stage_t = typing::graphics::ShaderStage;
        Vecf3 const eye = camera.camera.position;
        Vecf2       fade{0.f};
        i32         first = 0;
        f32         curve = 2.f;
        auto vert_u = gfx::make_uniform_list(
            stage_t::Vertex,
            gfx::uniform_pair<Matf4 const>{{"camera"}, semantic::SpanOne<const Matf4>(camera.matrix)},
            gfx::uniform_pair<Vecf3 const>{{"eye"}, semantic::SpanOne<const Vecf3>(eye)},
            gfx::uniform_pair<Vecf2 const>{{"fade"}, semantic::SpanOne<const Vecf2>(fade)},
            gfx::uniform_pair<const i32>{{"first_instance"}, semantic::SpanOne<const i32>(first)});
        auto frag_u = gfx::make_uniform_list(
            stage_t::Fragment,
            gfx::uniform_pair<const f32>{{"exposure"}, semantic::SpanOne<const f32>(exposure)},
            gfx::uniform_pair<const f32>{{"diffuse_curve"}, semantic::SpanOne<const f32>(curve)},
            gfx::uniform_pair<Vecf3 const>{{"camera_position"}, semantic::SpanOne<const Vecf3>(eye)},
            gfx::uniform_pair<Vecf4 const>{
                {"atmosphere"}, semantic::Span<const Vecf4>(scene.atmosphere.data(), scene.atmosphere.size())});
        auto view = gfx::view_state{
            .view  = typing::vector_types::Veci4{0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
            .depth = gfx::depth_state{.range = typing::vector_types::Vecd2{0.0, 1.0}, .reversed = true},
        };
        for(auto const& set : scene.decorators)
        {
            fade  = set.fade;
            first = static_cast<i32>(set.first_instance);
            curve = set.curve;
            auto samplers = gfx::make_sampler_list(
                gfx::sampler_definition_t{stage_t::Vertex, {"instances", 0}, scene.decorator_instances_sampler},
                gfx::sampler_definition_t{stage_t::Fragment, {"diffuse", 1}, set.diffuse.sampler});
            gfx::draw_command command{
                .program       = m_decorator_program,
                .vertices      = m_vao,
                .render_target = resources.offscreen,
                .call          = {.indexed = true, .instanced = true},
                .data          = set.draws,
            };
            if(set.two_sided)
                m_api->submit(command, vert_u, frag_u, samplers, view, gfx::depth_extended_state{.depth_write = true});
            else
                m_api->submit(
                    command, vert_u, frag_u, samplers, view, gfx::depth_extended_state{.depth_write = true},
                    gfx::cull_state{.front_face = cull_front});
        }
    }

    /* Camouflaged objects, after everything else: the scene behind them,
     * refracted along their silhouettes */
    void draw_camouflage(
        Proxy&               p,
        H3Scene const&       scene,
        BlamResources const& resources,
        PlayerCamera const&  camera,
        Model const&         first_person,
        bool                 first_person_camo,
        f32                  camo_scale)
    {
        using stage_t = typing::graphics::ShaderStage;
        m_camo_draws.clear();
        for(auto const& mat : scene.materials)
        {
            if(!mat.visible || mat.variant == Variant::water)
                continue;
            for(auto const& object : mat.object_draws)
            {
                if(object.entity == first_person_entity)
                {
                    if(first_person_camo)
                        m_camo_draws.push_back({&object.draws, first_person.transform, true});
                    continue;
                }
                auto const* placed = p.template get<Model>(object.entity);
                if(placed && placed->camouflaged && in_view(camera.matrix, object.sphere(placed->transform)))
                    m_camo_draws.push_back({&object.draws, placed->transform, false});
            }
        }
        if(m_camo_draws.empty() || !copy_scene(resources, camera))
            return;
        Matf3 const rotation(camera.rotation);
        Vecf3 const offset{};
        f32 const   scale = 1.f;
        Vecf2 const unit{1.f, 1.f};
        Matf4       model_matrix(1.f);
        auto        vert_u = gfx::make_uniform_list(
            stage_t::Vertex,
            gfx::uniform_pair<Matf4 const>{{"camera"}, semantic::SpanOne<const Matf4>(camera.matrix)},
            gfx::uniform_pair<Vecf2 const>{{"uv_scale"}, semantic::SpanOne<const Vecf2>(unit)},
            gfx::uniform_pair<Vecf3 const>{{"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
            gfx::uniform_pair<const f32>{{"model_scale"}, semantic::SpanOne<const f32>(scale)},
            gfx::uniform_pair<Matf4 const>{{"model"}, semantic::SpanOne<const Matf4>(model_matrix)});
        auto frag_u = gfx::make_uniform_list(
            stage_t::Fragment,
            gfx::uniform_pair<Matf3 const>{{"camera_rotation"}, semantic::SpanOne<const Matf3>(rotation)},
            gfx::uniform_pair<const f32>{{"camo_scale"}, semantic::SpanOne<const f32>(camo_scale)});
        auto samplers = gfx::make_sampler_list(
            gfx::sampler_definition_t{stage_t::Fragment, {"source_scene", 0}, m_copy_sampler});
        auto view = gfx::view_state{
            .view  = typing::vector_types::Veci4{0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
            .depth = gfx::depth_state{.range = typing::vector_types::Vecd2{0.0, 1.0}, .reversed = true},
        };
        for(auto const& draw : m_camo_draws)
        {
            model_matrix      = draw.transform;
            view.depth->range = typing::vector_types::Vecd2{draw.first_person ? first_person_depth : 0.0, 1.0};
            auto res          = m_api->submit(
                {
                    .program       = m_camo_program,
                    .vertices      = draw.first_person ? m_first_person_vao : m_vao,
                    .render_target = resources.offscreen,
                    .call          = {.indexed = true, .instanced = true},
                    .data          = *draw.draws,
                },
                vert_u,
                frag_u,
                samplers,
                view,
                gfx::depth_extended_state{.depth_write = true},
                gfx::cull_state{.front_face = cull_front});
            if(res.has_value() && !m_draw_error_logged)
            {
                cWarning("H3: camouflage failed: {}", std::get<1>(*res));
                m_draw_error_logged = true;
            }
        }
    }

    /* The scene so far into m_copy: colour, and the distance to what each
     * pixel shows in alpha */
    bool copy_scene(BlamResources const& resources, PlayerCamera const& camera)
    {
        using stage_t = typing::graphics::ShaderStage;
        if(!resources.color || !resources.depth)
            return false;
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
        return !copy_res.has_value();
    }

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
        if(!any || !copy_scene(resources, camera))
            return;
        auto const size = resources.offscreen_size;
        Vecf3 const eye  = camera.camera.position;
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
        m_system  = &m_container.subsystem_cast<gfx::system>();
        m_api     = m_system;
        auto& api = *m_api;

        /* The level and objects, and the first person's own vertices */
        auto layout = [&](std::shared_ptr<gfx::vertex_array_t>& vao) {
            vao = api.alloc_vertex_array();
            vao->alloc();
            vao->add(gfx::vertex_attribute{
                .index = 0,
                .value = {.offset = 0, .stride = sizeof(Vertex), .count = 3},
            });
            vao->add(gfx::vertex_attribute{
                .index = 1,
                .value = {.offset = offsetof(Vertex, texcoord), .stride = sizeof(Vertex), .count = 2},
            });
            vao->add(gfx::vertex_attribute{
                .index = 2,
                .value = {.offset = offsetof(Vertex, normal), .stride = sizeof(Vertex), .count = 3},
            });
            vao->add(gfx::vertex_attribute{
                .index = 3,
                .value = {.offset = offsetof(Vertex, lightmap), .stride = sizeof(Vertex), .count = 3},
            });
            vao->add(gfx::vertex_attribute{
                .index = 4,
                .value =
                    {.offset = offsetof(Vertex, light),
                     .stride = sizeof(Vertex),
                     .count  = 4,
                     .type   = semantic::type_t::u8,
                     .flags  = gfx::vertex_attribute::attribute_flags::packed |
                              gfx::vertex_attribute::attribute_flags::normalized},
            });
            vao->set_attribute_names({
                {"position", 0},
                {"texcoord", 1},
                {"normal", 2},
                {"lightmap", 3},
                {"vertex_light", 4},
            });
        };
        layout(m_vao);
        layout(m_first_person_vao);

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
                .vertex_file   = "h3",
                .fragment_file = "h3_camo",
                .shader        = m_camo_program,
            });
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h3_quad",
                .fragment_file = "h3_bloom_bright",
                .shader        = m_bloom_bright_program,
            });
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h3_quad",
                .fragment_file = "h3_bloom",
                .shader        = m_bloom_program,
            });
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h3_decorator",
                .fragment_file = "h3_decorator",
                .shader        = m_decorator_program,
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
    gfx::system*            m_system{nullptr}; /*!< m_api, for GPU timers */
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
    std::shared_ptr<gfx::program_t>             m_decorator_program;
    std::shared_ptr<gfx::program_t>             m_shadow_program;
    blam_render::CascadedShadowMap              m_shadows;
    u32                                 m_first_person_generation{0};
    time_point                          m_first_person_sampled{};
    time_point                          m_first_person_time{};
    std::optional<std::array<Vecf4, 5>> m_first_person_light;
    std::optional<std::array<Vecf4, 5>> m_first_person_target;
    /* Halo 3 culls back faces */
    static constexpr bool cull_front = false;
    /* The near slice of the depth range the first-person models draw into */
    static constexpr double first_person_depth = 0.98;
    blam_render::CascadedShadowMap::sampling_t  m_shadow_sampling{};
    std::shared_ptr<gfx::texture_2d_t>          m_copy;
    std::shared_ptr<gfx::rendertarget_t>        m_copy_rt;
    std::shared_ptr<gfx::sampler_t>             m_copy_sampler;
    std::shared_ptr<gfx::sampler_t>             m_scene_color_sampler;
    std::shared_ptr<gfx::sampler_t>             m_scene_depth_sampler;
    Veci2                                       m_copy_size{};
    std::shared_ptr<gfx::program_t>             m_camo_program;
    struct CamoDraw
    {
        std::vector<gfx::draw_command::data_t> const* draws;
        Matf4                                         transform;
        bool                                          first_person;
    };
    std::vector<CamoDraw>                       m_camo_draws;
    std::shared_ptr<gfx::program_t>             m_bloom_bright_program;
    std::shared_ptr<gfx::program_t>             m_bloom_program;
    std::array<BloomTarget, bloom_target_count> m_bloom;
    Veci2                                       m_bloom_size{};
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
    struct FrameLight
    {
        DynamicLight const* light;
        Vecf3               position, direction;
        f32                 distance;
    };
    struct PackedLights
    {
        std::array<Vecf4, max_lights * 5> lights{};
        i32                               count{0};
    };
    std::vector<FrameLight>                     m_frame_lights;
    std::unordered_map<u64, PackedLights>       m_packed;
    std::shared_ptr<gfx::vertex_array_t>        m_first_person_vao;
};

void alloc_h3_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H3Renderer>(std::ref(e));
}

} // namespace h3
