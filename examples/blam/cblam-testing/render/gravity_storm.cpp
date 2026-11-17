#include "render/gravity_storm.h"

#include "loading/blam_files.h"
#include "components.h"
#include "crunched/loading_screen.h"
#include "graphics_api.h"
#include "loading/loading.h"
#include "render/rendering.h"
#include "selected_version.h"

#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi_debug.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>

#include <algorithm>
#include <array>
#include <map>
#include <vector>

namespace {

/* A unit quad in its XY plane, facing +Z; the rings draw on it */
const std::array<crunch::vertex, 4> ring_quad_vertices = {{
    {{-1.f, -1.f, 0.f}, {0.f, 0.f}, {255, 255, 255, 255}, {32768, 32768, 65535}},
    {{1.f, -1.f, 0.f}, {1.f, 0.f}, {255, 255, 255, 255}, {32768, 32768, 65535}},
    {{1.f, 1.f, 0.f}, {1.f, 1.f}, {255, 255, 255, 255}, {32768, 32768, 65535}},
    {{-1.f, 1.f, 0.f}, {0.f, 1.f}, {255, 255, 255, 255}, {32768, 32768, 65535}},
}};
constexpr std::array<u16, 6> ring_quad_indices = {{0, 1, 2, 0, 2, 3}};

} // namespace

template<typename V>
using GravityStormManifest = compo::SubsystemManifest<
    type_list_t<
        const Model,
        const HeldObject,
        const Attachment,
        const BipedEquipment,
        const PlayerInfo,
        const PlayerCamera,
        const PlayerInput>,
    type_list_t<const BlamFiles<V>, const LoadingStatus>,
    empty_list_t>;

/* A gravity rifle on the ground or in a player's hand calls down a storm: a
 * funnel of cloud in the sky over it and red wind around it, crunched meshes.
 * Holstered, it calms. Holding its trigger charges it: a wall of gold rings
 * rises before its wielder, one more the longer it is held. */
