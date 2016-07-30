#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "../../types/cdef/infotypes.h"
#include "../../types/cdef/pixtypes.h"
#include "../../types/edef/enumfun.h"

namespace Coffee{
namespace Display{

/*!
 * \brief Contains system-dependent window data. Is not allowed in the global namespace.
 */
struct CDWindow;

/*!
 * \brief Color space, typically for monitors
 */
struct CDColorSpace
{
    CDColorSpace():
        red(0),
        green(0),
        blue(0)
    {
    }

    uint8 red     = 0;
    uint8 green   = 0;
    uint8 blue    = 0;
    uint8 extra   = 0;
};

/*!
 * \brief Bit depths for context
 */
struct CDContextBits
{
    CDContextBits():
        accum(),
        red(0),
        green(0),
        blue(0),
        alpha(0),
        depth(0),
        stencil(0)
    {
    }

    CRGBA accum;

    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;

    uint8 depth;
    uint8 stencil;

    uint16 _pad;
};

/*!
 * \brief Monitor information
 */
struct CDMonitor{
    CDMonitor():
        screenArea(),
        name(nullptr),
        phySize(0,0),
        colorBits(),
        refresh(0),
        index(0)
    {
    }

    CRect           screenArea; /*!< Area occupied in window manager*/
    cstring         name; /*!< Name of monitor*/
    CSize           phySize; /*!< Physical size of monitor*/
    CDColorSpace    colorBits; /*!< Color depth bits*/
    int32           refresh:24;/*!< Refresh rate*/
    uint8           index; /*!< Real monitor index*/
};

/*!
 * \brief Window event
 */
struct CDEvent{
    CDEvent():
        type(),
        ts(0)
    {
    }

    enum EventType : uint8
    {
        Refresh = 0x1,
        Move    = 0x2,
        Resize  = 0x4,
        State   = 0x5,

        Focus   = 0x6,
    };
    EventType type; /*!< Event type*/
    uint32  ts; /*!< Event timestamp*/
};

/*!
 * \brief Window move event
 */
using CDMoveEvent = CPoint;
/*!
 * \brief Window resize event
 */
using CDResizeEvent = CSize;
/*!
 * \brief GL context version
 */
struct CGLVersion : public _cbasic_version<uint8>
{
    CGLVersion()
    {
        this->major = 0;
        this->minor = 0;
        this->revision = 0;
    }
    CGLVersion(uint8 maj, uint8 min)
    {
        this->major = maj;
        this->minor = min;
        this->revision = 0;
    }
    CString driver;
};

/*!
 * \brief Window state event
 */
struct CDStateEvent
{
    CDStateEvent():
        type()
    {
    }

    enum StateChange : uint8{
        Minimized   = 0x01,
        Maximized   = 0x02,
        Restored    = 0x03,

        Closed      = 0x04,

        Hidden      = 0x05,
        Shown       = 0x06,
    };
    StateChange type; /*!< Type of window state event*/
};

/*!
 * \brief Window focus event
 */
struct CDFocusEvent
{
    CDFocusEvent():
        mod()
    {
    }

    enum FocusMask : uint8
    {
        Mouse   = 0x1,
        Enter   = 0x2,
        Exposed = 0x4,
    };
    FocusMask mod; /*!< Type of focus event*/
};

C_FLAGS(CDFocusEvent::FocusMask,uint8);

/*!
 * \brief GL context properties to set on start-up
 */
struct GLProperties
{
    GLProperties():
        bits(),
        flags(GLCoreProfile),
        version(3,3)
    {
    }

    enum Flags : uint32{
        GLCoreProfile	    = 0x01, /*!< Set GL core profile*/
        GLVSync             = 0x02, /*!< Set GL vertical sync*/
        GLDebug             = 0x04, /*!< Set GL debug context*/
        GLAutoResize	    = 0x08, /*!< Set GL auto resize of context*/
        GLRobust            = 0x10, /*!< Set GL robustness*/
        GLPrintExtensions   = 0x20, /*!< Print GL extensions on startup*/
        GLES                = 0x40, /*!< Request that only GLES features are used*/

        GLFeatureLevelProfile = 0x80,

        NoGL                = 0x100,
    };
    CDContextBits       bits; /*!< Context bits*/
    Flags               flags; /*!< Context flags*/
    CGLVersion   version; /*!< Context version*/
};
/*!
 * \brief Window properties to set on start-up
 */
struct CDProperties
{
    CDProperties():
        gl(),
        title(nullptr),
        window(nullptr),
        icon(nullptr),
        size(0,0),
        flags(),
        monitor(0)
    {
    }

    enum State : uint16{
        FullScreen          = 0x001, /*!< Exclusive fullscreen mode*/
        WindowedFullScreen  = 0x002, /*!< Windowed fullscreen mode*/
        Windowed            = 0x004, /*!< Regular window mode*/

        Minimized           = 0x008, /*!< Minimized mode*/
        Maximized           = 0x010, /*!< Maximized mode*/

        Focused             = 0x020, /*!< Focused mode*/
        Resizable           = 0x040, /*!< Resizable mode*/

        Undecorated         = 0x080, /*!< Decorated mode*/
        Floating            = 0x100, /*!< Floating mode, not supported by all platforms and context managers*/
        Visible             = 0x200, /*!< Visibility*/

        HighDPI             = 0x400, /*!< HighDPI mode for platforms that support it (not Windows)*/

        Foreign             = 0x800, /*!< For windows not created by context manager*/

        Normal              = 0x1000, /*!< In SDL2, this restores the window*/
    };

    GLProperties    gl; /*!< Context properties to set*/
    cstring         title; /*!< Window title to start with*/
    CDWindow*       window; /*!< If applicable, contains data about a window*/
    CBitmap*        icon;
    CSize           size; /*!< Size of window*/
    State           flags; /*!< Window flags*/
    uint16          monitor; /*!< Monitor to use with fullscreen*/
};

C_FLAGS(CDProperties::State,uint16);
C_FLAGS(GLProperties::Flags,uint8);

extern CDProperties GetDefaultVisual(
        const int32& ctxtMajorVer = 3,
        const int32& ctxtMinorVer = 3);

}
}

#endif // CDISPLAY_H

