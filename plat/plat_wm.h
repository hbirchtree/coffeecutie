#ifndef COFFEE_PLAT_WM
#define COFFEE_PLAT_WM

#include "coffee_types.h"

#if defined(__linux__)
#include "linux_wm.h"
//#elif defined(_WIN32)
//#include "win_wm.h"
#elif defined(__APPLE__) && defined(__MACH__)

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
