#include "map_lifecycle.h"

#include "../physics.h"
#include "../sounds.h"

#include <coffee/core/task_queue/task.h>
#include <platforms/environment.h>

#if defined(BLAM_DIMETER)
#include <blam/dimeter/file_header.h>
#endif
#if defined(BLAM_ELEGY)
#include <blam/elegy/file_header.h>
#endif

namespace world {

void clear_objects(compo::EntityContainer& container)
{
    container.remove_entity_if([](compo::Entity const& en) { return stl_types::any_flag_of(en.tags, ObjectGC); });
    Physics::Event reset_ev{Physics::Event::Reset};
    Physics::Reset reset{};
    container.subsystem_cast<PhysicsBus>().process(reset_ev, &reset);
}

void clear_world(compo::EntityContainer& container)
{
    clear_objects(container);
    if(auto* sound_bus = container.service<comp_app::EventBus<SoundEvent>>())
    {
        SoundEvent ev{.type = SoundEvent::clear_all};
        sound_bus->inject(ev, nullptr);
    }
}

bool side_pread(char const* env)
{
    if(compile_info::platform::is_emscripten)
        return true;
    auto v = platform::env::var(env);
    return v.has_value() && v.value() == "1";
}

MapGameEvent::game_t detect_game(semantic::BytesConst const& head)
{
#if defined(BLAM_ELEGY)
    namespace el = blam::elegy;
    if(head.data && head.size >= sizeof(el::file_header_t))
    {
        auto const* header = reinterpret_cast<el::file_header_t const*>(head.data);
        if(header->valid() && header->version.value() == el::halo3_version)
            return MapGameEvent::halo3;
    }
#endif
#if defined(BLAM_DIMETER)
    /* Xbox and Vista headers share the id/version/footer positions */
    namespace dim = blam::dimeter;
    if(head.data && head.size >= sizeof(dim::file_header_xbox_t) &&
       reinterpret_cast<dim::file_header_xbox_t const*>(head.data)->valid())
        return MapGameEvent::halo2;
#endif
    return MapGameEvent::halo1;
}

void dispatch_map_loads(compo::EntityContainer& container)
{
    container.subsystem_cast<GameEventBus>().addEventFunction<MapLoadEvent>(
        0, [&container](GameEvent& ev, MapLoadEvent* load) {
            if(!load->file || load->game)
                return;
            ev.type    = GameEvent::None;
            auto& mapper = container.subsystem_cast<comp_app::FileMapper>();
            rq::runtime_queue::Queue(
                rq::dependent_task<std::vector<char>, void>::CreateSink(
                    mapper.read_head(*load->file, 0x3000),
                    [&container, load = *load](std::vector<char>* head) mutable {
                        load.game = detect_game(semantic::mem_chunk<const char>::ofContainer(*head));
                        GameEvent ev{GameEvent::MapLoadStart};
                        container.subsystem_cast<GameEventBus>().process(ev, &load);
                    }))
                .assume_value();
        });
}

platform::url::Url sibling_url(MapLoadEvent const& load, std::string_view name)
{
    return (load.file->path().dirname() / std::string(name)).url(load.file->flags);
}

} // namespace world
