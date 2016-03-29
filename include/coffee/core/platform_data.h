#pragma once

#include "coffee_mem_macros.h"
#include "plat/platform_detect.h"

namespace Coffee{
struct PlatformData
{
    STATICINLINE bool IsMobile()
    {
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_IOS)
        return false;
#else
        return true;
#endif
    }

    STATICINLINE bool IsGLES()
    {
#ifdef COFFEE_GLEAM_DESKTOP
        return false;
#else
        return true;
#endif
    }

    STATICINLINE bool UseVirtualFS()
    {
        return false;
    }
};
}