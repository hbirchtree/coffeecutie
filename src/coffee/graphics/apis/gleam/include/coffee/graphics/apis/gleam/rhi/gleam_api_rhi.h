#pragma once

#include "gleam_shader_rhi.h"
#include "gleam_types_rhi.h"

#include <coffee/interfaces/file_resolver.h>
#include <peripherals/stl/types.h>

namespace Coffee::RHI::GLEAM {

struct GLEAM_API : GraphicsAPI
{
    using API_CONTEXT = Function<bool(bool)>;

    using ERROR = gleam_error;

    using OPTS = GLEAM_Options;

    using G_DEV  = GraphicsDevice;
    using G_CTXT = GraphicsContext;

    /* These access the PBO queue */
    friend struct GLEAM_Surface2D;
    friend struct GLEAM_Surface3D_Base;
    friend struct GLEAM_SurfaceCube;
    friend struct GLEAM_SurfaceCubeArray;
    friend struct GLEAM_PipelineDumper;

    /* Types and relationships */
    using PRF = GLEAM_PRF;

    using StencilState = GraphicsAPI::StencilState<u32, u32>;
    using DepthState   = GraphicsAPI::DepthState<u32>;

    using DrawCall         = GraphicsAPI::DrawCall;
    using DrawInstanceData = GraphicsAPI::DrawInstanceData;

    using OccludeQuery = GLEAM_OccludeQuery;

    /* Just defining nicer names */
    using S_2D   = GLEAM_Surface2D;
    using S_3D   = GLEAM_Surface3D;
    using S_Cube = GLEAM_SurfaceCube;

    using S_2DA   = GLEAM_Surface2DArray;
    using S_CubeA = GLEAM_SurfaceCubeArray;

    using SM_2D   = GLEAM_Sampler2D;
    using SM_3D   = GLEAM_Sampler3D;
    using SM_Cube = GLEAM_SamplerCube;

    using SM_2DA   = GLEAM_Sampler2DArray;
    using SM_CubeA = GLEAM_SamplerCubeArray;

    using S_IMG = GLEAM_ShaderImage;

    using SHD = GLEAM_Shader;
    using PIP = GLEAM_Pipeline;

    using V_ATTR = GLEAM_VertAttribute;
    using V_DESC = GLEAM_VertDescriptor;

    using BUF_A    = GLEAM_ArrayBuffer;
    using BUF_E    = GLEAM_ElementBuffer;
    using BUF_U    = GLEAM_UniformBuffer;
    using BUF_S    = GLEAM_ShaderBuffer;
    using BUF_P    = GLEAM_PixelBuffer;
    using BUF_DRAW = GLEAM_IndirectBuffer;

    using D_CALL = DrawCall;
    using D_DATA = DrawInstanceData;

    using UNIFDESC = GLEAM_UniformDescriptor;
    using UNIFVAL  = GLEAM_UniformValue;
    using UNIFSMP  = GLEAM_SamplerHandle;
    using PPARAM   = GLEAM_ProgramParameter;

    using USTATE    = GLEAM_ShaderUniformState;
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

    using DataStore = UqPtr<GLEAM::DataStore>;

    using PipelineState = Map<ShaderStage, GLEAM_ShaderUniformState*>;

    using PSTATE = PipelineState;

    struct CommandBuffer
    {
        WkPtr<V_DESC> vertices;
        PSTATE*       state;

        struct Command
        {
            D_CALL call;
            D_DATA data;
        };

        Vector<Command> commands;
    };

    struct OptimizedDraw
    {
        Vector<CommandBuffer> cmdBufs;

        struct IndirectCall
        {
            union
            {
                struct
                {
                    u32 count;
                    u32 instanceCount;
                    u32 firstIndex;
                    u32 baseVertex;
                    u32 baseInstance;
                } i;
                struct
                {
                    u32 count;
                    u32 instanceCount;
                    u32 first;
                    u32 baseInstance;
                } a;
            };
        };

