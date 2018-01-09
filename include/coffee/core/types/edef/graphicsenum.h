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
    None,
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

enum class TexType
{
    None, T2D, T2A, T3D, Cube
};

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

namespace ShaderTypes
{
    enum ShaderType_t
    {
        None      = 0x0,

        /* Data types */
        Scalar_t  = 1 << 0,
        Int_t     = 1 << 1,
        UInt_t    = 1 << 2,
        BScalar_t = 1 << 3,

        /* Advanced types */
        Atomic_t  = 1 << 4,
        SubRt_t   = 1 << 5,

        UniBuf_t  = 1 << 6,
        ShSBuf_t  = 1 << 7,
        TFBuf_t   = 1 << 8,

        TypeMask_f = 0x1FF,

        /* Variable types */
        Sampler_v = 1 << 9,
        Uniform_v = 1 << 10,
        Attrib_v  = 1 << 11,
        FrgBuf_v  = 1 << 12,

        TFVar_v   = 1 << 13,

        StorageMask_f = 0x3E00,

        /* Delimiters */
        Vec_d     = 1 << 14,
        Mat_d     = 1 << 15,

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
        S2A = 1 << 26,
        SCube = 1 << 27,
        SCubeA = 1 << 28,

        Depth = 1 << 29,

        SizeMask_f = 0xFFFF0000,
    };

    using sdt_flag = unsigned int;

    /* TODO: Add verification that each template argument is valid */

    template<sdt_flag DataType, sdt_flag DataStructure, sdt_flag Storage>
    struct sdt
    {
        static const constexpr sdt_flag value = DataType|DataStructure|Storage;
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
}

C_FLAGS(PrimCre,uint32);
C_FLAGS(DBuffers,uint32);
C_FLAGS(FramebufferT,uint32);

}
