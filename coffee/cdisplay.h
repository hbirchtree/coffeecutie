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
        Minimized   = 0x1,
        Maximized   = 0x2,
        Closed      = 0x4,
        GainedFocus = 0x8,
        LostFocus   = 0x16,
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
        GLRobust        = 0x16,
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
        Maximized           = 0x016,

        Focused             = 0x032,
        Resizable           = 0x064,

        Decorated           = 0x0128,
        Floating            = 0x0256,
        Visible             = 0x0512,

        HighDPI             = 0x1024,

        Foreign             = 0x2048,
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

