#include "game_mode.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "selected_version.h"
#include "spawn_points.h"
#include "ticks.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>

using CampaignManifest = compo::SubsystemManifest<
    type_list_t<const PlayerInfo, const PlayerCamera, const NetworkInfo, const BipedStatus>,
    type_list_t<
        GameState,
        GameSetup,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* Campaign is the level's scripts and checkpoints. The fallen come back
 * beside a partner once things are quiet; with nobody left standing, the
 * world goes back to the last checkpoint. Checkpoints wait for quiet too */
struct CampaignController : compo::RestrictedSubsystem<CampaignController, CampaignManifest>
{
    using type  = CampaignController;
    using Proxy = compo::proxy_of<CampaignManifest>;

    CampaignController()
    {
        this->priority = 906;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_kill_queue->poll();
        m_won_queue->poll();
        m_checkpoint_queue->poll();
        auto       kills = std::exchange(m_kills, {});
        bool const won   = std::exchange(m_won, false);
        auto const save  = std::exchange(m_asked, {});
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       state = p.template subsystem<GameState>();
        if(m_load_generation != files.load_generation)
            begin(files);
        if(state.kind != GameState::kind_t::campaign)
            return;

        for(auto const& kill : kills)
            if(kill.player != ObjectHitEvent::no_player)
                m_waiting[kill.unit] = respawn_delay;
        listen(p, state, dt);
        /* A level starts with a checkpoint, once someone is in it */
        if(!m_started && !state.players.empty())
        {
            m_started = true;
            checkpoint(p, state);
        }
        if(save)
            m_save_wait = sooner(m_save_wait, *save);
        saving(p, state, dt);
        if(won)
            next_level(p, files);
        respawn(p, state, dt);
    }

    void begin(BlamFiles<halo_version> const& files)
    {
        m_load_generation = files.load_generation;
        m_waiting.clear();
        m_health.clear();
        m_calm      = quiet_time;
        m_started   = false;
        m_save_wait = std::nullopt;
    }

    /* ---- Quiet ---- */

    /* Quiet is a while with no player hurt.
     * TODO: Nearby hostiles keep it loud too, once there is AI */
    void listen(Proxy& p, GameState& state, f32 dt)
    {
        m_calm += dt;
        m_anyone_dead = false;
        m_anyone_alive = false;
        std::map<u64, f32> health;
        for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedStatus>())
        {
            auto [info, cam, net, status] = player.components();
            if(!biped_shown(info, cam, net))
                continue;
            m_anyone_dead  = m_anyone_dead || status.dead;
            m_anyone_alive = m_anyone_alive || !status.dead;
            f32 const now  = status.body + status.shield;
            if(auto it = m_health.find(player.id()); it != m_health.end() && now < it->second)
                m_calm = 0.f;
            health[player.id()] = now;
        }
        m_health    = std::move(health);
        state.quiet = m_calm >= quiet_time;
    }

    /* ---- Checkpoints ---- */

    /* A checkpoint asked for is taken once quiet with everyone standing,
     * unless it runs out of time first */
    void saving(Proxy& p, GameState& state, f32 dt)
    {
        if(m_save_wait)
        {
            bool const now = *m_save_wait <= 0.f || (state.quiet && !m_anyone_dead);
            if(now)
                checkpoint(p, state);
            if(now || (*m_save_wait -= dt) <= 0.f)
                m_save_wait = std::nullopt;
        }
        state.saving = m_save_wait.has_value();
    }

    /* 0 saves now, whoever asked; otherwise the longest wait */
    static f32 sooner(std::optional<f32> waiting, f32 timeout)
    {
        if(!waiting)
            return timeout;
        return *waiting <= 0.f || timeout <= 0.f ? 0.f : std::max(*waiting, timeout);
    }

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

    /* ---- Deaths ---- */

    void respawn(Proxy& p, GameState& state, f32 dt)
    {
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
            if(!m_anyone_alive)
            {
                GameEvent         ev{.type = GameEvent::WorldRestore};
                WorldRestoreEvent restore{};
                p.template subsystem<GameEventBus>().inject(ev, &restore);
                state.reverts++;
                m_waiting.clear();
                m_health.clear();
                m_save_wait = std::nullopt;
                break;
            }
            auto const spot = state.quiet ? partner_spot(p, *info) : spawn_points::spot_t{};
            if(!spot)
            {
                ++it;
                continue;
            }
            GameEvent        ev{.type = GameEvent::RespawnUnit};
            RespawnUnitEvent respawn{
                .unit     = it->first,
                .position = spot->first,
                .facing   = spot->second,
                .arrive   = true,
            };
            p.template subsystem<GameEventBus>().inject(ev, &respawn);
            it = m_waiting.erase(it);
        }
    }

    /* Beside a partner on foot */
    spawn_points::spot_t partner_spot(Proxy& p, PlayerInfo const& info)
    {
        for(auto other : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedStatus>())
        {
            auto [oinfo, cam, net, status] = other.components();
            if(status.dead || oinfo.player_idx == info.player_idx || !biped_in_play(oinfo, cam, net))
                continue;
            return std::pair{
                cam.camera.position - Vecf3{0.f, 0.f, oinfo.biped.eye_height} +
                    Vecf3{partner_offset, 0.f, 0.f},
                spawn_points::facing_of(cam)};
        }
        return std::nullopt;
    }

    static constexpr f32 respawn_delay  = 3.f; /* s */
    static constexpr f32 quiet_time     = 5.f; /* s without a player hurt */
    static constexpr f32 partner_offset = 1.f; /* wu, clear of their body */

    std::unique_ptr<TickQueue>                                 m_ticks;
    std::shared_ptr<GameEventBus::queue_type<UnitKilledEvent>> m_kill_queue;
    std::shared_ptr<GameEventBus::queue_type<GameWonEvent>>    m_won_queue;
    std::shared_ptr<GameEventBus::queue_type<CheckpointEvent>> m_checkpoint_queue;
    std::vector<UnitKilledEvent>                               m_kills;
    bool                                                       m_won{false};
    std::optional<f32>                                         m_asked; /*!< Longest timeout asked */

    u32                m_load_generation{0};
    std::map<u64, f32> m_waiting; /*!< Seconds left */
    std::map<u64, f32> m_health;  /*!< Body and shield last frame */
    f32                m_calm{quiet_time};
    bool               m_anyone_dead{false};
    bool               m_anyone_alive{false};
    bool               m_started{false};
    std::optional<f32> m_save_wait; /*!< Seconds left to find a quiet moment */
};

void alloc_campaign_controller(compo::EntityContainer& e)
{
    auto& mode   = e.register_subsystem_inplace<CampaignController>();
    auto& game   = e.subsystem_cast<GameEventBus>();
    mode.m_ticks = std::make_unique<TickQueue>(game);
    mode.m_kill_queue = game.addQueuedEventFunction<UnitKilledEvent>(
        0, [&mode](GameEvent&, UnitKilledEvent* kill) { mode.m_kills.push_back(*kill); });
    mode.m_won_queue = game.addQueuedEventFunction<GameWonEvent>(
        0, [&mode](GameEvent&, GameWonEvent*) { mode.m_won = true; });
    mode.m_checkpoint_queue = game.addQueuedEventFunction<CheckpointEvent>(
        0, [&mode](GameEvent&, CheckpointEvent* save) {
            mode.m_asked = CampaignController::sooner(mode.m_asked, save->timeout);
        });
}
