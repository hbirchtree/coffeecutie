#pragma once

#include <coffee/comp_app/services.h>

namespace glscreenshot {

struct ScreenshotProvider : comp_app::ScreenshotProvider
{
    comp_app::size_2d_t size() const;
    semantic::Span<typing::pixels::rgb_t> pixels() const;
};

}
