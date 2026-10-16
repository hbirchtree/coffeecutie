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
    compo::type_list_t<const H3Scene, const BlamResources, gfx::system>,
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
        Vecf3 const& eye       = camera->camera.position;
        Vecf3 const  offset{};
        f32 const    scale = 1.f;
        Vecf2 const  unit{1.f, 1.f};
        Vecf4 const  no_fog{0.f, 0.f, 0.f, 0.f};
        Vecf2 const  fog_range{0.f, 1.f};
        Vecf4 const  no_plane{0.f, 0.f, 1.f, -1e9f};
        /* Opaque first, then alpha blended, then additive */
        for(int pass = 0; pass < 3; pass++)
            for(auto const& mat : scene.materials)
            {
                int const mat_pass = mat.additive ? 2 : mat.blended ? 1 : 0;
                if(mat_pass != pass || mat.draws.empty() || !mat.visible)
                    continue;
                auto vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"}, semantic::SpanOne<const Matf4>(camera->matrix)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"uv_scale"}, semantic::SpanOne<const Vecf2>(mat.uv_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
                    gfx::uniform_pair<const f32>{
                        {"model_scale"}, semantic::SpanOne<const f32>(scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail2_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"illum_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"mask_scale"}, semantic::SpanOne<const Vecf2>(unit)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                i32  alpha_test = mat.alpha_test ? 1 : 0;
                i32  blended    = pass > 0 ? 1 : 0;
                auto frag_u     = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair<const i32>{
                        {"alpha_test"}, semantic::SpanOne<const i32>(alpha_test)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"albedo_tint"}, semantic::SpanOne<const Vecf4>(mat.tint)},
                    gfx::uniform_pair<const i32>{
                        {"blended"}, semantic::SpanOne<const i32>(blended)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"fog_color"}, semantic::SpanOne<const Vecf4>(no_fog)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"fog_range"}, semantic::SpanOne<const Vecf2>(fog_range)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"fog_eye"}, semantic::SpanOne<const Vecf3>(eye)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"planar_fog_plane"}, semantic::SpanOne<const Vecf4>(no_plane)},
                    gfx::uniform_pair<Vecf4 const>{
                        {"planar_fog_color"}, semantic::SpanOne<const Vecf4>(no_fog)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"planar_fog_opaque"}, semantic::SpanOne<const Vecf2>(unit)});
                auto diffuse  = mat.diffuse.sampler ? mat.diffuse.sampler : m_white_sampler;
                auto samplers = gfx::make_sampler_list(
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment, {"diffuse", 0}, diffuse},
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment, {"detail", 1}, m_white_sampler},
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment, {"detail2", 2}, m_white_sampler},
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment, {"illum", 3}, m_white_sampler},
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"lightmaps", 4},
                        m_no_lightmaps_sampler},
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment, {"mask", 5}, m_white_sampler});
                auto res = m_api->submit(
                    {
                        .program       = m_program,
                        .vertices      = m_vao,
                        .render_target = resources.offscreen,
                        .call          = {.indexed = true, .instanced = true},
                        .data          = mat.draws,
                    },
                    vert_u,
                    frag_u,
                    samplers,
                    gfx::view_state{
                        .view = typing::vector_types::Veci4{
                            0, 0, resources.offscreen_size.x, resources.offscreen_size.y},
                        .depth = gfx::depth_state{.reversed = true},
                    },
                    gfx::depth_extended_state{.depth_write = pass == 0},
                    gfx::blend_state{.additive = mat.additive});
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

        m_no_lightmaps = std::make_shared<gfx::compat::texture_2da_t>(
            &api, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_no_lightmaps->alloc(size_3d<u32>{2, 2, 1});
        m_no_lightmaps->upload(
            std::vector<u8>(16, 0xFF), Veci3{0, 0, 0}, size_3d<i32>{2, 2, 1});
        m_no_lightmaps_sampler = m_no_lightmaps->sampler();
        m_no_lightmaps_sampler->alloc();

        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h2",
                .fragment_file = "h3_base",
                .shader        = m_program,
            });
    }

    compo::EntityContainer& m_container;
    gfx::api*               m_api{nullptr};
    u32                     m_generation{0};

    std::shared_ptr<gfx::vertex_array_t>        m_vao;
    std::shared_ptr<gfx::program_t>             m_program;
    std::shared_ptr<gfx::texture_2d_t>          m_white;
    std::shared_ptr<gfx::sampler_t>             m_white_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> m_no_lightmaps;
    std::shared_ptr<gfx::sampler_t>             m_no_lightmaps_sampler;
    bool                                        m_draw_error_logged{false};
};

void alloc_h3_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H3Renderer>(std::ref(e));
}

} // namespace h3
