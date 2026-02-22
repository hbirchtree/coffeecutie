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

    static DeviceHandle dummy()
    {
        return DeviceHandle{
            .dummy = DummyInfo{
                .fmt = Format{
                    .frequency = 48000,
                    .channels = 2,
                    .bits = 16,
                    .format = format_t::f32,
                },
                .speed = 1.f,
            },
        };
    }
};

} // namespace oaf
