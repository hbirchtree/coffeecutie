#include "checkpoint.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "selected_version.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>

using CheckpointManifest = compo::SubsystemManifest<
    type_list_t<
        Model,
        ObjectPhysics,
        BipedStatus,
        BipedEquipment,
        const NetworkInfo,
        const PlayerInfo,
        const PlayerCamera>,
    type_list_t<
        GameEventBus,
        BSPCache<halo_version>,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* The world as a checkpoint left it: which networked objects there were,
 * where, how fast and how hurt, where each player stood with what in hand,
 * and the structure section. Going back despawns what came since, spawns
 * again what went, and puts the rest back; replicas follow through the
 * usual spawns, despawns, object syncs and vitality */
struct CheckpointSystem : compo::RestrictedSubsystem<CheckpointSystem, CheckpointManifest>
{
    using type  = CheckpointSystem;
    using Proxy = compo::proxy_of<CheckpointManifest>;

    CheckpointSystem()
    {
        this->priority = 906;
    }

    struct object_t
    {
        blam::tagref_t object{};
        Vecf3          position{};
        Quatf          rotation{1.f, 0.f, 0.f, 0.f};
        Vecf3          linear_velocity{};
        Vecf3          angular_velocity{};
        std::optional<BipedStatus> status;
    };

    struct player_t
    {
        Vecf3       feet{};
        f32         facing{0.f};
        BipedStatus status;
        std::optional<BipedEquipment> equipment; /*!< Our own players' only */
    };

    struct snapshot_t
    {
        std::map<u32, object_t> objects; /*!< By net id */
        std::map<u32, player_t> players; /*!< By player index */
        i16                     section{-1};
        u32                     load_generation{0};
    };

    void start_restricted(Proxy& p, time_point const&)
    {
        m_save_queue->poll();
        m_restore_queue->poll();
        bool const save    = std::exchange(m_save, false);
        bool const restore = std::exchange(m_restore, false);
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const generation = p.template subsystem<BlamFiles<halo_version>>().load_generation;
        if(m_snapshot && m_snapshot->load_generation != generation)
        {
            m_snapshot.reset();
            m_pending.clear();
        }
        if(save)
            take(p, generation);
        if(restore && m_snapshot)
            put_back(p);
        settle(p);
    }

    /* ---- Taking ---- */

    void take(Proxy& p, u32 generation)
    {
        snapshot_t out{
            .section         = p.template subsystem<BSPCache<halo_version>>().active_section,
            .load_generation = generation,
        };
        for(auto entity : p.template select<Model, NetworkInfo>())
        {
            auto [model, net] = entity.components();
            if(net.instance_id == 0 || p.template get<PlayerInfo>(entity.id()))
                continue;
            object_t object{
                .object   = net.object,
                .position = model.position,
                .rotation = model.rotation,
            };
            if(auto const* physics = p.template get<ObjectPhysics>(entity.id()))
            {
                object.linear_velocity  = physics->linear_velocity;
                object.angular_velocity = physics->angular_velocity;
            }
            if(auto const* status = p.template get<BipedStatus>(entity.id()))
                object.status = *status;
            out.objects[net.instance_id] = object;
        }
        for(auto entity : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedStatus>())
        {
            auto [info, cam, net, status] = entity.components();
            if(!biped_shown(info, cam, net))
                continue;
            Vecf3 const forward = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            player_t player{
                .feet   = cam.camera.position - Vecf3{0.f, 0.f, info.biped.eye_height},
                .facing = std::atan2(forward.y, forward.x),
                .status = status,
            };
            /* Clients keep their own loadout */
            if(auto const* equip = p.template get<BipedEquipment>(entity.id());
               equip && !info.is_remote())
                player.equipment = *equip;
            out.players[info.player_idx] = std::move(player);
        }
        m_snapshot = std::move(out);
    }

    /* ---- Going back ---- */

