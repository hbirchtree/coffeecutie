#include "weapons.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
#include "ticks.h"

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_unit.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using WeaponManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerInput,
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo,
        const Attachment,
        const AnimationPlayback,
        Model>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const NetworkState,
        const BlamFiles<halo_version>>,
    empty_list_t>;

struct WeaponSystem : compo::RestrictedSubsystem<WeaponSystem, WeaponManifest>
{
    using type  = WeaponSystem;
    using Proxy = compo::proxy_of<WeaponManifest>;

    using weapon_t  = blam::scn::weapon;
    using trigger_t = weapon_t::trigger_t;
    using input_t   = weapon_t::weapon_function_in_t;

    struct state_t
    {
        f32  cooldown{0.f};
        f32  illumination{0.f}; /* the flash, 1 as it fires */
        u32  shots{0};          /* alternates twin barrels */
        bool firing{false};     /* held last frame */
        bool fired{false};      /* shot this frame */
    };

    WeaponSystem()
    {
        this->priority = 850;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const ticks = m_ticks->take();

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(files.load_generation != m_generation)
        {
            m_generation = files.load_generation;
            m_states.clear();
            m_fire_queue->poll();
            m_shots.clear();
        }
        auto const& magic = files.container.magic;

        std::set<u64> live;
        for(auto ent : p.template select<
                       PlayerInput,
                       PlayerInfo,
                       PlayerCamera,
                       NetworkInfo>())
        {
            auto [input, info, cam, net] = ent.components();
            /* A gunner fires the vehicle's weapon, a body its own; the free
             * camera has nothing to shoot with */
            u64 const   vehicle = info.riding.vehicle;
            /* Held weapons are only good for the map they were found in */
            auto const* weapon_tag =
                vehicle ? mounted_weapon(p, info, files)
                : info.biped.load_generation == files.load_generation
                    ? info.biped.weapon
                    : nullptr;
            if(info.is_remote() || !weapon_tag ||
               (vehicle ? info.riding.exiting
                        : !info.mode.physics || !biped_in_play(info, cam, net)))
                continue;
            auto weapon = weapon_tag->template data<weapon_t>(magic);
            if(!weapon.has_value())
                continue;
            auto triggers = weapon.value()[0].triggers.data(magic);
            if(!triggers.has_value() || triggers.value().empty())
                continue;
            live.insert(ent.id());
            auto&            state   = m_states[ent.id()];
            trigger_t const& trigger = triggers.value()[0];
            bool const       held    = input.intent.fire;

            bool const repeats =
                (trigger.flags & trigger_t::flags_t::does_not_repeat_automatically) ==
                trigger_t::flags_t::none;
            /* The trigger is looked at once a tick */
            for(auto const& tick : ticks)
            {
                f32 const dt   = tick.seconds;
                state.fired    = false;
                state.cooldown = std::max(state.cooldown - dt, 0.f);
                if(held && state.cooldown <= 0.f && (repeats || !state.firing))
                {
                    f32 const rate = std::max(
                        trigger.maximum_rate_of_fire.y > 0.f
                            ? trigger.maximum_rate_of_fire.y
                            : trigger.maximum_rate_of_fire.x,
                        .5f);
                    state.cooldown += 1.f / rate;
                    state.fired        = true;
                    state.illumination = 1.f;
                    pull(p, ent.id(), info, cam, *weapon_tag, state.shots++);
                }
                state.firing = held;
                if(trigger.illumination_recovery_time > 0.f)
                    state.illumination = std::max(
                        state.illumination -
                            dt / trigger.illumination_recovery_time,
                        0.f);
                else if(!state.fired)
                    state.illumination = 0.f;
            }

            if(!vehicle)
                drive_functions(
                    p, ent.id(), weapon.value()[0], state, held, magic);
        }
        std::erase_if(m_states, [&live](auto const& e) {
            return !live.contains(e.first);
        });

        m_fire_queue->poll();
        for(auto const& shot : std::exchange(m_shots, {}))
            fire(p, shot, files);
    }

