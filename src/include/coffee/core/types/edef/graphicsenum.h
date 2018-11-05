#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/graphics/buffer_types.h>
#include <peripherals/typing/enum/graphics/color_buffer.h>
#include <peripherals/typing/enum/graphics/primitive.h>
#include <peripherals/typing/enum/graphics/query_types.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>
#include <peripherals/typing/enum/graphics/shader_types.h>
#include <peripherals/typing/enum/graphics/texture_types.h>

namespace Coffee {

using namespace ::typing::graphics;

using DBuffers = typing::graphics::color_buffer;

enum class FramebufferT
{
    None,
    Draw = 0x1,
    Read = 0x2,
    All  = 0x3,
};

enum class ProfilingTerm
{
    ElapsedTime,
    ScreenSamples,
    ScreenPrimitives,
    TransformedPrimitives,
    Frametime,
};

enum class TexType
{
    None,
    T2D,
    T2A,
    T3D,
    Cube
};

namespace tex {

using namespace ::typing::graphics::tex;
}

namespace buf {

using namespace ::typing::graphics::buf;
}

namespace query {

using namespace ::typing::graphics::query;
}

namespace ShaderTypes {

using namespace ::typing::graphics::shader_type;
}

/*!
 * \brief Primitive types. Sorted such that, with 0,0
 * for Prim and PrimCre, it defaults to explicit triangles.
 */
using Prim    = typing::graphics::primitive::base;
using PrimCre = typing::graphics::primitive::creation;

C_FLAGS(FramebufferT, u32);

} // namespace Coffee