        struct MultiDrawData
        {
            Vector<i32> counts;
            Vector<i64> offsets;
            Vector<i32> baseVertex;

            Vector<IndirectCall> indirectCalls;

            TypeEnum etype;
            D_CALL   dc;
        };

        Map<CommandBuffer*, MultiDrawData> multiDrawData;
    };

    using OPT_DRAW = OptimizedDraw;

    using OptMap = Map<CommandBuffer*, OPT_DRAW::MultiDrawData>;

    struct RenderPass
    {
        WkPtr<PIP>  pipeline;
        WkPtr<FB_T> framebuffer;

        BLNDSTATE blend;
        RASTSTATE raster;
        DEPTSTATE depth;

        struct DrawCall
        {
            DrawCall(WkPtr<V_DESC> v, PSTATE* s, D_CALL call, D_DATA data) :
                vertices(v), state(s), d_call(call), d_data(data)
            {
            }

            WkPtr<V_DESC> vertices;
            PSTATE*       state;
            D_CALL        d_call;
            D_DATA        d_data;
        };

        Vector<DrawCall> draws;
    };

    /* "Loose" functions */
  public:
    static GraphicsQueue& Queue(u32 idx = 0);

    /* Dump the framebuffer pixels to a buffer, might be asynchronous */
    static void DumpFramebuffer(
        FB_T& fb, PixFmt c, BitFmt dt, Vector<byte_t>& storage);

    static void GetDefaultVersion(i32& major, i32& minor);

    static void GetDefaultProperties(Display::Properties& properties);

    static bool LoadAPI(
        DataStore store, bool debug = false, GLEAM_Options const& options = {});
    static bool UnloadAPI();

    static bool IsAPILoaded();

    static API_CONTEXT GetLoadAPI(GLEAM_Options const& options = {});

    /* i specifies view index for indexed views, 0 for  */
    static void SetRasterizerState(RASTSTATE const& rstate);
    static void SetViewportState(VIEWSTATE const& vstate, u32 i = 0);
    static void SetBlendState(BLNDSTATE const& bstate, u32 i = 0);
    static void SetDepthState(DEPTSTATE const& dstate, u32 i = 0);
    static void SetStencilState(STENSTATE const& sstate, u32 i = 0);

    static void GetRasterizerState(RASTSTATE& rstate);
    static void GetViewportState(VIEWSTATE& vstate, u32 i = 0);
    static void GetBlendState(BLNDSTATE& bstate, u32 i = 0);
    static void GetDepthState(DEPTSTATE& dstate, u32 i = 0);
    static void GetStencilState(STENSTATE& sstate, u32 i = 0);

    static void GetShaderUniformState(
        PIP const&        pipeline,
        Vector<UNIFDESC>* uniforms,
        Vector<PPARAM>*   params  = nullptr,
        Vector<PPARAM>*   outputs = nullptr)
    {
        GLEAM::GetShaderUniforms(pipeline, uniforms, params, outputs);
    }

    static void SetTessellatorState(TSLRSTATE const& tstate);
    static void SetPixelProcessState(
        PIXLSTATE const& pstate, bool unpack = true);

    static void SetShaderUniformState(
        const PIP&         pipeline,
        ShaderStage const& stage,
        USTATE const&      ustate,
        gleam_error&       error_code);

    STATICINLINE void SetShaderUniformState(
        const PIP& pipeline, ShaderStage const& stage, USTATE const& ustate)
    {
        gleam_error error_code;
        SetShaderUniformState(pipeline, stage, ustate, error_code);
    }

    /*!
     * \brief Perform any possible work that removes memory allocations made
     * related to setting up drawing pipelines and similar.
     */
    static void PreDrawCleanup();

    /*!
     * \brief Dispose of pixel buffer objects, which are used
     *  for uploading textures asynchronously.
     * If you are going to use PBOs while rendering,
     *  do *not* call this.
     */
    static void DisposePixelBuffers();

