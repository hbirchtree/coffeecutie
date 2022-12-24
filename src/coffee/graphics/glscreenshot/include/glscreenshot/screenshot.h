#pragma once

#include <coffee/comp_app/services.h>

namespace glscreenshot {

struct ScreenshotProvider
    : comp_app::interfaces::ScreenshotProvider,
      comp_app::AppService<ScreenshotProvider, comp_app::ScreenshotProvider>
{
    using type = ScreenshotProvider;

    comp_app::size_2d_t                         size() const;
    semantic::mem_chunk<typing::pixels::rgba_t> pixels() const;
};

} // namespace glscreenshot
