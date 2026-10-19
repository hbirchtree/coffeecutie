#include "game_mode.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "game_modes.h"
#include "network/networking.h"
#include "script_vm.h"
#include "selected_version.h"
#include "ticks.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using GameModeManifest = compo::SubsystemManifest<
    type_list_t<const PlayerInfo, const PlayerCamera, const NetworkInfo, const BipedStatus>,
    type_list_t<
        GameState,
        GameSetup,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* What a game is played for. Campaign deaths go back to a checkpoint, or to
 * a partner still alive; multiplayer is ruled by an in-memory HaloScript
 * program for the game type. Only the authority decides; every peer is
 * told the state */
struct GameModeSystem : compo::RestrictedSubsystem<GameModeSystem, GameModeManifest>
{
    using type  = GameModeSystem;
    using Proxy = compo::proxy_of<GameModeManifest>;
    using BC    = halo_version::bytecode_type;
    using vm_t  = script_vm::vm<BC>;
    using mode_op = game_modes::mode_op;

    GameModeSystem()
    {
        this->priority = 905;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_kill_queue->poll();
        m_won_queue->poll();
        m_checkpoint_queue->poll();
        auto kills       = std::exchange(m_kills, {});
        bool const won   = std::exchange(m_won, false);
        bool const saved = std::exchange(m_saved, false);
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state = p.template subsystem<GameState>();
        m_state           = &state;
        if(m_load_generation != files.load_generation)
            begin(p, state, files);
        if(p.template subsystem<NetworkState>().replica)
            return;

        join(p, state);
        /* A level starts with a checkpoint, once someone is in it */
        bool const first = state.kind == GameState::kind_t::campaign && !m_started &&
                           !state.players.empty();
        m_started        = m_started || first;
        if(saved || first)
            checkpoint(p, state);
        for(auto const& kill : kills)
            count(p, state, kill);
        if(won && state.kind == GameState::kind_t::campaign)
            next_level(p, files);
        if(state.kind == GameState::kind_t::multiplayer)
        {
            if(m_vm && state.phase == GameState::phase_t::playing && dt > 0.f)
                m_vm->tick();
            if(state.time_left > 0.f && (state.time_left -= dt) <= 0.f)
            {
                state.time_left = 0.f;
                over(state);
            }
        }
        respawn(p, state, dt);
    }

    /* ---- A new map, a new game ---- */

