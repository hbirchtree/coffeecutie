#pragma once

#include <coffee/comp_app/subsystems.h>

namespace comp_app {

extern detail::EntityContainer& createContainer();
/*! The container main created, or null: what JS entry points may use, as
 * a page polls them before the runtime is up */
extern detail::EntityContainer* currentContainer();

extern void configureDefaults(AppLoader& loader);

inline auto& configureDefaults(detail::EntityContainer& container)
{
    auto& loader = AppLoader::register_service(container);
    configureDefaults(loader);
    return loader;
}

extern void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec);

inline void addDefaults(
    AppLoadableService::entity_container& container, app_error& ec)
{
    addDefaults(container, *container.service<AppLoader>(), ec);
}

void setup_container(detail::EntityContainer& container);
bool loop_container(detail::EntityContainer& container);
void cleanup_container(detail::EntityContainer& container);

void setup_and_loop_container(detail::EntityContainer& container);

#if defined(COFFEE_EMSCRIPTEN)
void emscripten_loop();
#endif

struct BundleData
{
#if defined(COFFEE_EMSCRIPTEN)
    static constexpr void (*EmscriptenLoop)() = emscripten_loop;
#endif
};

} // namespace comp_app
