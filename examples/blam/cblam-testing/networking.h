#pragma once

#include <coffee/components/entity_container.h>

struct NetworkState : compo::SubsystemBase
{
    using type = NetworkState;

    enum class ClientState
    {
        None,
        Establishing,
        Connecting,
        Connected,
        Error,
        Disconnecting,
    } client_state;
    enum class ServerState
    {
        None,
        Listening,
        Error,
    } server_state;
    std::optional<std::string> error;
    std::optional<std::string> local_address;
    std::optional<std::string> remote_address;
};

void alloc_networking(compo::EntityContainer& e);
