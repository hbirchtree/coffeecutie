#pragma once

#include <coffee/components/components.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/discord/discord_binding.h>

namespace Coffee {
namespace Discord {

struct Subsystem : Components::SubsystemBase
{
    using type = Subsystem;

    Subsystem(rq::runtime_queue* queue, DiscordOptions&& options) :
        m_discordQueue(queue), m_options(std::move(options))
    {
        if(auto q = rq::runtime_queue::GetCurrentQueue(); q.has_value())
            m_mainQueue = q.value();

        m_delegate        = MkShared<DiscordDelegate>();
        m_delegate->ready = [this](Discord::PlayerInfo&& info) {
            m_playerInfo = std::move(info);
            m_startAwaiter.set_value();
        };
    }

    void start()
    {
        using namespace std::chrono_literals;

        if(m_taskId)
        {
            rq::runtime_queue::Unblock(m_taskId);
            return;
        }

        auto err = rq::runtime_queue::QueueImmediate(
            m_discordQueue,
            0ms,
            [this]() {
            m_service = CreateService(std::move(m_options), m_delegate);

            auto task = [service = m_service]() { service->poll(); };
            if(auto taskId = rq::runtime_queue::QueuePeriodic(
                   m_discordQueue, Chrono::milliseconds(100), task);
               taskId.has_value())
                m_taskId = taskId.value();
        });
        if (err.has_error())
            Throw(rq::runtime_queue_error("failed to start Discord task"));
    }
    void stop()
    {
        rq::runtime_queue::Block(m_taskId);
    }

    DiscordDelegate& delegate()
    {
        return *m_delegate;
    }

    platform::online::Service& service()
    {
        return *m_service;
    }

    platform::online::PresenceDelegate& presence()
    {
        return *m_service->getPresence();
    }

    platform::online::GameDelegate& game()
    {
        return *m_service->getGame();
    }

    platform::online::Service const& service() const
    {
        return *m_service;
    }

    std::future<void> startCondition()
    {
        return m_startAwaiter.get_future();
    }

    PlayerInfo const& playerInfo() const
    {
        return m_playerInfo;
    }

    rq::runtime_queue* queue() const
    {
        return m_discordQueue;
    }

    template<typename T>
    auto on_started(std::function<T(Discord::Subsystem&)>&& func)
    {
        auto task = rq::dependent_task<void, T>::CreateTask(
            startCondition(),
            [this, func] (void*) {
                return func(*this);
            });
        auto output = task->output.get_future();
        if (rq::runtime_queue::Queue(m_discordQueue, std::move(task)).has_error())
            return decltype(output){};
        return output;
    }

  private:
    ShPtr<DiscordDelegate>           m_delegate;
    ShPtr<platform::online::Service> m_service;
    rq::runtime_queue*               m_discordQueue{nullptr};
    rq::runtime_queue*               m_mainQueue{nullptr};
    u64                              m_taskId{0};
    DiscordOptions                   m_options;
    std::promise<void>               m_startAwaiter;

    PlayerInfo m_playerInfo;
};

} // namespace Discord
} // namespace Coffee
