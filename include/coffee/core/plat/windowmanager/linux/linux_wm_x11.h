#pragma once

#include "coffee/core/plat/platform_detect.h"

#ifdef COFFEE_X11

#include "../../../coffee_mem_macros.h"
#include "../plat_windowtype.h"

namespace CoffeePlat{
namespace X11{

extern void set_decorations(Display* disp, Window target,bool enable = false);
extern void set_alwaysontop(Display* disp, Window target, bool enable);

} //X11

namespace Wayland{

} //Wayland

}

#endif // COFFEE_X11

