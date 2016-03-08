#pragma once

#include "coffee_mem_macros.h"
#include "plat/platform_detect.h"

namespace Coffee{
struct PlatformData
{
    STATICINLINE bool IsMobile()
    {
#ifndef COFFEE_ANDROID
        return false;
#else
        return true;
#endif
    }
};
}