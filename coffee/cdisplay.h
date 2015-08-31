#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "coffee.h"

namespace Coffee{
namespace CDisplay{
struct CDColorSpace{
    uint8_t bits_red;
    uint8_t bits_green;
    uint8_t bits_blue;
};

struct CDContextBits{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    uint8_t depth;
    uint8_t alpha;
    uint8_t stencil;
};

struct CDMonitor{
    uint8_t         index;      //Real monitor index
    CDColorSpace    colorBits;  //Color depth
    CRect           screenArea; //Area occupied on screen
    CString         name;       //Name of monitor
    CSize           phySize;    //Physical size, reported by some ctxts
    int32_t         refresh;    //Refresh rate of monitor
};
struct CDWindow{
    uintptr_t       handle;     //Native handle, in X11, the Window value,
    CString         title;      //Have a *wild* guess.
    CRect           screenArea;


};

struct CDEvent{
    enum EventType{
        WindowRefreshed        = 0x1, //Perhaps not so interesting?
        WindowMoved            = 0x2,
        WindowResized          = 0x4,
        WindowStateChanged     = 0x5, //Includes closing and focus change
    };
    uint8_t type;
};

struct CDResizeEvent{
    uint32_t old_w;
    uint32_t old_h;

    uint32_t new_w;
    uint32_t new_h;
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
    uint32_t old_x;
    uint32_t old_y;

    uint32_t new_x;
    uint32_t new_y;
};

}
}

#endif // CDISPLAY_H

