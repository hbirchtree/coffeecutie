#include "view_kick.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "selected_version.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using ViewKickManifest = compo::SubsystemManifest<
    type_list_t<PlayerCamera, const PlayerInfo>,
    type_list_t<GameEventBus, const LoadingStatus, const BlamFiles<halo_version>>,
    empty_list_t>;

/* A shot shakes and kicks its shooter's view as its firing jpt! says, and
 * the first-person model sways on a spring: back with each shot, down on
 * landing. Only seats played here */
struct ViewKick : compo::RestrictedSubsystem<ViewKick, ViewKickManifest>
{
    using type  = ViewKick;
    using Proxy = compo::proxy_of<ViewKickManifest>;
    using jpt_t = blam::scn::damage_effect;

    ViewKick()
    {
        this->priority = 790;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        f32 const dt = m_last ? std::min(std::chrono::duration<f32>(t - *m_last).count(), .1f)
                              : 0.f;
        m_last = t;
        m_shot_queue->poll();
        auto shots = std::exchange(m_shots, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(m_load_generation != files.load_generation)
        {
            m_load_generation = files.load_generation;
            m_index.emplace(files.container);
            m_seats.clear();
        }
        for(auto const& shot : shots)
            if(auto const* info = p.template get<PlayerInfo>(shot.shooter);
               info && !info->is_remote() && !shot.thrown)
                fired(shot.shooter, firing_damage(shot, files));

        for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto [cam, info] = ent.components();
            auto& seat       = m_seats[ent.id()];
            landing(seat, info, cam.camera.position.z, dt);
            cam.kick = {};
            if(info.is_remote())
                continue;
            for(auto& kick : seat.kicks)
            {
                apply(cam, kick, kick.t);
                kick.t += dt;
            }
            std::erase_if(seat.kicks, [](kick_t const& k) { return k.t >= k.length; });
            /* Damped, a touch under critical; small steps keep it stable at
             * any frame rate */
            for(f32 left = dt; left > 0.f; left -= spring_step)
            {
                f32 const   h    = std::min(left, spring_step);
                Vecf3 const pull = -spring_k * seat.spring - spring_damping * seat.spring_v;
                seat.spring_v += pull * h;
                seat.spring += seat.spring_v * h;
            }
            cam.kick.weapon += seat.spring;
            cam.kick.shift.y += seat.spring.z;
        }
    }

    struct kick_t
    {
        jpt_t const* jpt{nullptr};
        f32          t{0.f};
        f32          length{0.f};
        Vecf2        jitter{};
    };

    struct seat_t
    {
        std::vector<kick_t> kicks;
        Vecf3               spring{};
        Vecf3               spring_v{};
        f32                 falling{0.f}; /*!< The eye's speed down, last move */
        f32                 still{0.f};   /*!< Since the eye last moved, s */
        std::optional<f32>  eye_z;
    };

    /* The trigger's first firing effect's jpt!, or a strike's response */
    jpt_t const* firing_damage(WeaponFireEvent const& shot, BlamFiles<halo_version> const& files)
    {
        auto const& magic  = files.container.magic;
        auto        weapon = m_index->find(shot.weapon);
        if(weapon == m_index->end() || !(*weapon).matches(blam::tag_class_t::weap))
            return nullptr;
        auto data = (*weapon).template data<blam::scn::weapon>(magic);
        if(!data.has_value())
            return nullptr;
        if(shot.melee)
            return damage_of(data.value()[0].player_melee_response, magic);
        auto triggers = data.value()[0].triggers.data(magic);
        if(!triggers.has_value() || shot.trigger >= triggers.value().size())
            return nullptr;
        auto effects = triggers.value()[shot.trigger].firing_effects.data(magic);
        if(!effects.has_value() || effects.value().empty())
            return nullptr;
        return damage_of(effects.value()[0].firing_damage, magic);
    }

    jpt_t const* damage_of(blam::tagref_t const& ref, blam::map_ptr const& magic)
    {
        auto jpt = ref.valid() ? m_index->find(ref) : m_index->end();
        if(jpt == m_index->end() || !(*jpt).matches(blam::tag_class_t::jpt))
            return nullptr;
        auto out = (*jpt).template data<jpt_t>(magic);
        return out.has_value() ? out.value() : nullptr;
    }