template<typename V>
struct GravityStorm
    : compo::RestrictedSubsystem<GravityStorm<V>, GravityStormManifest<V>>
    , RenderHook
{
    using type  = GravityStorm<V>;
    using Proxy = compo::proxy_of<GravityStormManifest<V>>;

    GravityStorm(gfx::api* api)
        : m_api(api)
    {
        this->priority = 3060;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    struct gravity_storm_t
    {
        Vecf3      rifle;    /*!< On the ground, or its wielder */
        Vecf3      position; /*!< Trails the rifle: the sky and wide winds */
        time_point start;
        f32        strength{0.f};
        bool       found{false}; /*!< Its rifle is still about this frame */
    };

    struct storm_wind_t
    {
        f32 width;  /*!< Scale across, the cone is ~1 unit in radius */
        f32 height; /*!< Scale up, the cone is ~1.7 units tall */
        f32 spin;   /*!< rad/s about the vertical */
        f32 delay;  /*!< s after the storm starts before it rises */
        f32 bands;  /*!< Streak bands up the cone */
        f32 density{1.f}; /*!< More, fuller wisps per band */
        bool hugs{false}; /*!< On the rifle itself, not trailing it */
    };

    /* Two tight cones over the rifle, then flat ones sweeping the level */
    static constexpr std::array<storm_wind_t, 5> storm_winds = {{
        {3.f, 3.f, 2.6f, 0.f, 5.f, 1.f, true},
        {4.2f, 3.4f, 1.f, .3f, 8.f, 2.f, true},
        {18.f, 4.f, -1.1f, .6f, 3.f},
        {32.f, 6.f, .8f, 1.2f, 3.f},
        {50.f, 8.f, -.55f, 1.8f, 2.f},
    }};
    static constexpr f32 storm_fade_in  = 1.5f; /* s */
    static constexpr f32 storm_fade_out = 4.f;  /* s, once the rifle is gone */
    /* A rifle this close keeps its storm: picked up, dropped, switched back */
    static constexpr f32 storm_reach = 4.f; /* wu */
    static constexpr f32 storm_trail = 1.5f; /* 1/s */
    /* Cloud rim ~370 wu across, 45 wu over the rifle, trumpet 40 above that */
    static constexpr f32 storm_cloud_scale  = 40.f;
    static constexpr f32 storm_cloud_height = 45.f;

    /* Bolts from the cloud base to the ground, each slot striking anew
     * every period or so somewhere around the storm */
    static constexpr u32 lightning_slots     = 3;
    static constexpr f32 lightning_period    = 2.4f; /* s */
    static constexpr f32 lightning_leader    = .08f; /* s of the strike alone */
    static constexpr f32 lightning_flash     = .35f; /* s of the whole bolt */
    static constexpr f32 lightning_reach_min = 10.f; /* wu from the storm */
    static constexpr f32 lightning_reach_max = 60.f;
    /* Y-up, ground at y = 0, cloud end at y = 16.5 */
    static constexpr f32 lightning_mesh_height = 16.5f;
    /* The mesh is quads A..D, triangulated as tris k and k + 4; A strikes */
    static constexpr u32 lightning_quads = 4;

    /* Rings round a wielder holding the trigger */
    struct ring_charge_t
    {
        Vecf3 centre;    /*!< The wielder's chest */
        Vecf3 forward{1, 0, 0}; /*!< Where they look, level */
        f32   held{0.f}; /*!< s the trigger has been held */
        f32   shown{1.f}; /*!< Falls to 0 once let go */
        bool  firing{false};
        bool  found{false};
    };

    /* Rows fill from the middle out to either side, then stack up */
    static constexpr u32 ring_per_row  = 9;
    static constexpr u32 ring_most     = 3 * ring_per_row;
    static constexpr f32 ring_interval = .2f;  /* s between new rings */
    static constexpr f32 ring_grow     = .3f;  /* s to open up */
    static constexpr f32 ring_fade     = .4f;  /* s to close once let go */
    static constexpr f32 ring_radius   = .25f; /* wu */
    static constexpr f32 ring_spacing  = .6f;  /* wu between centres */
    static constexpr f32 ring_ahead    = 1.5f; /* wu before the wielder */
    static constexpr f32 ring_chest    = .4f;  /* wu over the wielder's feet */

    gfx::api*                            m_api;
    std::vector<gravity_storm_t>         m_storms;
    std::map<u64, ring_charge_t>         m_charges;
    f32                                  m_ring_clock{0.f}; /*!< s, wraps hourly */
    time_point                           m_storm_time{};
    blam::tag_t const*                   m_gravity_rifle{nullptr};
    u32                                  m_gravity_rifle_generation{~0u};
    gfx::draw_command::data_t            storm_cloud_draw;
    gfx::draw_command::data_t            storm_wind_draw;
    std::shared_ptr<gfx::buffer_t>       storm_ebo;
    std::shared_ptr<gfx::buffer_t>       storm_vbo;
    std::shared_ptr<gfx::vertex_array_t> storm_vao;
    std::shared_ptr<gfx::program_t>      storm_cloud_program;
    std::shared_ptr<gfx::program_t>      storm_wind_program;
    gfx::draw_command::data_t            lightning_draw;
    std::vector<gfx::draw_command::data_t> lightning_strike_draw;
    std::shared_ptr<gfx::program_t>      lightning_program;
    std::shared_ptr<gfx::texture_2d_t>   lightning_tex;
    std::shared_ptr<gfx::sampler_t>      lightning_sampler;
    gfx::draw_command::data_t            ring_draw;
    std::shared_ptr<gfx::program_t>      ring_program;
    bool                                 storm_failed{false};
    time_point                           m_now{};

    std::shared_ptr<gfx::program_t> storm_program(
        std::string_view frag,
        std::string_view name,
        std::string_view vert = blam::loading::gravity_storm_vert)
    {
        auto program = m_api->alloc_program();
        program->add(
            gfx::program_t::stage_t::Vertex,
            m_api->alloc_shader(
                semantic::mem_chunk<const char>::ofContainer(vert)));
        program->add(
            gfx::program_t::stage_t::Fragment,
            m_api->alloc_shader(
                semantic::mem_chunk<const char>::ofContainer(frag)));
        if(auto res = program->compile(); res.has_error())
        {
            cWarning("Failed to compile {} program: {}", name, res.error());
            return nullptr;
        }
        return program;
    }

    bool load_storm()
    {
        if(storm_vao)
            return true;
        if(storm_failed)
            return false;

        auto _ = m_api->debug().scope("GravityStorm::load");

        storm_cloud_program =
            storm_program(blam::loading::gravity_cloud_frag, "storm cloud");
        storm_wind_program =
            storm_program(blam::loading::gravity_wind_frag, "storm wind");
        lightning_program =
            storm_program(blam::loading::gravity_lightning_frag, "storm lightning");
        ring_program = storm_program(
            blam::loading::gravity_ring_frag,
            "storm ring",
            blam::loading::gravity_ring_vert);
        if(!storm_cloud_program || !storm_wind_program || !lightning_program ||
           !ring_program)
        {
            storm_failed = true;
            return false;
        }

        auto const& cloud_i = blam::loading::cloudy_cone_indices;
        auto const& cloud_v = blam::loading::cloudy_cone_vertices;
        auto const& wind_i  = blam::loading::cone_cone_indices;
        auto const& wind_v  = blam::loading::cone_cone_vertices;
        auto const& lightning_i  = blam::loading::lightning_plane_indices;
        auto const& lightning_v  = blam::loading::lightning_plane_vertices;

        storm_ebo = m_api->alloc_buffer(gfx::buffers::element, semantic::RSCA::ReadOnly);
        storm_vbo = m_api->alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        storm_ebo->alloc();
        storm_vbo->alloc();
        auto const& ring_i = ring_quad_indices;
        auto const& ring_v = ring_quad_vertices;
        size_t const ring_i_at = gsl::span(cloud_i).size_bytes() +
                                 gsl::span(wind_i).size_bytes() +
                                 gsl::span(lightning_i).size_bytes();
        size_t const ring_v_at = gsl::span(cloud_v).size_bytes() +
                                 gsl::span(wind_v).size_bytes() +
                                 gsl::span(lightning_v).size_bytes();
        storm_ebo->commit(ring_i_at + gsl::span(ring_i).size_bytes());
        storm_vbo->commit(ring_v_at + gsl::span(ring_v).size_bytes());
        storm_ebo->update(0, gsl::span(cloud_i));
        storm_ebo->update(gsl::span(cloud_i).size_bytes(), gsl::span(wind_i));
        storm_ebo->update(gsl::span(cloud_i).size_bytes() + gsl::span(wind_i).size_bytes(), gsl::span(lightning_i));
        storm_vbo->update(0, gsl::span(cloud_v));
        storm_vbo->update(gsl::span(cloud_v).size_bytes(), gsl::span(wind_v));
        storm_vbo->update(gsl::span(cloud_v).size_bytes() + gsl::span(wind_v).size_bytes(), gsl::span(lightning_v));
        storm_ebo->update(ring_i_at, gsl::span(ring_i));
        storm_vbo->update(ring_v_at, gsl::span(ring_v));

        storm_cloud_draw.elements = {
            .count = static_cast<u32>(cloud_i.size()),
            .type  = semantic::type_t::u16,
        };
        storm_wind_draw.elements = {
            .count         = static_cast<u32>(wind_i.size()),
            .offset        = gsl::span(cloud_i).size_bytes(),
            .vertex_offset = static_cast<u32>(cloud_v.size()),
            .type          = semantic::type_t::u16,
        };
        lightning_draw.elements = {
            .count         = static_cast<u32>(lightning_i.size()),
            .offset        = gsl::span(cloud_i).size_bytes() + gsl::span(wind_i).size_bytes(),
            .vertex_offset = static_cast<u32>(cloud_v.size() + wind_v.size()),
            .type          = semantic::type_t::u16,
        };
        ring_draw.elements = {
            .count         = static_cast<u32>(ring_i.size()),
            .offset        = ring_i_at,
            .vertex_offset = static_cast<u32>(
                cloud_v.size() + wind_v.size() + lightning_v.size()),
            .type = semantic::type_t::u16,
        };
        for(u32 tri : {0u, lightning_quads})
        {
            auto& strike = lightning_strike_draw.emplace_back(lightning_draw);
            strike.elements.count = 3;
            strike.elements.offset += tri * 3 * sizeof(u16);
        }

        auto const& bolt = blam::loading::lightning_desc;
        lightning_tex = m_api->alloc_texture(gfx::textures::d2, pix_fmt::RGB565, 1);
        lightning_tex->alloc(size_3d<u32>(bolt.width, bolt.height, 1));
        lightning_tex->upload(
            gsl::span(blam::loading::lightning_data),
            Veci2{0, 0},
            size_2d<i32>(bolt.width, bolt.height));
        lightning_sampler = lightning_tex->sampler();
        lightning_sampler->alloc();

        storm_vao = m_api->alloc_vertex_array();
        storm_vao->alloc();
        storm_vao->add(
            gfx::vertex_attribute::from_member(&crunch::vertex::position)
                .at(0));
        storm_vao->add(
            gfx::vertex_attribute::from_member(&crunch::vertex::texcoord)
                .at(1));
        storm_vao->add(gfx::vertex_attribute::from_member(
                           &crunch::vertex::color, gfx::vertex_float_type)
                           .at(2));
        storm_vao->set_attribute_names({
            {"pos", 0},
            {"tex", 1},
            {"color", 2},
        });
        storm_vao->force_attribute_names();
        storm_vao->set_buffer(gfx::buffers::element, storm_ebo);
        storm_vao->set_buffer(gfx::buffers::vertex, storm_vbo, 0);

        m_api->debug().annotate(*storm_ebo, "storm_ebo");
        m_api->debug().annotate(*storm_vbo, "storm_vbo");
        m_api->debug().annotate(*storm_vao, "storm_vao");
        return true;
    }

    /* Every gravity rifle on the ground or in a player's hand */
    std::vector<Vecf3> find_gravity_rifles(Proxy& p, u32 generation)
    {
        std::vector<Vecf3> rifles;
        for(auto const& entity : p.template select<Model>())
        {
            Model const& model = entity.template get<Model>();
            /* The held and first-person copies count through their wielder */
            if(model.origin_object == m_gravity_rifle &&
               !p.template get<HeldObject>(entity.id()) &&
               !p.template get<Attachment>(entity.id()))
                rifles.push_back(model.position);
        }
        for(auto const& entity : p.template select<BipedEquipment, PlayerInfo>())
        {
            auto const* held =
                entity.template get<BipedEquipment>().held(generation);
            if(!held || held->tag != m_gravity_rifle)
                continue;
            if(auto const* model = p.template get<Model>(entity.id()))
                rifles.push_back(model->position);
            else if(auto const* cam = p.template get<PlayerCamera>(entity.id()))
                rifles.push_back(cam->camera.position);
        }
        return rifles;
    }

    /* A storm follows each gravity rifle, and fades once it is gone */
    void start_restricted(Proxy& p, time_point const& time)
    {
        m_now = time;
        /* The tag index is not there to search until the map is in */
        if(p.template subsystem<LoadingStatus>().loading)
            return;
        auto const& files = p.template subsystem<BlamFiles<V>>();
        if(m_gravity_rifle_generation != files.load_generation)
        {
            m_gravity_rifle_generation = files.load_generation;
            m_gravity_rifle            = nullptr;
            m_storms.clear();
            m_charges.clear();
            if(files.container.map && files.container.tags)
                for(blam::tag_t const& tag :
                    blam::tag_index_view(files.container))
                    if(tag.matches(blam::tag_class_t::weap) &&
                       tag.to_name().to_string(files.container.magic) ==
                           gravity_rifle_tag)
                    {
                        m_gravity_rifle = &tag;
                        break;
                    }
        }

        f32 const dt = std::clamp(
            std::chrono::duration<f32>(time - m_storm_time).count(), 0.f, .1f);
        m_storm_time = time;

        for(auto& storm : m_storms)
            storm.found = false;
        if(m_gravity_rifle)
            for(Vecf3 const& rifle :
                find_gravity_rifles(p, files.load_generation))
            {
                gravity_storm_t* nearest = nullptr;
                f32              best    = storm_reach;
                for(auto& storm : m_storms)
                    if(f32 const d = glm::distance(storm.rifle, rifle);
                       !storm.found && d < best)
                    {
                        best    = d;
                        nearest = &storm;
                    }
                if(!nearest)
                    nearest = &m_storms.emplace_back(gravity_storm_t{
                        .rifle = rifle, .position = rifle, .start = time});
                nearest->rifle = rifle;
                nearest->found = true;
            }

        for(auto& storm : m_storms)
        {
            f32 const rate =
                storm.found ? 1.f / storm_fade_in : -1.f / storm_fade_out;
            storm.strength = std::clamp(storm.strength + rate * dt, 0.f, 1.f);
            storm.position +=
                (storm.rifle - storm.position) * std::min(1.f, storm_trail * dt);
        }
        std::erase_if(m_storms, [](gravity_storm_t const& storm) {
            return !storm.found && storm.strength <= 0.f;
        });

        update_charges(p, files.load_generation, dt);
    }

    /* Each wielder holding the trigger charges; let go, the rings close */
    void update_charges(Proxy& p, u32 generation, f32 dt)
    {
        m_ring_clock = std::fmod(m_ring_clock + dt, 3600.f);
        for(auto& [id, charge] : m_charges)
            charge.found = false;
        if(m_gravity_rifle)
            for(auto const& entity :
                p.template select<BipedEquipment, PlayerInfo, PlayerInput>())
            {
                auto const* held =
                    entity.template get<BipedEquipment>().held(generation);
                if(!held || held->tag != m_gravity_rifle)
                    continue;
                auto const* model = p.template get<Model>(entity.id());
                auto const* cam   = p.template get<PlayerCamera>(entity.id());
                Vecf3       centre;
                Vecf3       forward;
                if(model)
                {
                    centre  = model->position + Vecf3{0, 0, ring_chest};
                    forward = model->rotation * Vecf3{1, 0, 0};
                } else if(cam)
                    centre = cam->camera.position - Vecf3{0, 0, .2f};
                else
                    continue;
                if(cam)
                    forward = glm::transpose(Matf3(cam->rotation)) * Vecf3{0, 0, -1};
                bool const firing =
                    entity.template get<PlayerInput>().intent.fire;
                auto it = m_charges.find(entity.id());
                if(it == m_charges.end())
                {
                    if(!firing)
                        continue;
                    it = m_charges.emplace(entity.id(), ring_charge_t{}).first;
                }
                auto& charge = it->second;
                /* Pulled again while the last rings close: start over */
                if(firing && !charge.firing)
                    charge = ring_charge_t{};
                charge.centre = centre;
                if(Vecf2 const level{forward}; glm::length(level) > 1e-3f)
                    charge.forward = Vecf3(glm::normalize(level), 0.f);
                charge.firing = firing;
                charge.found  = true;
            }
        for(auto& [id, charge] : m_charges)
        {
            if(!charge.found)
                charge.firing = false;
            if(charge.firing)
                charge.held += dt;
            else
                charge.shown -= dt / ring_fade;
        }
        std::erase_if(m_charges, [](auto const& entry) {
            return entry.second.shown <= 0.f;
        });
    }

    /* The storm's strength, each wind tier rising `delay` after the first */
    static f32 storm_envelope(gravity_storm_t const& storm, f32 age, f32 delay)
    {
        f32 const rise = std::clamp((age - delay) / storm_fade_in, 0.f, 1.f);
        f32 const s    = storm.strength;
        return rise * rise * (3.f - 2.f * rise) * s * s * (3.f - 2.f * s);
    }

    void storm_draw(
        view_t const&                          view,
        std::shared_ptr<gfx::program_t> const& program,
        gfx::draw_command::data_t const&       draw,
        Matf4 const&                           model,
        f32                                    time,
        f32                                    strength,
        f32                                    bands,
        f32                                    density)
    {
        using namespace std::string_view_literals;
        gfx::view_state    state   = view.state;
        gfx::stencil_state stencil = view.stencil;
        auto               res     = m_api->submit(
            {
                .program       = program,
                .vertices      = storm_vao,
                .render_target = view.target,
                .call =
                    {
                        .indexed = true,
                        .mode    = gfx::drawing::primitive::triangle,
                    },
                .data = {draw},
            },
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv}, semantic::SpanOne(view.camera)},
                gfx::uniform_pair{{"model"sv}, semantic::SpanOne(model)}),
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"time"sv}, semantic::SpanOne<const f32>(time)},
                gfx::uniform_pair{
                    {"strength"sv}, semantic::SpanOne<const f32>(strength)},
                gfx::uniform_pair{
                    {"bands"sv}, semantic::SpanOne<const f32>(bands)},
                gfx::uniform_pair{
                    {"density"sv}, semantic::SpanOne<const f32>(density)}),
            gfx::blend_state{.premultiplied = true},
            gfx::depth_extended_state{.depth_write = false},
            state,
            stencil);
        if(res)
        {
            auto [err, msg] = *res;
            cWarning(
                "Failed to draw storm: {}: {}",
                gleam::detail::draw_error_to_string(err),
                msg);
        }
    }

    /* One viewport's storms, after the sky */
    void draw(stage_t, view_t const& view) override
    {
        using namespace std::string_view_literals;
        if((m_storms.empty() && m_charges.empty()) || !load_storm())
            return;

        auto _ = m_api->debug().scope("Gravity storms");
        Coffee::ProfContext __("GravityStorm::draw");

        /* The OBJs are Y-up */
        Matf4 const z_up =
            glm::rotate(Matf4(1), glm::half_pi<f32>(), Vecf3{1, 0, 0});
        for(auto const& storm : m_storms)
        {
            f32 const age =
                std::chrono::duration<f32>(m_now - storm.start).count();
            /* Its rim sits at y = -1 */
            Matf4 const cloud =
                glm::translate(
                    Matf4(1),
                    storm.position +
                        Vecf3{0, 0, storm_cloud_height + storm_cloud_scale}) *
                z_up * glm::scale(Matf4(1), Vecf3(storm_cloud_scale));
            storm_draw(
                view,
                storm_cloud_program,
                storm_cloud_draw,
                cloud,
                age,
                storm_envelope(storm, age, 0.f),
                0.f,
                1.f);
            for(auto const& wind : storm_winds)
            {
                f32 const strength = storm_envelope(storm, age, wind.delay);
                if(strength <= 0.f)
                    continue;
                Vecf3 const centre = wind.hugs ? storm.rifle : storm.position;
                Matf4 const model =
                    glm::translate(Matf4(1), centre) *
                    glm::rotate(Matf4(1), age * wind.spin, Vecf3{0, 0, 1}) *
                    glm::scale(
                        Matf4(1), Vecf3{wind.width, wind.width, wind.height}) *
                    z_up;
                storm_draw(
                    view,
                    storm_wind_program,
                    storm_wind_draw,
                    model,
                    age,
                    strength,
                    wind.bands,
                    wind.density);
            }
            draw_lightning(view, storm, age);
        }
        draw_rings(view);
    }

    struct ring_t
    {
        Matf4 model;
        Vecf3 across;
        Vecf3 out;
        f32   age;
        f32   strength;
        f32   distance;
    };

    /* A ring opens past its size and settles */
    static f32 ring_open(f32 t)
    {
        t = std::clamp(t, 0.f, 1.f) - 1.f;
        return 1.f + t * t * (2.70158f * t + 1.70158f);
    }

    /* One ring per interval held, on a wall facing the wielder's look: a
     * row out from the middle, then the next row up, set off by half */
    void draw_rings(view_t const& view)
    {
        using namespace std::string_view_literals;
        f32 const now = m_ring_clock;
        std::vector<ring_t> rings;
        for(auto const& [id, charge] : m_charges)
        {
            u32 const count = std::min(
                ring_most, 1 + static_cast<u32>(charge.held / ring_interval));
            f32 const close = ring_open(charge.shown);
            for(u32 i = 0; i < count; i++)
            {
                f32 const fi  = static_cast<f32>(i);
                f32 const age = charge.held - fi * ring_interval;
                f32 const size =
                    ring_radius * ring_open(age / ring_grow) * close;
                if(size <= 0.f)
                    continue;
                u32 const row  = i / ring_per_row;
                u32 const k    = i % ring_per_row;
                f32 const side = k % 2 ? 1.f : -1.f;
                f32 const x    = side * static_cast<f32>((k + 1) / 2) * ring_spacing +
                              (row % 2 ? ring_spacing * .5f : 0.f);
                f32 const y = static_cast<f32>(row) * ring_spacing * .87f +
                              std::sin(now * 1.7f + fi) * .03f;
                Vecf3 const out = -charge.forward;
                Vecf3 const across{charge.forward.y, -charge.forward.x, 0.f};
                Vecf3 const at = charge.centre + charge.forward * ring_ahead +
                                 across * x + Vecf3{0, 0, y};
                rings.push_back(ring_t{
                    .model = Matf4(
                        Vecf4(across * size, 0.f),
                        Vecf4(0.f, 0.f, size, 0.f),
                        Vecf4(out * size, 0.f),
                        Vecf4(at, 1.f)),
                    .across   = across,
                    .out      = out,
                    .age      = age,
                    .strength = std::clamp(charge.shown, 0.f, 1.f),
                    .distance = glm::distance(at, view.position),
                });
            }
        }
        /* Back to front, they blend over each other */
        std::sort(rings.begin(), rings.end(), [](ring_t const& a, ring_t const& b) {
            return a.distance > b.distance;
        });
        Vecf3 const up{0, 0, 1};
        for(auto const& ring : rings)
        {
            gfx::view_state    state   = view.state;
            gfx::stencil_state stencil = view.stencil;
            auto               res     = m_api->submit(
                {
                    .program       = ring_program,
                    .vertices      = storm_vao,
                    .render_target = view.target,
                    .call          = {.indexed = true},
                    .data          = {ring_draw},
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{{"camera"sv}, semantic::SpanOne(view.camera)},
                    gfx::uniform_pair{{"model"sv}, semantic::SpanOne(ring.model)}),
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair{{"time"sv}, semantic::SpanOne<const f32>(ring.age)},
                    gfx::uniform_pair{
                        {"strength"sv}, semantic::SpanOne<const f32>(ring.strength)},
                    gfx::uniform_pair{{"eye"sv}, semantic::SpanOne(view.position)},
                    gfx::uniform_pair{{"axis_t"sv}, semantic::SpanOne(ring.across)},
                    gfx::uniform_pair{{"axis_b"sv}, semantic::SpanOne(up)},
                    gfx::uniform_pair{{"axis_n"sv}, semantic::SpanOne(ring.out)}),
                gfx::blend_state{.premultiplied = true},
                gfx::depth_extended_state{.depth_write = false},
                state,
                stencil);
            if(res)
            {
                auto [err, msg] = *res;
                cWarning(
                    "Failed to draw ring: {}: {}",
                    gleam::detail::draw_error_to_string(err),
                    msg);
            }
        }
    }

    static f32 lightning_hash(u32 slot, u32 cycle, u32 salt)
    {
        u32 h = slot * 0x9e3779b9u ^ cycle * 0x85ebca6bu ^ salt * 0xc2b2ae35u;
        h ^= h >> 16;
        h *= 0x7feb352du;
        h ^= h >> 15;
        h *= 0x846ca68bu;
        h ^= h >> 16;
        return static_cast<f32>(h >> 8) / static_cast<f32>(1u << 24);
    }

    /* Each slot strikes once a period at a fresh spot: the leader quad, then
     * the whole bolt flickering out */
    void draw_lightning(view_t const& view, gravity_storm_t const& storm, f32 age)
    {
        using namespace std::string_view_literals;
        f32 const envelope = storm_envelope(storm, age, 1.f);
        if(envelope <= 0.f)
            return;
        f32 const height = storm_cloud_height;
        for(u32 slot = 0; slot < lightning_slots; slot++)
        {
            f32 const t =
                age + lightning_period * static_cast<f32>(slot) / lightning_slots;
            auto const cycle = static_cast<u32>(t / lightning_period);
            f32 const start  = lightning_hash(slot, cycle, 0) *
                              (lightning_period - lightning_leader - lightning_flash);
            f32 const local  = t - static_cast<f32>(cycle) * lightning_period - start;
            if(local < 0.f || local > lightning_leader + lightning_flash ||
               lightning_hash(slot, cycle, 1) < .2f)
                continue;
            bool const leader = local < lightning_leader;
            f32 const  fade   = leader ? 1.f
                                       : 1.f - (local - lightning_leader) / lightning_flash;
            /* Two dips while it fades */
            f32 const flicker = .6f + .4f * std::abs(std::cos(fade * 9.f));
            f32 const strength = envelope * fade * flicker;

            f32 const angle = lightning_hash(slot, cycle, 2) * glm::two_pi<f32>();
            f32 const reach = glm::mix(
                lightning_reach_min, lightning_reach_max, lightning_hash(slot, cycle, 3));
            Vecf3 const ground =
                storm.position + reach * Vecf3{std::cos(angle), std::sin(angle), 0.f};
            /* Faces the viewer about the vertical, mirrored at random */
            Vecf3 const to_eye = view.position - ground;
            f32 const   facing = std::atan2(to_eye.y, to_eye.x) - glm::half_pi<f32>();
            f32 const   mirror = lightning_hash(slot, cycle, 4) < .5f ? -1.f : 1.f;
            f32 const   scale  = height / lightning_mesh_height;
            Matf4 const model =
                glm::translate(Matf4(1), ground) *
                glm::rotate(Matf4(1), facing, Vecf3{0, 0, 1}) *
                glm::scale(Matf4(1), Vecf3{mirror * scale, scale, scale}) *
                glm::rotate(Matf4(1), glm::half_pi<f32>(), Vecf3{1, 0, 0});

            gfx::view_state    state   = view.state;
            gfx::stencil_state stencil = view.stencil;
            auto               res     = m_api->submit(
                {
                    .program       = lightning_program,
                    .vertices      = storm_vao,
                    .render_target = view.target,
                    .call          = {.indexed = true},
                    .data = leader ? lightning_strike_draw
                                   : std::vector{lightning_draw},
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{{"camera"sv}, semantic::SpanOne(view.camera)},
                    gfx::uniform_pair{{"model"sv}, semantic::SpanOne(model)}),
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair{
                        {"strength"sv}, semantic::SpanOne<const f32>(strength)}),
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"bolt", 0},
                    lightning_sampler}),
                gfx::blend_state{.additive = true},
                gfx::depth_extended_state{.depth_write = false},
                state,
                stencil);
            if(res)
            {
                auto [err, msg] = *res;
                cWarning(
                    "Failed to draw lightning: {}: {}",
                    gleam::detail::draw_error_to_string(err),
                    msg);
            }
        }
    }
};

void alloc_gravity_storm(compo::EntityContainer& container)
{
    auto& storm = container.register_subsystem_inplace<GravityStorm<halo_version>>(
        &container.subsystem_cast<gfx::system>());
    container.subsystem_cast<RenderHooks>().add(RenderHook::AfterSky, &storm);
}
