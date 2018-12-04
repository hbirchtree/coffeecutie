#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {

enum class TexType
{
    None,
    T2D,
    T2A,
    T3D,
    Cube
};

namespace texture_composition {


enum TextureType_t
{
    /* Bitshifting offsets */
    BASEFMT_OFFSET   = 0,
    MODIFIER_OFFSET  = BASEFMT_OFFSET + 3,
    DIRECTION_OFFSET = MODIFIER_OFFSET + 3,

    /* Base formats */
    T_3D   = 1 << (BASEFMT_OFFSET + 0),
    T_2D   = 1 << (BASEFMT_OFFSET + 1),
    T_Cube = 1 << (BASEFMT_OFFSET + 2),

    /* Modifiers */
    Proxy        = 1 << (MODIFIER_OFFSET + 0),
    Array        = 1 << (MODIFIER_OFFSET + 1),
    Multisampled = 1 << (MODIFIER_OFFSET + 2),

    /* For cubemaps, the direction of a given texture */
    North = 1 << (DIRECTION_OFFSET + 0),
    South = 1 << (DIRECTION_OFFSET + 1),
    West  = 1 << (DIRECTION_OFFSET + 2),
    East  = 1 << (DIRECTION_OFFSET + 3),
    Up    = 1 << (DIRECTION_OFFSET + 4),
    Down  = 1 << (DIRECTION_OFFSET + 5),

    DIRECTIONS_MASK = North | South | West | East | Up | Down,
};

using tex_flag = unsigned int;

template<tex_flag BaseFmt, tex_flag Modifiers>
struct tex_basetype
{
    static constexpr tex_flag base_fmt = BaseFmt;

    static constexpr bool array        = C_CAST<bool>(Modifiers & Array);
    static constexpr bool multisampled = C_CAST<bool>(Modifiers & Array);
    static constexpr bool proxy        = C_CAST<bool>(Modifiers & Array);

    enum ex
    {
        value = BaseFmt | Modifiers
    };

    template<tex_flag Direction>
    static constexpr tex_flag Specify()
    {
        return value + Direction;
    }

    FORCEDINLINE operator tex_flag()
    {
        return value;
    }
};

using tex_2d   = tex_basetype<T_2D, 0>;
using tex_cube = tex_basetype<T_Cube, 0>;
using tex_3d   = tex_basetype<T_3D, 0>;

using tex_2d_ms       = tex_basetype<T_2D, Multisampled>;
using tex_2d_array_ms = tex_basetype<T_2D, Array | Multisampled>;

using tex_2d_array   = tex_basetype<T_2D, Array>;
using tex_cube_array = tex_basetype<T_Cube, Array>;

} // namespace texture_composition

namespace tex {

using flag = texture_composition::tex_flag;

using t2d          = texture_composition::tex_2d;
using t2d_ms       = texture_composition::tex_2d_ms;
using t2d_array    = texture_composition::tex_2d_array;
using t2d_array_ms = texture_composition::tex_2d_array_ms;
using cube         = texture_composition::tex_cube;
using cube_array   = texture_composition::tex_cube_array;
using t3d          = texture_composition::tex_3d;

} // namespace tex

} // namespace graphics
} // namespace typing
