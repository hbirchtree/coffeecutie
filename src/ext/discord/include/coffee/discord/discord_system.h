#pragma once

#include <coffee/components/types.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/discord/discord_binding.h>

namespace Coffee {
namespace Discord {

namespace detail {

struct DiscordData
{
    ShPtr<DiscordDelegate>           delegate;
    ShPtr<platform::online::Service> service;
};
} // namespace detail

using Tag = Components::TagType<detail::DiscordData>;

struct Subsystem : Components::Globals::ValueSubsystem<Tag>
{
    Subsystem(RuntimeQueue* queue, DiscordOptions&& options) :
        m_discordQueue(queue), m_options(std::move(options))
    {
        runtime_queue_error ec;
        m_mainQueue = RuntimeQueue::GetCurrentQueue(ec);

        C_ERROR_CHECK(ec);

        delegate().ready = [&](Discord::PlayerInfo&& info) {
            m_playerInfo = std::move(info);
        };
    }

    void start()
    {
        auto& _service = service();

        get().service = CreateService(std::move(m_options), get().delegate);

        runtime_queue_error ec;
        RuntimeQueue::QueuePeriodic(
            m_discordQueue,
            Chrono::milliseconds(100),
            [&_service]() { _service.poll(); },
            ec);
    }

    DiscordDelegate& delegate()
    {
        return *get().delegate;
    }

    platform::online::Service& service()
    {
        return *get().service;
    }

    platform::online::PresenceDelegate& presence()
    {
        return *get().service->getPresence();
    }

    platform::online::GameDelegate& game()
    {
        return *get().service->getGame();
    }

    platform::online::Service const& service() const
    {
        return *get().service;
    }

  private:
    RuntimeQueue*  m_discordQueue;
    RuntimeQueue*  m_mainQueue;
    DiscordOptions m_options;

    PlayerInfo m_playerInfo;
};

} // namespace Discord
} // namespace Coffee
