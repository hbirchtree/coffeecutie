#pragma once

#include "game_events.h"
#include "network/networking.h"

#include <coffee/components/entity_container.h>
#include <coffee/components/restricted_subsystem.h>

#include <peripherals/stl/type_list.h>

using TickManifest = compo::SubsystemManifest<
    type_safety::empty_list_t,
    type_safety::type_list_t<GameEventBus, const NetworkState>,
    type_safety::empty_list_t>;

/*! The simulation clock. Each frame it raises a TickEvent for every tick
 *  that has passed, numbered by server time when joined to one (frame time
 *  otherwise), so peers agree on what tick N is. Simulation steps once per
 *  tick; drawing reads alpha to land between the last two. While paused no
 *  ticks pass, and it picks up from the same tick. */
struct TickClock : compo::RestrictedSubsystem<TickClock, TickManifest>
{
    using type  = TickClock;
    using Proxy = compo::proxy_of<TickManifest>;

    static constexpr libc_types::u32 rate = 30; /*!< Ticks per second */
    static constexpr libc_types::i64 tick_us = 1'000'000 / rate;
    static constexpr libc_types::f32 tick_seconds = 1.f / rate;
    /*! Ticks one frame may run; a peer further behind skips ahead */
    static constexpr libc_types::u32 max_catch_up = 6;
    /*! Notice a server gives its clients of a pause, in ticks, on top of
     *  the time it takes to reach them */
    static constexpr libc_types::u64 pause_lead = 6;

    TickClock()
    {
        /* After loading, before input and simulation */
        this->priority = 750;
    }

    void start_restricted(Proxy& p, time_point const& t);

    /*! The last tick raised */
    libc_types::u64 tick() const
    {
        return m_tick;
    }

    /*! How far the present is past the last tick, 0..1 */
    libc_types::f32 alpha() const
    {
        return m_alpha;
    }

    /*! Seconds on the tick clock, between the last two ticks by alpha */
    libc_types::f64 seconds() const
    {
        return (static_cast<libc_types::f64>(m_tick) + m_alpha) / rate;
    }

    /*! Times this peer fell too far behind and skipped ahead */
    libc_types::u32 resyncs() const
    {
        return m_resyncs;
    }

    /*! Stopped, or stopping at pause_at() */
    bool paused() const
    {
        return m_pause_at.has_value() && m_tick >= *m_pause_at;
    }

    /*! The last tick before a pause, once one is asked for */
    std::optional<libc_types::u64> pause_at() const
    {
        return m_pause_at;
    }

    /*! Share of real time the simulation keeps up with, over a second:
     *  below 1 when the server's frames can't keep up */
    libc_types::f32 speed() const
    {
        return m_speed;
    }

    /*! When the last tick began, on the server clock when networked */
    libc_types::i64 tick_began_us() const
    {
        return m_tick_began_us;
    }

  private:
    friend void alloc_ticks(compo::EntityContainer& e);

    std::shared_ptr<GameEventBus::queue_type<SimulationPauseEvent>> m_pause_queue;
    std::optional<bool> m_pause_request;
    /*! Time spent paused, taken out of the time base */
    libc_types::i64                 m_paused_us{0};
    std::optional<libc_types::i64>  m_paused; /*!< Since when */
    std::optional<libc_types::u64>  m_pause_at;

    libc_types::u64 m_tick{0};
    libc_types::f32 m_alpha{0.f};
    libc_types::u32 m_resyncs{0};
    bool            m_started{false};
    libc_types::i64 m_tick_began_us{0};
    libc_types::i64 m_frame_us{0}; /*!< Wall time a frame takes, smoothed */
    libc_types::f32 m_speed{1.f};
    libc_types::i64 m_window_held_us{0};
    libc_types::i64 m_window_us{0};
    std::chrono::steady_clock::time_point m_last_frame{};

    /*! What tick numbers count: frames on our own, the server's clock, or
     *  the ticks the server says it is on */
    enum class time_base_t
    {
        frame,
        server_time,
        server_ticks,
    } m_base{time_base_t::frame};
};

/*! A subsystem's subscription to ticks, drained on its own frame */
struct TickQueue
{
    explicit TickQueue(GameEventBus& bus)
        : m_queue(bus.addQueuedEventFunction<TickEvent>(
              0, [this](GameEvent&, TickEvent* tick) { m_ticks.push_back(*tick); }))
    {
    }

    TickQueue(TickQueue const&)            = delete;
    TickQueue& operator=(TickQueue const&) = delete;

    /*! Ticks since the last call, oldest first */
    std::vector<TickEvent> take()
    {
        m_queue->poll();
        return std::exchange(m_ticks, {});
    }

  private:
    std::vector<TickEvent>                               m_ticks;
    std::shared_ptr<GameEventBus::queue_type<TickEvent>> m_queue;
};

void alloc_ticks(compo::EntityContainer& e);
