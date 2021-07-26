#pragma once

#include <peripherals/enum/helpers.h>

namespace typing::graphics {

enum class ShaderStage
{
    Vertex   = 0x1,
    Fragment = 0x2,

    Compute  = 0x4,
    Geometry = 0x8,

    TessEval    = 0x10,
    TessControl = 0x20,

    All = Vertex | TessEval | TessControl | Geometry | Fragment | Compute
};

C_FLAGS(ShaderStage, int);

} // namespace typing::graphics
