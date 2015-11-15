#ifndef COFFEE_PLAT_WM
#define COFFEE_PLAT_WM

#include "coffee/core/plat/platform_detect.h"
#include "coffee/core/Types"

#if defined(COFFEE_LINUX)
#include "coffee_linux/linux_wm.h"
#elif defined(COFFEE_WINDOWS)
#include "coffee_windows/win_wm.h"
#elif defined(COFFEE_APPLE)

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
#ifdef _WIN32
        struct {

        } win32;
#endif

    } wininfo;

    cstring             title       = nullptr;
    CRect               screenArea;
};

}
}

#endif //COFFEE_PLAT_WM
