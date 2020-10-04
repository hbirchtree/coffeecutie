#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/version.h>

namespace Coffee {
namespace Display {
namespace GL {

/*!
 * \brief GL context version
 */
struct Version : public version<i32>
{
    Version()
    {
        this->major    = 0;
        this->minor    = 0;
        this->revision = 0;
    }
    Version(u8 maj, u8 min)
    {
        this->major    = maj;
        this->minor    = min;
        this->revision = 0;
    }
    CString driver;
};

} // namespace GL
} // namespace Display
} // namespace Coffee