    /* The weapon of the vehicle seat a gunner sits in */
    blam::tag_t const* mounted_weapon(
        Proxy& p, PlayerInfo const& info, BlamFiles<halo_version> const& files)
    {
        using flags_t      = blam::scn::unit::seat_t::seat_flags_t;
        auto const* model  = p.template get<Model>(info.riding.vehicle);
        auto const& magic  = files.container.magic;
        if(!model || !model->origin_object)
            return nullptr;
        auto unit = model->origin_object->template data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return nullptr;
        auto seats   = unit.value()[0].seats.data(magic);
        auto weapons = unit.value()[0].weapons.data(magic);
        if(!seats.has_value() || !weapons.has_value() ||
           weapons.value().empty() || info.riding.seat < 0 ||
           static_cast<size_t>(info.riding.seat) >= seats.value().size() ||
           (seats.value()[info.riding.seat].flags & flags_t::gunner) ==
               flags_t::none)
            return nullptr;
        blam::tag_index_view<halo_version> index(files.container);
        auto weapon = index.find(weapons.value()[0].weapon.tag_id);
        return weapon != index.end() ? &(*weapon) : nullptr;
    }

    /* Raises the shot, from the eye or a vehicle's gun; every peer fires it
     * from the event */
    void pull(
        Proxy&              p,
        u64                 player,
        PlayerInfo const&   info,
        PlayerCamera const& cam,
        blam::tag_t const&  weapon,
        u32                 shot_index)
    {
        Matf3 const     to_world = glm::transpose(Matf3(cam.rotation));
        Vecf3 const     forward  = to_world * Vecf3{0.f, 0.f, -1.f};
        WeaponFireEvent shot{
            .weapon     = weapon.as_ref(),
            .shooter    = player,
            .player_idx = info.player_idx,
            .seed       = static_cast<u32>(m_rng()),
            .position   = cam.camera.position + forward * muzzle_distance,
            .direction  = forward,
        };
        if(info.riding.vehicle != 0)
            mount(p, shot, cam, info.riding.vehicle, shot_index);
        GameEvent ev{.type = GameEvent::WeaponFire};
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* From the gun's marker, towards what the crosshair is on */
    void mount(
        Proxy&              p,
        WeaponFireEvent&    shot,
        PlayerCamera const& cam,
        u64                 vehicle,
        u32                 shot_index)
    {
        auto const* model = p.template get<Model>(vehicle);
        if(!model || !model->tag)
            return;
        auto marker = bind_marker(
            *model->tag,
            gun_marker,
            p.template subsystem<BlamFiles<halo_version>>().container.magic,
            shot_index);
        if(!marker)
            return;
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast aim{
            .from   = cam.camera.position,
            .to     = cam.camera.position + shot.direction * aim_reach,
            .ignore = vehicle,
        };
        p.template subsystem<PhysicsBus>().process(ev, &aim);
        Vecf3 const muzzle = Vecf3(model->transform * (*marker)[3]);
        Vecf3 const target = aim.hit ? aim.point : aim.to;
        shot.vehicle       = vehicle;
        shot.position      = muzzle;
        if(glm::distance(target, muzzle) > 0.f)
            shot.direction = glm::normalize(target - muzzle);
    }

    /* Projectiles from the shot's origin, the firing effect at the muzzle */
    void fire(
        Proxy&                         p,
        WeaponFireEvent const&         shot,
        BlamFiles<halo_version> const& files)
    {
        auto&       game  = p.template subsystem<GameEventBus>();
        auto const& magic = files.container.magic;

        blam::tag_index_view<halo_version> index(files.container);
        auto tag = index.find(shot.weapon.tag_id);
        if(tag == index.end() || !(*tag).matches(blam::tag_class_t::weap))
            return;
        auto weapon = (*tag).template data<weapon_t>(magic);
        if(!weapon.has_value())
            return;
        auto triggers = weapon.value()[0].triggers.data(magic);
        if(!triggers.has_value() || shot.trigger >= triggers.value().size())
            return;
        trigger_t const& trigger = triggers.value()[shot.trigger];

        std::minstd_rand rng(shot.seed);
        Vecf3 const      forward = shot.direction;
        if(auto projectile = index.find(trigger.projectile.tag_id);
           trigger.projectile.valid() && projectile != index.end())
        {
            auto const* proj =
                (*projectile).template data<blam::scn::projectile>(magic).value();
            /* Only the authority decides what bullets hit */
            bool const resolve = Projectile::hitscan(*proj) &&
                                 !p.template subsystem<NetworkState>().replica;
            i32 const count = std::max<i32>(trigger.projectiles_per_shot, 1);
            f32 const error = std::max(trigger.error_angle.x, trigger.minimum_error);
            for(i32 i = 0; i < count; i++)
            {
                f32 const spread =
                    count > 1 ? std::max(error, trigger.distribution_angle * .5f) : error;
                Vecf3 const dir = cone(rng, forward, spread);
                if(resolve)
                    trace(p, shot, (*projectile).as_ref(), *proj, dir);
                SpawnObjectEvent spawn{
                    .object   = trigger.projectile,
                    .position = shot.position,
                    .rotation = glm::angleAxis(std::atan2(dir.y, dir.x), Vecf3{0, 0, 1}) *
                                glm::angleAxis(
                                    -std::asin(std::clamp(dir.z, -1.f, 1.f)),
                                    Vecf3{0, 1, 0}),
                    .local_only = true,
                    .owner      = shot.vehicle ? shot.vehicle : shot.shooter,
                    .shooter    = shot.shooter,
                };
                GameEvent ev{.type = GameEvent::SpawnObject};
                game.inject(ev, &spawn);
            }
        }

        u64 const muzzle =
            shot.vehicle ? shot.vehicle : first_person(p, shot.shooter, true);
        if(auto effects = trigger.firing_effects.data(magic);
           effects.has_value() && !effects.value().empty())
        {
            size_t pick = 0;
            if((trigger.flags & trigger_t::flags_t::random_firing_effects) !=
               trigger_t::flags_t::none)
                pick = std::uniform_int_distribution<size_t>(
                    0, effects.value().size() - 1)(rng);
            auto const& effect = effects.value()[pick].firing_effect;
            if(effect.valid())
            {
                SpawnEffectEvent play{
                    .effect    = effect,
                    .position  = shot.position,
                    .direction = forward,
                    .parent    = muzzle,
                };
                GameEvent ev{.type = GameEvent::SpawnEffect};
                game.inject(ev, &play);
            }
        }

        /* The first-person hands and weapon kick together */
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            if(shot.vehicle)
                break;
            if(ent.template get<Attachment>().parent != shot.shooter)
                continue;
            auto const& anim = ent.template get<AnimationPlayback>();
            i32 const   clip = fire_clip(anim, magic);
            if(clip < 0)
                continue;
            PlayModelAnimationEvent play{
                .entity    = ent.id(),
                .animation = clip,
                .fade      = 0.f,
            };
            GameEvent ev{.type = GameEvent::PlayModelAnimation};
            game.inject(ev, &play);
        }
    }

