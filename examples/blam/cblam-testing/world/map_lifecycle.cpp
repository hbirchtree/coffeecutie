#include "map_lifecycle.h"

#include "../physics.h"
#include "../sounds.h"

#include <platforms/environment.h>

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

platform::url::Url sibling_url(MapLoadEvent const& load, std::string_view name)
{
    return (load.file->path().dirname() / std::string(name)).url(load.file->flags);
}

} // namespace world
