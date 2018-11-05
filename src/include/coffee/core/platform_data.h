#pragma once

#include <peripherals/stl/types.h>
#include <peripherals/libc/types.h>

namespace Coffee{

COFFEE_APP_CLASS struct PlatformData
{
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
        PlatformMinGW           = PlatformWin32   | 0x8,

        /* Variants of web platforms */
        PlatformEmscripten      = PlatformWeb | 0x1,
        PlatformNaCL            = PlatformWeb | 0x2,
    };
    
    struct SafeArea
    {
        union {
            struct {
                scalar left;
                scalar top;
                scalar right;
                scalar bottom;
            };
            scalar data[4];
        };
    };

    static
    DeviceType DeviceVariant();

    static
    Platform PlatformVariant();

    static
    /*!
     * \brief Device DPI, where 1.f is 1:1 pixel ratio. This is used to directly scale UI components.
     * \return
     */
    scalar DeviceDPI();
    
    static
    /*!
     * \brief On some devices, get the insets on the display which can
     *  safely be used by important UI/graphic elements.
     * \return true if the value returned is valid
     */
    bool DeviceSafeArea(SafeArea& area);

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
