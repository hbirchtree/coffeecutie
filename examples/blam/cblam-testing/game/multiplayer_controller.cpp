#include "game/game_mode.h"

#include "loading/blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "game/game_modes.h"
#include "network/networking.h"
#include "scripting/script_vm.h"
#include "selected_version.h"
#include "audio/sounds.h"
#include "player/spawn_points.h"
#include "ticks.h"

#include <blam/volta/blam_globals.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>
#include <set>

/* Sound entity ids for the announcer, one per line so a line restarts */
constexpr libc_types::u64 announcer_sound_base = 0x414e4e4f00000000ULL; /* 'ANNO' */

using MultiplayerManifest = compo::SubsystemManifest<
    type_list_t<const PlayerInfo, const PlayerCamera, const BipedStatus, Model>,
    type_list_t<
        GameState,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const GameSetup,
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
        m_touch_queue->poll();
        auto touches = std::exchange(m_touches, {});
        auto kills   = std::exchange(m_kills, {});
        auto rejoins = std::exchange(m_rejoins, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
            return;
        bool const  replica = p.template subsystem<NetworkState>().replica;
        auto const& files   = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state   = p.template subsystem<GameState>();
        m_state             = &state;
        if(m_load_generation != files.load_generation)
            begin(state, files, p.template subsystem<GameSetup>(), replica);
        if(state.kind != GameState::kind_t::multiplayer)
            return;
        state.over_for = state.phase == GameState::phase_t::over ? state.over_for + dt : 0.f;
        announcer(p, state, dt);
        team_colors(p, state);
        if(replica)
            return;
        for(auto const& kill : kills)
            killed(state, kill);
        for(u64 unit : rejoins)
            m_waiting[unit] = m_respawn_delay;
        m_proxy = &p;
        for(auto const& touch : touches)
            touched(state, touch);
        if(m_vm && state.phase == GameState::phase_t::playing && dt > 0.f)
            m_vm->tick();
        m_proxy = nullptr;
        if(state.time_left > 0.f && (state.time_left -= dt) <= 0.f)
        {
            state.time_left = 0.f;
            over(state);
        }
        respawn(p, state, files, dt);
        place(p, state, files);
    }

    void begin(GameState& state, BlamFiles<halo_version> const& files, GameSetup const& setup, bool replica)
    {
        m_load_generation = files.load_generation;
        m_waiting.clear();
        m_placed.clear();
        m_vm.reset();
        m_local_vm.reset();
        m_seen.clear();
        m_kill_times.clear();
        m_streaks.clear();
        m_over = false;
        m_respawn_delay       = default_delay;
        if(replica)
            return;
        state.vehicle_respawn = GameState::default_vehicle_respawn;
        if(state.kind != GameState::kind_t::multiplayer)
            return;
        m_program = game_modes::rules_for<BC>(setup);
        m_vm      = std::make_unique<vm_t>();
        m_vm->load(
            semantic::Span<vm_t::node_t const>(m_program.nodes.data(), m_program.nodes.size()),
            m_program.segment(),
            m_program.scripts(),
            semantic::Span<blam::hsc::global const>(m_program.globals.data(), m_program.globals.size()),
            [this](BC opcode, vm_t::node_t const&, std::vector<script_vm::value_t>& args, vm_t::thread_t&) {
                return native(*m_state, opcode, args);
            });
        configure(*m_vm, setup);
    }

    /* --game-option name=value: any global of the program by that name */
    static void configure(vm_t& vm, GameSetup const& setup)
    {
        using blam::hsc::type_t;
        for(auto const& [name, text] : setup.options)
        {
            auto* slot = vm.global(name);
            if(!slot)
                continue;
            if(slot->type == type_t::bool_)
                *slot = script_vm::value_t::boolean(text == "true" || text == "1" || text == "yes");
            else
                *slot = script_vm::value_t::number(slot->type, std::strtof(text.c_str(), nullptr));
            cDebug("Game option {} = {}", name, text);
        }
    }

    /* ---- The announcer, on every peer for its own players ---- */

    /* Started once the game is known here, so a peer joining late hears the
     * game type; it sees its players' kills and deaths as they are told */
    void announcer(Proxy& p, GameState const& state, f32 dt)
    {
        m_clock += dt;
        /* Started from what everyone is told: a peer waits for the scores */
        if(!m_local_vm && state.players.empty())
            return;
        if(!m_local_vm)
        {
            m_local_program = game_modes::announcer_for<BC>(state.game_type);
            m_local_vm      = std::make_unique<vm_t>();
            m_local_vm->load(
                semantic::Span<vm_t::node_t const>(m_local_program.nodes.data(), m_local_program.nodes.size()),
                m_local_program.segment(),
                m_local_program.scripts(),
                semantic::Span<blam::hsc::global const>(
                    m_local_program.globals.data(), m_local_program.globals.size()),
                [this](BC opcode, vm_t::node_t const&, std::vector<script_vm::value_t>& args, vm_t::thread_t&) {
                    return local_native(*m_state, opcode, args);
                });
            configure(*m_local_vm, p.template subsystem<GameSetup>());
            /* What happened before it started is not news */
            for(auto const& player : state.players)
                m_seen[player.player_idx] = {player.kills, player.deaths};
        }
        m_game = &p.template subsystem<GameEventBus>();
        std::set<u32> mine;
        for(auto ent : p.template select<PlayerInfo>())
            if(auto const& info = ent.template get<PlayerInfo>(); !info.is_remote())
                mine.insert(info.player_idx);
        for(u32 idx : mine)
        {
            auto const* player = state.player(idx);
            if(!player)
                continue;
            auto& seen = m_seen.try_emplace(idx, seen_t{player->kills, player->deaths}).first->second;
            if(player->deaths > seen.deaths)
                m_streaks[idx] = 0;
            i16 const fresh = player->kills - seen.kills;
            seen = {player->kills, player->deaths};
            if(fresh <= 0)
                continue;
            auto& times = m_kill_times[idx];
            for(i16 k = 0; k < fresh; k++)
                times.push_back(m_clock);
            std::erase_if(times, [this](f32 at) { return m_clock - at > kill_memory; });
            m_streaks[idx] += fresh;
            if(auto* slot = m_local_vm->global("event_player"))
                *slot = script_vm::value_t::number(blam::hsc::type_t::short_, static_cast<f32>(idx));
            m_local_vm->wake("on_local_kill");
            m_local_vm->tick();
        }
        if(state.phase == GameState::phase_t::over && !std::exchange(m_over, true))
            m_local_vm->wake("on_game_over");
        if(dt > 0.f)
            m_local_vm->tick();
    }

    script_vm::value_t local_native(GameState const& state, BC opcode, std::vector<script_vm::value_t>& args)
    {
        using script_vm::value_t;
        using blam::hsc::type_t;
        auto arg = [&args](size_t i) { return i < args.size() ? args[i].as_real() : 0.f; };
        switch(static_cast<mode_op>(static_cast<u16>(opcode)))
        {
        case mode_op::announce: {
            GameEvent     ev{.type = GameEvent::Announce};
            AnnounceEvent say{.sound = static_cast<u16>(arg(0))};
            m_game->inject(ev, &say);
            return {};
        }
        case mode_op::teams:
            return value_t::boolean(state.teams);
        case mode_op::flag_taken: {
            auto const t = static_cast<size_t>(std::max(arg(0), 0.f));
            return value_t::boolean(t < state.flags.size() && state.flags[t].taken);
        }
        case mode_op::team_score: {
            auto const t = static_cast<size_t>(std::max(arg(0), 0.f));
            return value_t::number(type_t::short_, t < state.team_scores.size() ? state.team_scores[t] : 0);
        }
        case mode_op::kills_within: {
            auto const it = arg(0) >= 0.f ? m_kill_times.find(static_cast<u32>(arg(0))) : m_kill_times.end();
            i16 count = 0;
            if(it != m_kill_times.end())
                for(f32 at : it->second)
                    count += m_clock - at <= arg(1);
            return value_t::number(type_t::short_, count);
        }
        case mode_op::kill_streak: {
            auto const it = arg(0) >= 0.f ? m_streaks.find(static_cast<u32>(arg(0))) : m_streaks.end();
            return value_t::number(type_t::short_, it != m_streaks.end() ? it->second : 0);
        }
        default:
            return {};
        }
    }

    /* ---- What the rules may do ---- */

    script_vm::value_t native(GameState& state, BC opcode, std::vector<script_vm::value_t>& args)
    {
        using script_vm::value_t;
        using blam::hsc::type_t;
        auto arg = [&args](size_t i) { return i < args.size() ? args[i].as_real() : 0.f; };
        auto who = [&](size_t i) { return state.player(static_cast<u32>(arg(i))); };
        auto const op = static_cast<mode_op>(static_cast<u16>(opcode));
        switch(op)
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
        case mode_op::flag_give:
        case mode_op::flag_reset: {
            GameEvent        ev{.type = GameEvent::FlagCommand};
            FlagCommandEvent command{
                .kind   = op == mode_op::flag_give ? FlagCommandEvent::give : FlagCommandEvent::reset,
                .team   = static_cast<u16>(arg(0)),
                .player = static_cast<u32>(arg(1)),
            };
            if(arg(0) >= 0.f && (op == mode_op::flag_reset || arg(1) >= 0.f))
                m_game->inject(ev, &command);
            return {};
        }
        case mode_op::flag_taken:
        case mode_op::flag_carrier:
        case mode_op::flag_idle: {
            auto const t = static_cast<size_t>(std::max(arg(0), 0.f));
            if(t >= state.flags.size())
                return op == mode_op::flag_taken ? value_t::boolean(false)
                                                 : value_t::number(type_t::short_, -1);
            auto const& flag = state.flags[t];
            if(op == mode_op::flag_taken)
                return value_t::boolean(flag.taken);
            if(op == mode_op::flag_idle)
                return value_t::number(type_t::real_, flag.idle);
            return value_t::number(type_t::short_, static_cast<f32>(flag.carrier));
        }
        case mode_op::flag_reach_set:
            state.flag_reach = std::max(arg(0), 0.f);
            return {};
        case mode_op::team_of: {
            auto const* player = arg(0) >= 0.f ? who(0) : nullptr;
            return value_t::number(type_t::short_, player ? player->team : -1);
        }
        case mode_op::player_near_base: {
            auto const t = static_cast<size_t>(std::max(arg(1), 0.f));
            if(arg(0) < 0.f || t >= state.flags.size() || !state.flags[t].placed || !m_proxy)
                return value_t::boolean(false);
            for(auto ent : m_proxy->template select<PlayerInfo, Model>())
                if(ent.template get<PlayerInfo>().player_idx == static_cast<u32>(arg(0)))
                    return value_t::boolean(
                        glm::distance(ent.template get<Model>().position, state.flags[t].base) < arg(2));
            return value_t::boolean(false);
        }
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
        if(m_game)
        {
            GameEvent     ev{.type = GameEvent::KillFeed};
            KillFeedEvent feed{.victim = kill.player, .killer = kill.killer, .cause = kill.cause};
            m_game->inject(ev, &feed);
        }
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

    /* A player reached a flag on the ground: the rules say what that does */
    void touched(GameState const& state, FlagTouchEvent const& touch)
    {
        if(!m_vm || state.phase != GameState::phase_t::playing)
            return;
        auto set = [this](std::string_view name, f32 value) {
            if(auto* slot = m_vm->global(name))
                *slot = script_vm::value_t::number(blam::hsc::type_t::short_, value);
        };
        set("event_player", static_cast<f32>(touch.player));
        set("event_team", static_cast<f32>(touch.team));
        m_vm->wake("on_flag_touched");
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

    /* A team game puts each player first at one of its team's start points */
    void place(Proxy& p, GameState const& state, BlamFiles<halo_version> const& files)
    {
        if(!state.teams || state.phase == GameState::phase_t::over)
            return;
        for(auto ent : p.template select<PlayerInfo, BipedStatus>())
        {
            auto [info, status] = ent.components();
            if(status.dead || !state.player(info.player_idx) || m_placed.contains(info.player_idx))
                continue;
            auto const spot = spot_for(p, state, files, info);
            if(!spot)
                continue;
            m_placed.insert(info.player_idx);
            GameEvent        ev{.type = GameEvent::RespawnUnit};
            RespawnUnitEvent respawn{
                .unit = ent.id(), .position = spot->first, .facing = spot->second, .place = true};
            p.template subsystem<GameEventBus>().inject(ev, &respawn);
        }
    }

    /* Players wear their team's colour as change colour C, which cyborg_mp leaves to the game */
    static void team_colors(Proxy& p, GameState const& state)
    {
        if(!state.teams)
            return;
        for(auto ent : p.template select<PlayerInfo, Model>())
        {
            auto [info, model] = ent.components();
            auto const* player = state.player(info.player_idx);
            if(!player || player->team < 0)
                continue;
            model.change_colors[2] = pack_color(player->team == 0 ? Vecf3{1.f, 0.f, 0.f} : Vecf3{0.f, 0.f, 1.f});
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

    static constexpr f32 default_delay = 3.f;  /* s, until the rules say */
    static constexpr f32 kill_memory   = 30.f; /* s of kill times kept for kills_within */

    struct seen_t
    {
        i16 kills{0};
        i16 deaths{0};
    };

    std::unique_ptr<TickQueue>                                 m_ticks;
    std::shared_ptr<GameEventBus::queue_type<UnitKilledEvent>> m_kill_queue;
    std::vector<UnitKilledEvent>                               m_kills;
    std::shared_ptr<GameEventBus::queue_type<UnitRejoinedEvent>> m_rejoin_queue;
    std::shared_ptr<GameEventBus::queue_type<FlagTouchEvent>>    m_touch_queue;
    std::vector<FlagTouchEvent>                                  m_touches;
    Proxy*                                                       m_proxy{nullptr}; /*!< While the rules run */
    std::vector<u64>                                           m_rejoins;

    GameState*                 m_state{nullptr};
    u32                        m_load_generation{0};
    hsc_builder::program_t<BC> m_program;
    std::unique_ptr<vm_t>      m_vm;
    f32                        m_respawn_delay{default_delay};
    std::map<u64, f32>         m_waiting; /*!< Seconds left */
    std::set<u32>              m_placed;  /*!< Players put at their team's start */

    hsc_builder::program_t<BC>      m_local_program;
    std::unique_ptr<vm_t>           m_local_vm;
    GameEventBus*                   m_game{nullptr};
    f32                             m_clock{0.f}; /*!< Seconds of play here */
    bool                            m_over{false};
    std::map<u32, seen_t>           m_seen;       /*!< By player: kills and deaths last seen */
    std::map<u32, std::vector<f32>> m_kill_times; /*!< By player: when they killed */
    std::map<u32, i16>              m_streaks;    /*!< By player: kills since dying */
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
    mode.m_touch_queue = game.addQueuedEventFunction<FlagTouchEvent>(
        0, [&mode](GameEvent&, FlagTouchEvent* touch) { mode.m_touches.push_back(*touch); });
    /* The announcer's lines are matg's multiplayer sounds, heard here */
    game.addEventFunction<AnnounceEvent>(0, [&e](GameEvent&, AnnounceEvent* say) {
        auto* sounds = e.service<comp_app::EventBus<SoundEvent>>();
        if(!sounds)
            return;
        auto const& files = e.subsystem_cast<BlamFiles<halo_version>>();
        blam::tag_index_view<halo_version> index(files.container);
        auto globals_tag = index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(files.container.magic);
        if(!globals.has_value())
            return;
        auto mp = globals.value()->multiplayer.data(files.container.magic);
        if(!mp.has_value() || mp.value().empty())
            return;
        auto lines = mp.value()[0].sounds.data(files.container.magic);
        if(!lines.has_value() || say->sound >= lines.value().size() || !lines.value()[say->sound].valid())
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound, .entity_id = announcer_sound_base | say->sound};
        PlaySoundEvent play{
            .sound    = &static_cast<blam::tagref_t const&>(lines.value()[say->sound]),
            .relative = true,
        };
        sounds->inject(ev, &play);
    });
}
