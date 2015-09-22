#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "coffee.h"

namespace Coffee{
namespace CDisplay{
struct CDColorSpace{
    uint8 red     = 0;
    uint8 green   = 0;
    uint8 blue    = 0;
};

struct CDContextBits{
    //Bit depths for a GL context
    uint8 red     = 0;
    uint8 green   = 0;
    uint8 blue    = 0;
    uint8 alpha   = 0;

    uint8 depth   = 0;
    uint8 stencil = 0;
};

struct CDMonitor{
    uint16          index = 0;  //Real monitor index
    CDColorSpace    colorBits;  //Color depth
    CRect           screenArea; //Area occupied on screen
    cstring         name;       //Name of monitor
    CSize           phySize;    //Physical size, reported by some ctxts
    int32           refresh = 0;//Refresh rate of monitor
};
struct CDWindow{
    void*           handle_p    = nullptr; //Native handle, in X11, the Window value
    ptr_u           handle      = 0;
    cstring         title       = nullptr; //Have a *wild* guess.
    CRect           screenArea;
};

struct CDEvent{
    enum EventType{
        WindowRefreshed        = 0x1, //Perhaps not so interesting?
        WindowMoved            = 0x2,
        WindowResized          = 0x4,
        WindowStateChanged     = 0x5, //Includes closing and focus change
        FramebufferResized     = 0x6,
    };
    uint8   type = 0;
};

struct CDResizeEvent{
    uint32   w = 0;
    uint32   h = 0;
};

struct CDStateEvent{
    enum StateChange{
        Minimized   = 0x01,
        Maximized   = 0x02,
        Closed      = 0x04,
        GainedFocus = 0x08,
        LostFocus   = 0x10,
        //TODO : Add more states here
    };
    uint8   bits;
};

struct CDMoveEvent{
    uint32   x = 0;
    uint32   y = 0;
};

struct CGLContextVersion
{
    //Default context is OpenGL core 3.3const
    int major = 3;
    int minor = 3;
};

struct CGLContextProperties
{
    enum ContextProperties{
        GLCoreProfile   = 0x01,      //GL core profile
        GLVSync         = 0x02,     //vertical sync
        GLDebug         = 0x04,    //GL debugging context
        GLAutoResize    = 0x08,   //Whether the context auto-resizes
        GLRobust        = 0x10,
    };

    uint16              flags = GLCoreProfile;
    CGLContextVersion   version;
    CDContextBits       bits;
};
struct CDWindowProperties
{
    enum WindowState{
        FullScreen          = 0x001,
        WindowedFullScreen  = 0x002,
        Windowed            = 0x004,

        Minimized           = 0x008,
        Maximized           = 0x010,

        Focused             = 0x020,
        Resizable           = 0x040,

        Decorated           = 0x080,
        Floating            = 0x100,
        Visible             = 0x200,

        HighDPI             = 0x400,

        Foreign             = 0x800,
    };

    uint16                  flags           = 0;
    //For all of you with 65536 monitors, this is for you.
    uint16                  monitor         = 0;
    CSize                   size;
    CGLContextProperties    contextProperties;
    cstring                 title           = nullptr;
};

}
}

#endif // CDISPLAY_H

