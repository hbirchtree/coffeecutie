#include "h3_scene.h"

#include "data.h"
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
    compo::type_list_t<const PlayerCamera, const PlayerInfo>,
    compo::type_list_t<const H3Scene, const BlamResources, const PostProcessParameters, gfx::system>,
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

    void start_restricted(Proxy& p, time_point const&)
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
        auto const&  post      = p.template subsystem<PostProcessParameters>();
        Vecf2 const  composite{post.exposure, post.gamma};
        Vecf3 const  offset{};
        f32 const    scale = 1.f;
        Vecf2 const  unit{1.f, 1.f};
        /* Opaque first, then alpha blended, then additive */
        for(int pass = 0; pass < 3; pass++)
            for(auto const& mat : scene.materials)
            {
                int const mat_pass = mat.additive ? 2 : mat.blended ? 1 : 0;
                if(mat_pass != pass || mat.draws.empty() || !mat.visible)
                    continue;
                bool const terrain = mat.variant == Variant::terrain;
                /* Terrain scales each layer in the fragment program */
                Vecf2 const uv_scale = terrain ? unit : mat.uv_scale;
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
                        {"illum_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"mask_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"bump_scale"}, semantic::SpanOne<const Vecf2>(mat.bump_scale)});
                i32 alpha_test = mat.alpha_test ? 1 : 0;
                i32 blended    = pass > 0 ? 1 : 0;
                auto view = gfx::view_state{
                    .view = typing::vector_types::Veci4{
                        0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
                    .depth = gfx::depth_state{.reversed = true},
                };
                auto submit = [&](auto& frag_u, auto& samplers) {
                    return m_api->submit(
                        {
                            .program       = m_programs[static_cast<size_t>(mat.variant)],
                            .vertices      = m_vao,
                            .render_target = resources.offscreen,
                            .call          = {.indexed = true, .instanced = true},
                            .data          = mat.draws,
                        },
                        vert_u,
                        frag_u,
                        samplers,
                        view,
                        gfx::depth_extended_state{.depth_write = pass == 0},
                        gfx::blend_state{.additive = mat.additive});
                };
                auto or_white = [&](Texture const& t) {
                    return t.sampler ? t.sampler : m_white_sampler;
                };
                auto const& lightprobe = scene.lightprobe_sampler ? scene.lightprobe_sampler
                                                                  : m_no_lightprobe_sampler;
                Vecf3 const eye   = camera->camera.position;
                i32 const   model = static_cast<i32>(mat.model);
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
                            {"lightprobe_scale"},
                            semantic::SpanOne<const Vecf4>(scene.lightprobe_scale)},
                        gfx::uniform_pair<const f32>{
                            {"exposure"}, semantic::SpanOne<const f32>(scene.exposure)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"composite"}, semantic::SpanOne<const Vecf2>(composite)},
                        gfx::uniform_pair<const f32>{
                            {"intensity_scale"}, semantic::SpanOne<const f32>(scene.intensity_scale)});
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
                        gfx::sampler_definition_t{stage_t::Fragment, {"lightprobe", 9}, lightprobe});
                    res = submit(frag_u, samplers);
                } else
                {
                    auto frag_u = gfx::make_uniform_list(
                        stage_t::Fragment,
                        gfx::uniform_pair<const i32>{
                            {"alpha_test"}, semantic::SpanOne<const i32>(alpha_test)},
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
                            {"lightprobe_scale"},
                            semantic::SpanOne<const Vecf4>(scene.lightprobe_scale)},
                        gfx::uniform_pair<const f32>{
                            {"exposure"}, semantic::SpanOne<const f32>(scene.exposure)},
                        gfx::uniform_pair<Vecf2 const>{
                            {"composite"}, semantic::SpanOne<const Vecf2>(composite)},
                        gfx::uniform_pair<const f32>{
                            {"intensity_scale"}, semantic::SpanOne<const f32>(scene.intensity_scale)},
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
                        gfx::uniform_pair<Vecf3 const>{
                            {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                    auto samplers = gfx::make_sampler_list(
                        gfx::sampler_definition_t{stage_t::Fragment, {"diffuse", 0}, or_white(mat.diffuse)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail", 1}, or_white(mat.detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"detail2", 2}, or_white(mat.detail2)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"bump_map", 3}, or_white(mat.bump)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"bump_detail_map", 4}, or_white(mat.bump_detail)},
                        gfx::sampler_definition_t{stage_t::Fragment, {"lightprobe", 9}, lightprobe},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"specular_mask_map", 10}, or_white(mat.specular_mask_map)},
                        gfx::sampler_definition_t{
                            stage_t::Fragment, {"material_map", 11}, or_white(mat.material_map)});
                    res = submit(frag_u, samplers);
                }
                if(res.has_value() && !m_draw_error_logged)
                {
                    cWarning("H3: failed to draw: {}", std::get<1>(*res));
                    m_draw_error_logged = true;
                }
            }
    }

  private:
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

    std::shared_ptr<gfx::vertex_array_t>        m_vao;
    std::array<std::shared_ptr<gfx::program_t>, static_cast<size_t>(Variant::count)>
        m_programs;
    std::shared_ptr<gfx::texture_2d_t>          m_white;
    std::shared_ptr<gfx::sampler_t>             m_white_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> m_no_lightprobe;
    std::shared_ptr<gfx::sampler_t>             m_no_lightprobe_sampler;
    bool                                        m_draw_error_logged{false};
};

void alloc_h3_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H3Renderer>(std::ref(e));
}

} // namespace h3
