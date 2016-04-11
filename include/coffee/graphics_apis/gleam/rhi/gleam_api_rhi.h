#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_API : GraphicsAPI
{
    enum TextureFlags
    {
        TextureDMABuffered = 1,
    };

    enum BufferFlags
    {

    };

    enum UniformFlags
    {
        UBuffer = 0x1,
        SBuffer = 0x2,

        Integer = 0x4,
        UInteger = 0x8,
        Scalar = 0x10,
        BigScalar = 0x20,

        Mat2 = 0x40,
        Mat3 = 0x80,
        Mat4 = 0x100,

        Mat2x4 = 0x200,
        Mat2x3 = 0x400,

        Mat3x2 = 0x800,
        Mat3x4 = 0x1000,

        Mat4x2 = 0x2000,
        Mat4x3 = 0x4000,

        Vec2 = 0x8000,
        Vec3 = 0x10000,
        Vec4 = 0x20000,
    };

    using StencilState = GraphicsAPI::StencilState<CGenum,CGenum>;
    using DepthState = GraphicsAPI::DepthState<CGenum>;

    /* These access the PBO queue */
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface3D;
    friend struct GLEAM_SurfaceCube;
    friend struct GLEAM_Surface2DArray;
    friend struct GLEAM_SurfaceCubeArray;

    /* Just defining nicer names */
    using Surface2D = GLEAM_Surface2D;
    using Surface3D = GLEAM_Surface3D;
    using SurfaceCube = GLEAM_SurfaceCube;

    using Surface2DArray = GLEAM_Surface2DArray;
    using SurfaceCubeArray = GLEAM_SurfaceCubeArray;

    using Sampler2D = GLEAM_Sampler2D;
    using Sampler3D = GLEAM_Sampler3D;
    using SamplerCube = GLEAM_SamplerCube;

    using Sampler2DArray = GLEAM_Sampler2DArray;
    using SamplerCubeArray = GLEAM_SamplerCubeArray;

    using Shader = GLEAM_Shader;
    using Pipeline = GLEAM_Pipeline;

    using VertexAttribute = GLEAM_VertAttribute;
    using VertexDescriptor = GLEAM_VertDescriptor;

    using ArrayBuffer = GLEAM_ArrayBuffer;
    using ElementBuffer = GLEAM_ElementBuffer;
    using UniformBuffer = GLEAM_UniformBuffer;
    using ShaderBuffer = GLEAM_ShaderBuffer;

protected:
    struct GLEAM_Instance_Data;
    static thread_local GLEAM_Instance_Data* instance_data;

public:
    static void LoadAPI();

    static void SetRasterizerState(RasterizerState const& rstate);
    static void SetTessellatorState(TessellatorState const& tstate);
    static void SetViewportState(ViewportState const& vstate);
    static void SetBlendState(BlendState const& bstate);
    static void SetDepthState(DepthState const& dstate);
    static void SetStencilState(StencilState const& sstate);
    static void SetPixelProcessState(PixelProcessState const& pstate);
    static void SetShaderUniformState(GLEAM_ShaderUniformState const& ustate);
};

}
}
}
