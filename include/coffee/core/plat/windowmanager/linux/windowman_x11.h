#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_X11

#include "../windowman.h"
#include "linux_wm_x11.h"

namespace Coffee{
namespace WindowManager{
namespace Linux{

struct X11WindowManImpl : WindowControlDef
{
    STATICINLINE void SetDecorated(Window* w, bool e)
    {
        CoffeePlat::X11::set_decorations(w->wininfo.x11.display,
                                         w->wininfo.x11.window,
                                         e);
    }
    STATICINLINE void SetAlwaysTop(Window* w, bool e)
    {
        CoffeePlat::X11::set_alwaysontop(w->wininfo.x11.display,
                                         w->wininfo.x11.window,
                                         e);
    }
};

}
}

using WM = WindowManager::Linux::X11WindowManImpl;

}
#endif