    /*!
     * \brief Optimize a render pass into a buffer which may be consumed by
     * MultiDraw(). Will attempt to maximize use of instancing and glMultiDraw*
     * functionality, and also minimize state changes between draw calls (eg.
     * swapping less textures and uniforms). \param rpass A pre-defined render
     * pass by the user code \param buffer An output structure which is ready to
     * be drawn in an optimized fashion.
     */
    static void OptimizeRenderPass(
        RenderPass& rpass, OPT_DRAW& buffer, u32 baseinstance = 0);

    /*!
     * \brief Providing data output by OptimizeRenderPass(), use this function
     * to perform the best possible combination of drawcalls. \param pipeline
     * The pipeline with which the drawcalls are to be rendered with. This
     * method cannot optimize between different pipeline usages. \param draws
     * The drawcalls to be rendered
     */
    static void MultiDraw(PIP const& pipeline, OPT_DRAW const& draws)
    {
        gleam_error error_code;
        MultiDraw(pipeline, draws, error_code);
    }

    static void MultiDraw(
        PIP const& pipeline, OPT_DRAW const& draws, gleam_error& ec);

    /*!
     * \brief Draw objects with the provided data. Will mostly map to a single
     * drawcall. \param pipeline \param ustate \param vertices \param d \param i
     * \param query
     */
    static void Draw(
        PIP const&    pipeline,
        PSTATE const& ustate,
        V_DESC&       vertices,
        D_CALL const& d,
        D_DATA const& i,
        gleam_error&  ec,
        Q_OCC*        query = nullptr);

    static void Draw(
        PIP const&    pipeline,
        PSTATE const& ustate,
        V_DESC&       vertices,
        D_CALL const& d,
        D_DATA const& i,
        Q_OCC*        query = nullptr)
    {
        gleam_error ec;
        Draw(pipeline, ustate, vertices, d, i, ec, query);
    }

    /*!
     * \brief As Draw(), except it can be drawn depending on an occlusion query
     * output by a Draw() step with occlusion query capture. \param pipeline
     * \param ustate
     * \param vertices
     * \param d
     * \param i
     * \param c
     */
    static void DrawConditional(
        PIP const&    pipeline,
        PSTATE const& ustate,
        V_DESC&       vertices,
        D_CALL const& d,
        D_DATA const& i,
        Q_OCC&        c,
        gleam_error&  ec);

    static void DrawConditional(
        PIP const&    pipeline,
        PSTATE const& ustate,
        V_DESC&       vertices,
        D_CALL const& d,
        D_DATA const& i,
        Q_OCC&        c)
    {
        gleam_error ec;
        DrawConditional(pipeline, ustate, vertices, d, i, c, ec);
    }

    static ShPtr<FB_T> DefaultFramebuffer();

    using APIClass = RHI::GLEAM::APIClass;
    static APILevel Level();

    STATICINLINE bool LevelIsOfClass(APILevel l, APIClass c)
    {
        return APILevelIsOfClass(l, c);
    }

    static CString GetAPIName(GraphicsDevice const&);
    static bool    GetAPIVersion(GraphicsDevice const&, SWVersionInfo*);
    static bool    GetRendererInfo(GraphicsDevice const&, HWDeviceInfo*);
    static bool    GetRendererDriverInfo(GraphicsDevice const&, SWVersionInfo*);
    static CString GetShaderLanguageName(GraphicsContext const&);
    static bool    GetShaderLanguageVersion(
           GraphicsContext const&, SWVersionInfo*);

    static bool TextureFormatSupport(PixFmt fmt, CompFlags flags);

    struct DBG
    {
        using SCOPE = GLEAM_ScopeMarker;
    };
};

struct GLEAM_PRF
{
    using QRY_PERF   = GLEAM_PrfQuery;
    using QRY_DBUF   = GLEAM_DBufQuery;
    using QRY_PIPDMP = GLEAM_PipelineDumper;
    using DRAW_QUAD  = GLEAM_Quad_Drawer;
};

} // namespace Coffee::RHI::GLEAM

namespace Coffee::Strings {

extern cstring to_string(RHI::GLEAM::APILevel lev);

}
