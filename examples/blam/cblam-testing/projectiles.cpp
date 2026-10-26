#include "projectiles.h"

#include <glm/gtx/rotate_vector.hpp>

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_unit.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <random>
#include <set>

using ProjectileManifest = compo::SubsystemManifest<
    type_list_t<Projectile, Model, const ObjectPhysics, const PlayerInfo, const PlayerCamera, const BipedStatus>,
    type_list_t<
        PhysicsBus,
        GameEventBus,
        const NetworkState,
        const BlamFiles<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct ProjectileSystem
    : compo::RestrictedSubsystem<ProjectileSystem, ProjectileManifest>
{
    using type  = ProjectileSystem;
    using Proxy = compo::proxy_of<ProjectileManifest>;

    using proj_t     = blam::scn::projectile;
    using response_t = proj_t::response_t;
    using material_t = blam::shader::radiosity_properties::physics_material;

    ProjectileSystem()
    {
        this->priority = 903;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        m_blast_queue->poll();
        for(auto const& blast : std::exchange(m_blasts, {}))
            thrown(p, blast);
        std::set<u64> spent;
        for(auto const& tick : m_ticks->take())
            for(auto ent : p.template select<Projectile, Model>())
            {
                auto [flight, model] = ent.components();
                if(!spent.contains(ent.id()) &&
                   (!flight.tag || !fly(p, flight, model, tick.seconds)))
                    spent.insert(ent.id());
            }
        if(spent.empty())
            return;
        std::set<u64> doomed = spent;
        for(auto id : spent)
            if(auto const* model = p.template get<Model>(id))
                for(auto const& part : model->parts)
                    doomed.insert(part.id());
        p.remove_entity_if([&doomed](compo::Entity const& e) {
            return doomed.contains(e.id);
        });
    }

    /* One step of flight; false once it is spent */
    bool fly(Proxy& p, Projectile& flight, Model& model, f32 dt)
    {
        proj_t const& tag = *flight.tag;
        flight.age += dt;
        if(flight.combined)
            return false;

        if(flight.timer >= 0.f)
        {
            flight.timer -= dt;
            if(flight.timer <= 0.f)
            {
                /* Stuck in something, it hurts that, not what is near */
                if(flight.attached && flight.attached_to)
                    hit(p, flight, model, flight.attached_to, tag.attached_detonation_damage,
                        model.position, {0, 0, 1}, -1, false);
                return detonate(p, flight, model.position, {0, 0, 1}, nullptr);
            }
        }
        if(tag.maximum_range > 0.f && flight.travelled >= tag.maximum_range)
            return detonate(
                p, flight, model.position, -direction(flight), nullptr);

        /* Speed eases from initial to final over the range, until it bounces */
        f32 const speed = glm::length(flight.velocity);
        if(flight.bounces == 0 && speed > 0.f &&
           tag.initial_velocity != tag.final_velocity && tag.maximum_range > 0.f)
        {
            f32 const along =
                std::min(flight.travelled / tag.maximum_range, 1.f);
            flight.velocity *=
                glm::mix(tag.initial_velocity, tag.final_velocity, along) *
                ticks_per_second / speed;
        }
        if(flight.resting)
            return true;
        if(flight.attached)
        {
            /* Stuck to a unit, it goes where the unit goes */
            if(auto const* host = flight.attached_to ? p.template get<Model>(flight.attached_to) : nullptr)
                place(flight, model, Vecf3(host->transform * Vecf4(flight.attach_offset, 1.f)));
            return true;
        }
        flight.velocity.z -= halo_gravity * tag.air_gravity_scale * dt;
        seek(p, flight, model.position, dt);

        Vecf3 const from = model.position;
        Vecf3 const to   = from + flight.velocity * dt;
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = from, .to = to, .ignore = flight.owner};
        p.template subsystem<PhysicsBus>().process(ev, &ray);

        fly_by(p, flight, from, ray.hit ? ray.point : to);
        if(!ray.hit)
        {
            flight.travelled += glm::distance(from, to);
            place(flight, model, to);
            return true;
        }
        flight.travelled += glm::distance(from, ray.point);
        return impact(p, flight, model, ray);
    }

    bool impact(
        Proxy&                  p,
        Projectile&             flight,
        Model&                  model,
        Physics::Raycast const& ray)
    {
        proj_t const&              tag      = *flight.tag;
        auto const&                magic    = files(p).container.magic;
        auto const                 material = struck_material(p, ray, files(p));
        proj_t::material_response_t const* response = nullptr;
        if(auto responses = tag.material_responses.data(magic);
           responses.has_value() &&
           static_cast<size_t>(material) < responses.value().size())
            response = &responses.value()[static_cast<size_t>(material)];

        response_t what = response ? response->default_response : response_t::detonate;
        /* The potential response takes over where the tag conditions it:
         * against (or away from) units, within a speed or an angle to the
         * surface, or a fraction of the time */
        if(response && (response->potential_flags != 0 || response->potential_and.y > 0.f ||
                        response->potential_between.y > 0.f ||
                        response->potential_skip_fraction > 0.f))
        {
            bool const unit        = ray.entity_id != 0 && is_unit(p, ray.entity_id);
            bool const only_units  = (response->potential_flags & 0x1) != 0;
            bool const never_units = (response->potential_flags & 0x2) != 0;
            f32 const  speed       = glm::length(flight.velocity) / ticks_per_second;
            f32 const  incidence =
                std::acos(std::clamp(-glm::dot(direction(flight), ray.normal), -1.f, 1.f));
            bool apply = (!only_units || unit) && (!never_units || !unit);
            if(response->potential_and.y > 0.f)
                apply = apply && speed >= response->potential_and.x &&
                        speed <= response->potential_and.y;
            if(response->potential_between.y > 0.f)
                apply = apply && incidence >= response->potential_between.x &&
                        incidence <= response->potential_between.y;
            if(response->potential_skip_fraction > 0.f)
                apply = apply && std::uniform_real_distribution<f32>(0.f, 1.f)(m_rng) >=
                                     response->potential_skip_fraction;
            if(apply)
                what = response->potential_response;
        }
        /* Along what it fell onto: it slides, without a clatter each
         * step, and settles once slow */
        if(what == response_t::reflect && ray.entity_id == 0 &&
           -glm::dot(flight.velocity, ray.normal) < rest_speed)
        {
            Vecf3 const along = flight.velocity - ray.normal * glm::dot(flight.velocity, ray.normal);
            flight.velocity   = along * (1.f - (response ? response->parallel_friction : 0.f));
            place(flight, model, ray.point + ray.normal * surface_gap);
            if(glm::length(flight.velocity) < rest_speed)
            {
                flight.velocity = {};
                flight.resting  = true;
                start_timer(flight);
            }
            return true;
        }
        if(ray.entity_id != 0)
            struck(p, flight, model, ray);
        if(response)
            play(p, response->default_effect, ray.point, ray.normal, {});

        switch(what)
        {
        case response_t::reflect: {
            Vecf3 const n      = ray.normal;
            Vecf3 const v      = flight.velocity;
            Vecf3 const normal = n * glm::dot(v, n);
            Vecf3 const along  = v - normal;
            f32 const   keep_n =
                1.f - (response ? response->perpendicular_friction : 0.f);
            f32 const keep_t =
                1.f - (response ? response->parallel_friction : 0.f);
            flight.velocity = along * keep_t - normal * keep_n;
            flight.bounces++;
            if(tag.detonation_timer_starts ==
               proj_t::timer_starts_t::after_first_bounce)
                start_timer(flight);
            /* Off the surface a little, so the next step starts clear */
            place(flight, model, ray.point + n * surface_gap);
            if(glm::length(flight.velocity) < rest_speed)
            {
                flight.velocity = {};
                start_timer(flight);
            }
            return true;
        }
        case response_t::attach:
            flight.velocity = {};
            flight.attached = true;
            place(flight, model, ray.point + ray.normal * surface_gap);
            if(auto const* host = ray.entity_id ? p.template get<Model>(ray.entity_id) : nullptr)
            {
                flight.attached_to   = ray.entity_id;
                flight.attach_offset = Vecf3(glm::inverse(host->transform) * Vecf4(model.position, 1.f));
                if(super_combine(p, flight, *host))
                    return false;
            }
            start_timer(flight);
            return true;
        case response_t::overpenetrate:
            if(ray.entity_id != 0)
            {
                flight.owner = ray.entity_id;
                place(flight, model, ray.point);
                return true;
            }
            [[fallthrough]];
        case response_t::detonate:
            return detonate(
                p,
                flight,
                ray.point,
                ray.normal,
                response && response->detonation_effect.valid()
                    ? &static_cast<blam::tagref_t const&>(
                          response->detonation_effect)
                    : nullptr);
        case response_t::disappear:
        default:
            return false;
        }
    }

    /* Its whizz, once, where it passes nearest a local player it was not
     * fired by */
    void fly_by(Proxy& p, Projectile& flight, Vecf3 const& from, Vecf3 const& to)
    {
        if(flight.flew_by || !flight.tag->flyby_sound.valid())
            return;
        Vecf3 const step   = to - from;
        f32 const   length = glm::dot(step, step);
        for(auto ent : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = ent.components();
            if(info.is_remote() || !cam.is_active() || ent.id() == flight.shooter ||
               ent.id() == flight.owner)
                continue;
            Vecf3 const ear = cam.camera.position;
            f32 const   t =
                length > 0.f ? std::clamp(glm::dot(ear - from, step) / length, 0.f, 1.f) : 0.f;
            Vecf3 const nearest = from + step * t;
            if(glm::distance(nearest, ear) > flyby_reach)
                continue;
            flight.flew_by = true;
            if(auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>())
            {
                SoundEvent     ev{.type = SoundEvent::play_sound};
                PlaySoundEvent play{
                    .sound    = &static_cast<blam::tagref_t const&>(flight.tag->flyby_sound),
                    .position = nearest,
                };
                sounds->inject(ev, &play);
            }
            return;
        }
    }

    /* A biped or a vehicle: what a grenade sticks to */
    /* A guided projectile turns toward the unit it was fired at, as fast
     * as its tag lets it */
    void seek(Proxy& p, Projectile& flight, Vecf3 const& from, f32 dt)
    {
        f32 const rate = flight.tag->guided_angular_velocity;
        if(!flight.target || rate <= 0.f)
            return;
        auto const* target = p.template get<Model>(flight.target);
        auto const* status = p.template get<BipedStatus>(flight.target);
        if(!target || (status && status->dead))
        {
            flight.target = 0;
            return;
        }
        f32 const speed = glm::length(flight.velocity);
        Vecf3 const to  = target->position + Vecf3{0.f, 0.f, aim_height(p, *target)} - from;
        if(speed < 1e-4f || glm::dot(to, to) < 1e-6f)
            return;
        Vecf3 const have  = flight.velocity / speed;
        Vecf3 const want  = glm::normalize(to);
        f32 const   angle = std::acos(std::clamp(glm::dot(have, want), -1.f, 1.f));
        f32 const   turn  = rate * dt;
        Vecf3 const axis  = glm::cross(have, want);
        if(angle <= turn || glm::dot(axis, axis) < 1e-8f)
            flight.velocity = (angle <= turn ? want : have) * speed;
        else
            flight.velocity = glm::rotate(have, turn, glm::normalize(axis)) * speed;
    }

    /* The middle of a biped's standing pill, so a grunt is not overshot;
     * other targets by a fixed height */
    f32 aim_height(Proxy& p, Model const& target)
    {
        if(!target.origin_object || !target.origin_object->matches(blam::tag_class_t::bipd))
            return seek_height;
        auto biped = target.origin_object->template data<blam::scn::biped>(files(p).container.magic);
        if(!biped.has_value() || biped.value()[0].standing_collision_height <= 0.f)
            return seek_height;
        return biped.value()[0].standing_collision_height * .5f;
    }

    bool is_unit(Proxy& p, u64 id)
    {
        if(p.template get<PlayerInfo>(id))
            return true;
        auto const* physics = p.template get<ObjectPhysics>(id);
        return physics && (physics->upright || physics->drive);
    }

    /* Hits on bodies, from the authority's copy; bullets fired as hitscan
     * were resolved when the trigger was pulled */
    void struck(
        Proxy&                  p,
        Projectile const&       flight,
        Model const&            model,
        Physics::Raycast const& ray)
    {
        if(Projectile::hitscan(*flight.tag))
            return;
        hit(p, flight, model, ray.entity_id, flight.tag->impact_damage,
            ray.point, ray.normal, ray.part, ray.head);
    }

    void hit(
        Proxy&                p,
        Projectile const&     flight,
        Model const&          model,
        u64                   entity,
        blam::tagref_t const& damage,
        Vecf3 const&          point,
        Vecf3 const&          normal,
        i16                   part,
        bool                  head)
    {
        if(!damage.valid() || p.template subsystem<NetworkState>().replica)
            return;
        auto const* target  = p.template get<PlayerInfo>(entity);
        auto const* shooter = p.template get<PlayerInfo>(flight.shooter);
        ObjectHitEvent hit{
            .projectile = model.origin_object ? model.origin_object->as_ref()
                                              : blam::tagref_t{},
            .damage     = damage,
            .target     = entity,
            .shooter    = flight.shooter,
            .target_player =
                target ? target->player_idx : ObjectHitEvent::no_player,
            .shooter_player =
                shooter ? shooter->player_idx : ObjectHitEvent::no_player,
            .point     = point,
            .normal    = normal,
            .direction = direction(flight),
            .part      = part,
            .head      = head,
        };
        GameEvent ev{.type = GameEvent::ObjectHit};
        p.template subsystem<GameEventBus>().inject(ev, &hit);
    }

    /* Enough of a super-combining kind stuck in one unit go off together,
     * as one super detonation in its middle */
    bool super_combine(Proxy& p, Projectile const& flight, Model const& host)
    {
        if((flight.tag->projectile_flags & proj_t::flags_t::has_super_combining_explosion) ==
               proj_t::flags_t::none ||
           !flight.tag->super_detonation.valid())
            return false;
        std::vector<Projectile*> stuck;
        for(auto ent : p.template select<Projectile>())
        {
            auto& other = ent.template get<Projectile>();
            if(&other != &flight && other.attached && !other.combined &&
               other.attached_to == flight.attached_to && other.tag == flight.tag)
                stuck.push_back(&other);
        }
        if(stuck.size() + 1 < super_combine_count)
            return false;
        for(auto* other : stuck)
            other->combined = true;
        play(p, flight.tag->super_detonation, host.position + Vecf3{0.f, 0.f, aim_height(p, host)}, {0, 0, 1}, {});
        return true;
    }

    /* Plays its own detonation, and what the material adds to it */
    bool detonate(
        Proxy&                p,
        Projectile const&     flight,
        Vecf3 const&          at,
        Vecf3 const&          normal,
        blam::tagref_t const* effect)
    {
        blam::tagref_t const& own = flight.tag->detonation_effect;
        play(p, own, at, normal, {});
        if(effect && effect->tag_id != own.tag_id)
            play(p, *effect, at, normal, {});
        return false;
    }

    /* What falls is thrown by a blast, as on every peer; what flies straight
     * or is stuck somewhere stays its course */
    void thrown(Proxy& p, Physics::Blast const& blast)
    {
        f32 const inner = std::min(blast.radius.x, blast.radius.y);
        for(auto ent : p.template select<Projectile, Model>())
        {
            auto [flight, model] = ent.components();
            if(!flight.tag || flight.attached || flight.tag->air_gravity_scale <= 0.f)
                continue;
            Vecf3 const away = model.position - blast.position;
            f32 const   d    = glm::length(away);
            if(d > blast.radius.y)
                continue;
            f32 const scale =
                d <= inner ? 1.f : 1.f - (d - inner) / std::max(blast.radius.y - inner, 1e-3f);
            Vecf3 dir = d > 1e-3f ? away / d : Vecf3{0, 0, 1};
            /* Thrown up off the ground, not slid along it */
            if(dir.z < blast_lean)
            {
                Vecf2 const flat = Vecf2(dir);
                dir = glm::normalize(
                    Vecf3(glm::length(flat) > 1e-3f ? glm::normalize(flat) : Vecf2{1, 0}, 1.f));
            }
            flight.velocity += dir * (blast.speed * scale);
            flight.resting = false;
        }
    }

    /* Once; a zero timer means it never goes off by itself */
    void start_timer(Projectile& flight)
    {
        Vecf2 const timer = flight.tag->timer;
        if(flight.timer >= 0.f || timer.y <= 0.f)
            return;
        flight.timer = std::uniform_real_distribution<f32>(
            std::min(timer.x, timer.y), timer.y)(m_rng);
    }

    void place(Projectile const& flight, Model& model, Vecf3 const& at)
    {
        model.position = at;
        if((flight.tag->projectile_flags &
            proj_t::flags_t::oriented_along_velocity) != proj_t::flags_t::none)
        {
            Vecf3 const dir = direction(flight);
            if(glm::length(dir) > 0.f)
                model.rotation =
                    glm::angleAxis(std::atan2(dir.y, dir.x), Vecf3{0, 0, 1}) *
                    glm::angleAxis(
                        -std::asin(std::clamp(dir.z, -1.f, 1.f)),
                        Vecf3{0, 1, 0});
        }
        model.update_matrix();
    }

    static Vecf3 direction(Projectile const& flight)
    {
        f32 const speed = glm::length(flight.velocity);
        return speed > 0.f ? flight.velocity / speed : Vecf3{};
    }

    /* The structure's shader says; bodies go by what they are */
    void play(
        Proxy&                p,
        blam::tagref_t const& effect,
        Vecf3 const&          at,
        Vecf3 const&          direction,
        Vecf3 const&          velocity)
    {
        if(!effect.valid())
            return;
        GameEvent        ev{.type = GameEvent::SpawnEffect};
        SpawnEffectEvent spawn{
            .effect    = effect,
            .position  = at,
            .direction = direction,
            .velocity  = velocity,
        };
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
    }

    BlamFiles<halo_version> const& files(Proxy& p)
    {
        return p.template subsystem<BlamFiles<halo_version>>();
    }

    static constexpr f32 ticks_per_second = 30.f;
    static constexpr f32 halo_gravity     = 9.81f / 3.048f; /* wu/s^2 */
    static constexpr f32 surface_gap      = .01f;
    static constexpr f32 seek_height      = .6f; /* wu above a non-biped's origin aimed for */
    static constexpr size_t super_combine_count = 7; /* stuck in one unit */
    static constexpr f32 rest_speed       = .3f; /* wu/s */
    static constexpr f32 flyby_reach      = 1.5f; /* wu, from the ear */
    static constexpr f32 blast_lean       = .7071f; /* sin 45 */

    std::minstd_rand           m_rng{std::random_device{}()};
    std::unique_ptr<TickQueue> m_ticks;
    std::shared_ptr<PhysicsBus::queue_type<Physics::Blast>> m_blast_queue;
    std::vector<Physics::Blast>                             m_blasts;

    friend void alloc_projectiles(compo::EntityContainer& e);
};

void alloc_projectiles(compo::EntityContainer& e)
{
    auto& projectiles = e.register_subsystem_inplace<ProjectileSystem>();
    projectiles.m_ticks =
        std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    projectiles.m_blast_queue =
        e.subsystem_cast<PhysicsBus>().addQueuedEventFunction<Physics::Blast>(
            0, [&projectiles](Physics::Event&, Physics::Blast* blast) {
                projectiles.m_blasts.push_back(*blast);
            });
}
