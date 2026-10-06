#pragma once

/* A combat session in this process: optionally the server, optionally one
 * client driven by the caller (a player), and any number of bot clients
 * connected to our own server. Ticks everything at a fixed 30 Hz. */

#include "bot.h"
#include "game.h"
#include "peer.h"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace poc {

class Session
{
  public:
    using clock       = std::chrono::steady_clock;
    using LocalInput  = std::function<void(Peer<ClientGame>&)>;

    Session(World const& world, GameConfig const& config)
        : m_world(world)
        , m_config(config)
    {
    }

    bool host(std::uint16_t port);
    bool join(std::string const& address, BipedPose spawn);
    bool add_bot(Bot bot);

    /* Runs however many ticks are due since the last call (at most a few,
     * so a stall doesn't turn into a burst) */
    int update(LocalInput const& local);
    /* Runs exactly one tick */
    void tick(LocalInput const& local);

    GameConfig&             config()
    {
        return m_config;
    }
    Report&                 report()
    {
        return m_report;
    }
    World const&            world() const
    {
        return m_world;
    }
    Peer<ServerGame>*       server()
    {
        return m_server.get();
    }
    Peer<ClientGame>*       local()
    {
        return m_local.get();
    }
    std::uint16_t port() const
    {
        return m_port;
    }

    struct BotPeer
    {
        Bot              bot;
        Peer<ClientGame> peer;
    };

    std::vector<std::unique_ptr<BotPeer>>& bots()
    {
        return m_bots;
    }

  private:
    World const&                      m_world;
    GameConfig                        m_config;
    Report                            m_report;
    std::uint16_t                     m_port{0};
    std::unique_ptr<Peer<ServerGame>> m_server;
    std::unique_ptr<Peer<ClientGame>> m_local;
    std::vector<std::unique_ptr<BotPeer>> m_bots;
    clock::time_point                 m_next{};
};

} // namespace poc
