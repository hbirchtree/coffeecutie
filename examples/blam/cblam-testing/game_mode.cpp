#include "game_mode.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "selected_version.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

using GameModeManifest = compo::SubsystemManifest<
    type_list_t<const PlayerInfo, const PlayerCamera, const NetworkInfo>,
    type_list_t<
        GameState,
        const GameSetup,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* What every game has: its kind from the map, who is playing, and their
 * kills and deaths. The campaign and multiplayer controllers run the rest.
 * Only the authority decides; every peer is told the state */
struct GameModeSystem : compo::RestrictedSubsystem<GameModeSystem, GameModeManifest>
{
    using type  = GameModeSystem;
    using Proxy = compo::proxy_of<GameModeManifest>;

    GameModeSystem()
    {
        this->priority = 905;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        m_kill_queue->poll();
        auto kills = std::exchange(m_kills, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state = p.template subsystem<GameState>();
        if(m_load_generation != files.load_generation)
            begin(p, state, files);
        if(p.template subsystem<NetworkState>().replica)
            return;
        join(p, state);
        for(auto const& kill : kills)
            count(state, kill);
    }

    /* A new map, a new game */
    void begin(Proxy& p, GameState& state, BlamFiles<halo_version> const& files)
    {
        m_load_generation = files.load_generation;
        auto const& setup = p.template subsystem<GameSetup>();
        auto const  type  = files.container.map ? files.container.map->map_type
                                                : blam::maptype_t::ui;
        state.kind = type == blam::maptype_t::multiplayer    ? GameState::kind_t::multiplayer
                     : type == blam::maptype_t::singleplayer ? GameState::kind_t::campaign
                                                             : GameState::kind_t::none;
        state.phase       = GameState::phase_t::playing;
        state.game_type   = setup.game_type;
        state.teams       = false;
        state.difficulty  = setup.difficulty;
        state.players.clear();
        state.team_scores = {};
        state.winner      = -1;
        state.time_left   = -1.f;
        state.saves = state.reverts = 0;
        state.quiet  = true;
        state.saving = false;
    }

    /* Players are counted from when they are first in the game */
    void join(Proxy& p, GameState& state)
    {
        for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo>())
        {
            auto [info, cam, net] = player.components();
            if(!biped_shown(info, cam, net) || state.player(info.player_idx))
                continue;
            state.players.push_back({
                .player_idx = info.player_idx,
                .team = state.teams ? static_cast<i16>(info.player_idx % 2) : i16(-1),
            });
        }
    }

    void count(GameState& state, UnitKilledEvent const& kill)
    {
        if(kill.player == ObjectHitEvent::no_player)
            return;
        if(auto* victim = state.player(kill.player))
            victim->deaths++;
        if(auto* killer = kill.killer != kill.player ? state.player(kill.killer) : nullptr)
            killer->kills++;
    }

    std::shared_ptr<GameEventBus::queue_type<UnitKilledEvent>> m_kill_queue;
    std::vector<UnitKilledEvent>                               m_kills;
    u32                                                        m_load_generation{0};
};

void alloc_game_mode(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<GameState>();
    auto& mode = e.register_subsystem_inplace<GameModeSystem>();
    auto& game = e.subsystem_cast<GameEventBus>();
    mode.m_kill_queue = game.addQueuedEventFunction<UnitKilledEvent>(
        0, [&mode](GameEvent&, UnitKilledEvent* kill) { mode.m_kills.push_back(*kill); });
    alloc_campaign_controller(e);
    alloc_multiplayer_controller(e);
}
