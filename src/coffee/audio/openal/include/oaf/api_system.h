#pragma once

#include "api.h"
#include <coffee/comp_app/services.h>

namespace oaf {

struct system
    : api
    , compo::SubsystemBase
{
    using type = system;

    std::optional<std::string> load(
        compo::EntityContainer& e, DeviceHandle&& device = {});

    void start_frame(compo::ContainerProxy& p, const compo::time_point&);
    void collect_info(comp_app::interfaces::AppInfo& appInfo);
};

} // namespace oaf
