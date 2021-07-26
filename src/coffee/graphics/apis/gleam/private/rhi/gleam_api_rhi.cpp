#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

//#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
//#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/core/CProfiling>
#include <coffee/core/platform_data.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/properties.h>
#include <coffee/interfaces/cgraphics_pixops.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>
#include <peripherals/concepts/graphics_api.h>

#include <coffee/core/CDebug>

#include "gleam_internal_types.h"

using namespace ::platform;

namespace Coffee::RHI::GLEAM {

UqPtr<DataStore> m_store;

// GraphicsAPI_Threading::GraphicsQueue& GLEAM_API::Queue(u32 idx)
//{
//    C_PTR_CHECK(m_store);

//    if(idx != 0)
//        throw std::invalid_argument("multi-threading not supported");

//    return GLEAM_API_THREAD;
//}

void GLEAM_API::DumpFramebuffer(
    GLEAM_API::FB_T& fb, PixFmt c, BitFmt dt, Vector<byte_t>& storage)
{
    auto size = fb.size();
    if(size.area() <= 0)
        return;

    storage.resize(properties::get<properties::pixel_size>(
        BitFmt::UByte,
        typing::pixels::convert::to<PixCmp>(c),
        C_FCAST<szptr>(size.area())));

    fb.use(FramebufferT::Read);
    i32 len = 0;
    std::array<i32, 2> test;
//    glw::cmd::get_program_resourceiv(0, glw::program_interface::program_input, 0, std::vector<glw::program_resource_property>{
//            glw::program_resource_property::active_variables,
//            glw::program_resource_property::array_size,
//        }, len, test);
//    glw::cmd::read_pixels(
//        Veci2{0, 0}, size.convert<i32>(),
//        glw::pixel_format::rgba, glw::pixel_type::unsigned_byte,
//        semantic::mem_chunk<std::byte>::ofContainer(storage).view);
    //    gl::vlow::ReadPixels(
    //        {0, 0}, size, typing::pixels::convert::to<PixCmp>(c), dt,
    //        &storage[0]);
}

void GLEAM_API::GetDefaultVersion(i32& major, i32& minor)
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

void GLEAM_API::GetDefaultProperties(Display::Properties& props)
{
    props.gl.flags |= Display::GL::Properties::GLNoFlag
#if !defined(COFFEE_DISABLE_SRGB_SUPPORT)
                      | Display::GL::Properties::GLSRGB
#endif
#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
                      | Display::GL::Properties::GLES
#endif
#if defined(COFFEE_ALWAYS_VSYNC)
                      | Display::GL::Properties::GLVSync;
#endif
    ;

#if defined(COFFEE_USE_IMMERSIVE_VIEW)
    props.flags ^= Display::Properties::Windowed;
    props.flags |= Display::Properties::FullScreen;
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

void InstanceDataDeleter::operator()(InstanceData* p)
{
    delete p;
}

static bool SetAPIVersion(
    DataStore* store, APILevel& systemLevel, GLEAM_API::OPTS const& opts)
{
    cVerbose(10, GLM_API "Getting GL context version");
    Display::GL::Version ver = {};

    cVerbose(11, GLM_API "Got version {0}", ver);

    cVerbose(10, GLM_API "Matching GL API...");
#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
    const Display::GL::Version ver33(3, 3);
    const Display::GL::Version ver43(4, 3);
    const Display::GL::Version ver45(4, 5);
    const Display::GL::Version ver46(4, 6);

    /* If higher level of API is not achieved, stay at the lower one */
//    if(ver >= ver46)
//        /* Unimplemented both on CGL level and here */
//        store->CURR_API = GL_4_6;
//    else if(ver >= ver45)
//        store->CURR_API = GL_4_5;
//    else if(ver >= ver43)
//        store->CURR_API = GL_4_3;
//    else if(ver >= ver33)
//        store->CURR_API = GL_3_3;
#else
    const Display::GL::Version ver20es(2, 0);

#if GL_VERSION_VERIFY(0x330, 0x300)
    const Display::GL::Version ver30es(3, 0);
    const Display::GL::Version ver32es(3, 2);

    if(ver >= ver32es)
        store->CURR_API = GLES_3_2;
    else if(ver >= ver30es)
        store->CURR_API = GLES_3_0;
    else
#endif
        if(ver >= ver20es)
        store->CURR_API = GLES_2_0;
#endif

    systemLevel  = store->CURR_API;
    auto prevApi = store->CURR_API;

#if GL_VERSION_VERIFY(0x300, 0x300)
    /* Emulation mode; differs slightly from compiling against an API,
     *  such as when ES 2.0 excludes pixel formats and etc. */
    /* TODO: Document this feature */
    if(auto api_ver = env::var("GLEAM_API"); api_ver.has_value())
    {
        store->CURR_API = gl_level_from_string(api_ver.value());

        if(store->CURR_API == GL_Nothing)
            throw undefined_behavior("invalid GLEAM API: " + api_ver.value());
    }

    /* If we are emulating ES 2.0, create a global vertex array.
     *  All other code assumes that VertexAttribPointer
     * runs without binding a VAO. */
    /* TODO: Destroy this object when performing reload */
    if(prevApi != GLES_2_0 && store->CURR_API == GLES_2_0)
    {
        glhnd vao;
        glw::cmd::gen_vertex_arrays(SpanOne(vao.hnd));
//        gl::vlow::VAOAlloc(SpanOne(vao.hnd));
//        gl::vlow::VAOBind(vao);
        vao.release();
    }
#endif

    cVerbose(8, GLM_API "Got API: {0}", store->CURR_API);

    if(store->CURR_API == GL_Nothing)
    {
        cWarning(
            GLM_API "Totally failed to create a GLEAM context,"
                    " got version: {0}",
            ver);
//        glw::cmd::();
        return false;
    }

//#if GL_VERSION_VERIFY(0x300, 0x300)
//    if(Extensions::SRGB_Supported(store->inst_data->dbgContext) && opts.srgb)
//    {
//        cVerbose(6, GLM_API "Enabling SRGB color for framebuffers");
//        gl::vlow::Enable(Feature::FramebufferSRGB);
//    }
//#endif

//    cVerbose(
//        4,
//        GLM_API "Initialized API level {0}",
//        stl_types::str::print::pointerify(store->CURR_API));

//#if !MODE_LOWFAT
//    using DBG = CGL::Debug;
//    using LIM = CGL_Shared_Limits;

//    ExtraData::Add(
//        "glsl:version", Strings::to_string(DBG::ShaderLanguageVersion()));
//    ExtraData::Add("gl:version", Strings::to_string(DBG::ContextVersion()));
//    ExtraData::Add("gl:driver", DBG::ContextVersion().driver);
//    {
//        auto device   = DBG::Renderer();
//        auto renderer = platform::info::HardwareDevice(
//            device.manufacturer, device.model, {});
//        ExtraData::Add("gl:renderer", Strings::to_string(renderer));
//    }
//    ExtraData::Add("gl:vendor", DBG::Renderer().manufacturer);

//    CString props = {};
//    for(auto i : Range<u32>(LIM::Max_Property))
//        if(auto name = LIM::MaxName(i); name)
//        {
//            props += LIM::MaxName(i);
//            props += "=";
//            props += Strings::to_string(LIM::Max(i));
//            props += ",";
//        }
//    ExtraData::Add("gl:limits", props);
//    ExtraData::Add("gl:extensions", store->inst_data->dbgContext.extensionList);
//#endif

//    return true;
}

//static void ConstructContextObjects(GLEAM_API::DataStore const& store)
//{
//#if GL_VERSION_VERIFY(0x330, 0x300)
//    do
//    {
//        const szptr num_pbos = 5;
//        cVerbose(7, GLM_API "Creating PBO storage, {0} units", num_pbos);

//        /* Check if someone else has initialized it */
//        if(store->inst_data->pboQueue.buffers.size() == num_pbos)
//            break;

//        Vector<CGhnd> bufs;
//        bufs.resize(num_pbos);

//        gl::vlow::BufAlloc(bufs);

//        store->inst_data->pboQueue.buffers.reserve(num_pbos);
//        for(u32 i = 0; i < num_pbos; i++)
//        {
//            GLEAM_PboQueue::Pbo pbo;
//            pbo.buf   = bufs[i];
//            pbo.flags = 0;
//            store->inst_data->pboQueue.buffers.push_back(pbo);
//        }
//    } while(false);
//#endif
//}

//static void SetAPIFeatures(GLEAM_API::DataStore const& store)
//{
//    auto api        = store->CURR_API;
//    bool is_desktop = APILevelIsOfClass(api, APIClass::GLCore);

//    auto& features = store->features;

//    features.is_gles = !is_desktop;
//    features.gles20  = (api == GLES_2_0);

//#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
//    features.direct_state =
//        features.direct_state || (is_desktop && api >= GL_4_5);
//    features.base_instance =
//        features.direct_state || (is_desktop && api >= GL_4_6);
//#endif

//    features.draw_base_instance = (is_desktop && api >= GL_4_5);
//    features.draw_multi_indirect =
//        (api == GL_4_3) || (api == GL_4_5) || (api == GL_4_6);
//    features.draw_indirect = (api == GL_4_3) || (api == GL_4_5) ||
//                             (api == GL_4_6) || (api == GLES_3_2);
//    features.draw_buffers_blend = is_desktop && (api != GL_3_3);
//    features.draw_color_mask    = is_desktop || (api == GLES_3_2);

//    /* For BaseInstance to be fully effective, we need
//     *  both the gl_BaseInstanceARB GLSL variable
//     *  and Draw*BaseInstance. */
//#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
//    features.base_instance =
//#endif
//        features.draw_base_instance =
//            features.base_instance && features.draw_base_instance;

//    features.draw_multi_indirect =
//        features.draw_multi_indirect && features.draw_base_instance;

//    features.rasterizer_discard = (api != GLES_2_0);
//    features.depth_clamp        = is_desktop && false;
//    features.viewport_indexed   = (api != GLES_2_0) && (api != GLES_3_0);

//    features.separable_programs = is_desktop && (api != GL_3_3);

//    features.texture_storage = (api != GL_3_3) && (api != GLES_2_0);

//    features.buffer_storage = (api == GL_4_3);
//    features.buffer_persistent =
//        (api != GL_3_3) && (api != GLES_2_0) && features.buffer_storage;
//    features.vertex_format = (api == GL_4_3) || (api == GLES_3_2) ||
//                             (api == GL_4_5) || (api == GL_4_6);

//    features.element_buffer_bind =
//        !is_desktop || (is_desktop && store->CURR_API < GL_4_5);
//}

//static void SetExtensionFeatures(GLEAM_API::DataStore const& store)
//{
//#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
////    if(store->features.qcom_tiling)
////    {
////        cVerbose(5, "Qualcomm tiling enabled");
////    }
//#endif

//#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
//    /* base_instance is const false on GLES */

//    store->features.base_instance =
//        Extensions::DrawParametersSupported(store->inst_data->dbgContext);
//    store->features.direct_state =
//        Extensions::DirectStateSupported(store->inst_data->dbgContext);

//#endif
//#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
//    store->features.anisotropic =
//        Extensions::AnisotropicSupported(store->inst_data->dbgContext) ||
//        Extensions::AnisotropicExtSupported(store->inst_data->dbgContext);
//#endif
//}

//static void SetCompatibilityFeatures(
//    GLEAM_API::DataStore const& store, bool forced_api)
//{
//#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
//    if(forced_api && store->CURR_API < GL_4_5 && store->CURR_API < GLES_MIN)
//    {
//        store->features.base_instance = false;
//        store->features.direct_state  = false;
//    }

//    /* If we are emulating, base_instance would skew the results */
//    if(APILevelIsOfClass(store->CURR_API, APIClass::GLES))
//    {
//        store->features.base_instance = false;
//        store->features.direct_state  = false;
//    }
//#endif
//}

//struct debug_exception : undefined_behavior
//{
//    using undefined_behavior::undefined_behavior;
//};

//bool GLEAM_API::LoadAPI(
//    DataStore store, bool debug, GLEAM_API::OPTS const& options)
//{
//    DProfContext _(GLM_API "LoadAPI()");

//#if GL_VERSION_VERIFY(0x430, 0x320)
//    gl::common::PushDebugGroup(GLM_API "Loading GLEAM");
//#endif

//    if(m_store)
//    {
//        cVerbose(8, GLM_API "Data store already created, skipping");
//        return true;
//    }

//    cVerbose(8, GLM_API "Entering LoadAPI()");
//    if(!store)
//    {
//        cWarning(GLM_API "No data store provided");
//        gl::common::PopDebugGroup();
//        return false;
//    }

//    runtime_queue_error ec;
//    store->GpuThread = GraphicsQueue(RuntimeQueue::GetCurrentQueue(ec));
//    C_ERROR_CHECK(ec)

//    cVerbose(8, GLM_API "Creating instance data");
//    store->inst_data = MkUqDST<GLEAM_Instance_Data, InstanceDataDeleter>();

//    store->options = options;
//#if MODE_DEBUG && defined(GL_KHR_debug) && \
//    GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
//    store->DEBUG_MODE = debug;
//    if(debug)
//    {
//#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x310)
//        using KHR_debug = glwrap::ext::KHR_debug<GLESVER_31>;
//#else
//        using KHR_debug = gl::v43;
//#endif

//        gl::vlow::Enable(Feature::DebugOutputSync);
//        gl::v43::DebugMessageControl(
//            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, {}, true);
//        gl::v43::DebugMessageCallback(
//            [](GLenum source,
//               GLenum type,
//               GLuint id,
//               GLenum severity,
//               GLsizei,
//               const GLchar* message,
//               const void*) {
//                switch(severity)
//                {
//                case GL_DEBUG_SEVERITY_NOTIFICATION:
//                    cVerbose(8, "GL Debug: {0}", message);
//                    break;
//                case GL_DEBUG_SEVERITY_HIGH:
//                    Throw(debug_exception(message));
//                default:
//                    cDebug("GL Debug: {0}", message);
//                    break;
//                }
//            },
//            nullptr);
//    }
//#endif

//    Debug::GetExtensions(store->inst_data->dbgContext);

//    APILevel systemLevel;
//    SetAPIVersion(store, systemLevel, options);

//    bool forced_api = (store->CURR_API != systemLevel);

//    ConstructContextObjects(store);

//    /* First, check if an extension is available for behavior */
//    if(auto disable_exts = env::var("GLEAM_DISABLE_EXTENSIONS");
//       disable_exts.has_value() && disable_exts.value() == "1")
//        SetExtensionFeatures(store);
//    SetAPIFeatures(store);

//    /* Disable features when emulating lower API levels */
//    SetCompatibilityFeatures(store, forced_api);

//    m_store = std::move(store);

//    DefaultFramebuffer()->size();

//    gl::vlow::Enable(Feature::Culling);
//    gl::vlow::CullFace(Face::Back);

//    gl::vlow::FrontFace(GL_CCW);

//#if GL_VERSION_VERIFY(0x430, 0x320)
//    gl::common::PopDebugGroup();
//#endif

//#if GL_VERSION_VERIFY(0x300, 0x300)
//    if(m_store->CURR_API != APILevel::GLES_2_0)
//    {
//        m_store->inst_data->queries.push_back(MkShared<GLEAM_TimeQuery>());
//        m_store->inst_data->queries.at(0)->alloc();
//    }
//#endif

//#if GL_VERSION_VERIFY(0x400, 0x310)
//    if(GLEAM_FEATURES.draw_multi_indirect ||
//       m_store->CURR_API > APILevel::GL_4_3)
//    {
//        m_store->inst_data->indirectBuf =
//            MkShared<GLEAM_IndirectBuffer>(RSCA::ReadOnly);
//        m_store->inst_data->indirectBuf->alloc();
//    }
//#endif

//#if MODE_DEBUG
//    m_store->debug_drawer = MkUq<GLEAM_Quad_Drawer>();
//#endif

//    ThreadSetName(0x8085, "OpenGL GPU-0");

//    return true;
//}

//bool GLEAM_API::UnloadAPI()
//{
//    m_store->inst_data.reset();
//    m_store->debug_drawer.reset();
//    m_store.reset();
//    return true;
//}

///* Future improvement: cache changes, or maybe rely on driver for that */

//GLEAM_API::API_CONTEXT GLEAM_API::GetLoadAPI(GLEAM_API::OPTS const& options)
//{
//    cVerbose(8, GLM_API "Returning GLEAM loader...");
//    return [=](bool debug = false) {
//        cVerbose(8, GLM_API "Running GLEAM loader");
//        return LoadAPI(MkUq<GLEAM_DataStore>(), debug, options);
//    };
//}

//bool Coffee::RHI::GLEAM::GLEAM_API::IsAPILoaded()
//{
//    return m_store != nullptr;
//}

//void GLEAM_API::SetRasterizerState(const RASTSTATE& rstate)
//{
//#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
//    gl::vlow::PolygonMode(
//        Face::Both,
//        (rstate.wireframeRender()) ? DrawMode::Line : DrawMode::Fill);
//#endif
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    {
//        if(rstate.discard())
//            gl::vlow::Enable(Feature::RasterizerDiscard);
//        else
//            gl::vlow::Disable(Feature::RasterizerDiscard);
//    }
//#endif

//    gl::vlow::PolygonOffset(1, rstate.polyOffset());

//    if(rstate.culling())
//    {
//        gl::vlow::Enable(Feature::Culling);
//        gl::vlow::CullFace(C_CAST<Face>(rstate.culling()) & Face::FaceMask);
//    } else
//        gl::vlow::Disable(Feature::Culling);
//}

//void GLEAM_API::GetRasterizerState(GLEAM_API::RASTSTATE& rstate)
//{
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    rstate.m_discard = gl::vlow::IsEnabled(Feature::RasterizerDiscard);
//#endif
//    rstate.m_doCull = gl::vlow::IsEnabled(Feature::Culling);

//#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
//    {
//        /* TODO: Get* for PolygonMode() */
//    }
//#endif

//    {
//        i32 culled_faces = 0;
//        gl::vlow::IntegerGetv(GL_CULL_FACE_MODE, &culled_faces);

//        switch(culled_faces)
//        {
//        case GL_FRONT:
//            rstate.m_culling = C_CAST<u32>(Face::Front);
//            break;
//        case GL_BACK:
//            rstate.m_culling = C_CAST<u32>(Face::Back);
//            break;
//        case GL_FRONT_AND_BACK:
//            rstate.m_culling = C_CAST<u32>(Face::Both);
//            break;
//        default:
//            Throw(implementation_error("unhandled face value"));
//        }
//    }
//}

//void GLEAM_API::SetTessellatorState(C_UNUSED(const TSLRSTATE& tstate))
//{
//#if GL_VERSION_VERIFY(0x400, 0x320)
//    if(Extensions::TessellationSupported(CGL_DBG_CTXT))
//    {
//        gl::v40::PatchParameteri(PatchProperty::Vertices, tstate.patchCount());
//        /*TODO: Add configurability for inner and outer levels in place of TCS
//         */
//    }
//#endif
//}

//void GLEAM_API::SetViewportState(const VIEWSTATE& vstate, C_UNUSED(u32 i))
//{
//    if(!Extensions::ViewportArraySupported(CGL_DBG_CTXT) && i >= 1)
//        Throw(implementation_error("viewport arrays not supported"));

//#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
//    if(vstate.multiview())
//    {
//#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
//        if(Extensions::ViewportArraySupported(CGL_DBG_CTXT))
//        {
//            Vector<RectF> varr;

//            for(u32 k = i; k < vstate.viewCount(); k++)
//            {
//                RectF& e = varr[k];
//                auto   s = vstate.view(k);

//                e.x = s.x;
//                e.y = s.y;
//                e.w = s.w;
//                e.h = s.h;
//            }

//            gl::v41::DepthRangeArrayv(
//                i,
//                mem_chunk<const f64>::ofBytes(
//                    &vstate.depth(i), vstate.viewCount())
//                    .view);
//            gl::v41::ScissorArrayv(
//                i,
//                mem_chunk<const i32>::ofBytes(
//                    &vstate.scissor(i), vstate.viewCount())
//                    .view);
//            gl::v41::ViewportArrayv(
//                i, mem_chunk<const f32>::ofContainer(varr).view);

//            gl::vlow::Enablei(Feature::ClipDist, 0);
//            gl::vlow::Enablei(Feature::ClipDist, 1);
//            gl::vlow::Enablei(Feature::ClipDist, 2);
//            gl::vlow::Enablei(Feature::ClipDist, 3);

//        } else
//#endif
//            if(Extensions::ClipDistanceSupported(CGL_DBG_CTXT))
//        {
//            /* TODO: Expand on this feature */
//            gl::vlow::Enablei(Feature::ClipDist, 0);
//        }
//    } else
//#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
//    if(vstate.multiview())
//    {
//        /* Find some way to emulate multiview on GLES 3.0+ */
//    } else
//#else
//    if(vstate.multiview())
//    {
//        /* We're screwed. Throw the framebuffer on the floor, quick. */
//    } else
//#endif
//    {
//        if(vstate.m_view.size() > 0)
//        {
//            auto sview = vstate.view(0);
//            gl::vlow::Viewport(
//                sview.bottomleft().toVector<i32>(), sview.size());
//        }
//        if(vstate.m_depth.size() > 0)
//#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
//            gl::vlow::DepthRange(vstate.depth(0).near_, vstate.depth(0).far_);
//#else
//            gl::vlow::DepthRangef(vstate.depth(0).near_, vstate.depth(0).far_);
//#endif
//        if(vstate.m_scissor.size() > 0)
//        {
//            auto sview = vstate.scissor(0);
//            gl::vlow::Scissor(sview.bottomleft().toVector<i32>(), sview.size());
//            gl::vlow::Enable(Feature::ScissorTest);
//        } else
//            gl::vlow::Disable(Feature::ScissorTest);
//    }
//}

//void GLEAM_API::GetViewportState(GLEAM_API::VIEWSTATE& vstate, u32 i)
//{
//#if GL_VERSION_VERIFY(0x300, 0x300)
//    gl::v33::IntegerGeti_v(GL_VIEWPORT, i, vstate.m_view.at(0).data);
//    if(gl::v33::IsEnabled(Feature::ScissorTest))
//        gl::v33::IntegerGeti_v(GL_SCISSOR_BOX, i, vstate.m_scissor.at(0).data);
//    else
//        vstate.m_scissor.clear();
//#else
//    gl::v33::IntegerGetv(GL_VIEWPORT, vstate.m_view.at(0).data);
//    gl::v33::IntegerGetv(GL_SCISSOR_BOX, vstate.m_scissor.at(0).data);
//#endif

//    typing::graphics::field<scalar> tmp_field;
//    gl::v33::ScalarfGetv(GL_DEPTH_RANGE, tmp_field.data);
//    vstate.m_depth.at(0) = tmp_field.convert<bigscalar>();
//}

//void GLEAM_API::SetBlendState(const BLNDSTATE& bstate, u32 i)
//{
//    if(!GLEAM_FEATURES.viewport_indexed)
//    {
//        if(bstate.blend())
//        {
//            gl::vlow::Enable(Feature::Blend);
//        } else
//            gl::vlow::Disable(Feature::Blend);
//    }
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    else
//    {
//        if(bstate.blend())
//            gl::vlow::Enablei(Feature::Blend, i);
//        else
//            gl::vlow::Disablei(Feature::Blend, i);
//    }
//#endif

//    if(bstate.blend())
//    {
//        if(!GLEAM_FEATURES.draw_buffers_blend)
//        {
//            if(bstate.m_lighten)
//            {
//                gl::vlow::BlendFunc(GL_ONE, GL_ONE);
//            } else if(bstate.additive())
//            {
//                gl::vlow::BlendFunc(GL_SRC_ALPHA, GL_ONE);
//                /*TODO: Add indexed alternative, BlendFunci */
//            } else
//                gl::vlow::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }
//#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
//        else if(GLEAM_FEATURES.draw_buffers_blend)
//        {
//            if(bstate.m_lighten)
//            {
//                gl::vlow::BlendFunc(GL_ONE, GL_ONE);
//            } else if(bstate.additive())
//            {
//                gl::v41::BlendFunci(i, GL_SRC_ALPHA, GL_ONE);
//                /*TODO: Add indexed alternative, BlendFunci */
//            } else
//            {
//                gl::v41::BlendFunci(i, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//            }
//        }
//#endif
//    }

//    if(bstate.sampleAlphaCoverage())
//    {
//        gl::vlow::Enable(Feature::SampleAlphaToCoverage);
//    } else
//    {
//        gl::vlow::Disable(Feature::SampleAlphaToCoverage);
//    }

//    /*TODO: Add more advanced blending options*/

//    /*TODO: Find semantics for SampleCoverage*/
//}

//void GLEAM_API::GetBlendState(GLEAM_API::BLNDSTATE& bstate, u32 i)
//{
//    if(!GLEAM_FEATURES.viewport_indexed)
//    {
//        bstate.m_doBlend = gl::vlow::IsEnabled(Feature::Blend);
//    }
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    else
//    {
//        bstate.m_doBlend = gl::vlow::IsEnabledi(Feature::Blend, i);
//    }
//#endif

//    /* TODO: Blend functions */
//}

//void GLEAM_API::SetDepthState(const DEPTSTATE& dstate, u32 i)
//{
//    if(dstate.testDepth())
//        gl::vlow::Enable(Feature::DepthTest);
//    else
//    {
//        gl::vlow::Disable(Feature::DepthTest);
//        return;
//    }

//    gl::vlow::DepthMask(dstate.mask());

//    if(dstate.fun())
//        gl::vlow::DepthFunc(to_enum(C_CAST<ValueComparison>(dstate.fun())));

//#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
//    if(GLEAM_FEATURES.depth_clamp)
//    {
//        if(dstate.clampDepth())
//            gl::vlow::Enablei(Feature::DepthClamp, i);
//        else
//            gl::vlow::Disablei(Feature::DepthClamp, i);
//    }
//#endif
//    /*TODO: Implement clamping*/
//}

//void GLEAM_API::GetDepthState(GLEAM_API::DEPTSTATE& dstate, u32 i)
//{
//    dstate.m_test = gl::vlow::IsEnabled(Feature::DepthTest);

//    u8 mask = 0;
//    gl::vlow::BoolGetv(GL_DEPTH_WRITEMASK, &mask);
//    dstate.m_mask = mask;
//}

//void GLEAM_API::SetStencilState(const STENSTATE& sstate, u32 i)
//{
//    if(!GLEAM_FEATURES.viewport_indexed)
//    {
//        if(sstate.testStencil())
//            gl::vlow::Enable(Feature::StencilTest);
//        else
//            gl::vlow::Disable(Feature::StencilTest);
//    }
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    else if(GLEAM_FEATURES.viewport_indexed)
//    {
//        if(sstate.testStencil())
//            gl::vlow::Enablei(Feature::StencilTest, i);
//        else
//            gl::vlow::Disablei(Feature::StencilTest, i);
//    }
//#endif

//    //    gl::vlow::StencilFuncSeparate(Face::Both,0x00000000);

//    /*TODO: Implement functionality for more operations */
//}

//void GLEAM_API::GetStencilState(GLEAM_API::STENSTATE& sstate, u32 i)
//{
//    if(!GLEAM_FEATURES.viewport_indexed)
//        sstate.m_test = gl::vlow::IsEnabled(Feature::StencilTest);
//#if GL_VERSION_VERIFY(0x300, 0x320)
//    else
//        sstate.m_test = gl::vlow::IsEnabledi(Feature::StencilTest, i);
//#endif
//}

//void GLEAM_API::SetPixelProcessState(const PIXLSTATE& pstate, bool unpack)
//{
//    Throw(implementation_error("not implemented!"));

//    /*TODO: Implement more processing switches */
//}

//void GLEAM_API::PreDrawCleanup()
//{
//#if GL_VERSION_VERIFY(0x410, 0x200)
//    DPROF_CONTEXT_FUNC(GLM_API);
//    gl::v20es::ShaderReleaseCompiler();
//#endif
//}

//void GLEAM_API::DisposePixelBuffers()
//{
//#if GL_VERSION_VERIFY(0x300, 0x300)
//    DPROF_CONTEXT_FUNC(GLM_API);

//    auto& queue = GLEAM_API_INSTANCE_DATA->pboQueue;
//    for(auto& buf : queue.buffers)
//        gl::v33::BufFree(SpanOne(buf.buf));

//    queue.buffers.clear();
//#endif
//}

//ShPtr<GLEAM_API::FB_T> GLEAM_API::DefaultFramebuffer()
//{
//    return m_store->DefaultFramebuffer;
//}

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
    case APIError::DrawNoVertexBuffer:
        return "No vertex buffer specified";
    case APIError::DrawNotCompatible:
        return "Draw is not compatbile with current configuration";
    case APIError::ShaderCompileFailed:
        return "Failed to compile shader";
    case APIError::ShaderStageNotValid:
        return "Invalid shader stage";
    case APIError::ShaderNoData:
        return "Shader compile without data";
    case APIError::PipelineValidationError:
        return "Pipeline validation failed";
    case APIError::PipelineLinkError:
        return "Pipeline linking failed";
    case APIError::BufferMappingFailed:
        return "Buffer mapping failed";
    case APIError::BufferMappingOutOfBounds:
        return "Buffer mapping out of bounds";
    case APIError::HandleAllocationFailed:
        return "Failed to allocate object handle";
    case APIError::HandleDeallocFailed:
        return "Failed to deallocate object handle";
    case APIError::InvalidObject:
        return "Invalid GL object";
    case APIError::InvalidElementType:
        return "Invalid element type";
    case APIError::UnimplementedPath:
        return "Unimplemented rendering path";
    case APIError::GeneralError:
        return "General error";
    case APIError::NoData:
        return "No data provided";
    case APIError::None:
        return "No error";
    }

    C_ERROR_CODE_OUT_OF_BOUNDS();
}

} // namespace Coffee::RHI::GLEAM

namespace Coffee::Strings {
cstring to_string(RHI::GLEAM::APILevel lev)
{
    using LEV = RHI::GLEAM::APILevel;

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

    case LEV::GLES_2_0:
        return "GL ES 2.0";
    case LEV::GLES_3_0:
        return "GL ES 3.0";
    case LEV::GLES_3_2:
        return "GL ES 3.2";
    default:
        break;
    }

    return "?";
}
} // namespace Coffee::Strings
