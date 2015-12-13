#ifndef COFFEE_CORE_PLAT_WINDOWTYPE_H
#define COFFEE_CORE_PLAT_WINDOWTYPE_H

#include "coffee/core/CTypes"
#include "coffee/core/plat/platform_detect.h"
#include <X11/Xlib.h>

namespace Coffee{
namespace CDisplay{

struct CDWindow{

    union {
#if defined(COFFEE_LINUX) && defined(COFFEE_X11)
        struct {
            Window window;
            Display* display;
        } x11;
#elif defined(COFFEE_WINDOWS)
        struct {

        } win32;
#elif defined(COFFEE_APPLE)

#endif

    } wininfo;

    cstring             title       = nullptr;
    CRect               screenArea;
};

}
}

#endif
