#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/colorspace.h>
#include <coffee/core/types/rect.h>
#include <coffee/core/types/size.h>

namespace Coffee {
namespace Display {
/*!
 * \brief Monitor information
 */
struct Monitor
{
    Monitor() :
        screenArea(), name(nullptr), phySize(0, 0), colorBits(), refresh(0),
        index(0)
    {
    }

    Rect       screenArea; /*!< Area occupied in window manager*/
    cstring    name;       /*!< Name of monitor*/
    Size       phySize;    /*!< Physical size of monitor*/
    Size       resolution;
    ColorSpace colorBits;    /*!< Color depth bits*/
    i32        refresh : 24; /*!< Refresh rate*/
    u8         index;        /*!< Real monitor index*/
};

} // namespace Display
} // namespace Coffee
