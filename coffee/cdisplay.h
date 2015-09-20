#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "coffee.h"

namespace Coffee{
namespace CDisplay{
struct CDColorSpace{
    uint8_t red     = 0;
    uint8_t green   = 0;
    uint8_t blue    = 0;
};

struct CDContextBits{
    //Bit depths for a GL context
    uint8_t red     = 0;
    uint8_t green   = 0;
    uint8_t blue    = 0;
    uint8_t alpha   = 0;

    uint8_t depth   = 0;
    uint8_t stencil = 0;
};

struct CDMonitor{
    uint8_t         index = 0;  //Real monitor index
    CDColorSpace    colorBits;  //Color depth
    CRect           screenArea; //Area occupied on screen
    cstring         name;       //Name of monitor
    CSize           phySize;    //Physical size, reported by some ctxts
    int32_t         refresh = 0;//Refresh rate of monitor
};
struct CDWindow{
    uintptr_t       handle; //Native handle, in X11, the Window value
    cstring         title = nullptr;      //Have a *wild* guess.
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
    uint8_t type;
};

struct CDResizeEvent{
    uint32_t w;
    uint32_t h;
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
    uint8_t bits;
};

struct CDMoveEvent{
    uint32_t x;
    uint32_t y;
};

struct CGLContextVersion
{
    //Default context is OpenGL core 3.3
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
    };

    uint16_t            flags = 0b1;
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
    };

    uint16_t                flags           = 0;
    //For all of you with 65536 monitors, this is for you.
    uint16_t                monitor         = 0;
    CSize                   size;
    CGLContextProperties    contextProperties;
};

}
}

#endif // CDISPLAY_H

