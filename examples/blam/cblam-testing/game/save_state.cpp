#include "game/save_state.h"

#include "loading/blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "selected_version.h"

SaveState::u64 SaveState::remap_t::operator()(u64 id) const
{
    if(id == 0)
        return 0;
    if(auto it = saved_keys->find(id); it != saved_keys->end())
    {
        auto found = live->find(it->second);
        return found != live->end() ? found->second : 0;
    }
    /* Not one kept: the same entity, if still here */
    return container->exists(id) ? id : 0;
}

std::map<SaveState::key_t, SaveState::u64> SaveState::live_entities(compo::EntityContainer& e) const
{
    std::map<key_t, u64> out;
    for(auto const& entity : e.select<NetworkInfo>())
        if(auto const net_id = entity.template get<NetworkInfo>().instance_id;
           net_id != 0 && !e.get<PlayerInfo>(entity.id()))
            out[net_id] = entity.id();
    for(auto const& entity : e.select<PlayerInfo>())
        out[player_key(entity.template get<PlayerInfo>().player_idx)] = entity.id();
    return out;
}

void SaveState::take(compo::EntityContainer& e, u32 generation)
{
    m_saved_keys.clear();
    for(auto const& [key, id] : live_entities(e))
        m_saved_keys[id] = key;
    for(auto& binding : m_entities)
        binding->take(e, m_saved_keys);
    for(auto& binding : m_objects)
        binding->take();
    m_pending.clear();
    m_saved      = true;
    m_generation = generation;
    cDebug("save state: kept {} entities", m_saved_keys.size());
}

void SaveState::put(compo::EntityContainer& e, u32 generation)
{
    if(!m_saved || generation != m_generation)
        return;
    for(auto const& reset : m_resets)
        reset(e);
    for(auto& binding : m_objects)
        binding->put();
    m_pending.clear();
    for(size_t i = 0; i < m_entities.size(); i++)
    {
        std::vector<key_t> keys;
        m_entities[i]->keys(keys);
        for(key_t key : keys)
            m_pending[{i, key}] = pending_frames;
    }
    cDebug("save state: putting back {} values", m_pending.size());
    settle(e);
}

void SaveState::settle(compo::EntityContainer& e)
{
    if(m_pending.empty())
        return;
    auto const    live = live_entities(e);
    remap_t const remap{.container = &e, .saved_keys = &m_saved_keys, .live = &live};
    for(auto it = m_pending.begin(); it != m_pending.end();)
    {
        auto const [binding, key] = it->first;
        auto const found          = live.find(key);
        bool const done = found != live.end() && m_entities[binding]->put(e, found->second, key, remap);
        if(!done && it->second == 1)
            cDebug("save state: binding {} gave up on {:#x}", binding, key);
        if(done || --it->second == 0)
            it = m_pending.erase(it);
        else
            ++it;
    }
}

SaveState& alloc_save_state(compo::EntityContainer& e)
{
    auto& state = e.register_subsystem_inplace<SaveState>();
    auto& game  = e.subsystem_cast<GameEventBus>();

    struct requests_t
    {
        bool save{false};
        bool restore{false};

        std::shared_ptr<GameEventBus::queue_type<WorldSaveEvent>>    save_queue;
        std::shared_ptr<GameEventBus::queue_type<WorldRestoreEvent>> restore_queue;
    };
    auto requests        = std::make_shared<requests_t>();
    requests->save_queue = game.addQueuedEventFunction<WorldSaveEvent>(
        0, [requests = requests.get()](GameEvent&, WorldSaveEvent*) { requests->save = true; });
    requests->restore_queue = game.addQueuedEventFunction<WorldRestoreEvent>(
        0, [requests = requests.get()](GameEvent&, WorldRestoreEvent*) { requests->restore = true; });

    e.add_frame_end_callback([&e, &state, requests] {
        requests->save_queue->poll();
        requests->restore_queue->poll();
        bool const save    = std::exchange(requests->save, false);
        bool const restore = std::exchange(requests->restore, false);
        if(e.subsystem_cast<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           e.subsystem_cast<NetworkState>().replica)
            return;
        u32 const generation = e.subsystem_cast<BlamFiles<halo_version>>().load_generation;
        if(save)
            state.take(e, generation);
        if(restore)
            state.put(e, generation);
        state.settle(e);
    });
    return state;
}
