#include "effects.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"

#include <blam/volta/blam_particles.h>
#include <blam/volta/blam_unit.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <deque>
#include <map>
#include <random>
#include <unordered_map>

using EffectManifest = compo::SubsystemManifest<
    type_list_t<
        const Model,
        const Projectile,
        const ObjectPhysics,
        const PlayerCamera,
        const PlayerInfo>,
    type_list_t<
        EffectScene,
        PhysicsBus,
        const BlamFiles<halo_version>,
        const BSPCache<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

namespace {

using blam::scn::contrail;
using blam::scn::effect;
using blam::scn::lens_flare;
using blam::scn::light;
using blam::scn::light_volume;
using blam::scn::particle;
using blam::scn::particle_system;
using blam::scn::point_physics;
using blend_t = EffectScene::blend_t;

constexpr f32 halo_gravity = 9.81f / 3.048f; /* wu/s^2 */

/* Tag colors are a, r, g, b */
Vecf4 rgba(Vecf4 const& argb)
{
    return {argb.y, argb.z, argb.w, argb.x};
}

/* A point and the way it faces, where something is created */
struct place_t
{
    Vecf3 position{};
    Vecf3 forward{1.f, 0.f, 0.f};
};

/* How a point_physics moves things */
struct motion_t
{
    f32  gravity{0.f}; /* wu/s^2, up is positive */
    f32  drag{0.f};    /* per second */
    f32  elasticity{0.f};
    bool collides{false};

    static motion_t from(point_physics const* pphy)
    {
        if(!pphy)
            return {};
        using flags_t = point_physics::flags_t;
        bool const no_gravity =
            (pphy->flags & flags_t::no_gravity) != flags_t::none;
        return {
            .gravity    = no_gravity ? 0.f : pphy->air_gravity_scale * halo_gravity,
            .drag       = pphy->mass_scale > 0.f
                              ? pphy->air_friction / pphy->mass_scale
                              : 0.f,
            .elasticity = pphy->elasticity,
            .collides   = (pphy->flags & flags_t::collides_with_structures) !=
                        flags_t::none,
        };
    }
};

} // namespace