    void put_back(Proxy& p)
    {
        auto const& snapshot = *m_snapshot;
        auto&       game     = p.template subsystem<GameEventBus>();
        p.template subsystem<BSPCache<halo_version>>().active_section = snapshot.section;

        std::map<u32, u64> live;
        for(auto entity : p.template select<Model, NetworkInfo>())
            if(auto const id = entity.template get<NetworkInfo>().instance_id;
               id != 0 && !p.template get<PlayerInfo>(entity.id()))
                live[id] = entity.id();

        /* What came since goes; what went comes back */
        for(auto const& [net_id, _] : live)
            if(!snapshot.objects.contains(net_id))
            {
                GameEvent          ev{.type = GameEvent::DespawnObject};
                DespawnObjectEvent despawn{.net_id = net_id};
                game.inject(ev, &despawn);
            }
        m_pending.clear();
        for(auto const& [net_id, object] : snapshot.objects)
        {
            if(auto it = live.find(net_id); it != live.end())
            {
                place(p, it->second, object);
                continue;
            }
            GameEvent        ev{.type = GameEvent::SpawnObject};
            SpawnObjectEvent spawn{
                .object       = object.object,
                .position     = object.position,
                .rotation     = object.rotation,
                .net_id       = net_id,
                .server_owned = true,
            };
            game.inject(ev, &spawn);
            m_pending[net_id] = pending_frames;
        }

        for(auto entity : p.template select<PlayerInfo, BipedEquipment>())
        {
            auto [info, equip] = entity.components();
            auto const it = snapshot.players.find(info.player_idx);
            if(it == snapshot.players.end())
                continue;
            auto const& saved = it->second;
            GameEvent        ev{.type = GameEvent::RespawnUnit};
            RespawnUnitEvent respawn{
                .unit     = entity.id(),
                .position = saved.feet,
                .facing   = saved.facing,
                .restore  = true,
                .body     = saved.status.body,
                .shield   = saved.status.shield,
            };
            game.inject(ev, &respawn);
            if(saved.equipment && saved.equipment->load_generation == equip.load_generation)
            {
                u64 const held = equip.weapons[0].held;
                equip          = *saved.equipment;
                /* The hand's model is whatever it holds now */
                equip.weapons[0].held = held;
            }
        }
    }

    /* Objects spawned again take their saved state once they exist */
    void settle(Proxy& p)
    {
        if(m_pending.empty() || !m_snapshot)
            return;
        std::map<u32, u64> live;
        for(auto entity : p.template select<Model, NetworkInfo>())
            if(auto const id = entity.template get<NetworkInfo>().instance_id; m_pending.contains(id))
                live[id] = entity.id();
        for(auto it = m_pending.begin(); it != m_pending.end();)
        {
            if(auto found = live.find(it->first); found != live.end())
            {
                place(p, found->second, m_snapshot->objects.at(it->first));
                it = m_pending.erase(it);
            } else if(--it->second == 0)
                it = m_pending.erase(it);
            else
                ++it;
        }
    }

    void place(Proxy& p, u64 entity, object_t const& object)
    {
        if(auto* model = p.template get<Model>(entity))
        {
            model->position = object.position;
            model->rotation = object.rotation;
            model->update_matrix();
        }
        if(auto* physics = p.template get<ObjectPhysics>(entity))
        {
            physics->linear_velocity  = object.linear_velocity;
            physics->angular_velocity = object.angular_velocity;
            physics->teleported       = true;
        }
        if(auto* status = p.template get<BipedStatus>(entity); status && object.status)
        {
            *status = *object.status;
            GameEvent         ev{.type = GameEvent::UnitVitality};
            UnitVitalityEvent vitality{
                .unit   = entity,
                .body   = status->body,
                .shield = status->shield,
                .dead   = status->dead,
            };
            p.template subsystem<GameEventBus>().inject(ev, &vitality);
        }
    }

    /* How long a respawned object may take to turn up */
    static constexpr u32 pending_frames = 300;

    std::shared_ptr<GameEventBus::queue_type<WorldSaveEvent>>    m_save_queue;
    std::shared_ptr<GameEventBus::queue_type<WorldRestoreEvent>> m_restore_queue;
    bool                       m_save{false};
    bool                       m_restore{false};
    std::optional<snapshot_t>  m_snapshot;
    std::map<u32, u32>         m_pending; /*!< Net id, frames left */
};

void alloc_checkpoints(compo::EntityContainer& e)
{
    auto& checkpoints = e.register_subsystem_inplace<CheckpointSystem>();
    auto& game        = e.subsystem_cast<GameEventBus>();
    checkpoints.m_save_queue = game.addQueuedEventFunction<WorldSaveEvent>(
        0, [&checkpoints](GameEvent&, WorldSaveEvent*) { checkpoints.m_save = true; });
    checkpoints.m_restore_queue = game.addQueuedEventFunction<WorldRestoreEvent>(
        0, [&checkpoints](GameEvent&, WorldRestoreEvent*) { checkpoints.m_restore = true; });
}
