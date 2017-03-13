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

    SDL_VERSION(&info.version)

            if(SDL_GetWindowWMInfo(window,&info)){
        switch(info.subsystem){
#if defined(COFFEE_X11)
        case SDL_SYSWM_X11:
            win->wininfo.x11.window = info.info.x11.window;
            win->wininfo.x11.display = info.info.x11.display;
            break;
#elif defined(COFFEE_WINDOWS)
        case SDL_SYSWM_WINDOWS:
            break;
#elif defined(COFFEE_APPLE)
        case SDL_SYSWM_COCOA:
            break;
        case SDL_SYSWM_UIKIT:
            break;
#endif
        default:
            break;
        }
    }else{
        cDebug("Failed to acquire information on window: %s",SDL_GetError());
    }
}

}
}

#endif
