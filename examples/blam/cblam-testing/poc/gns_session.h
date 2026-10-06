#pragma once

/* One ad-hoc GameNetworkingSockets endpoint: a listen socket (server) or a
 * single connection (client). Owns everything it opens and closes it all in
 * its destructor, so sessions can be created and torn down per run in one
 * process. Turns NetSend events into GNS sends and GNS traffic into
 * NetReceived/NetConnected/NetDisconnected/NetStats events. */

#include "events.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace poc {

struct NetSimulation
{
    float rtt_ms{0.f};
    float jitter_ms{0.f};
    float loss_pct{0.f};
};

class GnsSession
{
  public:
    /* Once per process; a no-op when GNS is already up */
    static bool init(std::string& error);
    static void shutdown();
    /* GNS fake-network settings are process-global. Applied on send only:
     * with both ends in one process every packet gets delayed exactly once,
     * so the round trip is rtt_ms. */
    static void simulate(NetSimulation const& sim);
    /* Dispatches connection status callbacks for every session */
    static void run_callbacks();

    static std::unique_ptr<GnsSession> listen(std::uint16_t port);
    static std::unique_ptr<GnsSession> connect(std::string const& address);

    GnsSession(GnsSession const&)            = delete;
    GnsSession& operator=(GnsSession const&) = delete;
    ~GnsSession();

    bool valid() const;
    bool is_server() const
    {
        return m_listen != 0;
    }

    /* Moves received traffic and status changes onto the bus */
    void poll(Bus& bus);
    void handle(NetSend const& ev);

    void on_status(void* info);

  private:
    GnsSession();

    std::uint32_t        m_listen{0};
    std::uint32_t        m_poll_group{0};
    std::uint32_t        m_conn{0}; /* client side */
    std::vector<conn_id> m_clients; /* server side */
    std::vector<Event>   m_pending;
};

} // namespace poc
