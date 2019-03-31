#pragma once

#include <peripherals/base.h>
#include <peripherals/libc/types.h>

#if defined(COFFEE_X11) || defined(COFFEE_USE_MAEMO_X11)
#include <X11/Xlib.h>
#elif defined(COFFEE_RASPBERRY_DMX)
#include <EGL/egl.h>
#include <bcm_host.h>
#elif defined(COFFEE_WINDOWS_UWP)
#include <Inspectable.h>
#endif

namespace Coffee {
namespace Display {

struct Window
{
    union
    {
#if defined(COFFEE_X11) || defined(COFFEE_USE_MAEMO_X11)
        struct
        {
            ::Window   window;
            ::Display* display;
        } x11;
#elif defined(COFFEE_RASPBERRY_DMX)
        struct
        {
            ::EGL_DISPMANX_WINDOW_T window;
        } dmx;
#elif defined(COFFEE_WINDOWS_UWP)
        struct
        {
            IInspectable* window;
        } winrt;
#elif defined(COFFEE_WINDOWS)
        struct
        {
            ::HWND window;
            ::HDC  hdc;
        } win32;
#elif defined(COFFEE_APPLE)

#endif

    } wininfo;
};

} // namespace Display
} // namespace Coffee
