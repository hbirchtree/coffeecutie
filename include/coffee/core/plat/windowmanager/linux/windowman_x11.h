#pragma once

#include "../windowman.h"
#include "../plat_windowtype.h"
#include "linux_wm_x11.h"

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

namespace Coffee{
namespace WindowManager{

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

using WM = WindowManager::X11WindowManImpl;

}