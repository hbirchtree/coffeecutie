#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/core/platform_data.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/interfaces/cgraphics_pixops.h>

#include <coffee/core/CProfiling>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
using GLC = CGL33;
#else
using GLC = CGL43;
#endif

GraphicsAPI_Threading::GraphicsQueue& GLEAM_API::Queue(u32 idx)
{
    C_PTR_CHECK(m_store);

    if(idx != 0)
        throw std::invalid_argument("multi-threading not supported");

    return GLEAM_API_THREAD;
}

void GLEAM_API::DumpFramebuffer(
    GLEAM_API::FB_T& fb, PixFmt c, BitFmt dt, Vector<byte_t>& storage)
{
    auto size = fb.size();
    if(size.area() <= 0)
        return;

    storage.resize(
        GetPixSize(BitFormat::UByte, GetPixComponent(c), size.area()));

    fb.use(FramebufferT::Read);
    CGL33::ReadPixels(0, 0, size, GetPixComponent(c), dt, &storage[0]);
}

void GLEAM_API::GetDefaultVersion(int32& major, int32& minor)
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    major = 4;
    minor = 6;
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
    major               = 3;
    minor               = 2;
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
    major = 2;
    minor = 0;
#else
    Throw(implementation_error("unhandled GL version"));
#endif
}

void GLEAM_API::GetDefaultProperties(Display::CDProperties& props)
{
    props.gl.flags |= Display::GLProperties::GLNoFlag
#if !defined(COFFEE_DISABLE_SRGB_SUPPORT)
                      | Display::GLProperties::GLSRGB
#endif
#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
                      | Display::GLProperties::GLES
#endif
#if defined(COFFEE_ALWAYS_VSYNC)
                      | Display::GLProperties::GLVSync;
#endif
    ;

#if defined(COFFEE_USE_IMMERSIVE_VIEW)
    props.flags ^= Display::CDProperties::Windowed;
    props.flags |= Display::CDProperties::FullScreen;
#endif

#if defined(COFFEE_MAEMO)
    props.size.w        = 800;
    props.size.h        = 480;
    props.gl.bits.alpha = 0;
    props.gl.bits.red   = 5;
    props.gl.bits.green = 6;
    props.gl.bits.blue  = 5;
#else
    props.gl.bits.alpha = 8;
    props.gl.bits.red   = 8;
    props.gl.bits.blue  = 8;
    props.gl.bits.green = 8;

    props.gl.bits.depth   = 24;
    props.gl.bits.stencil = 8;
#endif
    props.gl.bits.samples = 0;
}

void InstanceDataDeleter::operator()(GLEAM_Instance_Data* p)
{
    delete p;
}