    void fired(u64 shooter, jpt_t const* jpt)
    {
        auto& seat = m_seats[shooter];
        f32   rise = 0.f;
        if(jpt)
        {
            auto const& impulse = jpt->temporary_camera_impulse;
            auto const& shake   = jpt->camera_shaking;
            std::uniform_real_distribution<f32> unit(-1.f, 1.f);
            seat.kicks.push_back({
                .jpt    = jpt,
                .length = std::max(impulse.duration, shake.duration),
                .jitter = Vecf2{unit(m_random), unit(m_random)},
            });
            rise = impulse.rotation + shake.random_rotation;
        }
        seat.spring_v.x -= shot_pull + shot_pull_per_radian * rise;
    }

    /* The impulse fades out over its time, the shake jumps about within its
     * own, both linearly */
    void apply(PlayerCamera& cam, kick_t const& kick, f32 t)
    {
        auto const& impulse = kick.jpt->temporary_camera_impulse;
        auto const& shake   = kick.jpt->camera_shaking;
        if(t < impulse.duration)
        {
            f32 const w = 1.f - t / impulse.duration;
            cam.kick.turn.x += impulse.rotation * w;
            cam.kick.turn += Vecf3(kick.jitter * impulse.jitter, 0.f) * w;
            cam.kick.shift.z += impulse.pushback * w;
        }
        if(t < shake.duration)
        {
            f32 w = 1.f - t / shake.duration;
            if(shake.wobble_period > 0.f)
                w *= 1.f - shake.wobble_weight +
                     shake.wobble_weight *
                         std::abs(std::sin(glm::two_pi<f32>() * t / shake.wobble_period));
            std::uniform_real_distribution<f32> unit(-1.f, 1.f);
            cam.kick.turn += Vecf3{unit(m_random), unit(m_random), unit(m_random)} *
                             shake.random_rotation * w;
            cam.kick.shift += Vecf3{unit(m_random), unit(m_random), unit(m_random)} *
                              shake.random_translation * w;
        }
    }

    /* Coming down hard dips the view and the weapon: the eye falling fast,
     * then stopped. Bodies report neither speed nor a steady ground flag,
     * and the eye moves on ticks, so its speed is between moves */
    void landing(seat_t& seat, PlayerInfo const& info, f32 eye_z, f32 dt)
    {
        seat.still += dt;
        bool const on_foot = info.mode.physics && !info.riding.vehicle;
        bool const moved   = !seat.eye_z || *seat.eye_z != eye_z;
        f32 const  down    = seat.eye_z && moved ? (*seat.eye_z - eye_z) / seat.still : 0.f;
        if(moved)
        {
            seat.eye_z = eye_z;
            seat.still = 0.f;
        }
        /* A teleport is no fall */
        if(!on_foot || down > land_teleport)
        {
            seat.falling = 0.f;
            return;
        }
        bool const stopped = moved ? down < seat.falling * land_stopped : seat.still > land_still;
        if(seat.falling > land_soft && stopped)
        {
            seat.spring_v.z -= std::min((seat.falling - land_soft) * land_dip, land_dip_most);
            seat.falling = 0.f;
        }
        if(moved)
            seat.falling = std::max(down, 0.f);
    }

    static constexpr f32 spring_k             = 180.f;
    static constexpr f32 spring_step          = 1.f / 240.f; /* s */
    static constexpr f32 spring_damping       = 22.f;
    static constexpr f32 shot_pull            = .15f; /* wu/s */
    static constexpr f32 shot_pull_per_radian = 3.f;
    static constexpr f32 land_soft            = 2.f;  /* wu/s */
    static constexpr f32 land_stopped         = .3f;  /* of the speed it fell at */
    static constexpr f32 land_teleport        = 50.f; /* wu/s */
    static constexpr f32 land_still           = .1f;  /* s */
    static constexpr f32 land_dip             = .15f;
    static constexpr f32 land_dip_most        = .8f;  /* wu/s */

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_shot_queue;
    std::vector<WeaponFireEvent>                               m_shots;
    std::optional<time_point>                                  m_last;
    u32                                                        m_load_generation{0};
    std::optional<blam::tag_index_view<halo_version>>          m_index;
    std::map<u64, seat_t>                                      m_seats;
    std::mt19937                                               m_random{std::random_device{}()};
};

void alloc_view_kick(compo::EntityContainer& e)
{
    auto& kick        = e.register_subsystem_inplace<ViewKick>();
    kick.m_shot_queue = e.subsystem_cast<GameEventBus>().addQueuedEventFunction<WeaponFireEvent>(
        0, [&kick](GameEvent&, WeaponFireEvent* shot) { kick.m_shots.push_back(*shot); });
}
