#pragma once

#include <coffee/comp_app/subsystems.h>

namespace comp_app {

extern detail::EntityContainer& createContainer();

extern void configureDefaults(AppLoader& loader);

extern void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec);

#if defined(COFFEE_EMSCRIPTEN)
void emscripten_loop();
#endif

struct BundleData
{
#if defined(COFFEE_EMSCRIPTEN)
    static constexpr void(*EmscriptenLoop)() = emscripten_loop;
#endif
};

}