static bool SetAPIVersion(GLEAM_API::DataStore store, APILevel& systemLevel)
{
    cVerbose(10, GLM_API "Getting GL context version");
    auto ver = CGL::Debug::ContextVersion();

    cVerbose(10, GLM_API "Matching GL API...");
#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
    cVerbose(8, GLM_API "Checking GL core versions");

    cVerbose(12, GLM_API "Constructing GL version structures");
    const Display::CGLVersion ver33(3, 3);
    const Display::CGLVersion ver43(4, 3);
    const Display::CGLVersion ver45(4, 5);
    const Display::CGLVersion ver46(4, 6);

    /* If higher level of API is not achieved, stay at the lower one */
    if(ver >= ver46)
        /* Unimplemented both on CGL level and here */
        store->CURR_API = GL_4_6;
    else if(ver >= ver45)
        store->CURR_API = GL_4_5;
    else if(ver >= ver43)
        store->CURR_API = GL_4_3;
    else if(ver >= ver33)
        store->CURR_API = GL_3_3;
#else
    cVerbose(8, GLM_API "Checking GLES versions");

    cVerbose(12, GLM_API "Constructing GL version structures");

    const Display::CGLVersion ver20es(2, 0);

#if GL_VERSION_VERIFY(0x330, 0x300)
    const Display::CGLVersion ver30es(3, 0);
    const Display::CGLVersion ver32es(3, 2);

    if(ver >= ver32es)
        store->CURR_API = GLES_3_2;
    else if(ver >= ver30es)
        store->CURR_API = GLES_3_0;
    else
#endif
        if(ver >= ver20es)
        store->CURR_API = GLES_2_0;
#endif

    systemLevel = store->CURR_API;
    auto prevApi = store->CURR_API;

#if GL_VERSION_VERIFY(0x300, 0x300)
    /* Emulation mode; differs slightly from compiling against an API,
     *  such as when ES 2.0 excludes pixel formats and etc. */
    /* TODO: Document this feature */
    if(Env::ExistsVar("GLEAM_API"))
    {
        auto level = Env::GetVar("GLEAM_API");

        store->CURR_API = gl_level_from_string(level);

        if(store->CURR_API == GL_Nothing)
            throw undefined_behavior("invalid GLEAM API: " + level);
    }

    /* If we are emulating ES 2.0, create a global vertex array.
     *  All other code assumes that VertexAttribPointer
     * runs without binding a VAO. */
    /* TODO: Destroy this object when performing reload */
    if(prevApi != GLES_2_0 && store->CURR_API == GLES_2_0)
    {
        GLuint vao = 0;
        CGL33::VAOAlloc(vao);
        CGL33::VAOBind(vao);
    }
#endif

    cVerbose(8, GLM_API "Got API: {0}", store->CURR_API);

    if(store->CURR_API == GL_Nothing)
    {
        cWarning(
            GLM_API "Totally failed to create a GLEAM context,"
                    " got version: {0}",
            ver);
        CGL::Debug::UnsetDebugGroup();
        return false;
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(Extensions::SRGB_Supported())
    {
        cVerbose(6, GLM_API "Enabling SRGB color for framebuffers");
        CGL33::Enable(Feature::FramebufferSRGB);
    }
#endif

    cVerbose(
        4,
        GLM_API "Initialized API level {0}",
        StrUtil::pointerify(store->CURR_API));

    return true;
}

static void ConstructContextObjects(GLEAM_API::DataStore store)
{
#if GL_VERSION_VERIFY(0x330, 0x300)
    do
    {
        const szptr num_pbos = 5;
        cVerbose(7, GLM_API "Creating PBO storage, {0} units", num_pbos);

        /* Check if someone else has initialized it */
        if(store->inst_data->pboQueue.buffers.size() == num_pbos)
            break;

        Vector<CGhnd> bufs;
        bufs.resize(num_pbos);

        CGL33::BufAlloc(bufs);

        store->inst_data->pboQueue.buffers.reserve(num_pbos);
        for(uint32 i = 0; i < num_pbos; i++)
        {
            GLEAM_PboQueue::Pbo pbo;
            pbo.buf   = bufs[i];
            pbo.flags = 0;
            store->inst_data->pboQueue.buffers.push_back(pbo);
        }
    } while(false);
#endif
}

static void SetAPIFeatures(GLEAM_API::DataStore store)
{
    auto api        = store->CURR_API;
    bool is_desktop = APILevelIsOfClass(api, APIClass::GLCore);

    auto& features = store->features;

    features.is_gles = !is_desktop;
    features.gles20  = (api == GLES_2_0);

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    features.direct_state =
        features.direct_state || (is_desktop && api >= GL_4_5);
    features.base_instance =
        features.direct_state || (is_desktop && api >= GL_4_6);
#endif

    features.draw_base_instance =
        (api != GLES_2_0) && (api != GLES_3_0) && (api != GL_3_3);
    features.draw_multi_indirect =
        (api == GL_4_3) || (api == GL_4_5) || (api == GL_4_6);
    features.draw_indirect = (api == GL_4_3) || (api == GL_4_5) ||
                             (api == GL_4_6) || (api == GLES_3_2);
    features.draw_buffers_blend = is_desktop && (api != GL_3_3);
    features.draw_color_mask    = is_desktop || (api == GLES_3_2);

    /* For BaseInstance to be fully effective, we need
     *  both the gl_BaseInstanceARB GLSL variable
     *  and Draw*BaseInstance. */
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    features.base_instance =
#endif
        features.draw_base_instance =
            features.base_instance && features.draw_base_instance;

    features.draw_multi_indirect =
        features.draw_multi_indirect && features.draw_base_instance;

    features.rasterizer_discard = (api != GLES_2_0);
    features.depth_clamp        = is_desktop && false;
    features.viewport_indexed   = (api != GLES_2_0) && (api != GLES_3_0);

    features.separable_programs = is_desktop && (api != GL_3_3);

    features.texture_storage = (api != GL_3_3) && (api != GLES_2_0);

    features.buffer_storage = (api == GL_4_3);
    features.buffer_persistent =
        (api != GL_3_3) && (api != GLES_2_0) && features.buffer_storage;
    features.vertex_format = (api == GL_4_3) || (api == GLES_3_2) ||
                             (api == GL_4_5) || (api == GL_4_6);

    features.element_buffer_bind =
        !is_desktop || (is_desktop && store->CURR_API < GL_4_5);
}

static void SetExtensionFeatures(GLEAM_API::DataStore store)
{
#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
//    if(store->features.qcom_tiling)
//    {
//        cVerbose(5, "Qualcomm tiling enabled");
//    }
#endif

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    /* base_instance is const false on GLES */

    store->features.base_instance = Extensions::DrawParametersSupported();
    store->features.direct_state  = Extensions::DirectStateSupported();

#endif
}

static void SetCompatibilityFeatures(
    GLEAM_API::DataStore store, bool forced_api)
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(forced_api && store->CURR_API < GL_4_5 && store->CURR_API < GLES_MIN)
    {
        store->features.base_instance = false;
        store->features.direct_state  = false;
    }

    /* If we are emulating, base_instance would skew the results */
    if(APILevelIsOfClass(store->CURR_API, APIClass::GLES))
    {
        store->features.base_instance = false;
        store->features.direct_state  = false;
    }
#endif
}

