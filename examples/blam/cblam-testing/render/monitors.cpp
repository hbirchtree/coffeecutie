#include "render/monitors.h"

#include "components.h"
#include "crunched/loading_screen.h"
#include "graphics_api.h"
#include "render/rendering.h"

#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_debug.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>
#include <peripherals/stl/accumulate.h>

#include <array>
#include <optional>
#include <vector>

using CameraMonitorsManifest = compo::SubsystemManifest<
    type_list_t<const PlayerCamera, const PlayerInfo>,
    empty_list_t,
    empty_list_t>;

/* The monitor model at each player camera, remote ones included, from the
 * crunched mesh */
struct CameraMonitors
    : compo::RestrictedSubsystem<CameraMonitors, CameraMonitorsManifest>
    , RenderHook
{
    using type  = CameraMonitors;
    using Proxy = compo::proxy_of<CameraMonitorsManifest>;

    CameraMonitors(gfx::api* api)
        : m_api(api)
    {
        this->priority = 3060;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    struct monitor_t
    {
        std::optional<u32> seat_idx; /*!< Local seat, to skip its own view */
        Matf4              transform;
    };

    struct monitor_part_t
    {
        gfx::draw_command::data_t draw;
        Vecf3                     color;
    };

    /* OBJ units are ~0.6 in radius; this makes it ~0.2 world units across */
    static constexpr f32 monitor_scale = 0.15f;

    gfx::api*                            m_api;
    std::vector<monitor_t>               m_monitors;
    std::vector<monitor_part_t>          monitor_parts;
    std::shared_ptr<gfx::buffer_t>       monitor_ebo;
    std::shared_ptr<gfx::buffer_t>       monitor_vbo;
    std::shared_ptr<gfx::vertex_array_t> monitor_vao;
    std::shared_ptr<gfx::program_t>      monitor_program;
    bool                                 monitor_failed{false};

    void start_restricted(Proxy& p, time_point const&)
    {
        m_monitors.clear();
        for(auto const entity : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = entity.components();
            if(!info.is_remote() && !cam.is_active())
                continue;
            // Skip monitor render when physics mode is on for player
            if(info.mode.physics)
                continue;
            /* Same basis graphics.cpp folds into the view matrix */
            static const Matf4 bsp_basis{
                {0, 0, 1, 0},
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 1},
            };
            /* Camera-to-world is the inverse of the view rotation; the
             * monitor's eye faces +X in the OBJ, turned onto camera -Z */
            Matf4 rotation = glm::mat4_cast(cam.camera.rotation) * bsp_basis;
            m_monitors.push_back({
                .seat_idx = info.is_remote()
                                ? std::nullopt
                                : std::optional<u32>(info.seat_idx),
                .transform =
                    glm::translate(Matf4(1), cam.camera.position) *
                    glm::transpose(rotation) *
                    glm::rotate(Matf4(1), glm::half_pi<f32>(), Vecf3{0, 1, 0}) *
                    glm::scale(Matf4(1), Vecf3(monitor_scale)),
            });
        }
    }

    bool load_monitor()
    {
        if(monitor_program)
            return true;
        if(monitor_failed)
            return false;

        auto _ = m_api->debug().scope("CameraMonitors::load");

        struct mesh_source_t
        {
            gsl::span<const u16>            indices;
            gsl::span<const crunch::vertex> vertices;
            Vecf3                           color;
        };

        std::array<mesh_source_t, 5> meshes = {{
            /* Shell, core, fins, eye */
            {gsl::span(blam::loading::monitor_sphere_001_indices),
             gsl::span(blam::loading::monitor_sphere_001_vertices),
             Vecf3{0.55f, 0.57f, 0.6f}},
            {gsl::span(blam::loading::monitor_icosphere_indices),
             gsl::span(blam::loading::monitor_icosphere_vertices),
             Vecf3{0.15f, 0.16f, 0.18f}},
            {gsl::span(blam::loading::monitor_cube_indices),
             gsl::span(blam::loading::monitor_cube_vertices),
             Vecf3{0.4f, 0.42f, 0.45f}},
            {gsl::span(blam::loading::monitor_cube_001_indices),
             gsl::span(blam::loading::monitor_cube_001_vertices),
             Vecf3{0.4f, 0.42f, 0.45f}},
            {gsl::span(blam::loading::monitor_cube_002_indices),
             gsl::span(blam::loading::monitor_cube_002_vertices),
             Vecf3{0.2f, 0.6f, 1.f}},
        }};

        monitor_ebo =
            m_api->alloc_buffer(gfx::buffers::element, semantic::RSCA::ReadOnly);
        monitor_vbo = m_api->alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        monitor_ebo->alloc();
        monitor_vbo->alloc();
        monitor_ebo->commit(
            stl_types::accumulate(meshes, 0u, [](auto const& m, u32 size) {
                return m.indices.size_bytes() + size;
            }));
        monitor_vbo->commit(
            stl_types::accumulate(meshes, 0u, [](auto const& m, u32 size) {
                return m.vertices.size_bytes() + size;
            }));
        size_t ebo_ptr{0}, vbo_ptr{0};
        for(auto const& mesh : meshes)
        {
            monitor_ebo->update(ebo_ptr, mesh.indices);
            monitor_vbo->update(vbo_ptr, mesh.vertices);
            monitor_parts.push_back({
                .draw =
                    {
                        .elements =
                            {
                                .count  = static_cast<u32>(mesh.indices.size()),
                                .offset = ebo_ptr,
                                .vertex_offset =
                                    vbo_ptr / sizeof(crunch::vertex),
                                .type = semantic::type_t::u16,
                            },
                    },
                .color = mesh.color,
            });
            ebo_ptr += mesh.indices.size_bytes();
            vbo_ptr += mesh.vertices.size_bytes();
        }

        monitor_vao = m_api->alloc_vertex_array();
        monitor_vao->alloc();
        monitor_vao->add(
            gfx::vertex_attribute::from_member(&crunch::vertex::position)
                .at(0));
        monitor_vao->add(gfx::vertex_attribute::from_member(
                             &crunch::vertex::normal, gfx::vertex_float_type)
                             .at(1));
        monitor_vao->set_attribute_names({
            {"pos", 0},
            {"normal", 1},
        });
        monitor_vao->force_attribute_names();
        monitor_vao->set_buffer(gfx::buffers::element, monitor_ebo);
        monitor_vao->set_buffer(gfx::buffers::vertex, monitor_vbo, 0);

        auto program = m_api->alloc_program();
        program->add(
            gfx::program_t::stage_t::Vertex,
            m_api->alloc_shader(semantic::mem_chunk<const char>::ofContainer(
                blam::loading::monitor_vert)));
        program->add(
            gfx::program_t::stage_t::Fragment,
            m_api->alloc_shader(semantic::mem_chunk<const char>::ofContainer(
                blam::loading::monitor_frag)));
        if(auto res = program->compile(); res.has_error())
        {
            cWarning("Failed to compile monitor program: {}", res.error());
            monitor_failed = true;
            return false;
        }
        monitor_program = program;

        m_api->debug().annotate(*monitor_ebo, "monitor_ebo");
        m_api->debug().annotate(*monitor_vbo, "monitor_vbo");
        m_api->debug().annotate(*monitor_vao, "monitor_vao");
        return true;
    }

    /* Opaque, after the world so they depth test */
    void draw(stage_t, view_t const& view) override
    {
        using namespace std::string_view_literals;
        if(m_monitors.empty() || !load_monitor())
            return;

        auto _ = m_api->debug().scope("Monitors");
        Coffee::ProfContext __("CameraMonitors::draw");

        for(auto const& monitor : m_monitors)
        {
            /* Don't draw a monitor around the camera looking out of it */
            if(monitor.seat_idx == view.seat)
                continue;
            for(auto const& part : monitor_parts)
            {
                gfx::view_state    state   = view.state;
                gfx::stencil_state stencil = view.stencil;
                auto               res     = m_api->submit(
                    {
                        .program       = monitor_program,
                        .vertices      = monitor_vao,
                        .render_target = view.target,
                        .call =
                            {
                                .indexed = true,
                                .mode    = gfx::drawing::primitive::triangle,
                            },
                        .data = {part.draw},
                    },
                    gfx::make_uniform_list(
                        typing::graphics::ShaderStage::Vertex,
                        gfx::uniform_pair{
                            {"camera"sv}, semantic::SpanOne(view.camera)},
                        gfx::uniform_pair{
                            {"model"sv}, semantic::SpanOne(monitor.transform)},
                        gfx::uniform_pair{
                            {"camera_position"sv},
                            semantic::SpanOne<const Vecf3>(view.position)}),
                    gfx::make_uniform_list(
                        typing::graphics::ShaderStage::Fragment,
                        gfx::uniform_pair{
                            {"color"sv},
                            semantic::SpanOne<const Vecf3>(part.color)}),
                    state,
                    stencil);
                if(res)
                {
                    auto [err, msg] = *res;
                    cWarning(
                        "Failed to draw monitor: {}: {}",
                        gleam::detail::draw_error_to_string(err),
                        msg);
                    return;
                }
            }
        }
    }
};

void alloc_monitors(compo::EntityContainer& container)
{
    auto& monitors = container.register_subsystem_inplace<CameraMonitors>(
        &container.subsystem_cast<gfx::system>());
    container.subsystem_cast<RenderHooks>().add(RenderHook::AfterOpaque, &monitors);
}
