#pragma once

#include <coffee/comp_app/services.h>

namespace glscreenshot {

struct ScreenshotProvider : comp_app::interfaces::ScreenshotProvider,
                            comp_app::AppService<ScreenshotProvider>
{
    using services = comp_app::subsystem_list<
        comp_app::interfaces::ScreenshotProvider,
        ScreenshotProvider>;

    comp_app::size_2d_t                        size() const;
    semantic::mem_chunk<typing::pixels::rgb_t> pixels() const;
};

} // namespace glscreenshot
