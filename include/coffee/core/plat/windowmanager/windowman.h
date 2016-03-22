#ifndef COFFEE_PLAT_WINDOWMANAGER_DEF_H
#define COFFEE_PLAT_WINDOWMANAGER_DEF_H

#include "../../types/cdef/pixtypes.h"
#include "../../types/tdef/integertypes.h"

namespace Coffee{
namespace CDisplay{

struct CDWindow;

}
namespace WindowManager{

using Window = CDisplay::CDWindow;

struct WindowControlDef
{
    static void SetDecorated(Window*,bool)
    {
    }
    static void SetAlwaysTop(Window*,bool)
    {
    }

    static void SetTransparency(Window*,CRGBA)
    {
    }
    static void SetIcon(Window*,CBitmap const&)
    {
    }

    static void Ping(Window*)
    {
    }
    static void Message(Window*,cstring)
    {
    }
};

}
}

#endif

#include "android/windowman.h"
#include "linux/windowman.h"
#include "windows/windowman.h"