    void begin(Proxy& p, GameState& state, BlamFiles<halo_version> const& files)
    {
        m_load_generation = files.load_generation;
        m_magic           = &files.container.magic;
        m_waiting.clear();
        m_started = false;
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
        m_vm.reset();
        if(state.kind != GameState::kind_t::multiplayer)
            return;
        m_program = game_modes::rules_for<BC>(state.game_type);
        m_vm      = std::make_unique<vm_t>();
        m_vm->load(
            semantic::Span<vm_t::node_t const>(m_program.nodes.data(), m_program.nodes.size()),
            m_program.segment(),
            m_program.scripts(),
            semantic::Span<blam::hsc::global const>(m_program.globals.data(), m_program.globals.size()),
            [this](BC opcode, vm_t::node_t const&, std::vector<script_vm::value_t>& args, vm_t::thread_t&) {
                return native(*m_state, opcode, args);
            });
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

    /* ---- What the rules may do ---- */

    script_vm::value_t native(GameState& state, BC opcode, std::vector<script_vm::value_t>& args)
    {
        using script_vm::value_t;
        using blam::hsc::type_t;
        auto arg = [&args](size_t i) { return i < args.size() ? args[i].as_real() : 0.f; };
        auto who = [&](size_t i) { return state.player(static_cast<u32>(arg(i))); };
        switch(static_cast<mode_op>(static_cast<u16>(opcode)))
        {
        case mode_op::score_add:
            if(auto* player = arg(0) >= 0.f ? who(0) : nullptr)
            {
                player->score += static_cast<i16>(arg(1));
                if(player->team >= 0)
                    state.team_scores[player->team] += static_cast<i16>(arg(1));
            }
            return {};
        case mode_op::player_score: {
            auto const* player = arg(0) >= 0.f ? who(0) : nullptr;
            return value_t::number(type_t::short_, player ? player->score : 0);
        }
        case mode_op::leader_score:
            return value_t::number(type_t::short_, leader(state).second);
        case mode_op::same_team: {
            auto const *a = arg(0) >= 0.f ? who(0) : nullptr, *b = arg(1) >= 0.f ? who(1) : nullptr;
            return value_t::boolean(a && b && a != b && a->team >= 0 && a->team == b->team);
        }
        case mode_op::game_over:
            over(state);
            return {};
        case mode_op::respawn_delay_set:
            m_respawn_delay = std::max(arg(0), 0.f);
            return {};
        case mode_op::time_limit_set:
            state.time_left = arg(0) > 0.f ? arg(0) : -1.f;
            return {};
        default:
            return {};
        }
    }

    /* The best team or player and their score */
    static std::pair<i32, i16> leader(GameState const& state)
    {
        std::pair<i32, i16> best{-1, std::numeric_limits<i16>::min()};
        if(state.teams)
        {
            for(i32 t = 0; t < static_cast<i32>(state.team_scores.size()); t++)
                if(state.team_scores[t] > best.second)
                    best = {t, state.team_scores[t]};
        } else
            for(auto const& player : state.players)
                if(player.score > best.second)
                    best = {static_cast<i32>(player.player_idx), player.score};
        if(best.first < 0)
            best.second = 0;
        return best;
    }

    void over(GameState& state)
    {
        if(state.phase == GameState::phase_t::over)
            return;
        state.phase  = GameState::phase_t::over;
        state.winner = leader(state).first;
    }

    /* ---- Deaths ---- */

    void count(Proxy& p, GameState& state, UnitKilledEvent const& kill)
    {
        if(kill.player == ObjectHitEvent::no_player)
            return;
        if(auto* victim = state.player(kill.player))
            victim->deaths++;
        if(auto* killer = kill.killer != kill.player ? state.player(kill.killer) : nullptr)
            killer->kills++;
        m_waiting[kill.unit] = state.kind == GameState::kind_t::multiplayer ? m_respawn_delay
                                                                            : campaign_delay;
        if(state.kind != GameState::kind_t::multiplayer || !m_vm ||
           state.phase != GameState::phase_t::playing)
            return;
        auto set = [this](std::string_view name, u32 player) {
            if(auto* slot = m_vm->global(name))
                *slot = script_vm::value_t::number(
                    blam::hsc::type_t::short_,
                    player == ObjectHitEvent::no_player ? -1.f : static_cast<f32>(player));
        };
        set("event_killer", kill.killer);
        set("event_victim", kill.player);
        m_vm->wake("on_unit_killed");
        m_vm->tick();
    }

    /* The dead come back when their wait is over, where the game says */
    void respawn(Proxy& p, GameState& state, f32 dt)
    {
        if(state.phase == GameState::phase_t::over)
            return;
        bool const campaign = state.kind == GameState::kind_t::campaign;
        /* Campaign: alone or all fallen, everyone goes back to the
         * checkpoint together */
        bool const anyone_alive = std::any_of(
            m_alive.begin(), m_alive.end(), [](auto const& a) { return a.second; });
        for(auto it = m_waiting.begin(); it != m_waiting.end();)
        {
            auto const* status = p.template get<BipedStatus>(it->first);
            auto const* info   = p.template get<PlayerInfo>(it->first);
            if(!status || !info || !status->dead)
            {
                it = m_waiting.erase(it);
                continue;
            }
            if((it->second -= dt) > 0.f)
            {
                ++it;
                continue;
            }
            /* Nobody left standing: the whole world goes back */
            if(campaign && !anyone_alive)
            {
                GameEvent         ev{.type = GameEvent::WorldRestore};
                WorldRestoreEvent restore{};
                p.template subsystem<GameEventBus>().inject(ev, &restore);
                state.reverts++;
                m_waiting.clear();
                break;
            }
            auto const spot = campaign ? campaign_spot(p, *info)
                                       : multiplayer_spot(p, state, *info);
            if(!spot)
            {
                ++it;
                continue;
            }
            GameEvent        ev{.type = GameEvent::RespawnUnit};
            RespawnUnitEvent respawn{.unit = it->first, .position = spot->first, .facing = spot->second};
            p.template subsystem<GameEventBus>().inject(ev, &respawn);
            it = m_waiting.erase(it);
        }
        m_alive.clear();
        for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedStatus>())
        {
            auto [info, cam, net, status] = player.components();
            if(biped_shown(info, cam, net))
                m_alive[player.id()] = !status.dead;
        }
    }

