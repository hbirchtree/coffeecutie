#pragma once

#include "enumfun.h"

namespace Coffee{

enum class ShaderStage
{
    Vertex = 0x1,
    Fragment = 0x2,

    Compute = 0x4,
    Geometry = 0x8,

    TessEval = 0x10,
    TessControl = 0x20,

    All = Vertex|Fragment
    |Compute|Geometry
    |TessEval|TessControl,
};

}
