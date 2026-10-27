#include "game_mode.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "game_modes.h"
#include "network/networking.h"
#include "script_vm.h"
#include "selected_version.h"
#include "spawn_points.h"
#include "ticks.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using MultiplayerManifest = compo::SubsystemManifest<
    type_list_t<const PlayerInfo, const PlayerCamera, const BipedStatus>,
    type_list_t<
        GameState,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* Multiplayer is ruled by an in-memory HaloScript program for the game
 * type: kills wake it, and it gives out score and ends the game. The dead
 * come back after a delay at a free start point away from their enemies */
struct MultiplayerController
    : compo::RestrictedSubsystem<MultiplayerController, MultiplayerManifest>
{
    using type    = MultiplayerController;
    using Proxy   = compo::proxy_of<MultiplayerManifest>;
    using BC      = halo_version::bytecode_type;
    using vm_t    = script_vm::vm<BC>;
    using mode_op = game_modes::mode_op;

    MultiplayerController()
    {
        this->priority = 907;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_kill_queue->poll();
        m_rejoin_queue->poll();
        auto kills   = std::exchange(m_kills, {});
        auto rejoins = std::exchange(m_rejoins, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state = p.template subsystem<GameState>();
        m_state           = &state;
        if(m_load_generation != files.load_generation)
            begin(state, files);
        if(state.kind != GameState::kind_t::multiplayer)
            return;
        for(auto const& kill : kills)
            killed(state, kill);
        for(u64 unit : rejoins)
            m_waiting[unit] = m_respawn_delay;
        if(m_vm && state.phase == GameState::phase_t::playing && dt > 0.f)
            m_vm->tick();
        if(state.time_left > 0.f && (state.time_left -= dt) <= 0.f)
        {
            state.time_left = 0.f;
            over(state);
        }
        respawn(p, state, files, dt);
    }

    void begin(GameState& state, BlamFiles<halo_version> const& files)
    {
        m_load_generation = files.load_generation;
        m_waiting.clear();
        m_vm.reset();
        m_respawn_delay       = default_delay;
        state.vehicle_respawn = GameState::default_vehicle_respawn;
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
        case mode_op::vehicle_respawn_set:
            state.vehicle_respawn = arg(0);
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

    static void over(GameState& state)
    {
        if(state.phase == GameState::phase_t::over)
            return;
        state.phase  = GameState::phase_t::over;
        state.winner = leader(state).first;
    }

    /* ---- Deaths ---- */

    void killed(GameState const& state, UnitKilledEvent const& kill)
    {
        if(kill.player == ObjectHitEvent::no_player)
            return;
        m_waiting[kill.unit] = m_respawn_delay;
        if(!m_vm || state.phase != GameState::phase_t::playing)
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

    void respawn(Proxy& p, GameState const& state, BlamFiles<halo_version> const& files, f32 dt)
    {
        if(state.phase == GameState::phase_t::over)
            return;
        for(auto it = m_waiting.begin(); it != m_waiting.end();)
        {
            auto const* status = p.template get<BipedStatus>(it->first);
            auto const* info   = p.template get<PlayerInfo>(it->first);
            if(!status || !info || !status->dead)
            {
                it = m_waiting.erase(it);
                continue;
            }
            auto const spot = (it->second -= dt) > 0.f ? spawn_points::spot_t{}
                                                       : spot_for(p, state, files, *info);
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
    }

    /* A start point for this game type and team, far from enemies */
    spawn_points::spot_t spot_for(
        Proxy& p, GameState const& state, BlamFiles<halo_version> const& files, PlayerInfo const& info)
    {
        auto const* me = state.player(info.player_idx);
        std::vector<Vecf3> enemies, standing;
        for(auto other : p.template select<PlayerInfo, PlayerCamera, BipedStatus>())
        {
            auto [oinfo, cam, status] = other.components();
            auto const* them = state.player(oinfo.player_idx);
            if(status.dead || oinfo.player_idx == info.player_idx)
                continue;
            Vecf3 const feet = cam.camera.position - Vecf3{0.f, 0.f, oinfo.eye_height()};
            standing.push_back(feet);
            if(!(me && them && me->team >= 0 && me->team == them->team))
                enemies.push_back(feet);
        }
        return spawn_points::pick(
            files,
            m_random,
            enemies,
            spawn_points::game_mode_of(state.game_type),
            me ? me->team : i16(-1),
            &standing);
    }

    static constexpr f32 default_delay = 3.f; /* s, until the rules say */

    std::unique_ptr<TickQueue>                                 m_ticks;
    std::shared_ptr<GameEventBus::queue_type<UnitKilledEvent>> m_kill_queue;
    std::vector<UnitKilledEvent>                               m_kills;
    std::shared_ptr<GameEventBus::queue_type<UnitRejoinedEvent>> m_rejoin_queue;
    std::vector<u64>                                           m_rejoins;

    GameState*                 m_state{nullptr};
    u32                        m_load_generation{0};
    hsc_builder::program_t<BC> m_program;
    std::unique_ptr<vm_t>      m_vm;
    f32                        m_respawn_delay{default_delay};
    std::map<u64, f32>         m_waiting; /*!< Seconds left */
    std::mt19937               m_random{std::random_device{}()};
};

void alloc_multiplayer_controller(compo::EntityContainer& e)
{
    auto& mode   = e.register_subsystem_inplace<MultiplayerController>();
    auto& game   = e.subsystem_cast<GameEventBus>();
    mode.m_ticks = std::make_unique<TickQueue>(game);
    mode.m_kill_queue = game.addQueuedEventFunction<UnitKilledEvent>(
        0, [&mode](GameEvent&, UnitKilledEvent* kill) { mode.m_kills.push_back(*kill); });
    mode.m_rejoin_queue = game.addQueuedEventFunction<UnitRejoinedEvent>(
        0, [&mode](GameEvent&, UnitRejoinedEvent* back) { mode.m_rejoins.push_back(back->unit); });
}
