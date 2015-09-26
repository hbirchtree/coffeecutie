#ifndef LINUX_WM_H
#define LINUX_WM_H

#define __COFFEE_X11
#define __COFFEE_WAYLAND

#include <stdlib.h> //for getenv()
#include <X11/Xlib.h>
#include "X11/X.h"
#include <X11/Xatom.h>

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

namespace CoffeePlat{

namespace X11{

struct X11Window
{
    Window handle;
    Display* display;
};

typedef struct {
    unsigned long   flags;
    unsigned long   functions;
    unsigned long   decorations;
    long            inputMode;
    unsigned long   status;
} Hints;

static void set_decorations(Display* disp, Window target,bool enable = false)
{
    Hints   w_hints;
    Atom    prop;

    w_hints.flags = 2;
    w_hints.decorations = enable;

    prop = XInternAtom(disp,"_MOTIF_WM_HINTS",True);
    XChangeProperty(disp,target,prop,prop,32,PropModeReplace,(unsigned char*)&w_hints,5);
}

static void set_alwaysontop(Display* disp, Window target, bool enable)
{
    XEvent ev;

    ev.xclient.type = ClientMessage;
    ev.xclient.serial = 0;
    ev.xclient.send_event = True;
    ev.xclient.display = disp;
    ev.xclient.window = target;
    ev.xclient.message_type = XInternAtom(disp,"_NET_WM_STATE",False);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = _NET_WM_STATE_ADD;
    ev.xclient.data.l[1] = XInternAtom(disp,"_NET_WM_STATE_ABOVE",False);
    ev.xclient.data.l[2] = 0;
    ev.xclient.data.l[3] = 0;
    ev.xclient.data.l[4] = 0;

    Status exit = XSendEvent(disp,
               DefaultRootWindow(disp),
               False,
               SubstructureRedirectMask|SubstructureNotifyMask,
               &ev);
}
} //X11

namespace Wayland{

} //Wayland

}

#endif // LINUX_WM_H

