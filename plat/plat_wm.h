#ifndef COFFEE_PLAT_WM
#define COFFEE_PLAT_WM

#include "coffee_types.h"

#ifdef __linux__
#include "linux_wm.h"
#else

namespace Coffee{
namespace CDisplay{

struct CDNativeWindowInfo
{
};

} //CDisplay
} //Coffee

#endif

namespace Coffee{
namespace CDisplay{

struct CDWindow{

    union {
#ifdef __COFFEE_X11
        struct {
            Window window;
            Display* display;
        } x11;
#endif

    } wininfo;

    cstring             title       = nullptr;
    CRect               screenArea;
};

}
}

#endif
