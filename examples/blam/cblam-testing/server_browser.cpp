#include "data.h"
#include "networking.h"

#include <coffee/components/restricted_subsystem.h>
#include <coffee/components/subsystem.h>
#include <coffee/net/net_resource.h>

using ServerBrowserManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    compo::empty_list_t,
    compo::empty_list_t>;

struct ServerBrowser
    : compo::RestrictedSubsystem<ServerBrowser, ServerBrowserManifest>
{
    using type  = ServerBrowser;
    using Proxy = compo::proxy_of<ServerBrowserManifest>;

    ServerBrowser(GameEventBus& bus)
        : m_game_bus(bus)
    {
    }

    void start_restricted(Proxy& proxy, compo::time_point const&)
    {
    }

    void end_restricted(Proxy& proxy, compo::time_point const&)
    {
    }

    GameEventBus& m_game_bus;
};

void alloc_server_browser(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<ServerBrowser>(
        std::ref(e.subsystem_cast<GameEventBus>()));
}
