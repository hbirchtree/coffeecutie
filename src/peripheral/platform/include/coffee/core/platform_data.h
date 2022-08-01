#pragma once

#include <peripherals/identify/compiler/function_export.h>
#include <peripherals/constants.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace info {

using namespace ::libc_types;
using namespace ::stl_types;


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

namespace device::display {

//extern scalar   dpi();
//extern SafeArea safe_area();

} // namespace display

//extern Platform platform_variant();

} // namespace info
} // namespace platform

namespace compile_info {

/*!
 * \brief The underlying engine version
 */
extern const libc_types::cstring engine_version;

} // namespace compile_info

//namespace Coffee {

//COFFEE_APP_CLASS struct PlatformData
//{
//    using DeviceType = platform::info::DeviceType;
//    using Platform   = platform::info::Platform;

//    C_DEPRECATED_S("use platform::info::device::variant()")
//    STATICINLINE DeviceType DeviceVariant()
//    {
//        return platform::info::device::variant();
//    }

//    C_DEPRECATED_S("use platform::info::platform::variant()")
//    STATICINLINE Platform PlatformVariant()
//    {
//        return platform::info::platform_variant();
//    }

//    C_DEPRECATED_S("use platform::info::device::display::dpi()")
//    STATICINLINE
//    /*!
//     * \brief Device DPI, where 1.f is 1:1 pixel ratio. This is used to
//     * directly scale UI components. \return
//     */
//    libc_types::scalar DeviceDPI()
//    {
//        return platform::info::device::display::dpi();
//    }

//    C_DEPRECATED_S("use platform::info::device::display::safe_area()")
//    STATICINLINE
//    /*!
//     * \brief On some devices, get the insets on the display which can
//     *  safely be used by important UI/graphic elements.
//     * \return true if the value returned is valid
//     */
//    bool DeviceSafeArea(platform::info::SafeArea& area)
//    {
//        area = platform::info::device::display::safe_area();
//        return true;
//    }
//};
//} // namespace Coffee
