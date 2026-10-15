#include "ticks.h"

#include "network/networking.h"

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

    /* A new time base: start over from it */
    bool const networked = server.has_value();
    bool const restart   = !m_started || networked != m_networked;
    if(restart)
    {
        m_started   = true;
        m_networked = networked;
        m_paused_us = 0;
        if(m_paused)
            m_paused = base_us;
    }

    m_pause_queue->poll();
    if(auto pause = std::exchange(m_pause_request, std::nullopt);
       pause && *pause != paused())
    {
        if(*pause)
            m_paused = base_us;
        else
            m_paused_us += base_us - *std::exchange(m_paused, std::nullopt);
    }
    if(m_paused)
        return;
    i64 const now_us = base_us - m_paused_us;
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
}

void alloc_ticks(compo::EntityContainer& e)
{
    auto& clock = e.register_subsystem_inplace<TickClock>();
    clock.m_pause_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<SimulationPauseEvent>(
            0, [&clock](GameEvent&, SimulationPauseEvent* pause) {
                clock.m_pause_request = pause->paused;
            });
}
