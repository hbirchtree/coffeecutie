#pragma once

#include <coffee/core/types/debug/component.h>
#include <coffee/core/types/debug/severity.h>
#include <coffee/core/types/debug/type.h>
#include <coffee/core/types/filtering.h>
#include <coffee/core/types/pixel_components.h>
#include <coffee/core/types/pixel_format.h>
#include <coffee/core/types/pixel_transform.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/rect.h>
#include <coffee/core/types/rsca.h>
#include <coffee/core/types/size.h>
#include <coffee/core/types/vector_types.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/semantic/handle.h>
#include <peripherals/typing/enum/debug/severity.h>
#include <peripherals/typing/enum/delay.h>
#include <peripherals/typing/enum/graphics/buffer_types.h>
#include <peripherals/typing/enum/graphics/color_buffer.h>
#include <peripherals/typing/enum/graphics/framebuffer_types.h>
#include <peripherals/typing/enum/graphics/primitive.h>
#include <peripherals/typing/enum/graphics/profiling_term.h>
#include <peripherals/typing/enum/graphics/query_types.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>
#include <peripherals/typing/enum/graphics/shader_types.h>
#include <peripherals/typing/enum/graphics/texture_types.h>
#include <peripherals/typing/enum/graphics/vertex_face.h>
#include <peripherals/typing/enum/graphics/vertex_winding.h>
#include <peripherals/typing/enum/logic/comparison.h>
#include <peripherals/typing/enum/logic/operator.h>
#include <peripherals/typing/enum/logic/value_policy.h>
#include <peripherals/typing/enum/pixels/operation.h>
#include <peripherals/typing/enum/pixels/wrapping.h>
#include <peripherals/typing/graphics/field.h>
#include <peripherals/typing/pixels/colormask.h>

namespace Coffee {
namespace RHI {
namespace Datatypes {

using colormask_t = typing::pixels::colormask;

using semantic::Delay;
using semantic::TypeEnum;
using typing::LogicOp;
using typing::Operator;
using typing::PixelOperation;
using typing::ValueComparison;
using typing::ValueHandling;
using typing::WrapPolicy;
using typing::graphics::ProfilingTerm;
using typing::graphics::ShaderStage;

using Face = typing::graphics::VertexFace;
using typing::graphics::VertexWinding;

using DBuffers     = typing::graphics::color_buffer;
using FramebufferT = typing::graphics::framebuffer_type;

using Prim    = typing::graphics::primitive::base;
using PrimCre = typing::graphics::primitive::creation;

namespace tex {
using namespace typing::graphics::texture_composition;
using namespace typing::graphics::tex;
} // namespace tex

namespace buf {
using namespace typing::graphics::buffer_composition;
using namespace typing::graphics::buf;
} // namespace buf

namespace query {
using namespace typing::graphics::query_composition;
using namespace typing::graphics::query;
} // namespace query

namespace ShaderTypes {
using namespace typing::graphics::shader_type;
}

namespace QueryComp {
using namespace query;
}

namespace TexComp {
using namespace tex;
}

namespace BufferComp {
using namespace buf;
}

using ZField64 = typing::graphics::field<bigscalar>;
using RectF    = typing::geometry::rect<scalar>;

} // namespace Datatypes

using namespace Datatypes;
} // namespace RHI
} // namespace Coffee
