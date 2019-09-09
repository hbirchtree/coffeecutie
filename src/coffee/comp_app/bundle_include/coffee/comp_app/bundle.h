#pragma once

#include <coffee/comp_app/subsystems.h>

namespace comp_app {

extern void configureDefaults(AppLoader& loader);

extern void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec);
}
