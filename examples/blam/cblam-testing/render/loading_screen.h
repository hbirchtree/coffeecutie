#pragma once

#include "crunched/crunch_types.h"
#include "crunched/loading_screen.h"
#include "render/rendering.h"

#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/constants.h>
#include <peripherals/stl/accumulate.h>
#include <peripherals/stl/enumerate.h>
#include <peripherals/stl/map_values.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>
#include <peripherals/typing/geometry/rect.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

using semantic::RSCA;
using namespace std::string_view_literals;

void LoadingScreen::end_restricted(Proxy& e, const time_point& time)
{
    using namespace std::chrono_literals;

    gfx::system*   api;
    ScreenClear*   screen_clear;
    LoadingStatus* status;
    e.subsystem(api);
    e.subsystem(screen_clear);
    e.subsystem(status);

    if(!status->loading)
    {
        if(!was_loading)
            return;
        if(loading_screen_gone_time.has_value() &&
           time > *loading_screen_gone_time)
        {
            was_loading = false;
            loading_screen_gone_time.reset();
            frames_since_loaded = 0;
            return;
        }
        if(frames_since_loaded < scene_frames_before_fade)
            frames_since_loaded++;
        else if(!loading_screen_gone_time.has_value())
            loading_screen_gone_time = time + 2s;
    }

    if(!loading_program)
        load_resources(*api);

    auto _ = api->debug().scope("LoadingScreen::end_restricted");

    auto screen_aspect =
        e.service<comp_app::GraphicsFramebuffer>()->size().aspect();
    f32 timef = std::chrono::duration_cast<stl_types::chrono::seconds_f32>(
                    time.time_since_epoch())
                    .count();

    render_ring_texture(*api);

    // Full-screen pass: radial blur over the baked ring, standing in for the
    // original's accumulation buffer. The quad's first four vertices are the
    // full-screen one, in 0..1, so map them onto clip space.
    Matf4 zoom_transform = glm::scale(
        glm::translate(glm::identity<Matf4>(), Vecf3{-1, -1, 0}),
        Vecf3{2, 2, 1});
    Vecf2 zoom_offset{0.f, 0.f};
    // The original swept the convergence point slowly side to side; timef is
    // still seconds here, before the spinner reuses it. Its resting place was
    // measured off loading.dds at (0.95, 0.20).
    constexpr f32   sweep_period = 12.f; // seconds for a full cycle
    constexpr f32   sweep_extent = 0.5f; // how far either side of rest
    constexpr Vecf2 sweep_rest{0.5f, 0.5f};
    Vecf2           zoom_center{
        sweep_rest.x + sweep_extent * std::sin(
                                          timef * (stl_types::math::pi_f * 2.f /
                                                   sweep_period) -
                                          stl_types::math::pi_f / 2.f),
        sweep_rest.y};
    f32 zoom_strength{0.5f};
    f32 zoom_exposure{2.5f};
    // An ellipse rather than a disc: narrow across x so the sweep reads as a
    // travelling band, tall enough in y to reach past both screen edges. The
    // 0.6 exponent is the decay measured off the original.
    Vecf2 falloff_extent{0.1f, 1.f};
    f32   falloff_power{0.6f};

    auto zoom_res = api->submit(
        gfx::draw_command{
            .program  = zoom_program,
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
            gfx::uniform_pair{
                {"transform"sv}, semantic::SpanOne(zoom_transform)},
            gfx::uniform_pair{
                {"tex_offset"sv}, semantic::SpanOne(zoom_offset)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"zoom_center"sv}, semantic::SpanOne(zoom_center)},
            gfx::uniform_pair{
                {"zoom_strength"sv}, semantic::SpanOne(zoom_strength)},
            gfx::uniform_pair{{"exposure"sv}, semantic::SpanOne(zoom_exposure)},
            gfx::uniform_pair{
                {"falloff_extent"sv}, semantic::SpanOne(falloff_extent)},
            gfx::uniform_pair{
                {"falloff_power"sv}, semantic::SpanOne(falloff_power)}),
        gfx::make_sampler_list(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source"},
                ring_color_sampler,
            }),
        gfx::blend_state{});
    if(zoom_res)
    {
        auto [err, msg] = *zoom_res;
        cWarning(
            "Failed to draw loading zoom: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }

    if(!status->loading)
        return;

    was_loading         = true;
    frames_since_loaded = 0;

    Matf4 transform = glm::translate(
        // glm::scale(glm::identity<Matf4>(), glm::vec3(0.2f)),
        glm::scale(
            glm::identity<Matf4>(), glm::vec3(0.2f / screen_aspect, 0.2f, 1)),
        glm::vec3(1 / (0.2f / screen_aspect) - 1, -5, 0));
    timef     = std::fmod(timef, 10.f) / 10.f;
    f32 start = timef;
    f32 end   = std::fmod(timef + 0.2f, 1.f);

    auto res = api->submit(
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
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;
    using semantic::SpanOver;

    auto __ = api.debug().scope("LoadingScreen::load_resources");

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
    };

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

    loading_program = api.alloc_program();
    loading_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_vert)));
    loading_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_spinner)));
    if(auto res = loading_program->compile(); res.has_error())
        cWarning("Error compiling loader shader: {}", res.error());

    loading_ebo  = api.alloc_buffer(gfx::buffers::element, RSCA::ReadOnly);
    loading_vbo  = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    loading_vao  = api.alloc_vertex_array();
    ring_program = api.alloc_program();
    machine_tex  = api.alloc_texture(
        gfx::textures::d2,
        PixDesc(pix_fmt::RGB565),
        1,
        gfx::textures::property::sync_upload);
    simplex_noise_tex =
        api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::R8), 1);

    loading_ebo->alloc();
    loading_vbo->alloc();

    struct mesh_source_t
    {
        model_name_t                    name;
        gsl::span<const u16>            indices;
        gsl::span<const crunch::vertex> vertices;
    };

    // Named explicitly; object order in the OBJ is not model_name_t order
    std::array<mesh_source_t, 3> meshes = {
        {{
             model_ring_exterior,
             gsl::span(blam::loading::loading_torus_indices),
             gsl::span(blam::loading::loading_torus_vertices),
         },
         {
             model_ring_interior,
             gsl::span(blam::loading::loading_cylinder_indices),
             gsl::span(blam::loading::loading_cylinder_vertices),
         },
         {
             model_threshold,
             gsl::span(blam::loading::loading_sphere_indices),
             gsl::span(blam::loading::loading_sphere_vertices),
         }}};
    loading_ebo->commit(
        stl_types::accumulate(meshes, 0u, [](auto const& m, u32 size) {
            return m.indices.size_bytes() + size;
        }));
    loading_vbo->commit(
        stl_types::accumulate(meshes, 0u, [](auto const& m, u32 size) {
            return m.vertices.size_bytes() + size;
        }));
    size_t ebo_ptr{0}, vbo_ptr{0};
    for(auto const& mesh : meshes)
    {
        loading_ebo->update(ebo_ptr, mesh.indices);
        loading_vbo->update(vbo_ptr, mesh.vertices);
        loading_draws[mesh.name] = {
            .elements =
                {
                    .count = static_cast<u32>(mesh.indices.size()),
                    // offset is in bytes, vertex_offset is in vertices
                    .offset        = ebo_ptr,
                    .vertex_offset = vbo_ptr / sizeof(crunch::vertex),
                    .type          = semantic::type_t::u16,
                },
            .debug_identifier = std::string(magic_enum::enum_name(mesh.name)),
        };
        ebo_ptr += mesh.indices.size_bytes();
        vbo_ptr += mesh.vertices.size_bytes();
    }
    loading_draws[model_ring_exterior].instances.offset = 0;
    loading_draws[model_ring_interior].instances.offset = 1;
    loading_draws[model_threshold].instances.offset     = 2;
    loading_vao->alloc();
    loading_vao->add(
        gfx::vertex_attribute::from_member(&crunch::vertex::position).at(0));
    loading_vao->add(
        gfx::vertex_attribute::from_member(&crunch::vertex::texcoord).at(1));
    loading_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    // The scene shaders declare attributes by name, without locations
    loading_vao->force_attribute_names();
    loading_vao->set_buffer(gfx::buffers::element, loading_ebo);
    loading_vao->set_buffer(gfx::buffers::vertex, loading_vbo, 0);

    machine_tex->alloc(size_3d<u32>(64, 128, 1));
    machine_tex->upload(
        blam::loading::machinery_data, Veci2{0, 0}, size_2d<i32>{64, 128});
    simplex_noise_tex->alloc(size_3d<u32>(128, 128, 1));
    simplex_noise_tex->upload(
        blam::loading::threshold_noise_data,
        Veci2{0, 0},
        size_2d<i32>{128, 128});
    simplex_noise_sampler = simplex_noise_tex->sampler();
    simplex_noise_sampler->alloc();

    loading_textures = {
        machine_tex,
        simplex_noise_tex,
        simplex_noise_tex,
    };

    ring_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_scene_vert)));
    ring_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_scene_frag)));
    if(auto res = ring_program->compile(); res.has_error())
    {
        cWarning("Failed to compile loading scene program: {}", res.error());
    }

    ring_color =
        api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::RGB565), 1);
    ring_color->alloc(size_3d<u32>{640, 360, 1});
    ring_color_sampler = ring_color->sampler();
    ring_color_sampler->alloc();
    ring_rt = api.alloc_rendertarget();
    ring_rt->alloc();
    ring_rt->attach(gfx::render_targets::attachment::color, *ring_color, 0);
    // Deeper than 16-bit where the driver offers it; the ring's inner and
    // outer surfaces are under half a unit apart at ~100 units out
    ring_rt->attach_renderbuffer(
        gfx::render_targets::attachment::depth,
        PixDesc(api.feature_info().rendertarget.high_precision_depth_format));
    ring_rt->resize(typing::geometry::rect<i32>{0, 0, 640, 360});

    // Radial blur over the baked ring texture, standing in for the original's
    // accumulation buffer
    zoom_program = api.alloc_program();
    zoom_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_vert)));
    zoom_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_zoom_frag)));
    if(auto res = zoom_program->compile(); res.has_error())
    {
        cWarning("Failed to compile loading zoom program: {}", res.error());
    }

    api.debug().annotate(*ring_color, "rendered_loading_screen");
    api.debug().annotate(*ring_rt, "rendered_loading_screen_render_target");
    api.debug().annotate(*loading_ebo, "loading_screen_ebo");
    api.debug().annotate(*loading_vao, "loading_screen_vao");
    api.debug().annotate(*loading_vbo, "loading_screen_vbo");
    api.debug().annotate(*machine_tex, "loading_machine_tex");
    api.debug().annotate(*simplex_noise_tex, "loading_simplex_tex");
    api.debug().annotate(*simplex_noise_sampler, "loading_simplex_sampler");
    api.debug().annotate(*zoom_program, "radial_zoom_shader");
}

