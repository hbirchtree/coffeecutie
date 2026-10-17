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
        ensure_depth_sampler(resources);
        Matf4 const inverse_camera = glm::inverse(camera->matrix);
        Vecf2 const viewport{resources.offscreen_size};
        /* The water fan goes around the camera at sea level: the section's
         * planar fog plane when it lies flat, lifted a little so sand at the
         * waterline sinks under it and fades in */
        Vecf2 const water_eye{camera->camera.position};
        f32 const   sea_level = vis.planar_fog.plane.z > .9f && vis.planar_fog.plane.w > -1e8f
                                    ? vis.planar_fog.plane.w / vis.planar_fog.plane.z + .05f
                                    : 0.f;
        Vecf3 const sun_direction =
            scene.sky_flares.empty() ? Vecf3{0.f, 0.f, 1.f} : scene.sky_flares[0].direction;
        Vecf3 const sun_color =
            scene.sky_flares.empty() ? Vecf3{0.f} : scene.sky_flares[0].color;
        Vecf4 const sun_spot{scene.water.sun_spot, scene.water.sun_power};
        /* Below the horizon, where the sky model ends */
        resources.offscreen->clear(Vecf4(scene.sky_clear, 1.f));
        size_t draws_total = 0, draws_visible = 0;
        /* Sky first (never writes depth), opaque, then water blended over
         * everything */
        for(int pass = 0; pass < 4; pass++)
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
                m_moving.clear();
                for(auto const& d : mat.section_draws)
                    if(!vis.draw_visible(d.tag, scene.sections))
                        continue;
                    else if(d.tag.object >= 0)
                        m_moving.push_back(d);
                    else
                        draws.push_back(d.cmd);
                draws_total += mat.section_draws.size();
                draws_visible += draws.size() + m_moving.size();
                if(draws.empty() && m_moving.empty())
                    continue;
                Matf4 model_matrix{1.f};
                Vecf2 const water_fan{mat.water_fan ? 1.f : 0.f, sea_level};
                Vecf3 offset = mat.sky ? camera->camera.position : Vecf3{};
                Vecf3 const& eye = camera->camera.position;
                f32   scale  = mat.sky ? scene.sky_scale : 1.f;
                auto  vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"},
                        semantic::SpanOne<const Matf4>(camera->matrix)},
                    gfx::uniform_pair<Matf4 const>{
                        {"model"}, semantic::SpanOne<const Matf4>(model_matrix)},
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
                auto frag_u     = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair<const i32>{
                        {"alpha_test"},
                        semantic::SpanOne<const i32>(alpha_test)},
                    gfx::uniform_pair<const i32>{
                        {"bumped"}, semantic::SpanOne<const i32>(bumped)},
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
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"bump", 6},
                        mat.bump.sampler ? mat.bump.sampler : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"env", 7},
                        mat.env_sampler ? mat.env_sampler : m_black_cube_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"scene_depth", 8},
                        m_depth_sampler,
                    });
                auto submit = [&](std::vector<gfx::draw_command::data_t> const& data) {
                    auto res = m_api->submit(
                        {
                            .program = m_programs[static_cast<size_t>(mat.variant)],
                            .vertices      = m_vao,
                            .render_target = resources.offscreen,
                            .call          = {.indexed = true, .instanced = true},
                            .data          = data,
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
                            .depth_write = !mat.sky && !mat.water && !mat.additive &&
                                           !mat.blended},
                        gfx::blend_state{.additive = mat.additive});
                    if(res.has_value() && !m_draw_error_logged)
                    {
                        cWarning("H2: failed to draw: {}", std::get<1>(*res));
                        m_draw_error_logged = true;
                    }
                };
                if(!draws.empty())
                    submit(draws);
                for(auto const& d : m_moving)
                {
                    auto idx = static_cast<size_t>(d.tag.object);
                    if(idx >= scene.moving_objects.size())
                        continue;
                    auto const* model = p.template get<Model>(scene.moving_objects[idx]);
                    if(!model)
                        continue;
                    model_matrix = model->transform;
                    m_single.assign(1, d.cmd);
                    submit(m_single);
                }
            }
        draw_flares(scene, *camera, resources);
        if(vis.camera_cluster != m_logged_cluster)
        {
            cDebug("H2: drew {} of {} ranges", draws_visible, draws_total);
            m_logged_cluster = vis.camera_cluster;
        }
    }

  private:
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
    void ensure_depth_sampler(BlamResources const& resources)
    {
        if(m_depth_source == resources.depth)
            return;
        m_depth_source  = resources.depth;
        m_depth_sampler = resources.depth->sampler();
        m_depth_sampler->alloc();
        m_depth_sampler->set_filtering(
            typing::Filtering::Nearest, typing::Filtering::Nearest);
    }

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

        m_black_cube = api.alloc_texture(
            gfx::textures::cube,
            typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8),
            1);
        m_black_cube->alloc(size_3d<u32>{1, 1, 1});
        {
            std::array<u8, 4>             black{0, 0, 0, 255};
            std::array<Span<const u8>, 6> faces;
            faces.fill(Span<const u8>(black.data(), black.size()));
            m_black_cube->upload(faces, Veci2{0, 0}, Veci2{1, 1}, 0);
        }
        m_black_cube_sampler = m_black_cube->sampler();
        m_black_cube_sampler->alloc();

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
    std::shared_ptr<gfx::texture_cube_t>        m_black_cube;
    std::shared_ptr<gfx::sampler_t>             m_black_cube_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> m_no_lightmaps;
    std::shared_ptr<gfx::sampler_t>             m_no_lightmaps_sampler;
    /* Per material: this frame's visible ranges */
    std::vector<std::vector<gfx::draw_command::data_t>> m_draws;
    std::vector<Material::draw_t>                       m_moving;
    std::vector<gfx::draw_command::data_t>              m_single;
    bool m_draw_error_logged{false};

    std::shared_ptr<gfx::program_t>              m_flare_program;
    std::shared_ptr<gfx::vertex_array_t>         m_flare_vao;
    std::shared_ptr<gfx::buffer_t>               m_flare_buf;
    std::vector<FlareVertex>                     m_flare_vertices;
    std::vector<std::shared_ptr<gfx::sampler_t>> m_flare_samplers;
    std::vector<Vecf2>                           m_flare_light;
    std::shared_ptr<gfx::texture_2d_t>           m_depth_source;
    std::shared_ptr<gfx::sampler_t>              m_depth_sampler;
};

void alloc_h2_renderer(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H2Renderer>(std::ref(e));
}

} // namespace h2
