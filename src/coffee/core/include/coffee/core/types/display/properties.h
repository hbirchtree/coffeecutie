#pragma once

#include <coffee/core/types/display/gl_properties.h>
#include <coffee/core/types/display/monitor.h>
#include <coffee/core/types/display/window.h>
#include <coffee/interfaces/cgraphics_api_basic.h>
#include <peripherals/enum/helpers.h>

namespace Coffee {
namespace Display {

/*!
 * \brief Window properties to set on start-up
 */
struct Properties
{
    Properties() :
        gl(), title(nullptr), window(nullptr), size(0, 0), flags(), monitor(0)
    {
    }

    enum State : uint16
    {
        FullScreen         = 0x001, /*!< Exclusive fullscreen mode*/
        WindowedFullScreen = 0x002, /*!< Windowed fullscreen mode*/
        Windowed           = 0x004, /*!< Regular window mode*/

        Minimized = 0x008, /*!< Minimized mode*/
        Maximized = 0x010, /*!< Maximized mode*/

        Focused   = 0x020, /*!< Focused mode*/
        Resizable = 0x040, /*!< Resizable mode*/

        Undecorated = 0x080, /*!< Decorated mode*/
        Floating = 0x100, /*!< Floating mode, not supported by all platforms and
                             context managers*/
        Visible = 0x200,  /*!< Visibility*/

        HighDPI = 0x400, /*!< HighDPI mode for platforms that support it (not
                            Windows)*/

        Foreign = 0x800, /*!< For windows not created by context manager*/

        Normal = 0x1000, /*!< In SDL2, this restores the window*/
    };

    GL::Properties gl;      /*!< Context properties to set*/
    cstring        title;   /*!< Window title to start with*/
    Window*        window;  /*!< If applicable, contains data about a window*/
    Size           size;    /*!< Size of window*/
    State          flags;   /*!< Window flags*/
    uint16         monitor; /*!< Monitor to use with fullscreen*/
};

C_FLAGS(Properties::State, uint16);

extern Properties GetDefaultVisual(
    const i32& ctxtMajorVer, const i32& ctxtMinorVer);

template<
    typename GL_LIB,

    typename implements<RHI::GraphicsAPI_Base, GL_LIB>::type* = nullptr

    >
STATICINLINE Properties GetDefaultVisual()
{
    i32 majver = 0, minver = 0;
    GL_LIB::GetDefaultVersion(majver, minver);
    auto visual = GetDefaultVisual(majver, minver);
    GL_LIB::GetDefaultProperties(visual);
    return visual;
}

} // namespace Display
} // namespace Coffee
