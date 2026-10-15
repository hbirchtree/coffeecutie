#include "ticks.h"

#include "components.h"
#include "network/networking.h"
#include "player_controller.h"

#include <coffee/components/proxy.h>

void TickClock::start_restricted(Proxy& p, time_point const& t)
{
    using namespace std::chrono;
    using libc_types::i64;
    using libc_types::u64;

    auto const& net    = p.subsystem<NetworkState>();
    auto const  server = net.server_now();
    i64 const   base_us =
        server ? duration_cast<microseconds>(server->time_since_epoch()).count()
               : duration_cast<microseconds>(t.time_since_epoch()).count();

    /* A client follows the server's ticks, pauses included */
    auto const& follow = net.replica && server ? net.tick_base : std::nullopt;

    /* A new time base: start over from it */
    auto const base      = !server   ? time_base_t::frame
                           : follow ? time_base_t::server_ticks
                                    : time_base_t::server_time;
    bool const restart   = !m_started || base != m_base;
    if(restart)
    {
        m_started   = true;
        m_base      = base;
        m_paused_us = 0;
        if(m_paused)
            m_paused = base_us;
    }

    m_pause_queue->poll();
    auto pause = std::exchange(m_pause_request, std::nullopt);
    if(follow)
        pause = follow->paused;
    if(pause && *pause != paused())
    {
        if(*pause)
            m_paused = base_us;
        else
            m_paused_us += base_us - *std::exchange(m_paused, std::nullopt);
    }
    if(m_paused)
        return;

    /* Where tick 0 began on this time base */
    i64 const origin_us =
        follow ? follow->start_us - static_cast<i64>(follow->tick) * tick_us
               : m_paused_us;
    i64 const now_us = base_us - origin_us;
    u64 const now    = static_cast<u64>(std::max<i64>(now_us, 0) / tick_us);

    auto&     bus  = p.subsystem<GameEventBus>();
    auto      emit = [&bus](u64 tick, bool resync) {
        GameEvent ev{.type = GameEvent::Tick};
        TickEvent data{.tick = tick, .seconds = tick_seconds, .resync = resync};
        bus.inject(ev, &data);
    };

    if(restart)
    {
        m_tick = now;
        emit(m_tick, true);
    } else if(now > m_tick)
    {
        u64 const behind = now - m_tick;
        /* The server's time is everyone's, so it never skips: it slows */
        if(behind > max_catch_up && !net.clock_authority)
        {
            m_tick = now;
            m_resyncs++;
            emit(m_tick, true);
        } else
            for(u64 i = 0; i < std::min<u64>(behind, max_catch_up); i++)
                emit(++m_tick, false);
    }

    m_alpha = std::clamp(
        static_cast<libc_types::f32>(
            now_us - static_cast<i64>(m_tick) * tick_us) /
            tick_us,
        0.f,
        1.f);
    m_tick_began_us = base_us - static_cast<i64>(m_alpha * tick_us);
}

namespace {

using MotionManifest = compo::SubsystemManifest<
    type_safety::type_list_t<
        Model,
        PlayerCamera,
        const PlayerInfo,
        const ObjectPhysics,
        const Projectile,
        const Attachment>,
    type_safety::type_list_t<const TickClock>,
    type_safety::empty_list_t>;

using MotionProxy = compo::proxy_of<MotionManifest>;

/* Moved by the simulation, a tick at a time */
bool ticked(MotionProxy& p, u64 id)
{
    if(p.get<Projectile>(id))
        return true;
    if(auto const* info = p.get<PlayerInfo>(id))
        return !info->is_remote() && info->mode.physics;
    auto const* physics = p.get<ObjectPhysics>(id);
    return physics && physics->mass > 0.f &&
           physics->authority == ObjectPhysics::Simulated;
}

/* A camera that follows a body or a vehicle seat */
bool ticked_eye(PlayerInfo const& info)
{
    return !info.is_remote() && (info.mode.physics || info.riding.vehicle);
}

/* Further than this in a tick is a teleport, not motion */
constexpr f32 max_tick_move = 10.f;

/*! Before a frame's ticks, keeps the pose they start from */
struct MotionSnapshot : compo::RestrictedSubsystem<MotionSnapshot, MotionManifest>
{
    using type = MotionSnapshot;

    MotionSnapshot()
    {
        this->priority = 760;
    }

    void start_restricted(MotionProxy& p, time_point const&)
    {
        if(m_ticks->take().empty())
            return;
        for(auto ent : p.select<Model>())
        {
            auto& model         = ent.get<Model>();
            model.tick_posed    = ticked(p, ent.id());
            model.tick_position = model.position;
            model.tick_rotation = model.rotation;
        }
        for(auto ent : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = ent.components();
            cam.tick_eye     = ticked_eye(info)
                                   ? std::optional(cam.camera.position)
                                   : std::nullopt;
        }
    }

    std::unique_ptr<TickQueue> m_ticks;
};

/*! After the simulation, draws what it moves alpha past the last tick */
struct MotionInterpolator
    : compo::RestrictedSubsystem<MotionInterpolator, MotionManifest>
{
    using type = MotionInterpolator;

    MotionInterpolator()
    {
        /* After simulation and animation, before effects attach to it */
        this->priority = 904;
    }

    void start_restricted(MotionProxy& p, time_point const&)
    {
        f32 const alpha = p.subsystem<TickClock>().alpha();
        for(auto ent : p.select<Model>())
        {
            auto& model = ent.get<Model>();
            if(!model.tick_posed || !ticked(p, ent.id()) ||
               glm::distance(model.tick_position, model.position) >
                   max_tick_move)
                continue;
            model.transform =
                glm::translate(
                    Matf4(1), glm::mix(model.tick_position, model.position, alpha)) *
                glm::mat4_cast(glm::slerp(model.tick_rotation, model.rotation, alpha));
        }
        for(auto ent : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = ent.components();
            if(!cam.tick_eye || !ticked_eye(info) ||
               glm::distance(*cam.tick_eye, cam.camera.position) > max_tick_move)
                continue;
            Vecf3 const eye   = glm::mix(*cam.tick_eye, cam.camera.position, alpha);
            Vecf3 const delta = eye - cam.camera.position;
            build_camera_matrix(cam, eye);
            /* First-person models hang off the eye */
            for(auto held : p.select<Attachment, Model>())
            {
                auto [attachment, model] = held.components();
                if(attachment.parent == ent.id())
                    model.transform = glm::translate(Matf4(1), delta) * model.transform;
            }
        }
    }
};

} // namespace

void alloc_ticks(compo::EntityContainer& e)
{
    auto& snapshot   = e.register_subsystem_inplace<MotionSnapshot>();
    snapshot.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    e.register_subsystem_inplace<MotionInterpolator>();
    auto& clock = e.register_subsystem_inplace<TickClock>();
    clock.m_pause_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<SimulationPauseEvent>(
            0, [&clock](GameEvent&, SimulationPauseEvent* pause) {
                clock.m_pause_request = pause->paused;
            });
}
