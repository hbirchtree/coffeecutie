#pragma once

#include <coffee/core/libc_types.h>

namespace Coffee {
namespace Display {
struct Properties;
}
namespace RHI {

struct GraphicsAPI_Base
{
    static void GetDefaultVersion(i32&, i32&)
    {
    }

    static void GetDefaultProperties(Display::Properties&)
    {
    }
};

} // namespace RHI
} // namespace Coffee