struct EffectSystem
    : compo::RestrictedSubsystem<EffectSystem, EffectManifest>
{
    using type  = EffectSystem;
    using Proxy = compo::proxy_of<EffectManifest>;

    /* A sprite with a life of its own, from a part tag or a pctl state */
    struct sprite_particle_t
    {
        EffectScene::sprite_t look{};
        Vecf4                 tint{1.f};
        Vecf3                 velocity{};
        motion_t              motion{};
        f32                   age{0.f};
        f32                   life{1.f};
        f32                   fade_in{0.f};
        f32                   fade_out{0.f};
        f32                   frame{0.f};
        f32                   frame_rate{0.f};
        f32                   spin{0.f};
        bool                  along_velocity{false};
        bool                  dies_on_contact{false};

        /* pctl particles walk their type's particle states */
        particle_system::particle_state_t const* states{nullptr};
        u32                                      state_count{0};
        u32                                      state{0};
        f32                                      state_time{0.f};
        f32                                      hold{0.f};
        f32                                      transition{0.f};
        f32                                      size_pick{0.f};
        f32                                      color_pick{0.f};
    };

    /* An attachment's object function; none = always full */
    using scale_t = blam::shader::animation_src;

    struct effect_instance_t
    {
        effect const* tag{nullptr};
        place_t       place{};
        Vecf3         velocity{};
        u64           parent{0};
        bool          attached{false}; /* loops while its object lives */
        scale_t       scale{};

        i32  event{0};
        f32  time{0.f};
        f32  delay{0.f};
        f32  duration{0.f};
        bool started{false};
        bool created{false};
    };

    struct light_instance_t
    {
        light const* tag{nullptr};
        place_t      place{};
        u64          parent{0};
        std::string  marker{};
        f32          radius{0.f};
        Vecf3        color{1.f};
        f32          age{0.f};
        f32          life{-1.f}; /* <0 = while its parent lives */
        u64          id{0};      /* keys its lens flare's fading */
        scale_t      scale{};
        f32          level{1.f}; /* this frame's object function */
    };

    struct system_instance_t
    {
        particle_system const* tag{nullptr};
        place_t                place{};
        u64                    parent{0};
        std::string            marker{};
        Vecf3                  velocity{};
        f32                    age{0.f};
        bool                   attached{false};
        scale_t                scale{};

        struct type_state_t
        {
            u32 state{0};
            f32 left{0.f}; /* in the current system state */
            f32 owed{0.f}; /* particles to create, fractional */
            bool done{false};
        };
        std::vector<type_state_t> types;
    };

    struct contrail_point_t
    {
        Vecf3 position{};
        Vecf3 velocity{};
        f32   age{0.f};
        f32   pick{0.f};
    };

    struct contrail_instance_t
    {
        contrail const*              tag{nullptr};
        u64                          parent{0};
        std::string                  marker{};
        Vecf3                        head{};
        bool                         has_head{false};
        bool                         detached{false};
        scale_t                      scale{};
        f32                          owed{0.f};
        std::deque<contrail_point_t> points; /* newest first */
    };

    struct volume_instance_t
    {
        light_volume const* tag{nullptr};
        u64                 parent{0};
        std::string         marker{};
        scale_t             scale{};
    };

    EffectSystem()
    {
        this->priority = 904;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        f32 const dt =
            m_last_frame == time_point{}
                ? 0.f
                : std::clamp(
                      std::chrono::duration<f32>(t - m_last_frame).count(),
                      0.f,
                      .1f);
        m_last_frame = t;

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(files.load_generation != m_generation)
        {
            m_generation = files.load_generation;
            m_effects.clear();
            m_lights.clear();
            m_systems.clear();
            m_contrails.clear();
            m_volumes.clear();
            m_particles.clear();
            m_attached.clear();
            m_requests.clear();
            m_flare_seen.clear();
        }
        m_magic = &files.container.magic;
        m_index = blam::tag_index_view<halo_version>(files.container);

        auto& scene = p.template subsystem<EffectScene>();
        scene.clear();

        attach(p);
        m_spawn_queue->poll();
        for(auto const& request : std::exchange(m_requests, {}))
            spawn(p, request);

        step_effects(p, dt);
        step_lights(p, scene, dt);
        step_systems(p, dt);
        step_particles(p, scene, dt);
        step_contrails(p, scene, dt);
        draw_volumes(p, scene);
        step_flares(p, scene, dt);
    }

    /* ---- Tag lookups ---- */

    template<typename T>
    T const* tag_data(blam::tagref_t const& ref, blam::tag_class_t cls)
    {
        if(!ref.valid())
            return nullptr;
        auto it = m_index.find(ref);
        if(it == m_index.end() || !(*it).matches(cls))
            return nullptr;
        auto data = (*it).template data<T>(*m_magic);
        return data.has_value() ? &data.value()[0] : nullptr;
    }

    blam::tag_class_t class_of(blam::tagref_t const& ref)
    {
        auto it = m_index.find(ref);
        return it == m_index.end() ? blam::tag_class_t::none
                                   : (*it).tag_class();
    }

    /* ---- Randomness ---- */

    f32 uniform(f32 a, f32 b)
    {
        if(b < a)
            std::swap(a, b);
        return a == b ? a : std::uniform_real_distribution<f32>(a, b)(m_rng);
    }

    f32 uniform(Vecf2 const& bounds)
    {
        return uniform(bounds.x, bounds.y);
    }

    static void basis(Vecf3 const& dir, Vecf3& x, Vecf3& y)
    {
        Vecf3 const up = std::abs(dir.z) < .99f ? Vecf3{0, 0, 1} : Vecf3{1, 0, 0};
        x              = glm::normalize(glm::cross(up, dir));
        y              = glm::cross(dir, x);
    }

    /* A direction within `angle` of dir */
    Vecf3 cone(Vecf3 const& dir, f32 angle)
    {
        if(angle <= 0.f)
            return dir;
        f32 const z   = uniform(std::cos(std::min(angle, glm::pi<f32>())), 1.f);
        f32 const phi = uniform(0.f, glm::two_pi<f32>());
        f32 const r   = std::sqrt(std::max(0.f, 1.f - z * z));
        Vecf3     x, y;
        basis(dir, x, y);
        return x * (r * std::cos(phi)) + y * (r * std::sin(phi)) + dir * z;
    }

    Vecf3 in_sphere(f32 radius)
    {
        if(radius <= 0.f)
            return {};
        Vecf3 const dir = cone(Vecf3{0, 0, 1}, glm::pi<f32>());
        return dir * (radius * std::cbrt(uniform(0.f, 1.f)));
    }

    /* ---- Placement ---- */

    /* Where a named marker is on an entity now, else its origin */
    std::optional<place_t> marker_place(
        Proxy& p, u64 entity, std::string const& marker)
    {
        auto const* model = p.template get<Model>(entity);
        if(!model)
            return std::nullopt;
        Matf4 at = model->transform;
        if(!marker.empty() && model->tag)
        {
            auto key = std::pair{model->tag, marker};
            auto it  = m_markers.find(key);
            if(it == m_markers.end())
                it = m_markers.emplace(key, bind_marker(*model->tag, marker, *m_magic))
                         .first;
            if(it->second)
                at = at * *it->second;
        }
        return place_t{
            .position = Vecf3(at[3]),
            .forward  = glm::normalize(Vecf3(at[0])),
        };
    }

    /* An attachment's object function, off until something sets it */
    f32 scale_of(Proxy& p, u64 entity, scale_t scale)
    {
        if(scale == scale_t::none)
            return 1.f;
        auto const* model = p.template get<Model>(entity);
        auto const  i     = static_cast<size_t>(scale) - 1;
        if(!model || i >= model->object_function.size())
            return 0.f;
        return std::clamp(model->object_function[i], 0.f, 1.f);
    }

    Vecf3 velocity_of(Proxy& p, u64 entity)
    {
        if(auto const* flight = p.template get<Projectile>(entity))
            return flight->velocity;
        if(auto const* physics = p.template get<ObjectPhysics>(entity))
            return physics->linear_velocity;
        return {};
    }

    /* An effect location: a marker on its parent, or the special names */
    place_t location(Proxy& p, effect_instance_t const& fx, i16 index)
    {
        std::string name;
        if(auto locations = fx.tag->locations.data(*m_magic);
           locations.has_value() && index >= 0 &&
           static_cast<size_t>(index) < locations.value().size())
            name = std::string(locations.value()[index].marker_name.str());
        if(name == "gravity")
            return {fx.place.position, {0.f, 0.f, -1.f}};
        if(fx.parent != 0 && name != "normal")
            if(auto at = marker_place(p, fx.parent, name))
                return *at;
        return fx.place;
    }

    /* ---- Spawning ---- */

    void spawn(Proxy& p, SpawnEffectEvent const& request)
    {
        place_t const place{
            .position = request.position,
            .forward  = glm::length(request.direction) > 0.f
                            ? glm::normalize(request.direction)
                            : Vecf3{1, 0, 0},
        };
        switch(class_of(request.effect))
        {
        case blam::tag_class_t::snd:
            play_sound(p, request.effect, place.position);
            break;
        case blam::tag_class_t::effe:
            if(auto const* tag = tag_data<effect>(request.effect, blam::tag_class_t::effe))
                m_effects.push_back({
                    .tag      = tag,
                    .place    = place,
                    .velocity = request.velocity,
                    .parent   = request.parent,
                });
            break;
        default:
            break;
        }
    }

    void play_sound(Proxy& p, blam::tagref_t const& sound, Vecf3 const& at)
    {
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!sounds || !sound.valid())
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{.sound = &sound, .position = at};
        sounds->inject(ev, &play);
    }

    /* ---- Object attachments ---- */

    /* Each object's attachments and widgets start once, when it is seen */
    void attach(Proxy& p)
    {
        std::set<u64> live;
        for(auto ent : p.template select<Model>())
        {
            auto const& model = ent.template get<Model>();
            if(!model.origin_object)
                continue;
            live.insert(ent.id());
            if(m_attached.contains(ent.id()))
                continue;
            m_attached.insert(ent.id());
            auto object = model.origin_object->template data<blam::scn::object>(
                *m_magic);
            if(!object.has_value())
                continue;
            auto const& obj = object.value()[0];
            if(auto attachments = obj.attachments.data(*m_magic);
               attachments.has_value())
                for(auto const& a : attachments.value())
                    attach_one(
                        p, ent.id(), a.type, std::string(a.marker.str()), a.primary_scale);
            if(auto widgets = obj.widgets.data(*m_magic); widgets.has_value())
                for(auto const& w : widgets.value())
                    attach_one(p, ent.id(), w.widget, {}, scale_t::none);
        }
        for(auto it = m_attached.begin(); it != m_attached.end();)
            it = live.contains(*it) ? std::next(it) : m_attached.erase(it);
    }

    void attach_one(
        Proxy&                p,
        u64                   entity,
        blam::tagref_t const& ref,
        std::string const&    marker,
        scale_t               scale)
    {
        using blam::tag_class_t;
        if(p.template get<Projectile>(entity))
        switch(class_of(ref))
        {
        case tag_class_t::cont:
            if(auto const* tag = tag_data<contrail>(ref, tag_class_t::cont))
                m_contrails.push_back(
                    {.tag = tag, .parent = entity, .marker = marker, .scale = scale});
            break;
        case tag_class_t::ligh:
            if(auto const* tag = tag_data<light>(ref, tag_class_t::ligh))
            {
                m_lights.push_back(make_light(tag, {}, entity, marker, -1.f));
                m_lights.back().scale = scale;
            }
            break;
        case tag_class_t::mgs2:
            if(auto const* tag = tag_data<light_volume>(ref, tag_class_t::mgs2))
                m_volumes.push_back(
                    {.tag = tag, .parent = entity, .marker = marker, .scale = scale});
            break;
        case tag_class_t::pctl:
            if(auto const* tag = tag_data<particle_system>(ref, tag_class_t::pctl))
                start_system(tag, {}, entity, marker, {}, true, scale);
            break;
        case tag_class_t::effe:
            if(auto const* tag = tag_data<effect>(ref, tag_class_t::effe))
            {
                auto at = marker_place(p, entity, marker);
                m_effects.push_back({
                    .tag      = tag,
                    .place    = at.value_or(place_t{}),
                    .parent   = entity,
                    .attached = true,
                    .scale    = scale,
                });
            }
            break;
        default:
            break;
        }
    }

    /* ---- Effects ---- */

    void step_effects(Proxy& p, f32 dt)
    {
        for(auto it = m_effects.begin(); it != m_effects.end();)
            it = step_effect(p, *it, dt) ? std::next(it) : m_effects.erase(it);
    }

    /* Events run in turn: wait out the delay, create, wait out the
     * duration. False once it has finished. */
    bool step_effect(Proxy& p, effect_instance_t& fx, f32 dt)
    {
        if(fx.parent != 0)
        {
            if(!p.template get<Model>(fx.parent))
                return false;
            if(scale_of(p, fx.parent, fx.scale) <= 0.f)
                return true;
            if(auto at = marker_place(p, fx.parent, {}))
                fx.place.position = at->position;
            fx.velocity = velocity_of(p, fx.parent);
        }
        auto events = fx.tag->events.data(*m_magic);
        if(!events.has_value() || events.value().empty())
            return false;
        auto const count = static_cast<i32>(events.value().size());

        fx.time += dt;
        /* A loop of zero length runs once a frame, not forever */
        for(i32 guard = 0; guard <= count; guard++)
        {
            if(fx.event >= count)
            {
                if(!fx.attached)
                    return false;
                fx.event = std::clamp<i32>(fx.tag->loop_start_event, 0, count - 1);
                if(guard > 0)
                    break;
            }
            auto const& ev = events.value()[fx.event];
            if(!fx.started)
            {
                fx.started = true;
                fx.created = false;
                fx.delay   = uniform(ev.delay_bounds);
                fx.duration = uniform(ev.duration_bounds);
            }
            if(!fx.created)
            {
                if(fx.time < fx.delay)
                    break;
                fx.created = true;
                if(ev.skip_fraction <= 0.f || uniform(0.f, 1.f) >= ev.skip_fraction)
                    create(p, fx, ev);
            }
            if(fx.time < fx.delay + fx.duration)
                break;
            fx.time -= fx.delay + fx.duration;
            fx.started = false;
            fx.event++;
        }
        return true;
    }

    void create(Proxy& p, effect_instance_t const& fx, effect::event_t const& ev)
    {
        using blam::tag_class_t;
        if(auto parts = ev.parts.data(*m_magic); parts.has_value())
            for(auto const& part : parts.value())
            {
                place_t const at = location(p, fx, part.location);
                switch(class_of(part.type))
                {
                case tag_class_t::snd:
                    play_sound(p, part.type, at.position);
                    break;
                case tag_class_t::ligh:
                    if(auto const* tag = tag_data<light>(part.type, tag_class_t::ligh))
                        m_lights.push_back(make_light(
                            tag,
                            at,
                            0,
                            {},
                            tag->duration > 0.f ? tag->duration / ticks_per_second
                                                : default_light_life));
                    break;
                case tag_class_t::pctl:
                    if(auto const* tag = tag_data<particle_system>(
                           part.type, tag_class_t::pctl))
                        start_system(tag, at, 0, {}, fx.velocity, false);
                    break;
                default:
                    /* Objects, decals and damage are not created yet */
                    break;
                }
            }
        if(auto particles = ev.particles.data(*m_magic); particles.has_value())
            for(auto const& burst : particles.value())
                emit(p, fx, burst);
    }

    /* A burst of part sprites */
    void emit(Proxy& p, effect_instance_t const& fx, effect::particle_t const& burst)
    {
        if(burst.create == effect::camera_mode_t::only_in_first_person)
            return;
        auto const* part =
            tag_data<particle>(burst.particle_type, blam::tag_class_t::part);
        if(!part)
            return;
        auto const* pphy =
            tag_data<point_physics>(part->physics, blam::tag_class_t::pphy);
        motion_t const motion = motion_t::from(pphy);

        place_t const at = location(p, fx, burst.location);
        Vecf3         x, y;
        basis(at.forward, x, y);
        /* relative_direction is yaw, pitch off the location's forward */
        f32 const   yaw   = burst.relative_direction.x;
        f32 const   pitch = burst.relative_direction.y;
        Vecf3 const dir   = glm::normalize(
            at.forward * (std::cos(pitch) * std::cos(yaw)) +
            x * (std::cos(pitch) * std::sin(yaw)) + y * std::sin(pitch));
        Vecf3 const origin = at.position + at.forward * burst.relative_offset.x +
                             x * burst.relative_offset.y +
                             y * burst.relative_offset.z;

        using pflags = particle::flags_t;
        auto const has = [part](pflags f) {
            return (part->flags & f) != pflags::none;
        };
        i32 const count = static_cast<i32>(
            std::round(uniform(static_cast<f32>(burst.count.x), static_cast<f32>(burst.count.y))));
        for(i32 i = 0; i < count && m_particles.size() < max_particles; i++)
        {
            sprite_particle_t s{};
            s.look.position =
                origin + in_sphere(uniform(burst.distribution_radius));
            s.look.radius   = uniform(burst.radius);
            s.look.bitmap   = &static_cast<blam::tagref_t const&>(part->bitmap);
            s.look.sequence = part->first_sequence_index;
            if(part->initial_sequence_count > 1)
                s.look.sequence += static_cast<i16>(
                    uniform(0.f, static_cast<f32>(part->initial_sequence_count) - .001f));
            s.look.blend = part->shader.blend_function;
            s.look.rotation =
                (burst.flags & effect::particle_t::flags_t::random_initial_angle) !=
                        effect::particle_t::flags_t::none
                    ? uniform(0.f, glm::two_pi<f32>())
                    : 0.f;
            s.tint = glm::mix(
                rgba(burst.tint_lower_bound),
                rgba(burst.tint_upper_bound),
                uniform(0.f, 1.f));
            s.velocity = cone(dir, burst.velocity_cone_angle) *
                             uniform(burst.velocity) +
                         fx.velocity;
            s.motion   = motion;
            s.life     = std::max(uniform(part->lifespan), .05f);
            s.fade_in  = part->fade_in_time;
            s.fade_out = part->fade_out_time;
            s.frame    = has(pflags::animation_starts_on_random_frame)
                             ? uniform(0.f, 64.f)
                             : 0.f;
            s.frame_rate = uniform(part->animation_rate);
            s.spin       = uniform(burst.angular_velocity) *
                     (uniform(0.f, 1.f) < .5f ? -1.f : 1.f);
            s.along_velocity = part->orientation ==
                               blam::scn::particle_orientation_t::parallel_to_direction;
            s.dies_on_contact = has(pflags::dies_on_contact_with_structure);
            m_particles.push_back(s);
        }
    }

    /* ---- Lights ---- */

    light_instance_t make_light(
        light const*        tag,
        place_t const&      at,
        u64                 parent,
        std::string const&  marker,
        f32                 life)
    {
        return {
            .tag    = tag,
            .place  = at,
            .parent = parent,
            .marker = marker,
            .radius = tag->radius * uniform(tag->radius_modifier),
            .color  = Vecf3(rgba(tag->color_upper_bound)),
            .life = life,
            .id   = ++m_light_ids,
        };
    }

    static f32 falloff(blam::scn::function_type_t function, f32 t)
    {
        using fn = blam::scn::function_type_t;
        t        = std::clamp(t, 0.f, 1.f);
        switch(function)
        {
        case fn::early:
            return 1.f - (1.f - t) * (1.f - t);
        case fn::very_early:
            return 1.f - std::pow(1.f - t, 4.f);
        case fn::late:
            return t * t;
        case fn::very_late:
            return std::pow(t, 4.f);
        case fn::cosine:
            return .5f - .5f * std::cos(t * glm::pi<f32>());
        default:
            return t;
        }
    }

    void step_lights(Proxy& p, EffectScene& scene, f32 dt)
    {
        for(auto it = m_lights.begin(); it != m_lights.end();)
        {
            auto& l = *it;
            l.age += dt;
            if(l.parent != 0)
            {
                auto at = marker_place(p, l.parent, l.marker);
                if(!at)
                {
                    it = m_lights.erase(it);
                    continue;
                }
                l.place = *at;
                l.level = scale_of(p, l.parent, l.scale);
            } else if(l.life >= 0.f && l.age >= l.life)
            {
                it = m_lights.erase(it);
                continue;
            }
            /* Timed lights fade from the upper color to the lower one */
            Vecf3 color = l.color * l.level;
            if(l.life > 0.f)
                color = glm::mix(
                    color,
                    Vecf3(rgba(l.tag->color_lower_bound)),
                    falloff(l.tag->falloff_function, l.age / l.life));
            if(glm::dot(color, color) <= 1e-6f)
            {
                ++it;
                continue;
            }
            scene.lights.push_back({
                .position   = l.place.position,
                .radius     = l.radius,
                .color      = color,
                .lens_flare = l.tag->lens_flare.valid()
                                  ? &static_cast<blam::tagref_t const&>(
                                        l.tag->lens_flare)
                                  : nullptr,
            });
            ++it;
        }
    }

    /* ---- Particle systems ---- */

    void start_system(
        particle_system const* tag,
        place_t const&         at,
        u64                    parent,
        std::string const&     marker,
        Vecf3 const&           velocity,
        bool                   attached,
        scale_t                scale = scale_t::none)
    {
        system_instance_t sys{
            .tag      = tag,
            .place    = at,
            .parent   = parent,
            .marker   = marker,
            .velocity = velocity,
            .attached = attached,
            .scale    = scale,
        };
        auto types = tag->particle_types.data(*m_magic);
        if(!types.has_value())
            return;
        for(auto const& type : types.value())
        {
            system_instance_t::type_state_t state{};
            if(auto states = type.states.data(*m_magic);
               states.has_value() && !states.value().empty())
                state.left = uniform(states.value()[0].duration_bounds);
            state.owed = static_cast<f32>(type.initial_particle_count);
            sys.types.push_back(state);
        }
        m_systems.push_back(std::move(sys));
    }

    void step_systems(Proxy& p, f32 dt)
    {
        for(auto it = m_systems.begin(); it != m_systems.end();)
            it = step_system(p, *it, dt) ? std::next(it) : m_systems.erase(it);
    }

    bool step_system(Proxy& p, system_instance_t& sys, f32 dt)
    {
        if(sys.parent != 0)
        {
            auto at = marker_place(p, sys.parent, sys.marker);
            if(!at)
                return false;
            sys.place    = *at;
            sys.velocity = velocity_of(p, sys.parent);
        }
        sys.age += dt;
        f32 const level =
            sys.parent != 0 ? scale_of(p, sys.parent, sys.scale) : 1.f;
        auto types = sys.tag->particle_types.data(*m_magic);
        if(!types.has_value())
            return false;
        bool live = false;
        for(size_t i = 0; i < types.value().size() && i < sys.types.size(); i++)
        {
            auto const& type  = types.value()[i];
            auto&       state = sys.types[i];
            using tflags      = particle_system::type_t::flags_t;
            if((type.flags & tflags::disabled) != tflags::none || state.done)
                continue;
            auto states = type.states.data(*m_magic);
            if(states.has_value() && state.state < states.value().size())
            {
                auto const& s = states.value()[state.state];
                state.owed += s.particle_creation_rate * dt * level;
                state.left -= dt;
                if(state.left <= 0.f)
                {
                    state.state++;
                    bool const loop =
                        sys.attached ||
                        (type.flags & tflags::type_states_loop) != tflags::none;
                    if(state.state >= states.value().size() && loop)
                        state.state = 0;
                    if(state.state < states.value().size())
                        state.left =
                            uniform(states.value()[state.state].duration_bounds);
                }
            }
            for(; level > 0.f && state.owed >= 1.f && m_particles.size() < max_particles;
                state.owed -= 1.f)
                emit_system(sys, type);
            state.owed = std::min(state.owed, 1.f);
            if(!states.has_value() || state.state >= states.value().size())
                state.done = !sys.attached;
            live = live || !state.done;
        }
        return live;
    }

    void emit_system(system_instance_t const& sys, particle_system::type_t const& type)
    {
        auto pstates = type.particle_states.data(*m_magic);
        if(!pstates.has_value() || pstates.value().empty())
            return;
        sprite_particle_t s{};
        Vecf3 const offset = in_sphere(type.radius);
        s.look.position    = sys.place.position + offset;
        switch(type.particle_creation_physics)
        {
        case particle_system::creation_physics_t::explosion:
            s.velocity = offset * explosion_speed;
            break;
        case particle_system::creation_physics_t::jet:
            s.velocity = sys.place.forward * type.radius * explosion_speed;
            break;
        default:
            break;
        }
        s.velocity += sys.velocity;
        s.states      = pstates.value().data();
        s.state_count = static_cast<u32>(pstates.value().size());
        s.size_pick   = uniform(0.f, 1.f);
        s.color_pick  = uniform(0.f, 1.f);
        s.spin        = uniform(s.states[0].rotation_rate) *
                 (uniform(0.f, 1.f) < .5f ? -1.f : 1.f);
        s.look.rotation = uniform(0.f, glm::two_pi<f32>());
        enter_state(s, 0);
        s.life = 0.f;
        for(u32 i = 0; i < s.state_count; i++)
            s.life += s.states[i].duration_bounds.y +
                      (i + 1 < s.state_count ? s.states[i].transition_time_bounds.y : 0.f);
        s.life = std::max(s.life, .05f);
        m_particles.push_back(s);
    }

    void enter_state(sprite_particle_t& s, u32 state)
    {
        s.state      = state;
        s.state_time = 0.f;
        auto const& ps = s.states[state];
        s.hold         = uniform(ps.duration_bounds);
        s.transition   = state + 1 < s.state_count ? uniform(ps.transition_time_bounds) : 0.f;
        s.motion = motion_t::from(
            tag_data<point_physics>(ps.point_physics, blam::tag_class_t::pphy));
        s.look.bitmap   = &static_cast<blam::tagref_t const&>(ps.bitmaps);
        s.look.sequence = ps.sequence_index;
        s.look.blend    = ps.shader.blend_function;
    }

    /* The look of a pctl particle, blended into its next state */
    void state_look(sprite_particle_t& s, f32 dt)
    {
        s.state_time += dt;
        if(s.state_time > s.hold + s.transition && s.state + 1 < s.state_count)
            enter_state(s, s.state + 1);
        auto look_of = [&s](u32 i) {
            auto const& ps     = s.states[i];
            f32 const   radius = glm::mix(ps.scale.x, ps.scale.y, s.size_pick) *
                               pctl_scale_to_radius * ps.radius_multiplier;
            Vecf4 const color  = glm::mix(rgba(ps.color_1), rgba(ps.color_2), s.color_pick);
            return std::pair{radius, color};
        };
        auto [radius, color] = look_of(s.state);
        if(s.transition > 0.f && s.state_time > s.hold && s.state + 1 < s.state_count)
        {
            f32 const t        = std::min((s.state_time - s.hold) / s.transition, 1.f);
            auto [r2, c2]      = look_of(s.state + 1);
            radius             = glm::mix(radius, r2, t);
            color              = glm::mix(color, c2, t);
        }
        s.look.radius = radius;
        s.tint        = color;
        if(s.state + 1 == s.state_count && s.state_time >= s.hold)
            s.age = s.life;
    }

    /* ---- Sprites ---- */

    void step_particles(Proxy& p, EffectScene& scene, f32 dt)
    {
        auto& physics = p.template subsystem<PhysicsBus>();
        for(size_t i = 0; i < m_particles.size();)
        {
            auto& s = m_particles[i];
            s.age += dt;
            if(s.states)
                state_look(s, dt);
            if(s.age >= s.life)
            {
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
                continue;
            }
            s.velocity.z += s.motion.gravity * dt;
            s.velocity *= std::exp(-s.motion.drag * dt);
            Vecf3 const to = s.look.position + s.velocity * dt;
            if(s.motion.collides || s.dies_on_contact)
            {
                Physics::Event   ev{Physics::Event::Raycast};
                Physics::Raycast ray{.from = s.look.position, .to = to};
                physics.process(ev, &ray);
                if(ray.hit && ray.entity_id == 0)
                {
                    if(s.dies_on_contact)
                    {
                        m_particles[i] = m_particles.back();
                        m_particles.pop_back();
                        continue;
                    }
                    Vecf3 const n = ray.normal;
                    s.velocity    = (s.velocity - 2.f * glm::dot(s.velocity, n) * n) *
                                 s.motion.elasticity;
                    s.look.position = ray.point + n * .01f;
                } else
                    s.look.position = to;
            } else
                s.look.position = to;
            s.look.rotation += s.spin * dt;
            s.frame += s.frame_rate * dt;

            f32 alpha = 1.f;
            if(s.fade_in > 0.f && s.age < s.fade_in)
                alpha *= s.age / s.fade_in;
            if(s.fade_out > 0.f && s.life - s.age < s.fade_out)
                alpha *= (s.life - s.age) / s.fade_out;

            EffectScene::sprite_t look = s.look;
            look.color = s.tint * Vecf4(1.f, 1.f, 1.f, alpha);
            look.frame = static_cast<i16>(s.frame);
            look.axis  = s.along_velocity ? s.velocity * dt : Vecf3{};
            scene.sprites.push_back(look);
            ++i;
        }
    }

    /* ---- Contrails ---- */

    /* How old a point is when it reaches each state, and when it dies */
    struct timeline_t
    {
        std::array<f32, 8> start{};
        std::array<f32, 8> hold_end{};
        u32                count{0};
        f32                life{0.f};
    };

    timeline_t timeline(contrail const& tag)
    {
        timeline_t out;
        auto       states = tag.point_states.data(*m_magic);
        if(!states.has_value())
            return out;
        f32 t = 0.f;
        for(auto const& s : states.value())
        {
            if(out.count == out.start.size())
                break;
            out.start[out.count]    = t;
            out.hold_end[out.count] = t + (s.duration.x + s.duration.y) * .5f;
            t = out.hold_end[out.count] +
                (s.transition_duration.x + s.transition_duration.y) * .5f;
            out.count++;
        }
        /* The last state is where points end */
        out.life = out.count ? out.hold_end[out.count - 1] : 0.f;
        if(out.count > 1)
            out.life = std::max(out.life, out.start[out.count - 1]);
        return out;
    }

    std::pair<f32, Vecf4> point_look(
        contrail const& tag, timeline_t const& tl, contrail_point_t const& pt)
    {
        auto states = tag.point_states.data(*m_magic);
        if(!states.has_value() || tl.count == 0)
            return {0.f, Vecf4(0.f)};
        auto look_of = [&](u32 i) {
            auto const& s = states.value()[i];
            return std::pair{
                s.width,
                glm::mix(rgba(s.color_lower_bound), rgba(s.color_upper_bound), pt.pick)};
        };
        for(u32 i = 0; i < tl.count; i++)
        {
            bool const last = i + 1 == tl.count;
            if(last || pt.age < tl.start[i + 1])
            {
                auto [w, c] = look_of(i);
                if(!last && pt.age > tl.hold_end[i])
                {
                    f32 const t = (pt.age - tl.hold_end[i]) /
                                  std::max(tl.start[i + 1] - tl.hold_end[i], 1e-5f);
                    auto [w2, c2] = look_of(i + 1);
                    w             = glm::mix(w, w2, t);
                    c             = glm::mix(c, c2, t);
                }
                return {w, c};
            }
        }
        return look_of(tl.count - 1);
    }

    void step_contrails(Proxy& p, EffectScene& scene, f32 dt)
    {
        for(auto it = m_contrails.begin(); it != m_contrails.end();)
        {
            auto&            c  = *it;
            timeline_t const tl = timeline(*c.tag);
            for(auto& pt : c.points)
            {
                pt.age += dt;
                pt.position += pt.velocity * dt;
            }

            std::optional<place_t> at;
            if(!c.detached)
                at = marker_place(p, c.parent, c.marker);
            if(!at)
                c.detached = true;
            else
            {
                /* Points are laid along the path since the last frame, at
                 * the ages they would have had */
                /* Nothing to trail behind something standing still */
                if(c.has_head && dt > 0.f && glm::distance(at->position, c.head) > 1e-3f)
                {
                    c.owed += c.tag->point_generation_rate * dt *
                              scale_of(p, c.parent, c.scale);
                    i32 const n = static_cast<i32>(c.owed);
                    c.owed -= static_cast<f32>(n);
                    /* Oldest first: last frame's head, then evenly to now */
                    for(i32 k = n + 1; k >= 1; k--)
                    {
                        f32 const back =
                            static_cast<f32>(k) / static_cast<f32>(n + 1);
                        c.points.push_front({
                            .position = glm::mix(at->position, c.head, back),
                            .velocity =
                                cone(at->forward, c.tag->point_velocity_cone_angle) *
                                uniform(c.tag->point_velocity),
                            .age  = dt * back,
                            .pick = uniform(0.f, 1.f),
                        });
                    }
                }
                c.head     = at->position;
                c.has_head = true;
            }

            /* Keep one point past the end, for the tail to reach */
            while(c.points.size() > 1 && c.points[c.points.size() - 2].age > tl.life)
                c.points.pop_back();
            while(c.points.size() > max_contrail_points)
                c.points.pop_back();

            if(c.detached && (c.points.empty() || c.points.front().age > tl.life))
            {
                it = m_contrails.erase(it);
                continue;
            }
            build_ribbon(scene, c, tl);
            ++it;
        }
    }

    void build_ribbon(EffectScene& scene, contrail_instance_t const& c, timeline_t const& tl)
    {
        EffectScene::ribbon_t ribbon{
            .bitmap   = &static_cast<blam::tagref_t const&>(c.tag->bitmap),
            .sequence = c.tag->first_sequence_index,
            .blend    = c.tag->shader.blend_function,
        };
        auto push = [&](contrail_point_t const& pt) {
            auto [width, color] = point_look(*c.tag, tl, pt);
            f32 const along     = tl.life > 0.f ? std::min(pt.age / tl.life, 1.f) : 0.f;
            ribbon.points.push_back({
                .position = pt.position,
                .width    = width,
                .color    = color,
                .v        = along * std::max(c.tag->texture_repeats_u, 1.f),
            });
        };
        if(!c.detached)
            push({.position = c.head, .age = 0.f, .pick = .5f});
        contrail_point_t const* prev = nullptr;
        for(auto const& pt : c.points)
        {
            if(pt.age > tl.life)
            {
                /* Cut where the path reaches the end of a point's life */
                if(prev || !c.detached)
                {
                    Vecf3 const from = prev ? prev->position : c.head;
                    f32 const   age0 = prev ? prev->age : 0.f;
                    f32 const   t    = (tl.life - age0) / std::max(pt.age - age0, 1e-5f);
                    push({
                        .position = glm::mix(from, pt.position, std::clamp(t, 0.f, 1.f)),
                        .age      = tl.life,
                        .pick     = pt.pick,
                    });
                }
                break;
            }
            push(pt);
            prev = &pt;
        }
        if(ribbon.points.size() >= 2)
            scene.ribbons.push_back(std::move(ribbon));
    }

    /* ---- Light volumes ---- */

    void draw_volumes(Proxy& p, EffectScene& scene)
    {
        for(auto it = m_volumes.begin(); it != m_volumes.end();)
        {
            auto at = marker_place(p, it->parent, it->marker);
            if(!at)
            {
                it = m_volumes.erase(it);
                continue;
            }
            f32 const level  = scale_of(p, it->parent, it->scale);
            auto      frames = it->tag->frames.data(*m_magic);
            if(level > 0.f && frames.has_value() && !frames.value().empty())
            {
                auto const& f     = frames.value()[0];
                i32 const   count = std::max<i32>(it->tag->count, 1);
                for(i32 i = 0; i < count; i++)
                {
                    f32 const t = count > 1 ? static_cast<f32>(i) / static_cast<f32>(count - 1) : 0.f;
                    f32 const along =
                        f.offset_from_marker + f.length * std::pow(t, std::max(f.offset_exponent, .01f));
                    f32 const rt = std::pow(t, std::max(f.radius_exponent, .01f));
                    f32 const ct = std::pow(t, std::max(f.tint_color_exponent, .01f));
                    scene.sprites.push_back({
                        .position = at->position + at->forward * along,
                        .radius   = glm::mix(f.radius_hither, f.radius_yon, rt),
                        .color    = glm::mix(
                                     rgba(f.tint_color_hither), rgba(f.tint_color_yon), ct) *
                                 Vecf4(1.f, 1.f, 1.f, level),
                        .bitmap   = it->tag->map.valid()
                                        ? &static_cast<blam::tagref_t const&>(it->tag->map)
                                        : nullptr,
                        .sequence = it->tag->sequence_index,
                        .blend    = blend_t::add,
                    });
                }
            }
            ++it;
        }
    }

    /* ---- Lens flares ---- */

    struct flare_source_t
    {
        lens_flare const* tag{nullptr};
        Vecf3             position{};
        Vecf3             direction{}; /* the way it shines, zero = all ways */
        bool              infinite{false};
        Vecf3             color{1.f};
        u64               key{0};
        u64               ignore{0}; /* its own object does not hide it */
    };

    struct viewer_t
    {
        u32   seat{0};
        Vecf3 eye{};
    };

    void step_flares(Proxy& p, EffectScene& scene, f32 dt)
    {
        std::vector<viewer_t> viewers;
        for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto [cam, info] = ent.components();
            if(info.is_remote() || info.seat_idx >= 4)
                continue;
            viewers.push_back({info.seat_idx, cam.camera.position});
        }
        if(viewers.empty())
            return;

        std::vector<flare_source_t> sources;
        for(auto const& l : m_lights)
            if(auto const* lens = l.level <= 0.f ? nullptr :
                   tag_data<lens_flare>(l.tag->lens_flare, blam::tag_class_t::lens))
                sources.push_back({
                    .tag       = lens,
                    .position  = l.place.position,
                    .direction = lens->cutoff_angle > 0.f ? l.place.forward : Vecf3{},
                    .color     = l.color * l.level,
                    .key       = l.id,
                    .ignore    = l.parent,
                });
        structure_flares(p, viewers.front().eye, sources);

        auto& physics = p.template subsystem<PhysicsBus>();
        f32 const step = std::min(1.f, dt * flare_fade_rate);
        for(auto const& src : sources)
        {
            auto& seen = m_flare_seen[src.key];
            EffectScene::flare_t flare{
                .tag      = src.tag,
                .position = src.infinite ? src.direction : src.position,
                .infinite = src.infinite,
                .color    = src.color,
            };
            bool any = false;
            for(auto const& v : viewers)
            {
                f32 const target = flare_target(physics, src, v.eye);
                f32&      vis    = seen[v.seat];
                vis += (target - vis) * step;
                flare.visibility[v.seat] = vis;
                any = any || vis > .002f;
            }
            if(any)
                scene.flares.push_back(flare);
        }
        std::erase_if(m_flare_seen, [&sources](auto const& entry) {
            return std::none_of(sources.begin(), sources.end(), [&entry](auto const& s) {
                return s.key == entry.first;
            });
        });
    }

    /* Fade by distance and angle, then nothing in the way */
    f32 flare_target(PhysicsBus& physics, flare_source_t const& src, Vecf3 const& eye)
    {
        lens_flare const& lens = *src.tag;
        f32               out  = 1.f;
        Vecf3             to_eye{};
        f32               distance = 0.f;
        if(!src.infinite)
        {
            to_eye   = eye - src.position;
            distance = glm::length(to_eye);
            if(distance < 1e-3f)
                return 0.f;
            to_eye /= distance;
            if(lens.far_fade_distance > lens.near_fade_distance)
                out *= 1.f - std::clamp(
                                 (distance - lens.near_fade_distance) /
                                     (lens.far_fade_distance - lens.near_fade_distance),
                                 0.f,
                                 1.f);
            if(lens.cutoff_angle > 0.f && glm::length(src.direction) > 0.f)
            {
                f32 const c = glm::dot(glm::normalize(src.direction), to_eye);
                out *= glm::smoothstep(
                    std::cos(lens.cutoff_angle), std::cos(lens.falloff_angle), c);
            }
        }
        if(out <= 0.f)
            return 0.f;
        if((lens.flags & lens_flare::flags_t::no_occlusion_test) !=
           lens_flare::flags_t::none)
            return out;
        Vecf3 const to = src.infinite
                             ? eye + src.direction * sun_distance
                             : src.position + to_eye * std::min(
                                                  lens.occlusion_radius, distance * .5f);
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = eye, .to = to, .ignore = src.ignore};
        physics.process(ev, &ray);
        if(ray.hit && ray.entity_id != src.ignore)
            return 0.f;
        return out;
    }

    /* The section's flare markers, and the sun of the sky above the eye */
    void structure_flares(Proxy& p, Vecf3 const& eye, std::vector<flare_source_t>& out)
    {
        auto const& cache = p.template subsystem<BSPCache<halo_version>>();
        BSPItem const* bsp = nullptr;
        for(auto const& [id, candidate] : cache.m_cache)
            if(candidate.valid() && candidate.section_idx == cache.active_section)
                bsp = &candidate;
        if(!bsp || !bsp->mesh)
            return;
        auto const& magic   = bsp->bsp_magic;
        auto        palette = bsp->mesh->lens_flares.data(magic);
        auto        markers = bsp->mesh->lens_flare_markers.data(magic);
        if(palette.has_value() && markers.has_value())
            for(size_t i = 0; i < markers.value().size(); i++)
            {
                auto const& m = markers.value()[i];
                if(m.lens_flare < 0 ||
                   static_cast<size_t>(m.lens_flare) >= palette.value().size())
                    continue;
                auto const* lens = tag_data<lens_flare>(
                    palette.value()[m.lens_flare], blam::tag_class_t::lens);
                if(!lens)
                    continue;
                out.push_back({
                    .tag       = lens,
                    .position  = m.position,
                    .direction = Vecf3(m.direction[0], m.direction[1], m.direction[2]) / 127.f,
                    .key       = (1ull << 62) | (u64(bsp->section_idx) << 20) | i,
                });
            }

        auto cluster = bsp->find_cluster_tree(eye);
        if(!cluster)
            return;
        auto clusters = bsp->mesh->clusters.data(magic);
        auto scenario = p.template subsystem<BlamFiles<halo_version>>()
                            .container.scenario();
        if(!clusters.has_value() || *cluster >= clusters.value().size() ||
           !scenario.has_value() || !scenario.value())
            return;
        i16 const sky = clusters.value()[*cluster].sky;
        auto      skies = scenario.value()->info.skyboxes.data(*m_magic);
        if(sky < 0 || !skies.has_value() || static_cast<size_t>(sky) >= skies.value().size())
            return;
        auto const* skybox =
            tag_data<blam::scn::skybox>(skies.value()[sky], blam::tag_class_t::sky);
        if(!skybox)
            return;
        auto lights = skybox->lights.data(*m_magic);
        if(!lights.has_value())
            return;
        for(size_t i = 0; i < lights.value().size(); i++)
        {
            auto const& l    = lights.value()[i];
            auto const* lens = tag_data<lens_flare>(l.lens_flare, blam::tag_class_t::lens);
            if(!lens)
                continue;
            f32 const yaw   = l.radiosity.direction.x;
            f32 const pitch = l.radiosity.direction.y;
            out.push_back({
                .tag       = lens,
                .direction = Vecf3(
                    std::cos(pitch) * std::cos(yaw),
                    std::cos(pitch) * std::sin(yaw),
                    std::sin(pitch)),
                .infinite = true,
                .color    = l.radiosity.color,
                .key      = (2ull << 62) | i,
            });
        }
    }

    static constexpr size_t max_particles        = 4096;
    static constexpr f32    flare_fade_rate      = 8.f;    /* per second */
    static constexpr f32    sun_distance         = 2000.f; /* wu */
    static constexpr size_t max_contrail_points  = 256;
    static constexpr f32    default_light_life   = .25f;
    static constexpr f32    ticks_per_second     = 30.f;
    static constexpr f32    explosion_speed      = 2.f;  /* per wu from the centre */
    static constexpr f32    pctl_scale_to_radius = 64.f; /* scale is per bitmap pixel */

    time_point m_last_frame{};
    u32        m_generation{0};

    blam::map_ptr const*               m_magic{nullptr};
    blam::tag_index_view<halo_version> m_index;
    std::minstd_rand                   m_rng{std::random_device{}()};

    std::vector<SpawnEffectEvent>    m_requests;
    std::vector<effect_instance_t>   m_effects;
    std::vector<light_instance_t>    m_lights;
    std::vector<system_instance_t>   m_systems;
    std::vector<contrail_instance_t> m_contrails;
    std::vector<volume_instance_t>   m_volumes;
    std::vector<sprite_particle_t>   m_particles;
    std::set<u64>                    m_attached;
    u64                              m_light_ids{0};
    std::unordered_map<u64, std::array<f32, 4>> m_flare_seen;

    std::map<std::pair<blam::tag_t const*, std::string>, std::optional<Matf4>>
        m_markers;

    std::shared_ptr<GameEventBus::queue_type<SpawnEffectEvent>> m_spawn_queue;
};

void alloc_effects(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<EffectScene>();
    auto& effects = e.register_subsystem_inplace<EffectSystem>();
    effects.m_spawn_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<SpawnEffectEvent>(
            0, [&effects](GameEvent&, SpawnEffectEvent* spawn) {
                effects.m_requests.push_back(*spawn);
            });
}