bool GLEAM_API::LoadAPI(
    DataStore store, bool debug, GLEAM_API::OPTS const& options)
{
    DProfContext _(GLM_API "LoadAPI()");

    CGL::Debug::SetDebugGroup("Loading GLEAM");

    if(m_store)
    {
        cVerbose(8, GLM_API "Data store already created, skipping");
        return true;
    }

    cVerbose(8, GLM_API "Entering LoadAPI()");
    if(!store)
    {
        cWarning(GLM_API "No data store provided");
        CGL::Debug::UnsetDebugGroup();
        return false;
    }

    runtime_queue_error ec;
    store->GpuThread = GraphicsQueue(RuntimeQueue::GetCurrentQueue(ec));
    C_ERROR_CHECK(ec);

    cVerbose(8, GLM_API "Creating instance data");
    store->inst_data = MkUqDST<GLEAM_Instance_Data, InstanceDataDeleter>();

    store->options = options;
#ifndef NDEBUG
    store->DEBUG_MODE = debug;
    if(!debug)
    {
        Debug::SetDebugMode(false);
    }
#endif

    APILevel systemLevel;
    SetAPIVersion(store, systemLevel);

    bool forced_api = (store->CURR_API != systemLevel);

    ConstructContextObjects(store);

    /* First, check if an extension is available for behavior */
    if(!Env::ExistsVar("GLEAM_DISABLE_EXTENSIONS"))
        SetExtensionFeatures(store);
    SetAPIFeatures(store);

    /* Disable features when emulating lower API levels */
    SetCompatibilityFeatures(store, forced_api);

    m_store = store;

    DefaultFramebuffer().size();

    CGL::Debug::UnsetDebugGroup();

    return true;
}

bool GLEAM_API::UnloadAPI()
{
    m_store = nullptr;
    return true;
}

/* Future improvement: cache changes, or maybe rely on driver for that */

GLEAM_API::API_CONTEXT GLEAM_API::GetLoadAPI(GLEAM_API::OPTS const& options)
{
    cVerbose(8, GLM_API "Returning GLEAM loader...");
    return [=](bool debug = false) {
        static GLEAM_DataStore m_gleam_data = {};
        cVerbose(8, GLM_API "Running GLEAM loader");
        return LoadAPI(&m_gleam_data, debug, options);
    };
}

bool Coffee::RHI::GLEAM::GLEAM_API::IsAPILoaded()
{
    return m_store != nullptr;
}

void GLEAM_API::SetRasterizerState(const RASTSTATE& rstate, uint32)
{
    {
        if(rstate.dither())
            GLC::Enable(Feature::Dither);
        else
            GLC::Disable(Feature::Dither);
    }

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
    GLC::PolygonMode(
        Face::Both,
        (rstate.wireframeRender()) ? DrawMode::Line : DrawMode::Fill);
#endif

    {
        if(rstate.discard())
            GLC::Enable(Feature::RasterizerDiscard);
        else
            GLC::Disable(Feature::RasterizerDiscard);
    }

    //    GLC::ColorLogicOp(rstate.colorOp());
    //    if(!GLEAM_FEATURES.draw_color_mask)
    //        GLC::ColorMask(rstate.colorMask());
    //    else
    //        GLC::ColorMaski(i, rstate.colorMask());

    if(rstate.culling())
    {
        GLC::Enable(Feature::Culling);
        GLC::CullFace(C_CAST<Face>(rstate.culling()) & Face::FaceMask);
    } else
        GLC::Disable(Feature::Culling);
}

