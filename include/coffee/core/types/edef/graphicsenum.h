#pragma once

#include "enumfun.h"

namespace Coffee{

enum class DBuffers
{
    None    = 0x0,
    Color   = 0x1,
    Depth   = 0x2,
    Stencil = 0x4,

    DepthStencil = 0x6,
};

enum class ShaderStage
{
    Vertex = 0x1,
    Fragment = 0x2,

    Compute = 0x4,
    Geometry = 0x8,

    TessEval = 0x10,
    TessControl = 0x20,

    All =
    Vertex
    |TessEval|TessControl
    |Geometry
    |Fragment
    |Compute
};

enum class FramebufferT
{
    Draw = 0x1,
    Read = 0x2,
    All = 0x3,
};

enum class ProfilingTerm
{
    ElapsedTime,
    ScreenSamples,
    ScreenPrimitives,
    TransformedPrimitives,
    Frametime,
};

C_FLAGS(DBuffers,uint32);
C_FLAGS(FramebufferT,uint32);

}
