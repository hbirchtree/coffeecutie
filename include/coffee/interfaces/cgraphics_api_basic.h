#pragma once

#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace Display{

struct CDProperties;

}
namespace RHI{

struct GraphicsAPI_Base
{
    static void GetDefaultVersion(i32&, i32&)
    {
    }

    static void GetDefaultProperties(Display::CDProperties&)
    {
    }
};

}
}