void GLEAM_API::SetTessellatorState(const TSLRSTATE& tstate)
{
    C_USED(tstate);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(Extensions::TessellationSupported())
    {
        CGL43::PatchParameteri(PatchProperty::Vertices, tstate.patchCount());
        /*TODO: Add configurability for inner and outer levels in place of TCS
         */
    }
#endif
}

void GLEAM_API::SetViewportState(const VIEWSTATE& vstate, uint32 i)
{
    C_USED(i);
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    if(vstate.multiview())
    {
        if(Extensions::ViewportArraySupported())
        {
            Vector<CRectF> varr;

            for(uint32 k = i; k < vstate.viewCount(); k++)
            {
                CRectF& e = varr[k];
                auto    s = vstate.view(k);

                e.x = s.x;
                e.y = s.y;
                e.w = s.w;
                e.h = s.h;
            }

            CGL43::DepthRangeArrayv(
                i,
                vstate.viewCount(),
                C_RCAST<bigscalar const*>(&vstate.depth(i)));
            CGL43::ScissorArrayv(
                i, vstate.viewCount(), C_RCAST<i32 const*>(&vstate.scissor(i)));
            CGL43::ViewportArrayv(
                i, vstate.viewCount(), C_RCAST<scalar const*>(varr.data()));

            GLC::Enablei(Feature::ClipDist, 0);
            GLC::Enablei(Feature::ClipDist, 1);
            GLC::Enablei(Feature::ClipDist, 2);
            GLC::Enablei(Feature::ClipDist, 3);

        } else if(Extensions::ClipDistanceSupported())
        {
            /* TODO: Expand on this feature */
            GLC::Enablei(Feature::ClipDist, 0);
        }
    } else
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
    if(vstate.multiview())
    {
        /* Find some way to emulate multiview on GLES 3.0+ */
    } else
#else
    if(vstate.multiview())
    {
        /* We're screwed. Throw the framebuffer on the floor, quick. */
    } else
#endif
    {
        if(vstate.m_view.size() > 0)
        {
            auto    sview = vstate.view(0);
            CRect64 tview(sview.x, sview.y, sview.w, sview.h);
            GLC::Viewport(tview.x, tview.y, tview.size());
        }
        if(vstate.m_depth.size() > 0)
#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
            GLC::DepthRange(vstate.depth(0).near_, vstate.depth(0).far_);
#else
            GLC::DepthRangef(vstate.depth(0).near_, vstate.depth(0).far_);
#endif
        if(vstate.m_scissor.size() > 0)
        {
            auto    sview = vstate.scissor(0);
            CRect64 tview(sview.x, sview.y, sview.w, sview.h);
            GLC::Scissor(tview.x, tview.y, tview.size());
            GLC::Enable(Feature::ScissorTest);
        } else
            GLC::Disable(Feature::ScissorTest);
    }
}

