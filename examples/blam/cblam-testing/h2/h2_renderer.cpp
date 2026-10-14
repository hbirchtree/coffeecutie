#include "h2_scene.h"

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

#include <array>
#include <limits>

namespace h2 {

using H2RendererManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo>,
    compo::type_list_t<
        const H2Scene,
        const H2Visibility,
        const BlamResources,
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

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const& scene = p.template subsystem<H2Scene>();
        auto const& vis   = p.template subsystem<H2Visibility>();
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

        auto& resources = p.template subsystem<BlamResources>();
        size_t draws_total = 0, draws_visible = 0;
        /* Sky first (never writes depth), opaque, then water blended over
         * everything */
        for(int pass = 0; pass < 3; pass++)
            for(auto const& [mi, mat] : stl_types::enumerate(scene.materials))
            {
                int const mat_pass = mat.sky ? 0 : (mat.water || mat.additive) ? 2 : 1;
                /* Glass, decals: nothing sensible to draw them with yet */
                if(mat_pass != pass || mat.section_draws.empty() ||
                   !mat.texture)
                    continue;
                auto& draws = m_draws[mi];
                draws.clear();
                for(auto const& d : mat.section_draws)
                    if(vis.draw_visible(d.tag, scene.sections))
                        draws.push_back(d.cmd);
                draws_total += mat.section_draws.size();
                draws_visible += draws.size();
                if(draws.empty())
                    continue;
                Vecf3 offset = mat.sky ? camera->camera.position : Vecf3{};
                Vecf3 const& eye = camera->camera.position;
                f32   scale  = mat.sky ? scene.sky_scale : 1.f;
                auto  vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"},
                        semantic::SpanOne<const Matf4>(camera->matrix)},
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
                    gfx::uniform_pair<Vecf3 const>{
                        {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                i32  alpha_test = mat.alpha_test ? 1 : 0;
                auto frag_u     = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair<const i32>{
                        {"alpha_test"},
                        semantic::SpanOne<const i32>(alpha_test)},
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
                        semantic::SpanOne<const Vecf2>(vis.planar_fog.opaque)});
                auto lm_sampler = scene.lightmap_sampler
                                      ? scene.lightmap_sampler
                                      : m_no_lightmaps_sampler;
                auto samplers = gfx::make_sampler_list(
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"diffuse", 0},
                        mat.sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail", 1},
                        mat.detail.sampler ? mat.detail.sampler : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail2", 2},
                        mat.detail2.sampler ? mat.detail2.sampler
                                            : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"illum", 3},
                        mat.illum.sampler ? mat.illum.sampler : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"lightmaps", 4},
                        lm_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"mask", 5},
                        mat.mask.sampler ? mat.mask.sampler : m_white_sampler,
                    });
                auto res = m_api->submit(
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
                    gfx::view_state{
                        .view = typing::vector_types::Veci4{
                            0,
                            0,
                            resources.offscreen_size.x,
                            resources.offscreen_size.y},
                        .depth = gfx::depth_state{.reversed = true},
                    },
                    gfx::depth_extended_state{
                        .depth_write = !mat.sky && !mat.water && !mat.additive},
                    gfx::blend_state{.additive = mat.additive});
                if(res.has_value() && !m_draw_error_logged)
                {
                    cWarning("H2: failed to draw: {}", std::get<1>(*res));
                    m_draw_error_logged = true;
                }
            }
        if(vis.camera_cluster != m_logged_cluster)
        {
            cDebug("H2: drew {} of {} ranges", draws_visible, draws_total);
            m_logged_cluster = vis.camera_cluster;
        }
    }

  private:
    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;

        m_vao = api.alloc_vertex_array();
        m_vao->alloc();
        m_vao->add(
            gfx::vertex_attribute{
                .index = 0,
                .value = {.offset = 0, .stride = sizeof(Vertex), .count = 3},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 1,
                .value =
                    {.offset = offsetof(Vertex, texcoord),
                     .stride = sizeof(Vertex),
                     .count  = 2},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 2,
                .value =
                    {.offset = offsetof(Vertex, normal),
                     .stride = sizeof(Vertex),
                     .count  = 3},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 3,
                .value =
                    {.offset = offsetof(Vertex, lightmap),
                     .stride = sizeof(Vertex),
                     .count  = 3},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 4,
                .value =
                    {.offset = offsetof(Vertex, light),
                     .stride = sizeof(Vertex),
                     .count  = 4,
                     .type   = semantic::type_t::u8,
                     /* packed: a float input, never an integer attribute */
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


        /* Stand-in for materials without a usable diffuse map */
        m_white = api.alloc_texture(
            gfx::textures::d2,
            typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8),
            1);
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
    std::shared_ptr<gfx::texture_2d_t>          m_white;
    std::shared_ptr<gfx::sampler_t>             m_white_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> m_no_lightmaps;
    std::shared_ptr<gfx::sampler_t>             m_no_lightmaps_sampler;
    /* Per material: this frame's visible ranges */
    std::vector<std::vector<gfx::draw_command::data_t>> m_draws;
    bool m_draw_error_logged{false};
};

void alloc_h2_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H2Renderer>(std::ref(e));
}

} // namespace h2