    using spot_t = std::optional<std::pair<Vecf3, f32>>;

    /* Next to a partner still standing */
    spot_t campaign_spot(Proxy& p, PlayerInfo const& info)
    {
        for(auto other : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedStatus>())
            {
                auto [oinfo, cam, net, status] = other.components();
                if(status.dead || oinfo.player_idx == info.player_idx ||
                   !biped_in_play(oinfo, cam, net))
                    continue;
                return std::pair{cam.camera.position - Vecf3{0.f, 0.f, oinfo.biped.eye_height} +
                                     Vecf3{partner_offset, 0.f, 0.f},
                                 yaw_of(cam)};
            }
        return start_spot(p, {});
    }

    /* A start point for this game type, far from anyone it could fight */
    spot_t multiplayer_spot(Proxy& p, GameState const& state, PlayerInfo const& info)
    {
        auto const* me = state.player(info.player_idx);
        std::vector<Vecf3> enemies, standing;
        for(auto other : p.template select<PlayerInfo, PlayerCamera, BipedStatus>())
        {
            auto [oinfo, cam, status] = other.components();
            auto const* them = state.player(oinfo.player_idx);
            if(status.dead || oinfo.player_idx == info.player_idx)
                continue;
            Vecf3 const feet = cam.camera.position - Vecf3{0.f, 0.f, oinfo.biped.eye_height};
            standing.push_back(feet);
            if(!(me && them && me->team >= 0 && me->team == them->team))
                enemies.push_back(feet);
        }
        return start_spot(
            p, enemies, game_mode_of(state.game_type), me ? me->team : i16(-1), &standing);
    }

    /* Of the start points for this game type and team, one nobody stands
     * on; none free and it waits */
    spot_t start_spot(
        Proxy& p, std::vector<Vecf3> const& enemies,
        blam::scn::gamemode_t mode = blam::scn::gamemode_t::none, i16 team = -1,
        std::vector<Vecf3> const* standing = nullptr)
    {
        auto const& files    = p.template subsystem<BlamFiles<halo_version>>();
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return std::nullopt;
        auto spots = scenario->player_start.locations.data(*m_magic);
        if(!spots.has_value() || spots.value().empty())
            return std::nullopt;
        using gm = blam::scn::gamemode_t;
        std::vector<std::pair<f32, size_t>> ranked;
        bool                                any_fit = false;
        for(size_t i = 0; i < spots.value().size(); i++)
        {
            auto const& spot = spots.value()[i];
            bool const  fits = mode == gm::none ||
                              std::any_of(std::begin(spot.types), std::end(spot.types), [mode](gm t) {
                                  return t == mode || t == gm::all_games;
                              });
            bool const ours = team < 0 || spot.team_index == static_cast<u16>(team);
            bool const taken =
                standing && std::any_of(standing->begin(), standing->end(), [&spot](Vecf3 const& at) {
                    Vecf3 const d = at - spot.pos;
                    return glm::length(Vecf2(d)) < spot_clearance && std::abs(d.z) < spot_height;
                });
            any_fit = any_fit || (fits && ours);
            if(!fits || !ours || taken)
                continue;
            f32 nearest = std::numeric_limits<f32>::max();
            for(Vecf3 const& enemy : enemies)
                nearest = std::min(nearest, glm::distance(enemy, spot.pos));
            ranked.push_back({nearest, i});
        }
        if(ranked.empty() && any_fit)
            return std::nullopt;
        if(ranked.empty())
            for(size_t i = 0; i < spots.value().size(); i++)
                ranked.push_back({0.f, i});
        /* Among the few furthest from trouble, any */
        std::sort(ranked.begin(), ranked.end(), std::greater<>());
        size_t const pick = std::uniform_int_distribution<size_t>(
            0, std::min<size_t>(ranked.size(), spawn_choices) - 1)(m_random);
        auto const& spot = spots.value()[ranked[pick].second];
        return std::pair{spot.pos, spot.rot};
    }

