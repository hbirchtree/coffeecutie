#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {

namespace buffer_composition {

enum BufferType_t
{
    BASETYPE_OFFSET  = 0,
    CONST_MOD_OFFSET = 6,

    Vertex      = 1 << (BASETYPE_OFFSET + 0),
    Elements    = 1 << (BASETYPE_OFFSET + 1),
    Constants   = 1 << (BASETYPE_OFFSET + 2),
    Indirect    = 1 << (BASETYPE_OFFSET + 3),
    PixelBuffer = 1 << (BASETYPE_OFFSET + 4),
    Feedback    = 1 << (BASETYPE_OFFSET + 5),

    /* Modifiers for Constants basetype */
    ReadOnly  = 1 << (CONST_MOD_OFFSET + 0),
    ReadWrite = 1 << (CONST_MOD_OFFSET + 1),
    Atomic    = 1 << (CONST_MOD_OFFSET + 2),
    Query     = 1 << (CONST_MOD_OFFSET + 3),

    /* Modifiers for Indirect basetype */
    Compute = 1 << (CONST_MOD_OFFSET + 4),

    /* Modifiers for PixelBuffer basetype */
    Unpack = 1 << (CONST_MOD_OFFSET + 5),
    Pack   = 1 << (CONST_MOD_OFFSET + 6),
};

using buf_flag = unsigned int;

template<buf_flag BaseType, buf_flag Modifiers>
struct buf_basetype
{
    /* Relation to shaders */
    static constexpr bool bindable = BaseType & Constants;

    /* GPU attributes */
    static constexpr bool gpu_modifiable =
        (BaseType & Indirect) ||
        ((BaseType & PixelBuffer) && (Modifiers & Pack)) ||
        ((BaseType & Constants) && (Modifiers & (ReadWrite | Atomic))) ||
        (BaseType & Feedback);
    static constexpr bool gpu_readable =
        (BaseType & (Constants | Vertex | Elements)) ||
        ((BaseType & PixelBuffer) && (Modifiers & Unpack));

    /* CPU attributes */
    static constexpr bool cpu_modifiable =
        (BaseType & (Vertex | Elements | Indirect | PixelBuffer)) ||
        ((BaseType & Constants) && (Modifiers & (ReadOnly | ReadWrite)));
    static constexpr bool cpu_readable =
        (BaseType & (Vertex | Elements | Constants | Indirect)) ||
        ((BaseType & PixelBuffer) && (Modifiers & Pack));

    enum ex
    {
        value = BaseType | Modifiers
    };

    FORCEDINLINE operator buf_flag()
    {
        return value;
    }
};

using buf_vertex   = buf_basetype<Vertex, 0>;
using buf_elements = buf_basetype<Elements, 0>;
using buf_feedback = buf_basetype<Feedback, 0>;

using buf_draw_indirect    = buf_basetype<Indirect, 0>;
using buf_compute_indirect = buf_basetype<Indirect, Compute>;

using buf_constants_ro = buf_basetype<Constants, ReadOnly>;
using buf_constants_rw = buf_basetype<Constants, ReadWrite>;
using buf_atomic       = buf_basetype<Constants, Atomic>;
using buf_query        = buf_basetype<Constants, Query>;

using buf_pixel        = buf_basetype<PixelBuffer, 0>;
using buf_pixel_pack   = buf_basetype<PixelBuffer, Pack>;
using buf_pixel_unpack = buf_basetype<PixelBuffer, Unpack>;

} // namespace buffer_composition

namespace buf {

using flags = buffer_composition::buf_flag;

using vertex           = buffer_composition::buf_vertex;
using elements         = buffer_composition::buf_elements;
using feedback         = buffer_composition::buf_feedback;
using draw_indirect    = buffer_composition::buf_draw_indirect;
using compute_indirect = buffer_composition::buf_compute_indirect;
using constants_ro     = buffer_composition::buf_constants_ro;
using constants_rw     = buffer_composition::buf_constants_rw;
using atomic           = buffer_composition::buf_atomic;
using query            = buffer_composition::buf_query;

using pixel        = buffer_composition::buf_pixel;
using pixel_pack   = buffer_composition::buf_pixel_pack;
using pixel_unpack = buffer_composition::buf_pixel_unpack;

} // namespace buf

} // namespace graphics
} // namespace typing
