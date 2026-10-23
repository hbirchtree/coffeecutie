#include "game_mode.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "selected_version.h"
#include "ticks.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>

using ObjectRespawnManifest = compo::SubsystemManifest<
    type_list_t<const Model, const NetworkInfo, const PlayerInfo>,
    type_list_t<
        const GameState,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    empty_list_t>;

/* In multiplayer, what the map places comes back: an item some time after it
 * is taken, by the map's timers, and a vehicle left unused away from its
 * spot, after as long as the game type says */
struct ObjectRespawn : compo::RestrictedSubsystem<ObjectRespawn, ObjectRespawnManifest>
{
    using type  = ObjectRespawn;
    using Proxy = compo::proxy_of<ObjectRespawnManifest>;

    ObjectRespawn()
    {
        this->priority = 908;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& state = p.template subsystem<GameState>();
        if(m_load_generation != files.load_generation)
        {
            m_load_generation = files.load_generation;
            m_slots.clear();
        }
        if(state.kind != GameState::kind_t::multiplayer || dt <= 0.f)
            return;

        std::map<u32, Model const*> live;
        for(auto object : p.template select<Model, NetworkInfo>())
            if(u32 const id = object.template get<NetworkInfo>().instance_id)
                live[id] = &object.template get<Model>();
        std::set<u32> ridden;
        for(auto player : p.template select<PlayerInfo>())
            if(u64 const vehicle = player.template get<PlayerInfo>().riding.vehicle)
                if(auto const* net = p.template get<NetworkInfo>(vehicle))
                    ridden.insert(net->instance_id);

        track(live, files);
        for(auto& [_, slot] : m_slots)
        {
            if(slot.pending)
                adopt(slot, live, dt);
            else if(slot.vehicle)
                tend_vehicle(p, slot, live, ridden.contains(slot.net_id), state.vehicle_respawn, dt);
            else
                tend_item(p, slot, live.contains(slot.net_id), dt);
        }
    }

    struct slot_t
    {
        blam::tag_t const* tag{nullptr};
        Vecf3              home{};
        Quatf              facing{1.f, 0.f, 0.f, 0.f};
        f32                respawn{0.f}; /*!< An item's, s */
        bool               vehicle{false};
        u32                net_id{0};
        bool               pending{false}; /*!< Its replacement is on the way */
        f32                waited{0.f};
    };

    /* What the map placed, as it first appears */
    void track(std::map<u32, Model const*> const& live, BlamFiles<halo_version> const& files)
    {
        for(auto const& [id, model] : live)
        {
            auto const group = SpawnObjectEvent::scenario_group(id);
            if((group != ScenarioGroup::NetgameEquipment && group != ScenarioGroup::Vehicle) ||
               m_slots.contains(id) || !model->origin_object)
                continue;
            bool const vehicle = group == ScenarioGroup::Vehicle;
            m_slots[id]        = {
                       .tag     = model->origin_object,
                       .home    = model->position,
                       .facing  = model->rotation,
                       .respawn = vehicle ? 0.f : item_time(files, (id >> 8) & 0xFFFF),
                       .vehicle = vehicle,
                       .net_id  = id,
            };
        }
    }

    /* The entry's own time, else its item collection's */
    f32 item_time(BlamFiles<halo_version> const& files, u32 entry)
    {
        auto const& magic    = files.container.magic;
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return default_item_time;
        auto equipment = scenario->netgame.equipment.data(magic);
        if(!equipment.has_value() || entry >= equipment.value().size())
            return default_item_time;
        auto const& netgame = equipment.value()[entry];
        if(netgame.spawn_time > 0)
            return static_cast<f32>(netgame.spawn_time);
        blam::tag_index_view<halo_version> index(files.container);
        auto collection = index.template data<blam::scn::item_collection>(netgame.item);
        if(collection.has_value() && collection.value()->spawn_time > 0)
            return static_cast<f32>(collection.value()->spawn_time);
        return default_item_time;
    }

    void tend_item(Proxy& p, slot_t& slot, bool present, f32 dt)
    {
        if(present && slot.net_id != 0)
        {
            slot.waited = 0.f;
            return;
        }
        if((slot.waited += dt) >= slot.respawn)
            replace(p, slot);
    }

    /* Ridden or home, it is in use; gone, it comes back at once */
    void tend_vehicle(
        Proxy&                              p,
        slot_t&                             slot,
        std::map<u32, Model const*> const&  live,
        bool                                ridden,
        f32                                 respawn,
        f32                                 dt)
    {
        auto const it = live.find(slot.net_id);
        if(it == live.end())
            return replace(p, slot);
        bool const home = glm::distance(it->second->position, slot.home) < home_reach &&
                          std::abs(glm::dot(it->second->rotation, slot.facing)) > home_turn;
        if(ridden || home || respawn < 0.f)
        {
            slot.waited = 0.f;
            return;
        }
        if((slot.waited += dt) < respawn)
            return;
        DespawnObjectEvent gone{.net_id = slot.net_id};
        GameEvent          ev{.type = GameEvent::DespawnObject};
        p.template subsystem<GameEventBus>().inject(ev, &gone);
        replace(p, slot);
    }

    void replace(Proxy& p, slot_t& slot)
    {
        SpawnObjectEvent spawn{
            .object       = slot.tag->as_ref(),
            .position     = slot.home,
            .rotation     = slot.facing,
            .server_owned = true,
        };
        GameEvent ev{.type = GameEvent::SpawnObject};
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
        slot.net_id  = 0;
        slot.pending = true;
        slot.waited  = 0.f;
    }

    /* The replacement, once it stands at the slot's spot; not seen there for
     * a while, it was taken as it came */
    void adopt(slot_t& slot, std::map<u32, Model const*> const& live, f32 dt)
    {
        if((slot.waited += dt) > adopt_grace)
        {
            slot.pending = false;
            slot.waited  = 0.f;
            return;
        }
        for(auto const& [id, model] : live)
            if(model->origin_object == slot.tag &&
               SpawnObjectEvent::scenario_group(id) == ScenarioGroup::None &&
               glm::distance(model->position, slot.home) < (slot.vehicle ? vehicle_reach : home_reach) &&
               std::none_of(m_slots.begin(), m_slots.end(), [id](auto const& s) {
                   return s.second.net_id == id;
               }))
            {
                slot.net_id  = id;
                slot.pending = false;
                slot.waited  = 0.f;
                return;
            }
    }

    static constexpr f32 default_item_time = 30.f; /* s */
    static constexpr f32 home_reach        = .5f;  /* wu */
    static constexpr f32 home_turn         = .995f; /* |cos| of half the turn */
    static constexpr f32 adopt_grace       = 2.f;   /* s */
    static constexpr f32 vehicle_reach     = 2.f;   /* wu, a fresh vehicle settles */

    std::unique_ptr<TickQueue> m_ticks;
    u32                        m_load_generation{0};
    std::map<u32, slot_t>      m_slots; /*!< By the map's own net id */
};

void alloc_object_respawn(compo::EntityContainer& e)
{
    auto& respawn   = e.register_subsystem_inplace<ObjectRespawn>();
    respawn.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
}
