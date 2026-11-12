#include "game/ctf.h"

#include "components.h"
#include "data.h"
#include "game_events.h"
#include "game/game_mode.h"
#include "network/networking.h"
#include "ticks.h"
#include "combat/weapons.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>

using FlagManifest = compo::SubsystemManifest<
    type_list_t<
        const Model,
        const NetworkInfo,
        const PlayerInfo,
        const PlayerCamera,
        const BipedEquipment,
        const BipedStatus>,
    type_list_t<
        GameState,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* Capture the flag on the authority: each team's flag lies at its base, is
 * carried as a weapon, and lies where its carrier fell. Who may take it,
 * when it goes back and what a capture is, the game type's rules decide:
 * they hear a player touching a flag on the ground and give or reset it */
struct FlagSystem : compo::RestrictedSubsystem<FlagSystem, FlagManifest>
{
    using type  = FlagSystem;
    using Proxy = compo::proxy_of<FlagManifest>;

    FlagSystem()
    {
        this->priority = 909;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_command_queue->poll();
        auto commands = std::exchange(m_commands, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state = p.template subsystem<GameState>();
        if(m_load_generation != files.load_generation)
            begin(state, files);
        if(state.kind != GameState::kind_t::multiplayer || state.game_type != GameSetup::game_type_t::ctf ||
           !m_flag)
            return;

        for(size_t team = 0; team < state.flags.size(); team++)
            if(state.flags[team].placed && !m_spawned[team])
                spawn(p, state, team);
        carriers(p, state);
        lying(p, state, dt);
        for(auto const& command : commands)
            if(command.team < state.flags.size() && state.flags[command.team].placed)
            {
                if(command.kind == FlagCommandEvent::give)
                    give(p, state, command.team, command.player);
                else
                    reset(p, state, command.team);
            }
        touches(p, state);
        markers(state);
    }

    /* Each team's base is its ctf_flag netgame flag */
    void begin(GameState& state, BlamFiles<halo_version> const& files)
    {
        m_load_generation = files.load_generation;
        m_flag            = flag_weapon(files);
        m_spawned         = {};
        m_released        = {-1, -1};
        m_touching.clear();
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;
        auto flags = scenario->netgame.flags.data(files.container.magic);
        if(!flags.has_value())
            return;
        using type_t = blam::scn::multiplayer_flag::flag_type_t;
        for(auto const& flag : flags.value())
            if(flag.type == type_t::ctf_flag && flag.team_index < state.flags.size())
            {
                auto& kept  = state.flags[flag.team_index];
                kept.base   = flag.pos;
                kept.placed = true;
            }
    }

    /* A flag in someone's hands stays theirs until they let go of it, by
     * dying or otherwise; it then lies near where they were */
    void carriers(Proxy& p, GameState& state)
    {
        std::map<u32, Vecf3> holding;
        for(auto ent : p.template select<PlayerInfo, BipedEquipment, Model>())
        {
            auto [info, equip, model] = ent.components();
            auto const* status        = p.template get<BipedStatus>(ent.id());
            if((status && status->dead) || equip.objective.tag != m_flag)
                continue;
            holding[info.player_idx] = model.position;
        }
        for(size_t team = 0; team < state.flags.size(); team++)
        {
            auto& flag = state.flags[team];
            if(flag.carrier < 0)
                continue;
            if(auto it = holding.find(static_cast<u32>(flag.carrier)); it != holding.end())
            {
                m_last_seen[team] = it->second;
                continue;
            }
            /* Whoever let go must step away before they reach it again */
            m_released[team] = flag.carrier;
            flag.carrier     = -1;
            flag.object  = 0;
            flag.idle    = 0.f;
        }
    }

    /* A flag on the ground is the flag weapon nearest where it was last
     * seen; while away from its base its idle time runs */
    void lying(Proxy& p, GameState& state, f32 dt)
    {
        std::map<u64, Vecf3> flags;
        for(auto ent : p.template select<Model, NetworkInfo>())
        {
            auto [model, net] = ent.components();
            if(model.origin_object == m_flag && net.instance_id != 0 &&
               !SpawnObjectEvent::is_impostor(net.instance_id) && !p.template get<PlayerInfo>(ent.id()))
                flags[ent.id()] = model.position;
        }
        for(size_t team = 0; team < state.flags.size(); team++)
        {
            auto& flag = state.flags[team];
            if(flag.object && !flags.contains(flag.object))
                flag.object = 0;
            if(flag.carrier < 0 && !flag.object)
            {
                f32 best = adopt_reach;
                for(auto const& [id, at] : flags)
                {
                    bool const other = std::any_of(state.flags.begin(), state.flags.end(), [id](auto const& f) {
                        return f.object == id;
                    });
                    if(f32 const d = glm::distance(at, m_last_seen[team]); !other && d < best)
                    {
                        best        = d;
                        flag.object = id;
                    }
                }
            }
            if(flag.object)
                m_last_seen[team] = flags[flag.object];
            flag.idle = flag.carrier < 0 && flag.taken ? flag.idle + dt : 0.f;
        }
    }

    /* The rules hear of a player reaching a flag on the ground once, as
     * they come within reach of it */
    void touches(Proxy& p, GameState const& state)
    {
        std::set<std::pair<size_t, u32>> now;
        for(auto ent : p.template select<PlayerInfo, PlayerCamera, NetworkInfo>())
        {
            auto [info, cam, net] = ent.components();
            auto const* status    = p.template get<BipedStatus>(ent.id());
            if((status && status->dead) || !info.mode.physics || !biped_in_play(info, cam, net))
                continue;
            for(size_t team = 0; team < state.flags.size(); team++)
            {
                auto const* flag = p.template get<Model>(state.flags[team].object);
                if(!flag)
                    continue;
                bool const reach = within_pickup_reach(info, cam, flag->position, state.flag_reach);
                if(m_released[team] == static_cast<i32>(info.player_idx))
                {
                    if(!reach)
                        m_released[team] = -1;
                    continue;
                }
                if(!reach)
                    continue;
                now.insert({team, info.player_idx});
                if(m_touching.contains({team, info.player_idx}))
                    continue;
                GameEvent      ev{.type = GameEvent::FlagTouch};
                FlagTouchEvent touch{.team = static_cast<u16>(team), .player = info.player_idx};
                p.template subsystem<GameEventBus>().inject(ev, &touch);
            }
        }
        m_touching = std::move(now);
    }

    /* As the decomp places them: a flag is marked for the team out to take
     * it, wherever it is, and its carrier is shown the way to their base */
    void markers(GameState& state)
    {
        auto navs = state.nav_points;
        std::erase_if(navs, [](auto const& nav) { return nav.flag.starts_with(marker_prefix); });
        for(size_t team = 0; team < state.flags.size(); team++)
        {
            auto const& flag = state.flags[team];
            if(!flag.placed)
                continue;
            GameState::nav_point_t mark{
                .arrow    = team == 0 ? "flag_red" : "flag_blue",
                .flag     = std::string(marker_prefix) + std::to_string(team),
                .position = flag.base,
                .offset   = marker_lift,
                .team     = static_cast<i16>(blam::scn::unit::team_t::player),
                .mp_team  = static_cast<i16>(1 - team),
            };
            if(flag.carrier >= 0)
                mark.unit = flag.carrier;
            else if(flag.object)
                mark.object = flag.object;
            navs.push_back(mark);
            if(flag.carrier >= 0 && 1 - team < state.flags.size())
                navs.push_back({
                    .arrow    = "default",
                    .flag     = std::string(marker_prefix) + "base",
                    .position = state.flags[1 - team].base,
                    .offset   = marker_lift,
                    .player   = flag.carrier,
                });
        }
        if(navs != state.nav_points)
            state.nav_points = std::move(navs);
    }

    void give(Proxy& p, GameState& state, size_t team, u32 player)
    {
        auto& flag = state.flags[team];
        u64   body = 0;
        for(auto ent : p.template select<PlayerInfo>())
            if(ent.template get<PlayerInfo>().player_idx == player)
                body = ent.id();
        if(!body || flag.carrier >= 0)
            return;
        auto& game = p.template subsystem<GameEventBus>();
        despawn(p, flag.object);
        EquipWeaponEvent equip{.entity_id = body, .weapon = m_flag->tag_id, .mode = EquipWeaponEvent::pick_up};
        GameEvent        ev{.type = GameEvent::EquipWeapon};
        game.inject(ev, &equip);
        flag.carrier     = static_cast<i32>(player);
        flag.object      = 0;
        flag.taken       = true;
        m_released[team] = -1;
        flag.idle    = 0.f;
    }

    void reset(Proxy& p, GameState& state, size_t team)
    {
        auto& flag = state.flags[team];
        if(flag.carrier >= 0)
            for(auto ent : p.template select<PlayerInfo>())
                if(ent.template get<PlayerInfo>().player_idx == static_cast<u32>(flag.carrier))
                {
                    EquipWeaponEvent equip{
                        .entity_id = ent.id(), .weapon = m_flag->tag_id, .mode = EquipWeaponEvent::remove};
                    GameEvent ev{.type = GameEvent::EquipWeapon};
                    p.template subsystem<GameEventBus>().inject(ev, &equip);
                }
        despawn(p, flag.object);
        flag.carrier     = -1;
        flag.object      = 0;
        flag.taken       = false;
        m_released[team] = -1;
        flag.idle    = 0.f;
        spawn(p, state, team);
    }

    void spawn(Proxy& p, GameState const& state, size_t team)
    {
        m_spawned[team]   = true;
        m_last_seen[team] = state.flags[team].base;
        SpawnObjectEvent spawn{
            .object       = m_flag->as_ref(),
            .position     = state.flags[team].base + Vecf3{0.f, 0.f, spawn_lift},
            .server_owned = true,
        };
        GameEvent ev{.type = GameEvent::SpawnObject};
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
    }

    void despawn(Proxy& p, u64 object)
    {
        auto const* net = object ? p.template get<NetworkInfo>(object) : nullptr;
        if(!net || net->instance_id == 0)
            return;
        DespawnObjectEvent gone{.net_id = net->instance_id};
        GameEvent          ev{.type = GameEvent::DespawnObject};
        p.template subsystem<GameEventBus>().inject(ev, &gone);
    }

    static constexpr f32  adopt_reach   = 4.f;  /* wu from where a flag was last seen */
    static constexpr f32  marker_lift   = .6f;  /* wu above what a marker is over */
    static constexpr auto marker_prefix = std::string_view("ctf:");
    static constexpr f32 spawn_lift  = .2f;  /* wu above its base */

    std::unique_ptr<TickQueue>                                  m_ticks;
    std::shared_ptr<GameEventBus::queue_type<FlagCommandEvent>> m_command_queue;
    std::vector<FlagCommandEvent>                               m_commands;
    u32                                                         m_load_generation{0};
    blam::tag_t const*                                          m_flag{nullptr};
    std::array<bool, 2>                                         m_spawned{};
    std::array<Vecf3, 2>                                        m_last_seen{};
    std::set<std::pair<size_t, u32>>                            m_touching;
    std::array<i32, 2>                                          m_released{-1, -1}; /*!< Let go of it, still in reach */
};

void alloc_ctf_flags(compo::EntityContainer& e)
{
    auto& flags   = e.register_subsystem_inplace<FlagSystem>();
    auto& game    = e.subsystem_cast<GameEventBus>();
    flags.m_ticks = std::make_unique<TickQueue>(game);
    flags.m_command_queue = game.addQueuedEventFunction<FlagCommandEvent>(
        0, [&flags](GameEvent&, FlagCommandEvent* command) { flags.m_commands.push_back(*command); });
}
