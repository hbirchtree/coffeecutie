#pragma once

#include <peripherals/libc/types.h>

#include <atomic>
#include <functional>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace comp_app {

/*!
 * \brief Mailbox handing events from producer threads to one consumer.
 */
template<typename EventType, typename SubEventType>
struct event_queue
{
    static_assert(
        std::is_copy_constructible_v<SubEventType>,
        "queued events are copied out of the producer's stack frame");

    using handler_type = std::function<void(EventType&, SubEventType*)>;

    static constexpr libc_types::szptr default_capacity = 1024;

    event_queue(
        handler_type&&      handler,
        libc_types::szptr   capacity = default_capacity)
        : m_handler(std::move(handler))
        , m_capacity(capacity)
    {
    }

    /*! Copy an event into the queue. Callable from any thread. */
    void post(EventType const& event, SubEventType const* data)
    {
        std::lock_guard _(m_lock);

        if(m_pending.size() >= m_capacity)
        {
            m_pending.erase(m_pending.begin());
            m_dropped++;
        }

        if(data)
            m_pending.push_back({event, *data});
        else if constexpr(std::is_default_constructible_v<SubEventType>)
            m_pending.push_back({event, SubEventType{}});
        else
            m_dropped++;
    }

    /*!
     * \brief Run the handler for every queued event, on this thread.
     * \return Number of events delivered.
     */
    libc_types::szptr poll()
    {
        std::vector<entry> batch;
        {
            std::lock_guard _(m_lock);
            batch.swap(m_pending);
        }

        for(auto& queued : batch)
            m_handler(queued.event, &queued.data);

        return batch.size();
    }

    libc_types::szptr pending() const
    {
        std::lock_guard _(m_lock);
        return m_pending.size();
    }

    /*! Events lost to the capacity limit, over the queue's lifetime. */
    libc_types::szptr dropped() const
    {
        return m_dropped.load(std::memory_order_relaxed);
    }

  private:
    struct entry
    {
        EventType    event;
        SubEventType data;
    };

    mutable std::mutex             m_lock;
    std::vector<entry>             m_pending;
    handler_type                   m_handler;
    libc_types::szptr              m_capacity;
    std::atomic<libc_types::szptr> m_dropped{0};
};

} // namespace comp_app