    /* A bullet's flight as one ray, against bipeds as the shooter saw them */
    void trace(
        Proxy&                       p,
        WeaponFireEvent const&       shot,
        blam::tagref_t const&        projectile,
        blam::scn::projectile const& proj,
        Vecf3 const&                 direction)
    {
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{
            .from   = shot.position,
            .to     = shot.position + direction * Projectile::reach(proj),
            .ignore = shot.vehicle ? shot.vehicle : shot.shooter,
            .rewind = shot.view_time,
        };
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        if(!ray.hit || ray.entity_id == 0)
            return;
        auto const*    target = p.template get<PlayerInfo>(ray.entity_id);
        ObjectHitEvent hit{
            .projectile     = projectile,
            .damage         = proj.impact_damage,
            .target         = ray.entity_id,
            .shooter        = shot.shooter,
            .target_player  = target ? target->player_idx : ObjectHitEvent::no_player,
            .shooter_player = shot.player_idx,
            .point          = ray.point,
            .normal         = ray.normal,
            .direction      = direction,
            .part           = ray.part,
            .head           = ray.head,
            .hitscan        = true,
        };
        GameEvent hit_ev{.type = GameEvent::ObjectHit};
        p.template subsystem<GameEventBus>().inject(hit_ev, &hit);
    }

    static i32 fire_clip(AnimationPlayback const& anim, blam::map_ptr const& magic)
    {
        if(!anim.graph)
            return -1;
        auto fp = anim.graph->first_person_weapons.data(magic);
        if(!fp.has_value() || fp.value().empty())
            return -1;
        auto slots = fp.value()[0].animations.data(magic);
        using fp_slot = blam::antr::first_person_weapon;
        if(!slots.has_value() || slots.value().size() <= fp_slot::fire_1)
            return -1;
        return slots.value()[fp_slot::fire_1].animation;
    }

