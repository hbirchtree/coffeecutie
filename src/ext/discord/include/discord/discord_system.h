#pragma once

#include <coffee/components/components.h>
#include <coffee/core/task_queue/task.h>
#include <discord/discord_binding.h>

namespace discord {

using namespace std::chrono_literals;

struct Subsystem : compo::SubsystemBase
{
    using type = Subsystem;

    Subsystem(rq::runtime_queue* queue, DiscordOptions&& options)
        : m_discordQueue(queue)
        , m_options(std::move(options))
    {
        if(auto q = rq::runtime_queue::GetCurrentQueue(); q.has_value())
            m_mainQueue = q.value();

        m_delegate        = std::make_shared<DiscordDelegate>();
        m_delegate->ready = [this](PlayerInfo&& info) {
            m_playerInfo = std::move(info);
            m_startAwaiter.set_value(true);
        };
    }

    void start()
    {
        using namespace std::chrono_literals;
        compo::DProfContext _("discord::Subsystem launch");

        if(m_taskId)
        {
            rq::runtime_queue::Unblock(m_taskId);
            return;
        }

        using namespace std::chrono_literals;
        m_startDeadline = std::chrono::system_clock().now() + 10s;

        auto err =
            rq::runtime_queue::QueueImmediate(m_discordQueue, 0ms, [this]() {
                compo::DProfContext _("discord::Subsystem start thread");
                m_service = CreateService(std::move(m_options), m_delegate);

                auto task = [this, service = m_service]() {
                    compo::DProfContext _("discord::Subsystem::poll");
                    service->poll();

                    if(m_playerInfo.has_value())
                        return;
                    if(std::chrono::system_clock::now() > m_startDeadline)
                    {
                        m_startAwaiter.set_value(false);
                        rq::runtime_queue::Block(
                            rq::runtime_queue::GetSelfId().value());
                    }
                };
                if(auto taskId = rq::runtime_queue::QueuePeriodic(
                       m_discordQueue, std::chrono::milliseconds(100), task);
                   taskId.has_value())
                    m_taskId = taskId.value();
            });
        if(err.has_error())
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

    discord::DiscordPresenceDelegate& presence()
    {
        return *std::static_pointer_cast<DiscordPresenceDelegate>(
            m_service->getPresence());
    }

    discord::DiscordGameDelegate& game()
    {
        return *std::static_pointer_cast<DiscordGameDelegate>(
            m_service->getGame());
    }

    platform::online::Service const& service() const
    {
        return *m_service;
    }

    std::future<bool> startCondition()
    {
        return m_startAwaiter.get_future();
    }

    PlayerInfo const& playerInfo() const
    {
        return m_playerInfo.value();
    }

    rq::runtime_queue* queue() const
    {
        return m_discordQueue;
    }

    template<typename T>
    auto on_started(
        std::function<T(discord::Subsystem&)>&& func,
        std::function<T()>&&                    failed = []() { return T(); })
    {
        auto task = rq::dependent_task<bool, T>::CreateProcessor(
            startCondition(),
            [this, func = std::move(func), failed = std::move(failed)](
                bool* started) {
                if(!(*started))
                    return failed();
                return func(*this);
            });
        auto output = task->output.get_future();
        if(rq::runtime_queue::Queue(m_discordQueue, std::move(task))
               .has_error())
            return decltype(output){};
        return output;
    }

  private:
    std::shared_ptr<DiscordDelegate>           m_delegate;
    std::shared_ptr<platform::online::Service> m_service;
    rq::runtime_queue*                         m_discordQueue{nullptr};
    rq::runtime_queue*                         m_mainQueue{nullptr};
    libc_types::u64                            m_taskId{0};
    DiscordOptions                             m_options;
    std::promise<bool>                         m_startAwaiter;
    std::chrono::system_clock::time_point      m_startDeadline{};

    std::optional<PlayerInfo> m_playerInfo;
};

} // namespace discord
