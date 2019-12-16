#pragma once

#include <coffee/components/components.h>
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
    using tag_type = Tag;

    Subsystem(RuntimeQueue* queue, DiscordOptions&& options) :
        m_discordQueue(queue), m_options(std::move(options))
    {
        runtime_queue_error ec;
        m_mainQueue = RuntimeQueue::GetCurrentQueue(ec);

        C_ERROR_CHECK(ec);

        get().delegate   = MkShared<DiscordDelegate>();
        delegate().ready = [&](Discord::PlayerInfo&& info) {
            m_playerInfo = std::move(info);
            if(!readyWait.mutx.try_lock())
                readyWait.var.notify_all();
        };
    }

    void start()
    {
        get().service = CreateService(std::move(m_options), get().delegate);
        auto _service = get().service;

        runtime_queue_error ec;
        RuntimeQueue::QueuePeriodic(
            m_discordQueue,
            Chrono::milliseconds(100),
            [_service]() { _service->poll(); },
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

    bool awaitReady(RuntimeTask::clock::duration timeout)
    {
        UqLock lk(readyWait.mutx, std::try_to_lock);

        if(!lk.owns_lock())
            return true;

        return readyWait.var.wait_for(lk, timeout) == cv_status::no_timeout;
    }

    PlayerInfo const& playerInfo() const
    {
        return m_playerInfo;
    }

  private:
    RuntimeQueue*  m_discordQueue;
    RuntimeQueue*  m_mainQueue;
    DiscordOptions m_options;

    struct
    {
        CondVar var;
        Mutex   mutx;
    } readyWait;

    PlayerInfo m_playerInfo;
};

} // namespace Discord
} // namespace Coffee
