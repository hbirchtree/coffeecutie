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
    uint16        index = 0;  //Real monitor index
    CDColorSpace    colorBits;  //Color depth
    CRect           screenArea; //Area occupied on screen
    cstring         name;       //Name of monitor
    CSize           phySize;    //Physical size, reported by some ctxts
    int32         refresh = 0;//Refresh rate of monitor
};
struct CDWindow{
    void*           handle_p    = nullptr; //Native handle, in X11, the Window value
    uintptr_t       handle      = 0;
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
        Minimized   = 0b1,
        Maximized   = 0b10,
        Closed      = 0b100,
        GainedFocus = 0b1000,
        LostFocus   = 0b10000,
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
        GLCoreProfile   = 0b1,      //GL core profile
        GLVSync         = 0b10,     //vertical sync
        GLDebug         = 0b100,    //GL debugging context
        GLAutoResize    = 0b1000,   //Whether the context auto-resizes
        GLRobust        = 0b10000,
    };

    uint16              flags = 0b1;
    CGLContextVersion   version;
    CDContextBits       bits;
};
struct CDWindowProperties
{
    enum WindowState{
        FullScreen          = 0b001,
        WindowedFullScreen  = 0b010,
        Windowed            = 0b100,

        Minimized           = 0b01000,
        Maximized           = 0b10000,

        Focused             = 0b0100000,
        Resizable           = 0b1000000,

        Decorated           = 0b00010000000,
        Floating            = 0b00100000000,
        Visible             = 0b01000000000,

        HighDPI             = 0b00001000000,
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

