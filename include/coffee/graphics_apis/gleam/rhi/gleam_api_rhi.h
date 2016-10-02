#pragma once

#include "gleam_types_rhi.h"
#include "gleam_shader_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Instance_Data;

struct GLEAM_API : GraphicsAPI
{

    /* Enumerations */
    enum TextureFlags
    {
        TextureDMABuffered = 1,
        TextureAutoMipmapped,
    };

    /* These access the PBO queue */
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface3D_Base;
    friend struct GLEAM_SurfaceCube;
    friend struct GLEAM_SurfaceCubeArray;
    friend struct GLEAM_PipelineDumper;

    /* Types and relationships */
    using PRF = GLEAM_PRF;

    using StencilState = GraphicsAPI::StencilState<CGenum,CGenum>;
    using DepthState = GraphicsAPI::DepthState<CGenum>;

    using DrawCall = GraphicsAPI::DrawCall;
    using DrawInstanceData = GraphicsAPI::DrawInstanceData;

    using OccludeQuery = GLEAM_OccludeQuery;

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

    using S_IMG = GLEAM_ShaderImage;

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
    using UNIFSMP = GLEAM_SamplerHandle;

    using USTATE = GLEAM_ShaderUniformState;
    using RASTSTATE = RasterizerState;
    using VIEWSTATE = ViewportState;
    using BLNDSTATE = BlendState;
    using DEPTSTATE = DepthState;
    using STENSTATE = StencilState;
    using TSLRSTATE = TessellatorState;
    using PIXLSTATE = PixelProcessState;

    using FB_T = GLEAM_RenderTarget;
    using RBUF = GLEAM_RenderDummy;

    using Q_OCC = OccludeQuery;

    /* "Loose" functions */
public:
    static void LoadAPI(bool debug = false);

    /* i specifies view index for indexed views, 0 for  */
    static void SetRasterizerState(RasterizerState const& rstate, uint32 i = 0);
    static void SetViewportState(ViewportState const& vstate, uint32 i = 0);
    static void SetBlendState(BlendState const& bstate, uint32 i = 0);
    static void SetDepthState(DepthState const& dstate, uint32 i = 0);
    static void SetStencilState(StencilState const& sstate, uint32 i = 0);

    static void GetShaderUniformState(GLEAM_Pipeline const& pipeline,
                                      Vector<GLEAM_UniformDescriptor>* uniforms,
                                      Vector<GLEAM_ProgramParameter>* params = nullptr)
    {
        GLEAM::GetShaderUniforms(pipeline,uniforms,params);
    }

    static void SetTessellatorState(TessellatorState const& tstate);
    static void SetPixelProcessState(PixelProcessState const& pstate, bool unpack = true);
    static void SetShaderUniformState(const GLEAM_Pipeline &pipeline, ShaderStage const& stage,
                                      GLEAM_ShaderUniformState const& ustate);

    static void PreDrawCleanup();

    static void Draw(DrawCall const& d,DrawInstanceData const& i);
    static void DrawConditional(DrawCall const& d,DrawInstanceData const& i,OccludeQuery const& c);

    /* Variables */
protected:
    static thread_local GLEAM_Instance_Data* instance_data;

public:
    static thread_local FB_T DefaultFramebuffer;
};

struct GLEAM_PRF
{
    using QRY_PERF = GLEAM_PrfQuery;
    using QRY_DBUF = GLEAM_DBufQuery;
    using QRY_PIPDMP = GLEAM_PipelineDumper;
};

}
}
}
