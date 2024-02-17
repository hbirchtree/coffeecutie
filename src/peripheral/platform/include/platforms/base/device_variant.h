#pragma once

namespace platform::info {

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
    DeviceConsole,
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

} // namespace platform::info
