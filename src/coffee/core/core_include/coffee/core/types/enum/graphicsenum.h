#pragma once

#include <coffee/core/libc_types.h>
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






namespace TexComp {

using namespace ::typing::graphics::texture_composition;
}

namespace BufferComp {

using namespace ::typing::graphics::buffer_composition;
}

namespace QueryComp {

using namespace ::typing::graphics::query_composition;
}

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
