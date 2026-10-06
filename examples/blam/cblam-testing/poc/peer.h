#pragma once

/* One process-side of a session: a bus, the modules on it and the routing
 * between them. Each event type has exactly one consumer: the session takes
 * NetSend, physics takes what it has a handler for, game logic the rest. */

#include "events.h"
#include "game.h"
#include "gns_session.h"
#include "physics.h"

#include <memory>

namespace poc {

template<typename Game>
struct Peer
{
    template<typename... Args>
    Peer(Terrain const&              terrain,
         HitModel const&             model,
         std::unique_ptr<GnsSession> session,
         Args&&... args)
        : physics(terrain, model, std::is_same_v<Game, ServerGame>)
        , game(std::forward<Args>(args)...)
        , session(std::move(session))
    {
    }

    Bus                         bus;
    Physics                     physics;
    Game                        game;
    std::unique_ptr<GnsSession> session;
    tick_t                      tick{0};

    void pump()
    {
        while(!bus.queue.empty())
        {
            Event ev = std::move(bus.queue.front());
            bus.queue.pop_front();
            std::visit([this](auto const& e) { route(e); }, ev);
        }
    }

    /* input: whatever drives this peer (a bot, a player), pushes Input*
     * events after game logic has updated what it displays */
    template<typename Input>
    void frame(Input&& input)
    {
        session->poll(bus);
        pump();
        if constexpr(std::is_same_v<Game, ServerGame>)
            game.tick(tick, bus);
        else
            game.tick(bus);
        input(*this);
        bus.push(PhysStep{tick});
        pump();
        tick++;
    }

  private:
    template<typename E>
    void route(E const& e)
    {
        if constexpr(std::is_same_v<E, NetSend>)
            session->handle(e);
        else if constexpr(requires { physics.handle(e, bus); })
            physics.handle(e, bus);
        else if constexpr(requires { game.handle(e, bus); })
            game.handle(e, bus);
    }
};

} // namespace poc
