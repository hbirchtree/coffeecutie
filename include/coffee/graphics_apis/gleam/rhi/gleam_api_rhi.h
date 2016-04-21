#pragma once

#include "gleam_types_rhi.h"
#include "gleam_shader_rhi.h"

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
        UniformT = 0x0,
        UBufferT = 0x1,
        SBufferT = 0x2,
        SamplerT = 0x3,
        ImageT = 0x4,

        UniformStorageMask = UBufferT|SBufferT|SamplerT,

        ScalarT = 0x0,
        IntegerT = 0x8,
        UIntegerT = 0x10,
        BigScalarT = 0x20,

        UniformDataMask = IntegerT|UIntegerT|ScalarT|BigScalarT,
    };

    enum VectorFlags
    {
        Mat2T = 0x40,
        Mat3T = 0x80,
        Mat4T = 0x100,

        Mat2x4T = 0x200,
        Mat2x3T = 0x400,

        Mat3x2T = 0x800,
        Mat3x4T = 0x1000,

        Mat4x2T = 0x2000,
        Mat4x3T = 0x4000,

        Vec2T = 0x8000,
        Vec3T = 0x10000,
        Vec4T = 0x20000,
    };

    enum SamplerFlags
    {
        Sam2D = 0x40,
        Sam3D = 0x80,
        Sam2DA = 0x100,
        SamCube = 0x200,
        SamCubeA = 0x400,
    };

    using StencilState = GraphicsAPI::StencilState<CGenum,CGenum>;
    using DepthState = GraphicsAPI::DepthState<CGenum>;

    using DrawCall = GraphicsAPI::DrawCall;

    /* These access the PBO queue */
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface3D;
    friend struct GLEAM_SurfaceCube;
    friend struct GLEAM_Surface2DArray;
    friend struct GLEAM_SurfaceCubeArray;

    /* Just defining nicer names */
    using S_2D = GLEAM_Surface2D;
    using S_3D = GLEAM_Surface3D;
    using S_Cube = GLEAM_SurfaceCube;

    using S_2DA = GLEAM_Surface2DArray;
    using S_CubeA = GLEAM_SurfaceCubeArray;

    using SM_2D = GLEAM_Sampler2D;
    using SM_3D = GLEAM_Sampler3D;
    using SM_Cube = GLEAM_SamplerCube;

    using SM_2DA = GLEAM_Sampler2DArray;
    using SM_CubeA = GLEAM_SamplerCubeArray;

    using SHD = GLEAM_Shader;
    using PIP = GLEAM_Pipeline;

    using V_ATTR = GLEAM_VertAttribute;
    using V_DESC = GLEAM_VertDescriptor;

    using BUF_A = GLEAM_ArrayBuffer;
    using BUF_E = GLEAM_ElementBuffer;
    using BUF_U = GLEAM_UniformBuffer;
    using BUF_S = GLEAM_ShaderBuffer;
    using BUF_P = GLEAM_PixelBuffer;

    using D_CALL = DrawCall;
    using D_DATA = DrawInstanceData;

    using UNIFDESC = GLEAM_UniformDescriptor;
    using UNIFVAL = GLEAM_UniformValue;

    using USTATE = GLEAM_ShaderUniformState;
    using RASTSTATE = RasterizerState;
    using VIEWSTATE = ViewportState;
    using BLNDSTATE = BlendState;
    using DEPTSTATE = DepthState;
    using STENSTATE = StencilState;
    using TSLRSTATE = TessellatorState;
    using PIXLSTATE = PixelProcessState;

protected:
    struct GLEAM_Instance_Data;
    static thread_local GLEAM_Instance_Data* instance_data;

public:
    static void LoadAPI();

    /* i specifies view index for indexed views, 0 for  */
    static void SetRasterizerState(RasterizerState const& rstate, uint32 i = 0);
    static void SetViewportState(ViewportState const& vstate, uint32 i = 0);
    static void SetBlendState(BlendState const& bstate, uint32 i = 0);
    static void SetDepthState(DepthState const& dstate, uint32 i = 0);
    static void SetStencilState(StencilState const& sstate, uint32 i = 0);

    static void GetShaderUniformState(GLEAM_Pipeline const& pipeline,
                                      Vector<GLEAM_UniformDescriptor>* uniforms)
    {
        GLEAM::GetShaderUniforms(pipeline,uniforms);
    }

    static void SetTessellatorState(TessellatorState const& tstate);
    static void SetPixelProcessState(PixelProcessState const& pstate);
    static void SetShaderUniformState(const GLEAM_Pipeline &pipeline, ShaderStage const& stage,
                                      GLEAM_ShaderUniformState const& ustate);

    static void Draw(DrawCall const& d,DrawInstanceData const& i);
    static void DrawConditional(DrawCall const& d,DrawInstanceData const& i,OccludeQuery const& c);
};

}
}
}
