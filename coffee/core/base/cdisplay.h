#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "coffee/core/coffee.h"

namespace Coffee{
namespace CDisplay{

/*!
 * \brief Color space, typically for monitors
 */
struct CDColorSpace{
    uint8 red     = 0;
    uint8 green   = 0;
    uint8 blue    = 0;
};

/*!
 * \brief Bit depths for context
 */
struct CDContextBits{
    uint8 red     = 0;
    uint8 green   = 0;
    uint8 blue    = 0;
    uint8 alpha   = 0;

    uint8 depth   = 0;
    uint8 stencil = 0;

    CRGBA accum;
};

/*!
 * \brief Monitor information
 */
struct CDMonitor{
    uint16          index = 0;  /*! Real monitor index*/
    CDColorSpace    colorBits;  /*! Color depth bits*/
    CRect           screenArea; /*! Area occupied in window manager*/
    cstring         name;       /*! Name of monitor*/
    CSize           phySize;    /*! Physical size of monitor*/
    int32           refresh = 0;/*! Refresh rate*/
};

/*!
 * \brief Window event
 */
struct CDEvent{
    enum EventType
    {
        Refresh = 0x1,
        Move    = 0x2,
        Resize  = 0x4,
        State   = 0x5,

        Focus   = 0x6,
    };
    uint8   type    = 0; /*! Event type*/
    uint32  ts      = 0; /*! Event timestamp*/
};

/*!
 * \brief Window move event
 */
typedef CPoint CDMoveEvent;
/*!
 * \brief Window resize event
 */
typedef CSize CDResizeEvent;
/*!
 * \brief GL context version
 */
struct CGLContextVersion : public _cbasic_version<uint8>
{
    CGLContextVersion(){}
    CGLContextVersion(uint8 maj, uint8 min){
        this->major = maj;
        this->minor = min;
    }
};

/*!
 * \brief Window state event
 */
struct CDStateEvent{
    enum StateChange{
        Minimized   = 0x01,
        Maximized   = 0x02,
        Restored    = 0x03,

        Closed      = 0x04,

        Hidden      = 0x05,
        Shown       = 0x06,
    };
    uint8   type    = 0; /*! Type of window state event*/
};

/*!
 * \brief Window focus event
 */
struct CDFocusEvent
{
    enum FocusMask
    {
        Mouse   = 0x1,
        Enter   = 0x2,
        Exposed = 0x4,
    };
    uint8   mod     = 0; /*! Type of focus event*/
};

/*!
 * \brief GL context properties to set on start-up
 */
struct CGLContextProperties
{
    enum ContextProperties{
        GLCoreProfile	    = 0x01, /*! Set GL core profile*/
        GLVSync		    = 0x02, /*! Set GL vertical sync*/
        GLDebug		    = 0x04, /*! Set GL debug context*/
        GLAutoResize	    = 0x08, /*! Set GL auto resize of context*/
        GLRobust	    = 0x10, /*! Set GL robustness*/
        GLPrintExtensions   = 0x20, /*! Print GL extensions on startup*/
    };
    uint16              flags = GLCoreProfile; /*! Context flags*/
    CGLContextVersion   version; /*! Context version*/
    CDContextBits       bits; /*! Context bits*/
};
/*!
 * \brief Window properties to set on start-up
 */
struct CDWindowProperties
{
    enum WindowState{
        FullScreen          = 0x001, /*! Exclusive fullscreen mode*/
        WindowedFullScreen  = 0x002, /*! Windowed fullscreen mode*/
        Windowed            = 0x004, /*! Regular window mode*/

        Minimized           = 0x008, /*! Minimized mode*/
        Maximized           = 0x010, /*! Maximized mode*/

        Focused             = 0x020, /*! Focused mode*/
        Resizable           = 0x040, /*! Resizable mode*/

        Undecorated         = 0x080, /*! Decorated mode*/
        Floating            = 0x100, /*! Floating mode, not supported by all platforms and context managers*/
        Visible             = 0x200, /*! Visibility*/

        HighDPI             = 0x400, /*! HighDPI mode for platforms that support it (not Windows)*/

        Foreign             = 0x800, /*! For windows not created by context manager*/
    };

    uint16                  flags           = 0; /*! Window flags*/
    uint16                  monitor         = 0; /*! Monitor to use with fullscreen*/
    CSize                   size; /*! Size of window*/
    CGLContextProperties    contextProperties; /*! Context properties to set*/
    cstring                 title           = nullptr; /*! Window title to start with*/
};

extern CDWindowProperties coffee_get_default_visual();

}
}

#endif // CDISPLAY_H

