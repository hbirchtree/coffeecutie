#pragma once

#include "types/tdef/stltypes.h"
#include "types/tdef/integertypes.h"

namespace Coffee{
struct PlatformData
{
    enum DeviceType
    {
        DeviceUnknown,
        DeviceDesktop,
        DeviceIOT,
        DeviceLaptop,
        DevicePhone,
        DeviceTablet,
    };

    enum Platform
    {
        PlatformUnknown = 0,

        /* Primary traits of a platform */
        PlatformPOSIX           = 0x1000,

        /* "Families" in POSIX */
        PlatformUnix            = PlatformPOSIX | 0x100,
        PlatformWindows         = PlatformPOSIX | 0x200,
        PlatformWeb             = PlatformPOSIX | 0x400,

        /* "Families" in Unix */
        PlatformLinux           = PlatformUnix | 0x10,
        PlatformMac             = PlatformUnix | 0x20,
        PlatformAndroid         = PlatformUnix | 0x30,

        /* Variants of Linux */
        PlatformLinuxRaspberry  = PlatformLinux | 0x1,
        PlatformLinuxMaemo      = PlatformLinux | 0x2,

        /* Variants of MacOS */
        PlatformMacIOS          = PlatformMac | 0x1,

        /* Variants of Windows */
        PlatformWin32           = PlatformWindows | 0x1,
        PlatformUWP             = PlatformWindows | 0x2,
        PlatformUWPMobile       = PlatformWindows | 0x4,

        /* Variants of web platforms */
        PlatformEmscripten      = PlatformWeb | 0x1,
        PlatformNaCL            = PlatformWeb | 0x2,
    };

    static
    DeviceType DeviceVariant();

    static
    Platform PlatformVariant();

    static
    scalar DeviceDPI();

    static
    /*!
     * \brief Displayable string of system and its version
     */
    CString SystemDisplayString();

    static
    /*!
     * \brief Defined on mobile devices, phones and tablets mostly
     * \return
     */
    bool IsMobile();

//    static
//    /*!
//     * \brief Defined when OpenGL ES is the graphics API
//     * \return
//     */
//    bool IsGLES();

    static
    /*!
     * \brief Defined for systems where some storage is not file-based, such as Android assets
     * \return
     */
    bool UseVirtualFS();

    static
    /*!
     * \brief IsDebug
     * \return
     */
    bool IsDebug();
};
}