    static blam::scn::gamemode_t game_mode_of(GameSetup::game_type_t type)
    {
        using t  = GameSetup::game_type_t;
        using gm = blam::scn::gamemode_t;
        switch(type)
        {
        case t::ctf:
            return gm::ctf;
        case t::koth:
            return gm::king_of_the_hill;
        case t::oddball:
            return gm::oddball;
        case t::race:
            return gm::race;
        default:
            return gm::slayer;
        }
    }

    static f32 yaw_of(PlayerCamera const& cam)
    {
        Vecf3 const forward = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        return std::atan2(forward.y, forward.x);
    }

    /* ---- Campaign ---- */

    /* The world as it is now, to come back to */
    void checkpoint(Proxy& p, GameState& state)
    {
        GameEvent      ev{.type = GameEvent::WorldSave};
        WorldSaveEvent save{};
        p.template subsystem<GameEventBus>().inject(ev, &save);
        state.saves++;
    }

    void next_level(Proxy& p, BlamFiles<halo_version> const& files)
    {
        auto const map  = files.container.internal_name();
        auto const next = next_campaign_map(map);
        p.template subsystem<GameSetup>().start(std::string(next ? *next : "ui"));
    }

    static constexpr f32    campaign_delay = 3.f;  /* s */
    static constexpr f32    partner_offset = 1.f;  /* wu, clear of their body */
    static constexpr size_t spawn_choices  = 3;
    /* A start point is taken while anyone stands this close to it */
    static constexpr f32    spot_clearance = 1.f; /* wu */
    static constexpr f32    spot_height    = 1.f; /* wu */

    std::unique_ptr<TickQueue> m_ticks;
    std::shared_ptr<GameEventBus::queue_type<UnitKilledEvent>> m_kill_queue;
    std::shared_ptr<GameEventBus::queue_type<GameWonEvent>>    m_won_queue;
    std::shared_ptr<GameEventBus::queue_type<CheckpointEvent>> m_checkpoint_queue;
    std::vector<UnitKilledEvent> m_kills;
    bool                         m_won{false};
    bool                         m_saved{false};

    GameState*                                  m_state{nullptr};
    u32                                         m_load_generation{0};
    blam::map_ptr const*                        m_magic{nullptr};
    hsc_builder::program_t<BC>                  m_program;
    std::unique_ptr<vm_t>                       m_vm;
    f32                                         m_respawn_delay{3.f};
    std::map<u64, f32>                          m_waiting; /*!< Seconds left */
    std::map<u64, bool>                         m_alive;
    bool                                        m_started{false};
    std::mt19937                                m_random{std::random_device{}()};
};

void alloc_game_mode(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<GameState>();
    auto& mode   = e.register_subsystem_inplace<GameModeSystem>();
    auto& game   = e.subsystem_cast<GameEventBus>();
    mode.m_ticks = std::make_unique<TickQueue>(game);
    mode.m_kill_queue = game.addQueuedEventFunction<UnitKilledEvent>(
        0, [&mode](GameEvent&, UnitKilledEvent* kill) { mode.m_kills.push_back(*kill); });
    mode.m_won_queue = game.addQueuedEventFunction<GameWonEvent>(
        0, [&mode](GameEvent&, GameWonEvent*) { mode.m_won = true; });
    mode.m_checkpoint_queue = game.addQueuedEventFunction<CheckpointEvent>(
        0, [&mode](GameEvent&, CheckpointEvent*) { mode.m_saved = true; });
}
