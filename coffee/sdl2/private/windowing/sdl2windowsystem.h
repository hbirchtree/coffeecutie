#ifndef COFFEE_CORE_CONTEXT_SDL2_WINDOWSYSTEM_H
#define COFFEE_CORE_CONTEXT_SDL2_WINDOWSYSTEM_H

#include <coffee/core/CDebug>
#include <coffee/core/plat/windowmanager/plat_windowtype.h>
#include "../types/sdl2datatypes.h"

namespace Coffee{
namespace SDL2{

void GetWindowPtr(SDL_Window *window, CDWindow *win)
{
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);

    if(!SDL_GetWindowWMInfo(window,&info))
    {
        cDebug("Failed to acquire information on window: %s",SDL_GetError());
        return;
    }
    switch(info.subsystem){
#if !defined(COFFEE_RASPBERRYPI) && (defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11))
    case SDL_SYSWM_X11:
        win->wininfo.x11.window = info.info.x11.window;
        win->wininfo.x11.display = info.info.x11.display;
        break;
#elif defined(COFFEE_USE_WINDOWS_ANGLE)
	case SDL_SYSWM_WINDOWS:
		win->wininfo.win32.window = info.info.win.window;
		win->wininfo.win32.hdc = info.info.win.hdc;
		break;
#endif
    default:
        break;
    }
}

}
}

#endif
