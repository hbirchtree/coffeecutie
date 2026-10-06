#include "session.h"

#include <fmt/format.h>

namespace poc {

bool Session::host(std::uint16_t port)
{
    auto session = GnsSession::listen(port);
    if(!session->valid())
        return false;
    m_port   = port;
    m_server = std::make_unique<Peer<ServerGame>>(
        m_world.terrain, m_world.model, std::move(session), m_config, m_world.model, m_report);
    return true;
}

bool Session::join(std::string const& address, BipedPose spawn)
{
    auto session = GnsSession::connect(address);
    if(!session->valid())
        return false;
    m_local = std::make_unique<Peer<ClientGame>>(
        m_world.terrain,
        m_world.model,
        std::move(session),
        m_config,
        m_world.model,
        m_report,
        spawn);
    return true;
}

bool Session::add_bot(Bot bot)
{
    if(!m_server)
        return false;
    auto session = GnsSession::connect(fmt::format("127.0.0.1:{}", m_port));
    if(!session->valid())
        return false;
    auto spawn = bot.spawn();
    m_bots.push_back(std::unique_ptr<BotPeer>(new BotPeer{
        .bot  = std::move(bot),
        .peer = Peer<ClientGame>(
            m_world.terrain,
            m_world.model,
            std::move(session),
            m_config,
            m_world.model,
            m_report,
            spawn),
    }));
    return true;
}

void Session::tick(LocalInput const& local)
{
    GnsSession::run_callbacks();
    if(m_server)
        m_server->frame([](auto&) {});
    if(m_local)
        m_local->frame([&](Peer<ClientGame>& p) {
            if(local)
                local(p);
        });
    for(auto& b : m_bots)
        b->peer.frame(b->bot);
}

int Session::update(LocalInput const& local)
{
    auto const interval = std::chrono::duration_cast<clock::duration>(
        std::chrono::duration<double>(tick_seconds));
    auto const now = clock::now();
    if(m_next == clock::time_point{} || now - m_next > interval * 4)
        m_next = now;
    int ran = 0;
    while(m_next <= now && ran < 4)
    {
        tick(local);
        m_next += interval;
        ran++;
    }
    return ran;
}

} // namespace poc
