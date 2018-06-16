#pragma once

#include "enumfun.h"
#include <coffee/core/coffee_mem_macros.h>

namespace Coffee {

enum class GPUType
{
    Undefined,
    Dedicated,
    Integrated,
};

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
    Vertex   = 0x1,
    Fragment = 0x2,

    Compute  = 0x4,
    Geometry = 0x8,

    TessEval    = 0x10,
    TessControl = 0x20,

    All = Vertex | TessEval | TessControl | Geometry | Fragment | Compute
};

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

namespace TexComp {

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

    static constexpr bool array        = Modifiers & Array;
    static constexpr bool multisampled = Modifiers & Array;
    static constexpr bool proxy        = Modifiers & Array;

    enum ex { value = BaseFmt | Modifiers };

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

} // namespace TexComp

namespace tex {

using flag = TexComp::tex_flag;

using t2d          = TexComp::tex_2d;
using t2d_ms       = TexComp::tex_2d_ms;
using t2d_array    = TexComp::tex_2d_array;
using t2d_array_ms = TexComp::tex_2d_array_ms;
using cube         = TexComp::tex_cube;
using cube_array   = TexComp::tex_cube_array;
using t3d          = TexComp::tex_3d;

} // namespace tex

namespace BufferComp {

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

    enum ex { value = BaseType | Modifiers };

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

} // namespace BufferComp

namespace buf {

using flags = BufferComp::buf_flag;

using vertex           = BufferComp::buf_vertex;
using elements         = BufferComp::buf_elements;
using feedback         = BufferComp::buf_feedback;
using draw_indirect    = BufferComp::buf_draw_indirect;
using compute_indirect = BufferComp::buf_compute_indirect;
using constants_ro     = BufferComp::buf_constants_ro;
using constants_rw     = BufferComp::buf_constants_rw;
using atomic           = BufferComp::buf_atomic;
using query            = BufferComp::buf_query;

using pixel        = BufferComp::buf_pixel;
using pixel_pack   = BufferComp::buf_pixel_pack;
using pixel_unpack = BufferComp::buf_pixel_unpack;

} // namespace buf

namespace QueryComp {

enum QueryType_t
{
    Samples    = 1 << 0,
    Primitives = 1 << 1,
    Timing     = 1 << 2,

    /* Modifiers */
    Feedback     = 1 << (8 + 0),
    Conservative = 1 << (8 + 1),
    Any          = 1 << (8 + 2),
};

using query_flag = unsigned int;

template<query_flag Base, query_flag Mod>
struct query_basetype
{
    enum ex { value = Base | Mod };

    FORCEDINLINE operator query_flag()
    {
        return value;
    }
};

using query_any_samples = query_basetype<Samples, Any>;
using query_any_samples_conservative =
    query_basetype<Samples, Any | Conservative>;
using query_samples = query_basetype<Samples, 0>;

using query_primitives          = query_basetype<Primitives, 0>;
using query_feedback_primitives = query_basetype<Primitives, Feedback>;

using query_timing = query_basetype<Timing, 0>;

} // namespace QueryComp

namespace query {

using flags = QueryComp::query_flag;

using any_samples              = QueryComp::query_any_samples;
using any_samples_conservative = QueryComp::query_any_samples_conservative;
using samples                  = QueryComp::query_samples;
using primitives               = QueryComp::query_primitives;
using feedback_primitives      = QueryComp::query_feedback_primitives;
using timing                   = QueryComp::query_timing;

} // namespace query

/*!
 * \brief Primitive types. Sorted such that, with 0,0
 * for Prim and PrimCre, it defaults to explicit triangles.
 */
enum class Prim
{
    Triangle,
    Point,
    Line,
    Patch,
};

enum class PrimCre
{
    Explicit,
    Strip,
    Fan,
    Loop,
    Adjacency,

    MaxValue,
};

namespace ShaderTypes {
enum ShaderType_t
{
    None = 0x0,

    /* Data types */
    Scalar_t  = 1 << 0,
    Int_t     = 1 << 1,
    UInt_t    = 1 << 2,
    BScalar_t = 1 << 3,

    /* Advanced types */
    Atomic_t = 1 << 4,
    SubRt_t  = 1 << 5,

    UniBuf_t = 1 << 6,
    ShSBuf_t = 1 << 7,
    TFBuf_t  = 1 << 8,

    TypeMask_f = 0x1FF,

    /* Variable types */
    Sampler_v = 1 << 9,
    Uniform_v = 1 << 10,
    Attrib_v  = 1 << 11,
    FrgBuf_v  = 1 << 12,

    TFVar_v = 1 << 13,

    StorageMask_f = 0x3E00,

    /* Delimiters */
    Vec_d = 1 << 14,
    Mat_d = 1 << 15,

    DelimitMask_f = 0xC000,

    /* Sizes */
    S1 = 1 << 16,

    S2 = 1 << 17,
    S3 = 1 << 18,
    S4 = 1 << 19,

    /* Special sauce matrix dimensions */
    S2x3 = 1 << 20,
    S2x4 = 1 << 21,
    S3x2 = 1 << 22,
    S3x4 = 1 << 23,
    S4x2 = 1 << 24,
    S4x3 = 1 << 25,

    /* Just to make sense of samplers */
    S2A    = 1 << 26,
    SCube  = 1 << 27,
    SCubeA = 1 << 28,

    Depth = 1 << 29,

    SizeMask_f = 0xFFFF0000,
};

using sdt_flag = unsigned int;

/* TODO: Add verification that each template argument is valid */

template<sdt_flag DataType, sdt_flag DataStructure, sdt_flag Storage>
struct sdt
{
    static const constexpr sdt_flag value = DataType | DataStructure | Storage;
};

template<sdt_flag DataType, sdt_flag DataStructure>
struct sdt_unif : public sdt<DataType, DataStructure, Uniform_v>
{
};

template<sdt_flag DataStructure>
struct sdt_uniff : public sdt_unif<Scalar_t, DataStructure>
{
};

template<sdt_flag DataStructure>
struct sdt_unifd : public sdt_unif<BScalar_t, DataStructure>
{
};

template<sdt_flag DataStructure>
struct sdt_unifi : public sdt_unif<Int_t, DataStructure>
{
};

template<sdt_flag DataStructure>
struct sdt_unifu : public sdt_unif<UInt_t, DataStructure>
{
};

template<sdt_flag DataType, sdt_flag DataStructure>
struct sdt_samp : public sdt<DataType, DataStructure, Sampler_v>
{
};

template<sdt_flag DataStructure>
struct sdt_sampf : public sdt_samp<Scalar_t, DataStructure>
{
};

template<sdt_flag DataStructure>
struct sdt_sampd : public sdt_samp<BScalar_t, DataStructure>
{
};

template<sdt_flag DataType, sdt_flag DataStructure>
struct sdt_ubuf : public sdt<DataType, DataStructure, UniBuf_t>
{
};

template<sdt_flag DataType, sdt_flag DataStructure>
struct sdt_attr : public sdt<DataType, DataStructure, Attrib_v>
{
};

enum ProgramData_t
{
    PerPatch = 1 << 26,
};
} // namespace ShaderTypes

C_FLAGS(PrimCre, u32);
C_FLAGS(DBuffers, u32);
C_FLAGS(FramebufferT, u32);

} // namespace Coffee
