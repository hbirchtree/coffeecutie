#pragma once

#include <peripherals/constants.h>
#include <peripherals/identify/compiler/function_export.h>
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
        f32 left;
        f32 top;
        f32 right;
        f32 bottom;
    };

    f32 data[4];
};

/* High-level properties of the system */

} // namespace info
} // namespace platform

namespace compile_info {

/*!
 * \brief The underlying engine version
 */
extern const libc_types::cstring engine_version;

} // namespace compile_info