    /* The player's first-person weapon model, or its hands */
    u64 first_person(Proxy& p, u64 player, bool weapon)
    {
        for(auto ent : p.template select<Attachment, Model>())
        {
            if(ent.template get<Attachment>().parent != player)
                continue;
            if(weapon == (ent.template get<Model>().origin_object != nullptr))
                return ent.id();
        }
        return 0;
    }

    /* The weapon's export inputs, through its object functions, onto the
     * first-person model: what turns its muzzle flash on */
    void drive_functions(
        Proxy&               p,
        u64                  player,
        weapon_t const&      weapon,
        state_t const&       state,
        bool                 held,
        blam::map_ptr const& magic)
    {
        u64 const id = first_person(p, player, true);
        if(id == 0)
            return;
        auto* model = p.template get<Model>(id);

        std::array<f32, 4> in{};
        for(size_t i = 0; i < in.size(); i++)
            in[i] = input_value(weapon.weapon_inputs[i], state, held);

        std::array<f32, 4> out = in;
        auto functions         = weapon.functions.data(magic);
        if(functions.has_value())
            for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
                out[i] = evaluate(functions.value()[i], in, out);
        model->object_function = out;
    }

    static f32 input_value(input_t input, state_t const& state, bool held)
    {
        switch(input)
        {
        case input_t::illumination:
        case input_t::primary_ejection_port:
            return state.illumination;
        case input_t::primary_firing:
        case input_t::primary_firing_on:
            return state.fired ? 1.f : 0.f;
        case input_t::primary_rate_of_fire:
            return held ? 1.f : 0.f;
        case input_t::primary_ammunition:
        case input_t::secondary_ammunition:
        case input_t::ready:
            return 1.f;
        default:
            return 0.f;
        }
    }

    /* Constant base, scaled and offset by the inputs it names */
    static f32 evaluate(
        blam::scn::object::function_t const& fn,
        std::array<f32, 4> const&            in,
        std::array<f32, 4> const&            out)
    {
        using src = blam::shader::param_src;
        auto value_of = [&](src s) -> std::optional<f32> {
            auto const i = static_cast<u32>(s);
            if(i >= 1 && i <= 4)
                return in[i - 1];
            if(i >= 5 && i <= 8)
                return out[i - 5];
            return std::nullopt;
        };
        f32 v = fn.function == blam::shader::animation_function::zero ? 0.f : 1.f;
        if(auto s = value_of(fn.scale_function_by))
            v *= *s;
        if(auto s = value_of(fn.add))
            v += *s;
        if(auto s = value_of(fn.scale_result_by))
            v *= *s;
        return std::clamp(v, 0.f, 1.f);
    }

    static Vecf3 cone(std::minstd_rand& rng, Vecf3 const& dir, f32 angle)
    {
        if(angle <= 0.f)
            return dir;
        std::uniform_real_distribution<f32> unit(0.f, 1.f);
        f32 const   z   = glm::mix(std::cos(angle), 1.f, unit(rng));
        f32 const   phi = unit(rng) * glm::two_pi<f32>();
        f32 const   r   = std::sqrt(std::max(0.f, 1.f - z * z));
        Vecf3 const up  = std::abs(dir.z) < .99f ? Vecf3{0, 0, 1} : Vecf3{1, 0, 0};
        Vecf3 const x   = glm::normalize(glm::cross(up, dir));
        Vecf3 const y   = glm::cross(dir, x);
        return x * (r * std::cos(phi)) + y * (r * std::sin(phi)) + dir * z;
    }

    /* Ahead of the eye, clear of the body that fires */
    static constexpr f32 muzzle_distance = .4f;
    /* Vehicle guns: where they fire from, and how far the crosshair looks */
    static constexpr std::string_view gun_marker = "primary trigger";
    static constexpr f32              aim_reach  = 200.f;

    u32                       m_generation{0};
    std::map<u64, state_t>    m_states;
    std::minstd_rand          m_rng{std::random_device{}()};

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    std::unique_ptr<TickQueue>                                 m_ticks;
    std::vector<WeaponFireEvent>                               m_shots;

    friend void alloc_weapons(compo::EntityContainer& e);
};

void alloc_weapons(compo::EntityContainer& e)
{
    auto& weapons   = e.register_subsystem_inplace<WeaponSystem>();
    weapons.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    weapons.m_fire_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<WeaponFireEvent>(
            0, [&weapons](GameEvent&, WeaponFireEvent* shot) {
                weapons.m_shots.push_back(*shot);
            });
}
