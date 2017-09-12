#pragma once

#include "gleam_types_rhi.h"
#include "gleam_shader_rhi.h"

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{

namespace RHI{
namespace GLEAM{

struct GLEAM_API : GraphicsAPI
{
    using API_CONTEXT = Function<bool(bool)>;

    using G_DEV = GraphicsDevice;
    using G_CTXT = GraphicsContext;

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
    using PPARAM = GLEAM_ProgramParameter;

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

    using DataStore = GLEAM_DataStore*;

    using PipelineState = Map<ShaderStage,const GLEAM_ShaderUniformState &>;

    using PSTATE = PipelineState;

    /* "Loose" functions */
public:
    /* Dump the framebuffer pixels to a buffer, might be asynchronous */
    static void DumpFramebuffer(FB_T& fb, PixelComponents c, TypeEnum dt,
                                Vector<byte_t>& storage);

    static void GetDefaultVersion(int32& major, int32& minor);

    static void GetDefaultProperties(Display::CDProperties& properties);

    static bool LoadAPI(DataStore store, bool debug = false);
    static bool UnloadAPI();

    static API_CONTEXT GetLoadAPI();

    /* i specifies view index for indexed views, 0 for  */
    static void SetRasterizerState(RASTSTATE const& rstate, uint32 i = 0);
    static void SetViewportState(VIEWSTATE const& vstate, uint32 i = 0);
    static void SetBlendState(BLNDSTATE const& bstate, uint32 i = 0);
    static void SetDepthState(DEPTSTATE const& dstate, uint32 i = 0);
    static void SetStencilState(STENSTATE const& sstate, uint32 i = 0);

    static void GetShaderUniformState(PIP const& pipeline,
                                      Vector<UNIFDESC>* uniforms,
                                      Vector<PPARAM>* params = nullptr)
    {
        GLEAM::GetShaderUniforms(pipeline,uniforms,params);
    }

    static void SetTessellatorState(TSLRSTATE const& tstate);
    static void SetPixelProcessState(PIXLSTATE const& pstate, bool unpack = true);
    static void SetShaderUniformState(const PIP &pipeline, ShaderStage const& stage,
                                      USTATE const& ustate);

    static void PreDrawCleanup();

    static void Draw(
            PIP const& pipeline,
            PSTATE const& ustate,
            V_DESC& vertices,
            D_CALL const& d,D_DATA const& i,
            Q_OCC* query = nullptr);
    static void DrawConditional(
            PIP const& pipeline,
            PSTATE const& ustate,
            V_DESC& vertices,
            D_CALL const& d,
            D_DATA const& i, Q_OCC &c);

    static FB_T& DefaultFramebuffer();

    using APIClass = RHI::GLEAM::APIClass;
    static APILevel Level();

    STATICINLINE bool LevelIsOfClass(APILevel l, APIClass c)
    {
        return APILevelIsOfClass(l, c);
    }

    static CString GetAPIName(GraphicsDevice const&);
    static bool GetAPIVersion(GraphicsDevice const&, SWVersionInfo*);
    static bool GetRendererInfo(GraphicsDevice const&, HWDeviceInfo*);
    static bool GetRendererDriverInfo(GraphicsDevice const&, SWVersionInfo*);
    static CString GetShaderLanguageName(GraphicsContext const&);
    static bool GetShaderLanguageVersion(GraphicsContext const&, SWVersionInfo*);
};

struct GLEAM_PRF
{
    using QRY_PERF = GLEAM_PrfQuery;
    using QRY_DBUF = GLEAM_DBufQuery;
    using QRY_PIPDMP = GLEAM_PipelineDumper;
    using DRAW_QUAD = GLEAM_Quad_Drawer;
};

}
}

namespace Strings{
extern cstring to_string(RHI::GLEAM::APILevel lev);
}
}
