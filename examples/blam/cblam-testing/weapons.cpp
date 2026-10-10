#include "weapons.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "selected_version.h"

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
    type_list_t<GameEventBus, const BlamFiles<halo_version>>,
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
        bool firing{false};     /* held last frame */
        bool fired{false};      /* shot this frame */
    };

    WeaponSystem()
    {
        this->priority = 850;
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
            m_states.clear();
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
            auto const* weapon_tag       = info.biped.weapon;
            if(info.is_remote() || !weapon_tag ||
               !biped_in_play(info, cam, net))
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

            state.fired    = false;
            state.cooldown = std::max(state.cooldown - dt, 0.f);
            bool const repeats =
                (trigger.flags & trigger_t::flags_t::does_not_repeat_automatically) ==
                trigger_t::flags_t::none;
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
                fire(p, ent.id(), cam, trigger, magic);
            }
            state.firing = held;
            if(trigger.illumination_recovery_time > 0.f)
                state.illumination = std::max(
                    state.illumination - dt / trigger.illumination_recovery_time,
                    0.f);
            else if(!state.fired)
                state.illumination = 0.f;

            drive_functions(p, ent.id(), weapon.value()[0], state, held, magic);
        }
        std::erase_if(m_states, [&live](auto const& e) {
            return !live.contains(e.first);
        });
    }

    /* Projectiles from the eye, the firing effect at the muzzle */
    void fire(
        Proxy&               p,
        u64                  player,
        PlayerCamera const&  cam,
        trigger_t const&     trigger,
        blam::map_ptr const& magic)
    {
        auto& game = p.template subsystem<GameEventBus>();

        Matf3 const to_world = glm::transpose(Matf3(cam.rotation));
        Vecf3 const forward  = to_world * Vecf3{0.f, 0.f, -1.f};
        Vecf3 const eye      = cam.camera.position;

        if(trigger.projectile.valid())
        {
            i32 const count = std::max<i32>(trigger.projectiles_per_shot, 1);
            f32 const error = std::max(trigger.error_angle.x, trigger.minimum_error);
            for(i32 i = 0; i < count; i++)
            {
                f32 const spread =
                    count > 1 ? std::max(error, trigger.distribution_angle * .5f) : error;
                Vecf3 const dir = cone(forward, spread);
                SpawnObjectEvent spawn{
                    .object   = trigger.projectile,
                    .position = eye + forward * muzzle_distance,
                    .rotation = glm::angleAxis(std::atan2(dir.y, dir.x), Vecf3{0, 0, 1}) *
                                glm::angleAxis(
                                    -std::asin(std::clamp(dir.z, -1.f, 1.f)),
                                    Vecf3{0, 1, 0}),
                };
                GameEvent ev{.type = GameEvent::SpawnObject};
                game.inject(ev, &spawn);
            }
        }

        u64 const muzzle = first_person(p, player, true);
        if(auto effects = trigger.firing_effects.data(magic);
           effects.has_value() && !effects.value().empty())
        {
            size_t pick = 0;
            if((trigger.flags & trigger_t::flags_t::random_firing_effects) !=
               trigger_t::flags_t::none)
                pick = std::uniform_int_distribution<size_t>(
                    0, effects.value().size() - 1)(m_rng);
            auto const& effect = effects.value()[pick].firing_effect;
            if(effect.valid())
            {
                SpawnEffectEvent play{
                    .effect    = effect,
                    .position  = eye + forward * muzzle_distance,
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
            if(ent.template get<Attachment>().parent != player)
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

    Vecf3 cone(Vecf3 const& dir, f32 angle)
    {
        if(angle <= 0.f)
            return dir;
        std::uniform_real_distribution<f32> unit(0.f, 1.f);
        f32 const   z   = glm::mix(std::cos(angle), 1.f, unit(m_rng));
        f32 const   phi = unit(m_rng) * glm::two_pi<f32>();
        f32 const   r   = std::sqrt(std::max(0.f, 1.f - z * z));
        Vecf3 const up  = std::abs(dir.z) < .99f ? Vecf3{0, 0, 1} : Vecf3{1, 0, 0};
        Vecf3 const x   = glm::normalize(glm::cross(up, dir));
        Vecf3 const y   = glm::cross(dir, x);
        return x * (r * std::cos(phi)) + y * (r * std::sin(phi)) + dir * z;
    }

    /* Ahead of the eye, clear of the body that fires */
    static constexpr f32 muzzle_distance = .4f;

    time_point                m_last_frame{};
    u32                       m_generation{0};
    std::map<u64, state_t>    m_states;
    std::minstd_rand          m_rng{std::random_device{}()};
};

void alloc_weapons(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<WeaponSystem>();
}
