#ifndef COFFEE_CORE_PLAT_WINDOWTYPE_H
#define COFFEE_CORE_PLAT_WINDOWTYPE_H

#include "../../types/tdef/integertypes.h"
#include "../../types/cdef/geometry.h"
#include "../platform_detect.h"

#if defined(COFFEE_X11)
#include <X11/Xlib.h>
#endif

namespace Coffee{
namespace CDisplay{

struct CDWindow{

    union {
#if defined(COFFEE_X11)
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
