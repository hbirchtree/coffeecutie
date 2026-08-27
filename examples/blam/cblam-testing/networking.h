#pragma once

#include <coffee/components/entity_container.h>

#include <optional>
#include <vector>

struct NetworkState : compo::SubsystemBase
{
    using type = NetworkState;

    enum class ClientState
    {
        None,
        Establishing,
        Connecting,
        Unstable,
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

    std::optional<std::string> local_player_name;

    struct RosterEntry
    {
        std::string     name;
        libc_types::u32 remote_idx{0};
        libc_types::u32 loading_progress{100};
        bool            is_self{false};
    };
};

struct PlayerInfo;

struct PlayerRoster : compo::SubsystemBase
{
    using type = PlayerRoster;

    PlayerRoster(compo::EntityContainer& container)
        : m_container(container)
    {
    }

    libc_types::u32 player_count();

    std::vector<NetworkState::RosterEntry> roster(
        std::optional<libc_types::u32> self_idx = std::nullopt);

  private:
    compo::EntityContainer& m_container;
};

void alloc_networking(
    compo::EntityContainer& e,
    std::string const&      gateway_register_url = {},
    std::string const&      gateway_auth_secret  = {},
    std::string const&      gateway_auth_key     = {});
