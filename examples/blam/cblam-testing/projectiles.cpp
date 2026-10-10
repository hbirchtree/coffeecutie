#include "projectiles.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "physics.h"
#include "selected_version.h"

#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_unit.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <random>
#include <set>

using ProjectileManifest = compo::SubsystemManifest<
    type_list_t<Projectile, Model, const ObjectPhysics, const PlayerInfo>,
    type_list_t<PhysicsBus, GameEventBus, const BlamFiles<halo_version>>,
    empty_list_t>;

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
        if(dt <= 0.f)
            return;

        std::set<u64> spent;
        for(auto ent : p.template select<Projectile, Model>())
        {
            auto [flight, model] = ent.components();
            if(!flight.tag || !fly(p, flight, model, dt))
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

        if(flight.timer >= 0.f)
        {
            flight.timer -= dt;
            if(flight.timer <= 0.f)
                return detonate(p, flight, model.position, {0, 0, 1}, nullptr);
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
        flight.velocity.z -= halo_gravity * tag.air_gravity_scale * dt;

        Vecf3 const from = model.position;
        Vecf3 const to   = from + flight.velocity * dt;
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = from, .to = to, .ignore = flight.owner};
        p.template subsystem<PhysicsBus>().process(ev, &ray);

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
        auto const                 material = material_of(p, ray);
        proj_t::material_response_t const* response = nullptr;
        if(auto responses = tag.material_responses.data(magic);
           responses.has_value() &&
           static_cast<size_t>(material) < responses.value().size())
            response = &responses.value()[static_cast<size_t>(material)];

        response_t const what =
            response ? response->default_response : response_t::detonate;
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
            place(flight, model, ray.point + ray.normal * surface_gap);
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

    /* Plays the detonation, by the material's override or its own */
    bool detonate(
        Proxy&                p,
        Projectile const&     flight,
        Vecf3 const&          at,
        Vecf3 const&          normal,
        blam::tagref_t const* effect)
    {
        blam::tagref_t const& own = flight.tag->detonation_effect;
        play(p, effect ? *effect : own, at, normal, {});
        return false;
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
    material_t material_of(Proxy& p, Physics::Raycast const& ray)
    {
        if(ray.entity_id == 0)
        {
            if(!ray.shader || !ray.shader->valid())
                return material_t::stone;
            auto const& f = files(p);
            blam::tag_index_view<halo_version> index(f.container);
            auto shader = index.find(*ray.shader);
            if(shader == index.end())
                return material_t::stone;
            auto surface = (*shader).template data<
                blam::shader::radiosity_properties>(f.container.magic);
            return surface.has_value() ? surface.value()[0].physics
                                       : material_t::stone;
        }
        if(p.template get<PlayerInfo>(ray.entity_id))
            return material_t::cyborg_armor;
        if(auto const* physics = p.template get<ObjectPhysics>(ray.entity_id);
           physics && physics->drive)
            return material_t::metal_thicc;
        return material_t::metal_thin;
    }

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
    static constexpr f32 rest_speed       = .3f; /* wu/s */

    time_point       m_last_frame{};
    std::minstd_rand m_rng{std::random_device{}()};
};

void alloc_projectiles(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<ProjectileSystem>();
}
