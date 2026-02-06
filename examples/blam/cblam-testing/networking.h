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

    std::optional<libc_types::u32> remote_player_idx;

    struct RosterEntry
    {
        std::string name;
        libc_types::u32 remote_idx{0};
        libc_types::u32 loading_progress{100};
        bool            is_self{false};
    };
    std::vector<RosterEntry> player_roster;
};

void alloc_networking(compo::EntityContainer& e);
