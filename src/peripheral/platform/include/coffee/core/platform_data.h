#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace info {

using namespace ::libc_types;
using namespace ::stl_types;

enum DeviceType
{
    DeviceUnknown,
    DeviceDesktop,
    DeviceIOT,
    DeviceLaptop,
    DevicePhone,
    DeviceTablet,
    DeviceServer,
    DeviceAllInOne,
};

enum Platform
{
    PlatformUnknown = 0,

    /* Primary traits of a platform */
    PlatformPOSIX = 0x1000,

    /* "Families" in POSIX */
    PlatformUnix    = PlatformPOSIX | 0x100,
    PlatformWindows = PlatformPOSIX | 0x200,
    PlatformWeb     = PlatformPOSIX | 0x400,

    /* "Families" in Unix */
    PlatformLinux   = PlatformUnix | 0x10,
    PlatformMac     = PlatformUnix | 0x20,
    PlatformAndroid = PlatformUnix | 0x30,

    /* Variants of Linux */
    PlatformLinuxRaspberry = PlatformLinux | 0x1,
    PlatformLinuxMaemo     = PlatformLinux | 0x2,

    /* Variants of MacOS */
    PlatformMacIOS = PlatformMac | 0x1,

    /* Variants of Windows */
    PlatformWin32     = PlatformWindows | 0x1,
    PlatformUWP       = PlatformWindows | 0x2,
    PlatformUWPMobile = PlatformWindows | 0x4,
    PlatformMinGW     = PlatformWin32 | 0x8,

    /* Variants of web platforms */
    PlatformEmscripten = PlatformWeb | 0x1,
};

union SafeArea
{
    struct
    {
        scalar left;
        scalar top;
        scalar right;
        scalar bottom;
    };
    scalar data[4];
};

/* High-level properties of the system */

extern CString system_name();

namespace device {
namespace display {

extern scalar   dpi();
extern SafeArea safe_area();

} // namespace display

namespace is {
extern const bool mobile;
}

extern DeviceType variant();

} // namespace device

namespace platform {
namespace uses {
extern const bool virtualfs;
extern const bool debug_mode;
} // namespace uses

extern Platform variant();

} // namespace platform

} // namespace info
} // namespace platform

namespace Coffee {

COFFEE_APP_CLASS struct PlatformData
{
    using DeviceType = platform::info::DeviceType;
    using Platform   = platform::info::Platform;

    STATICINLINE DeviceType DeviceVariant()
    {
        return platform::info::device::variant();
    }

    STATICINLINE Platform PlatformVariant()
    {
        return platform::info::platform::variant();
    }

    STATICINLINE
    /*!
     * \brief Device DPI, where 1.f is 1:1 pixel ratio. This is used to
     * directly scale UI components. \return
     */
    libc_types::scalar DeviceDPI()
    {
        return platform::info::device::display::dpi();
    }

    STATICINLINE
    /*!
     * \brief On some devices, get the insets on the display which can
     *  safely be used by important UI/graphic elements.
     * \return true if the value returned is valid
     */
    bool DeviceSafeArea(platform::info::SafeArea& area)
    {
        area = platform::info::device::display::safe_area();
        return true;
    }

    STATICINLINE
    /*!
     * \brief Displayable string of system and its version
     */
    stl_types::CString SystemDisplayString()
    {
        return platform::info::system_name();
    }

    STATICINLINE
    /*!
     * \brief Defined on mobile devices, phones and tablets mostly
     * \return
     */
    bool IsMobile()
    {
        return platform::info::device::is::mobile;
    }

    STATICINLINE
    /*!
     * \brief Defined for systems where some storage is not file-based, such
     * as Android assets \return
     */
    bool UseVirtualFS()
    {
        return platform::info::platform::uses::virtualfs;
    }

    STATICINLINE
    /*!
     * \brief IsDebug
     * \return
     */
    bool IsDebug()
    {
        return platform::info::platform::uses::debug_mode;
    }
};
} // namespace Coffee