void LoadingScreen::render_ring_texture(gfx::system& api)
{
    if(m_ring_rendered)
        return;

    // Blender is Z-up while the exported mesh is Y-up, so a Blender position
    // (x, y, z) belongs here as (x, z, -y): (5.3, 14.6, 18.4) becomes the
    // below. The camera sits inside the ring (67.7 units from a centre of
    // radius 87.9) and looks along it, which is what gives the surface its
    // receding sweep; aiming across the band instead just fills the frame.
    Matf4 camera =
        glm::translate(glm::identity<Matf4>(), Vecf3(5.30f, 18.40f, -14.60f)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(175.054f), Vecf3(0, 0, 1)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(-18.222f), Vecf3(0, 1, 0)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(-158.379f), Vecf3(1, 0, 0));

    // A camera transform is a world transform; the view matrix is its inverse
    Matf4 scene_transform =
        // Near plane dominates depth precision, and the closest geometry is
        // 8.6 units away: at 0.1 a 16-bit buffer resolves only ~1.5 units at
        // ring distance, while the ring's two surfaces come within 0.48, so
        // they z-fight. 5.0 clips nothing and resolves ~0.03 units.
        glm::perspective(glm::radians(60.f), 1.77f, 5.f, 350.f) *
        glm::inverse(camera);

    ring_rt->clear(1.f);
    ring_rt->clear(Vecf4{0, 0, 0, 1});
    auto res = api.submit(
        gfx::draw_command{
            .program       = ring_program,
            .vertices      = loading_vao,
            .render_target = ring_rt,
            .call =
                {
                    .indexed = true,
                    .mode    = gfx::drawing::primitive::triangle,
                },
            .data = stl_types::values(loading_draws),
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"transform"sv}, semantic::SpanOne(scene_transform)}),
        gfx::base_instance_sampler_list{
            .slots =
                {
                    gfx::base_instance_sampler_t{
                        .stage    = gfx::program_t::stage_t::Fragment,
                        .location = gfx::uniform_key{"surface"},
                        .sampler  = simplex_noise_sampler,
                        .textures = std::span(
                            loading_textures.begin(), loading_textures.end()),
                    },
                },
            .first_unit = 0,
        },
        gfx::view_state{
            .view  = typing::vector_types::Veci4{0, 0, 640, 360},
            .depth = gfx::depth_state{},
        });
    if(res)
    {
        auto [err, msg] = *res;
        cWarning(
            "Failed to draw loading scene: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }

    // Observed bug on ANGLE-over-GL:
    // Rendering to texture then using the resulting texture turns up
    // black. Running with sync such as glGetError causes it to show up
    // so there's some bizarre sync issue here.
    if constexpr(compile_info::platform::is_emscripten)
        return;

    // Everything below is released, so this must not run a second time
    m_ring_rendered = true;

    ring_rt->dealloc();
    ring_program->dealloc();
    loading_ebo->dealloc();
    loading_vbo->dealloc();
    loading_vao->dealloc();
    machine_tex->dealloc();
    simplex_noise_tex->dealloc();
    simplex_noise_sampler->dealloc();
    loading_textures.clear();
    loading_draws.clear();
}