void GLEAM_API::SetBlendState(const BLNDSTATE& bstate, uint32 i)
{
    if(!GLEAM_FEATURES.viewport_indexed)
    {
        if(bstate.blend())
        {
            GLC::Enable(Feature::Blend);
        } else
            GLC::Disable(Feature::Blend);
    }
#if GL_VERSION_VERIFY(0x300, 0x320)
    else if(GLEAM_FEATURES.viewport_indexed)
    {
        if(bstate.blend())
            GLC::Enablei(Feature::Blend, i);
        else
            GLC::Disablei(Feature::Blend, i);
    }
#endif

    if(bstate.blend())
    {
        if(!GLEAM_FEATURES.draw_buffers_blend)
        {
            if(bstate.additive())
            {
                GLC::BlendFunc(GL_SRC_ALPHA, GL_ONE);
                /*TODO: Add indexed alternative, BlendFunci */
            } else
                GLC::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
        else if(GLEAM_FEATURES.draw_buffers_blend)
        {
            if(bstate.additive())
            {
                CGL43::BlendFunci(i, GL_SRC_ALPHA, GL_ONE);
                /*TODO: Add indexed alternative, BlendFunci */
            } else
            {
                CGL43::BlendFunci(i, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
        }
#endif
    }

    /*TODO: Add more advanced blending options*/

    /*TODO: Find semantics for SampleCoverage*/
}

void GLEAM_API::SetDepthState(const DEPTSTATE& dstate, uint32 i)
{
    if(dstate.testDepth())
        GLC::Enable(Feature::DepthTest);
    else
    {
        GLC::Disable(Feature::DepthTest);
        return;
    }

    if(dstate.testDepth())
    {
        GLC::DepthMask(dstate.mask());

        if(dstate.fun())
            GLC::DepthFunc(to_enum(C_CAST<ValueComparison>(dstate.fun())));

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
        if(GLEAM_FEATURES.depth_clamp)
        {
            if(dstate.clampDepth())
                GLC::Enablei(Feature::DepthClamp, i);
            else
                GLC::Disablei(Feature::DepthClamp, i);
        }
#endif
        /*TODO: Implement clamping*/
    }
}

void GLEAM_API::SetStencilState(const STENSTATE& sstate, uint32 i)
{
    if(!GLEAM_FEATURES.viewport_indexed)
    {
        if(sstate.testStencil())
            GLC::Enable(Feature::StencilTest);
        else
            GLC::Disable(Feature::StencilTest);
    }
#if GL_VERSION_VERIFY(0x300, 0x320)
    else if(GLEAM_FEATURES.viewport_indexed)
    {
        if(sstate.testStencil())
            GLC::Enablei(Feature::StencilTest, i);
        else
            GLC::Disablei(Feature::StencilTest, i);
    }
#endif

    //    GLC::StencilFuncSeparate(Face::Both,0x00000000);

    /*TODO: Implement functionality for more operations */
}

void GLEAM_API::SetPixelProcessState(const PIXLSTATE& pstate, bool unpack)
{
    Throw(implementation_error("not implemented!"));

    /*TODO: Implement more processing switches */
}

void GLEAM_API::PreDrawCleanup()
{
    DPROF_CONTEXT_FUNC(GLM_API);
    CGL43::ShaderReleaseCompiler();
}

void GLEAM_API::DisposePixelBuffers()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    DPROF_CONTEXT_FUNC(GLM_API);

    auto& queue = GLEAM_API_INSTANCE_DATA->pboQueue;
    for(auto& buf : queue.buffers)
        CGL33::BufFree(buf.buf);

    queue.buffers.clear();
#endif
}

GLEAM_API::FB_T& GLEAM_API::DefaultFramebuffer()
{
    return m_store->DefaultFramebuffer;
}

APILevel GLEAM_API::Level()
{
    return GL_CURR_API;
}

const char* api_error::name() const noexcept
{
    return "RHI::GLEAM";
}

std::string api_error::message(int error_code) const
{
    APIError ec = C_CAST<APIError>(error_code);

    switch(ec)
    {
    case APIError::DrawNoIndexBuffer:
        return "No index buffer bound";
    case APIError::DrawNoElements:
    case APIError::DrawNoVertices:
        return "Drawcall with count==0";
    case APIError::DrawNoInstances:
        return "Drawcall with instanceCount==0";
    case APIError::DrawNullsizedMeshes:
        return "Drawcall has no meshes";
    case APIError::DrawNoMeshes:
        return "Drawcall has meshes, but null-sized";
    case APIError::UniformTypeUnhandled:
        return "Uniform type not handled";
    case APIError::UniformNoProgram:
        return "No program specified for uniforms";
    case APIError::UniformDataNullptr:
        return "Uniform data pointer was nullptr";
    default:
        Throw(implementation_error("message not implemented"));
    }
}

} // namespace GLEAM
} // namespace RHI

namespace Strings {
cstring to_string(RHI::GLEAM::APILevel lev)
{
    using LEV = RHI::GLEAM::APILevel;

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    switch(lev)
    {
    case LEV::GL_3_3:
        return "Desktop GL 3.3";
    case LEV::GL_4_3:
        return "Desktop GL 4.3";
    case LEV::GL_4_5:
        return "Desktop GL 4.5";
    case LEV::GL_4_6:
        return "Desktop GL 4.6";
    default:
        break;
    }
#else
    switch(lev)
    {
    case LEV::GLES_2_0:
        return "GL ES 2.0";
    case LEV::GLES_3_0:
        return "GL ES 3.0";
    case LEV::GLES_3_2:
        return "GL ES 3.2";
    default:
        break;
    }
#endif

    return "?";
}
} // namespace Strings
} // namespace Coffee
