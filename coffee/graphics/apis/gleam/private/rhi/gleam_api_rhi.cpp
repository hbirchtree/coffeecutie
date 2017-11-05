#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_query_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>

#include <coffee/core/platform_data.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/interfaces/cgraphics_pixops.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

using GLC = CGL_Implementation;

void GLEAM_API::DumpFramebuffer(GLEAM_API::FB_T &fb, PixelComponents c, BitFmt dt, Vector<byte_t> &storage)
{
    auto size = fb.size();
    if(size.area() <= 0)
        return;

    storage.resize(GetPixSize(BitFormat::UByte, c, size.area()));

    fb.use(FramebufferT::Read);
    CGL33::FBReadPixels(0, 0, size.w, size.h, c, dt, &storage[0]);
}

void GLEAM_API::GetDefaultVersion(int32 &major, int32 &minor)
{
#if defined(COFFEE_GLEAM_DESKTOP)
    major = 4; minor = 5;
#elif defined(COFFEE_ONLY_GLES20)
    major = 2; minor = 0;
#else
    major = 3; minor = 0;
#endif
}

void GLEAM_API::GetDefaultProperties(Display::CDProperties &props)
{
    props.gl.flags |= Display::GLProperties::GLNoFlag
#if !defined(COFFEE_DISABLE_SRGB_SUPPORT)
            |Display::GLProperties::GLSRGB
#endif
#if !defined(COFFEE_GLEAM_DESKTOP)
            |Display::GLProperties::GLES
#endif
#if defined(COFFEE_ALWAYS_VSYNC)
            |Display::GLProperties::GLVSync;
#endif
            ;

#if defined(COFFEE_USE_IMMERSIVE_VIEW)
    props.flags ^= Display::CDProperties::Windowed;
    props.flags |= Display::CDProperties::FullScreen;
#endif

#if defined(COFFEE_MAEMO)
    props.size.w = 800;
    props.size.h = 480;
    props.gl.bits.alpha = 0;
    props.gl.bits.red = 5;
    props.gl.bits.green = 6;
    props.gl.bits.blue = 5;
#else
    props.gl.bits.alpha = 8;
    props.gl.bits.red = 8;
    props.gl.bits.blue = 8;
    props.gl.bits.green = 8;

    props.gl.bits.depth = 24;
    props.gl.bits.stencil = 8;
#endif
    props.gl.bits.samples = 0;
}

void InstanceDataDeleter::operator()(GLEAM_Instance_Data *p)
{
    delete p;
}

bool GLEAM_API::LoadAPI(DataStore store, bool debug)
{
    cVerbose(8, "Entering LoadAPI()");
    if(!store)
    {
        cWarning("No data store provided");
        return false;
    }

    cVerbose(8, "Creating instance data");
    store->inst_data = MkUqDST<GLEAM_Instance_Data, InstanceDataDeleter>();

#ifndef NDEBUG
    store->DEBUG_MODE = debug;
#endif

#if !defined(COFFEE_ONLY_GLES20)
    {
        const szptr num_pbos = 5;
        cVerbose(7, "Creating PBO storage, {0} units", num_pbos);

        CGhnd* bufs = new CGhnd[num_pbos];
        CGL33::BufAlloc(num_pbos,bufs);

        store->inst_data->pboQueue.buffers.reserve(num_pbos);
        for(uint32 i=0;i<num_pbos;i++)
        {
            GLEAM_PboQueue::Pbo pbo;
            pbo.buf = bufs[i];
            pbo.flags = 0;
            store->inst_data->pboQueue.buffers.push_back(pbo);
        }
        delete[] bufs;
    }
#endif

    cVerbose(10, "Getting GL context version");
    auto ver = CGL_Implementation::Debug::ContextVersion();

    cVerbose(10, "Matching GL API...");
#if defined(COFFEE_GLEAM_DESKTOP)
    cVerbose(8, "Checking GL core versions");

    cVerbose(12, "Constructing GL version structures");
    const Display::CGLVersion ver33(3,3);
    const Display::CGLVersion ver43(4,3);
    const Display::CGLVersion ver45(4,5);

    /* If higher level of API is not achieved, stay at the lower one */
    if(ver>=ver45&& /* DISABLES CODE */ (false))
        /* Unimplemented both on CGL level and here */
        store->CURR_API = GL_4_5;
    else if(ver>=ver43)
        store->CURR_API = GL_4_3;
    else if(ver>=ver33)
        store->CURR_API = GL_3_3;
#else
    cVerbose(8, "Checking GLES versions");

    cVerbose(12, "Constructing GL version structures");
    const Display::CGLVersion ver20es(2,0);
#if !defined(COFFEE_ONLY_GLES20)
    const Display::CGLVersion ver30es(3,0);
    const Display::CGLVersion ver32es(3,2);

    if(ver>=ver32es)
        store->CURR_API = GLES_3_2;
    else if(ver>=ver30es)
        store->CURR_API = GLES_3_0;
    else
#endif
    if(ver>=ver20es)
        store->CURR_API = GLES_2_0;
#endif

    cVerbose(8, "Got API: {0}", store->CURR_API);

    if(store->CURR_API == GL_Nothing)
    {
        cWarning("Totally failed to create a GLEAM context, got version: {0}",ver);
        return false;
    }
    cVerbose(10, "Selected API: {0}", store->CURR_API);

#if !defined(COFFEE_ONLY_GLES20)
    if(CGL33::Tex_SRGB_Supported())
    {
        cVerbose(6,"Enabling SRGB color for framebuffers");
        CGL33::Enable(Feature::FramebufferSRGB);
    }
#endif

    cVerbose(4,"Initialized API level {0}", StrUtil::pointerify(store->CURR_API));

    m_store = store;

    return true;
}

bool GLEAM_API::UnloadAPI()
{
    m_store = nullptr;
    return true;
}

/* Future improvement: cache changes, or maybe rely on driver for that */

GLEAM_API::API_CONTEXT GLEAM_API::GetLoadAPI()
{
    cVerbose(8, "Returning GLEAM loader...");
    return [](bool debug = false)
    {
        static GLEAM_DataStore m_gleam_data = {};
        cVerbose(8, "Running GLEAM loader");
        return LoadAPI(&m_gleam_data, debug);
    };
}

void GLEAM_API::SetRasterizerState(const RASTSTATE &rstate, uint32 i)
{
    if(GL_CURR_API==GLES_3_0)
    {
        if(rstate.dither())
            GLC::Enable(Feature::Dither);
        else
            GLC::Disable(Feature::Dither);
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_3_2)
    {
        if(rstate.dither())
            GLC::Enable(Feature::Dither,i);
        else
            GLC::Disable(Feature::Dither,i);
    }

    GLC::PolyMode(Face::Both,
                  (rstate.wireframeRender())
                  ? DrawMode::Line
                  : DrawMode::Fill);

    if(GL_CURR_API==GLES_3_0)
    {
        if(rstate.discard())
            GLC::Enable(Feature::RasterizerDiscard);
        else
            GLC::Disable(Feature::RasterizerDiscard);
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_3_2)
    {
        if(rstate.discard())
            GLC::Enable(Feature::RasterizerDiscard,i);
        else
            GLC::Disable(Feature::RasterizerDiscard,i);
    }

    GLC::ColorLogicOp(rstate.colorOp());
    if(GL_CURR_API==GLES_3_0)
        GLC::ColorMask(rstate.colorMask());
    else
        GLC::ColorMaski(i,rstate.colorMask());

    if(rstate.culling())
    {
        GLC::Enable(Feature::Culling);
        GLC::CullMode(C_CAST<Face>(rstate.culling())&Face::FaceMask);
    }else
        GLC::Disable(Feature::Culling);
}

void GLEAM_API::SetTessellatorState(const TSLRSTATE& tstate)
{
    C_USED(tstate);
#if !defined(COFFEE_ONLY_GLES20)
    if(CGL43::TessellationSupported())
    {
        CGL43::PatchParameteri(PatchProperty::Vertices, tstate.patchCount());
        /*TODO: Add configurability for inner and outer levels in place of TCS */
    }
#endif
}

void GLEAM_API::SetViewportState(const VIEWSTATE& vstate, uint32 i)
{
    C_USED(i);
#if defined(COFFEE_GLEAM_DESKTOP)
    if(vstate.multiview())
    {
        if(CGL43::ViewportArraySupported())
        {
        CRectF* varr = new CRectF[vstate.viewCount()];

        for(uint32 k=i;k<vstate.viewCount();k++)
        {
            CRectF& e = varr[k];
            auto s = vstate.view(k);

            e.x = s.x;
            e.y = s.y;
            e.w = s.w;
            e.h = s.h;
        }

        CGL43::DepthArrayv(i,vstate.viewCount(),&vstate.depth(i));
        CGL43::ScissorArrayv(i,vstate.viewCount(),&vstate.scissor(i));
        CGL43::ViewportArrayv(i,vstate.viewCount(),varr);

        if(vstate.viewCount())
            delete[] varr;

        CGL33::Enable(Feature::ClipDist,0);

        }else if(CGL33::ClipDistanceSupported())
        {
            /* TODO: Expand on this feature */
            CGL33::Enable(Feature::ClipDist,0);
        }
    }else
#elif !defined(COFFEE_ONLY_GLES20)
    if(vstate.multiview())
    {
        /* Find some way to emulate multiview on GLES 3.0+ */
    }else
#else
    if(vstate.multiview())
    {
        /* We're screwed. Throw the framebuffer on the floor, quick. */
    }else
#endif
    {
        if(vstate.m_view.size() > 0)
        {
            auto sview = vstate.view(0);
            CRect64 tview(sview.x,sview.y,sview.w,sview.h);
            GLC::ViewportSet(tview);
        }
        if(vstate.m_depth.size() > 0)
            GLC::DepthSet(vstate.depth(0));
        if(vstate.m_scissor.size() > 0)
        {
            auto sview = vstate.scissor(0);
            CRect64 tview(sview.x,sview.y,sview.w,sview.h);
            GLC::ScissorSet(tview);
            GLC::Enable(Feature::ScissorTest);
        }else
            GLC::Disable(Feature::ScissorTest);
    }
}

void GLEAM_API::SetBlendState(const BLNDSTATE& bstate, uint32 i)
{
    if(GL_CURR_API==GLES_2_0 || GL_CURR_API == GLES_3_0)
    {
        if(bstate.blend())
        {
            GLC::Enable(Feature::Blend);
        }
        else
            GLC::Disable(Feature::Blend);
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_2_0
             || GL_CURR_API==GLES_3_2)
    {
        if(bstate.blend())
            GLC::Enable(Feature::Blend,i);
        else
            GLC::Disable(Feature::Blend,i);
    }

    if(bstate.blend())
    {
        if(GL_CURR_API==GL_3_3
                || GL_CURR_API==GLES_2_0
                || GL_CURR_API==GLES_3_0)
        {
            if(bstate.additive())
            {
                GLC::BlendFunc(GL_SRC_ALPHA,GL_ONE);
                /*TODO: Add indexed alternative, BlendFunci */
            }else
                GLC::BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        }
#if !defined(COFFEE_ONLY_GLES20)
        else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
        {
            if(bstate.additive())
            {
                CGL43::BlendFunci(i,GL_SRC_ALPHA,GL_ONE);
                /*TODO: Add indexed alternative, BlendFunci */
            }else{
                CGL43::BlendFunci(i,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            }
        }
#endif
    }

    /*TODO: Add more advanced blending options*/

    /*TODO: Find semantics for SampleCoverage*/
}

void GLEAM_API::SetDepthState(const DEPTSTATE& dstate, uint32 i)
{
    if(GL_CURR_API==GLES_2_0 || GL_CURR_API==GLES_3_0)
    {
        if(dstate.testDepth())
            GLC::Enable(Feature::DepthTest);
        else
        {
            GLC::Disable(Feature::DepthTest);
            return;
        }
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_3_2)
    {
        if(dstate.testDepth())
            GLC::Enable(Feature::DepthTest,i);
        else{
            GLC::Disable(Feature::DepthTest,i);
            return;
        }
    }

    if(dstate.testDepth())
    {
        GLC::DepthMask(dstate.mask());

        if(dstate.fun())
            GLC::DepthFunc(C_CAST<ValueComparison>(dstate.fun()));

#if defined(COFFEE_GLEAM_DESKTOP)
        if(GL_CURR_API==GL_3_3 || GL_CURR_API==GL_4_3)
        {
            if(dstate.clampDepth())
                GLC::Enable(Feature::DepthClamp,i);
            else
                GLC::Disable(Feature::DepthClamp,i);
        }
#endif
        /*TODO: Implement clamping*/
    }

}

void GLEAM_API::SetStencilState(const STENSTATE& sstate, uint32 i)
{
    if(GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_2_0)
    {
        if(sstate.testStencil())
            GLC::Enable(Feature::StencilTest);
        else
            GLC::Disable(Feature::StencilTest);
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_3_2)
    {
        if(sstate.testStencil())
            GLC::Enable(Feature::StencilTest,i);
        else
            GLC::Disable(Feature::StencilTest,i);
    }

    GLC::StencilMaskSep(Face::Both,0x00000000);

    /*TODO: Implement functionality for more operations */
}

void GLEAM_API::SetPixelProcessState(const PIXLSTATE& pstate, bool unpack)
{
    if(pstate.alignment())
        GLC::PixelStore(unpack,PixelOperation::Alignment,pstate.alignment());
    if(pstate.swapEndianness())
        GLC::PixelStore(unpack,PixelOperation::SwapEndiannes,
                        (pstate.swapEndianness()) ? GL_TRUE : GL_FALSE);

    /*TODO: Implement more processing switches */
}

template<typename T>
void SetUniform_wrapf(CGhnd prog, uint32 idx, const T* data,
                      szptr arr_size)
{
    C_USED(prog);
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API == GL_4_3 || GL_CURR_API == GLES_3_2)
        CGL43::Uniformfv(prog,C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),data);
    else
#endif
        CGL33::Uniformfv(C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),data);
}

template<typename T>
void SetUniform_wrapf_m(CGhnd prog, uint32 idx, const T* data,
                        szptr arr_size)
{
    C_USED(prog);
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API == GL_4_3 || GL_CURR_API == GLES_3_2)
        CGL43::Uniformfv(prog,C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),false,data);
    else
#endif
        CGL33::Uniformfv(C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),false,data);
}

template<typename T>
void SetUniform_wrapi(CGhnd prog, uint32 idx, const T* data,
                      szptr arr_size)
{
    C_USED(prog);
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API == GL_4_3 || GL_CURR_API == GLES_3_2)
        CGL43::Uniformiv(prog,C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),data);
    else
#endif
        CGL33::Uniformiv(C_CAST<i32>(idx),
                         C_CAST<i32>(arr_size / sizeof(T)),data);
}

#if !defined(COFFEE_ONLY_GLES20)
template<typename T>
void SetUniform_wrapui(CGhnd prog, uint32 idx, const T* data,
                       szptr arr_size)
{
    if(GL_CURR_API == GL_4_3 || GL_CURR_API == GLES_3_2)
        CGL43::Uniformuiv(prog,C_CAST<i32>(idx),
                          C_CAST<i32>(arr_size / sizeof(T)),data);
    else
        CGL33::Uniformuiv(C_CAST<i32>(idx),
                          C_CAST<i32>(arr_size / sizeof(T)),data);
}
#endif

void GLEAM_API::SetShaderUniformState(
        const GLEAM_Pipeline &pipeline,
        ShaderStage const& stage,
        const GLEAM_ShaderUniformState &ustate)
{
    using namespace ShaderTypes;

    /* TODO: Tie uniforms to their applicable stages */

    CGhnd prog = 0;

    if(GL_CURR_API==GL_3_3  || GL_CURR_API==GLES_2_0
            || GL_CURR_API==GLES_3_0)
        prog = pipeline.m_handle;
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        /*TODO: Find better way of doing this */
        for(auto s : pipeline.m_programs)
            if(feval(s.stages&stage))
            {
                prog = s.shader->m_handle;
                break;
            }
    }

    for(auto u : ustate.m_uniforms)
    {
        if(!feval(u.second.stages&stage))
            continue;
        CByteData const* db = u.second.value->data;
        if(!db)
            continue;

        /* TODO: Cache uniform state changes, only set them when necessary */

        uint32 const& idx = u.first;
        uint32 const& fgs = u.second.value->flags;

        using Matf2_t = sdt_uniff<Mat_d|S2>;
        using Matf3_t = sdt_uniff<Mat_d|S3>;
        using Matf4_t = sdt_uniff<Mat_d|S4>;

        using Vecf2_t = sdt_uniff<Vec_d|S2>;
        using Vecf3_t = sdt_uniff<Vec_d|S3>;
        using Vecf4_t = sdt_uniff<Vec_d|S4>;

        using Veci2_t = sdt_unifi<Vec_d|S2>;
        using Veci3_t = sdt_unifi<Vec_d|S3>;
        using Veci4_t = sdt_unifi<Vec_d|S4>;

        using Vecu2_t = sdt_unifu<Vec_d|S2>;
        using Vecu3_t = sdt_unifu<Vec_d|S3>;
        using Vecu4_t = sdt_unifu<Vec_d|S4>;

        using Valuef_t = sdt_uniff<S1>;
        using Valuei_t = sdt_unifi<S1>;
        using Valueu_t = sdt_unifu<S1>;

        union {
            c_cptr data;

            Matf2* m2;
            Matf3* m3;
            Matf4* m4;

            Vecf2* v2;
            Vecf3* v3;
            Vecf4* v4;

            Veci2* vi2;
            Veci3* vi3;
            Veci4* vi4;

            Vecui2* vu2;
            Vecui3* vu3;
            Vecui4* vu4;

            scalar* s;
            i32* i;
            u32* u;
        } ptr;

        ptr.data = db->data;

        switch(fgs)
        {
        case Matf2_t::value:
            SetUniform_wrapf_m(prog,idx,ptr.m2,db->size);
            break;
        case Matf3_t::value:
            SetUniform_wrapf_m(prog,idx,ptr.m3,db->size);
            break;
        case Matf4_t::value:
            SetUniform_wrapf_m(prog,idx,ptr.m4,db->size);
            break;

        case Vecf2_t::value:
            SetUniform_wrapf(prog,idx,ptr.v2,db->size);
            break;
        case Vecf3_t::value:
            SetUniform_wrapf(prog,idx,ptr.v3,db->size);
            break;
        case Vecf4_t::value:
            SetUniform_wrapf(prog,idx,ptr.v4,db->size);
            break;

        case Veci2_t::value:
            SetUniform_wrapi(prog,idx,ptr.vi2,db->size);
            break;
        case Veci3_t::value:
            SetUniform_wrapi(prog,idx,ptr.vi3,db->size);
            break;
        case Veci4_t::value:
            SetUniform_wrapi(prog,idx,ptr.vi4,db->size);
            break;

#if !defined(COFFEE_ONLY_GLES20)
        case Vecu2_t::value:
            SetUniform_wrapui(prog,idx,ptr.vu2,db->size);
            break;
        case Vecu3_t::value:
            SetUniform_wrapui(prog,idx,ptr.vu3,db->size);
            break;
        case Vecu4_t::value:
            SetUniform_wrapui(prog,idx,ptr.vu4,db->size);
            break;
#endif

        case Valuef_t::value:
            SetUniform_wrapf(prog,idx,ptr.s,db->size);
            break;
        case Valuei_t::value:
            SetUniform_wrapi(prog,idx,ptr.i,db->size);
            break;
#if !defined(COFFEE_ONLY_GLES20)
        case Valueu_t::value:
            SetUniform_wrapui(prog,idx,ptr.u,db->size);
            break;
#endif
        default:
            cWarning("Unhandled uniform type: {0}", fgs);
            break;
        }
    }

    for(auto s : ustate.m_samplers)
    {
        if(!feval(s.second.stages&stage))
            continue;

        auto& handle = s.second.value;
        if(GL_CURR_API==GL_3_3 || GL_CURR_API==GL_4_3
                || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2
                || GL_CURR_API==GLES_2_0)
        {
            /* Set up texture state */
            CGL33::TexActive(handle->m_unit);
            CGL33::TexBind(handle->m_type,handle->texture);
#if !defined(COFFEE_ONLY_GLES20)
            CGL33::SamplerBind(handle->m_unit,handle->m_sampler);

#if !defined(COFFEE_ONLY_GLES20)
            if(GL_CURR_API == GL_4_3 && GL_CURR_API == GLES_3_2)
            {
                CGL43::Uniformi(prog,s.first,handle->m_unit);
                /* Set texture handle in shader */
            }else
#endif
            {
                /* Set texture handle in shader */
                CGL33::Uniformi(s.first,handle->m_unit);
            }
#endif
        }
        /*TODO: Add bindless textures */
        /*TODO: Add optimized path where BindSamplers is used */
    }

#if !defined(COFFEE_ONLY_GLES20)
    for(auto b : ustate.m_ubuffers)
    {
        auto& det = b.second;
        auto& buf = det.buff;
        uint32 bindex = b.first;
        /* Bind uniform block to binding point */
        buf->bindrange(bindex,det.sec.offset,det.sec.size);
        CGL33::ProgramUnifBlockBind(prog,b.first,bindex);
    }
#endif

#if !defined(COFFEE_ONLY_GLES20)
    if(CGL43::ShaderStorageSupported())
        for(auto b : ustate.m_sbuffers)
        {
            auto& det = b.second;
            auto& buf = det.buff;
            uint32 bindex = b.first;
            /* Bind uniform block to binding point */
            buf->bindrange(bindex,det.sec.offset,det.sec.size);
            CGL43::SBufBind(prog,b.first,bindex);
        }
#endif
}

void GLEAM_API::PreDrawCleanup()
{
    CGL::CGL_ES2Compatibility::ShaderReleaseCompiler();
}

void GLEAM_API::OptimizeRenderPass(
        GLEAM_API::RenderPass &rpass,
        GLEAM_API::OPT_DRAW& buffer)
{
    Map<V_DESC*, Vector<RenderPass::DrawCall*>> vert_sort;
    auto& cmdBufs = buffer.cmdBufs;

    for(auto& call : rpass.draws)
    {
        vert_sort[&call.vertices].push_back(&call);
    }

    /* TODO: Add step to de-duplicate D_DATA into instancing */

    for(auto& group : vert_sort)
    {
        Map<PSTATE*, Vector<RenderPass::DrawCall*>> ustate_sort;

        for(auto& call : group.second)
        {
            ustate_sort[&call->state].push_back(call);
        }

        cmdBufs.reserve(cmdBufs.size() + ustate_sort.size());

        for(auto& ustate_group : ustate_sort)
        {
            cmdBufs.push_back({*group.first, *ustate_group.first, {}});
            auto& cmd_buf = cmdBufs.back();

            cmd_buf.commands.reserve(ustate_group.second.size());

            for(auto& call : ustate_group.second)
                cmd_buf.commands.push_back({
                                               call->d_call,
                                               call->d_data
                                           });
        }
    }

#if !defined(COFFEE_ONLY_GLES20)
    auto& mdData = buffer.multiDrawData;

    for(auto& buffer : cmdBufs)
    {
        auto& data = mdData[&buffer];

        /* TODO: Add functionality to determine whether we are
         *  using Indirect or not */

        data.counts.reserve(buffer.commands.size());
        data.offsets.reserve(buffer.commands.size());
        data.baseVertex.reserve(buffer.commands.size());
        data.indirectCalls.reserve(buffer.commands.size());

        for(auto& cmd : buffer.commands)
        {
            data.counts.push_back(cmd.data.elements());
            data.offsets.push_back(0);
            data.baseVertex.push_back(cmd.data.vertexOffset());

            data.indirectCalls.push_back(
            {
                            cmd.data.elements(),
                            cmd.data.instances(),
                            cmd.data.indexOffset(),
                            C_FCAST<u32>(cmd.data.vertexOffset()),
                            cmd.data.instanceOffset()
                        });

            data.etype = cmd.data.elementType();
            data.dc = cmd.call;
        }
    }
#endif
}

/*!
 * \brief The internal drawing function, making the assumption that
 *  all state is ready.
 * \param mode The primitive drawing mode
 * \param d Drawcall information
 * \param i Drawcall data, offsets and etc.
 * \return true if drawcall was possible on the current API
 */
static bool InternalDraw(
        CGhnd pipelineHandle,
        DrwMd const& mode,
        GLEAM_API::DrawCall const& d,
        GLEAM_API::DrawInstanceData const& i)
{
    if(d.indexed())
    {
        szptr elsize = 4;
        if(i.elementType()==TypeEnum::UShort)
            elsize = 2;
        if(i.elementType()==TypeEnum::UInt)
            elsize = 4;

        if(d.instanced())
        {
            /* TODO: Implement the disabled drawcalls using other means */
#ifdef COFFEE_GLEAM_DESKTOP
            if(GL_CURR_API==GL_4_3
                    && i.instanceOffset()>0
                    && i.vertexOffset()!=0)

                CGL43::DrawElementsInstancedBaseVertexBaseInstance(
                            mode,i.elements(),i.elementType(),
                            i.indexOffset()*elsize,i.instances(),
                            i.vertexOffset(),i.instanceOffset());

            else if(GL_CURR_API==GL_4_3 && i.instanceOffset()>0)

                CGL43::DrawElementsInstancedBaseInstance(
                            mode,
                            i.elements(),i.elementType(),
                            i.indexOffset()*elsize,i.instanceOffset(),
                            i.instances());

            else if(i.vertexOffset() > 0)

                CGL33::DrawElementInstancedBaseVertex(
                            mode,
                            i.elements(), i.elementType(),
                            i.indexOffset()*elsize, i.instances(),
                            i.vertexOffset());

            else
#endif
#if !defined(COFFEE_ONLY_GLES20)
                CGL33::DrawElementsInstanced(
                            mode,i.elements(),i.elementType(),
                            i.indexOffset()*elsize,i.instances());
#else
                CGL33::DrawElements(mode,i.elements(),i.elementType(),
                                    i.indexOffset()*elsize);
#endif
        }else{
#if !defined(COFFEE_ONLY_GLES20)
            if(i.vertexOffset() > 0)
                CGL33::DrawElementsBaseVertex(
                            mode, i.elements(), i.elementType(),
                            i.indexOffset()*elsize, i.vertexOffset());
            else
#endif
                CGL33::DrawElements(mode,i.elements(),i.elementType(),
                                    i.indexOffset()*elsize);
        }

    }else{
#if !defined(COFFEE_ONLY_GLES20)
        if(d.instanced())
            CGL33::DrawArraysInstanced(mode,i.vertexOffset(),
                                       i.vertices(),i.instances());
        else
#else
        if(d.instanced())
        {
            auto hnd = pipelineHandle;
            auto loc = glGetUniformLocation(hnd, "InstanceID");

            auto loc_all = glGetUniformLocation(hnd, "InstanceCount");

            if(loc_all != -1)
                    glUniform1i(loc_all, i.instances());

            for(uint32 j=0;j<i.instances();j++)
            {
                if(loc != -1)
                    glUniform1i(loc, C_CAST<i32>(j));
                CGL33::DrawArrays(mode, i.vertexOffset(), i.vertices());
            }
        }else
#endif
            CGL33::DrawArrays(mode,i.vertexOffset(),i.vertices());
    }

    return true;
}

#if defined(COFFEE_GLEAM_DESKTOP)
bool InternalMultiDraw(
        GLEAM_API::OptimizedDraw::MultiDrawData const& data)
{
    static CGhnd indirectBuf;

    using IndirectCall = GLEAM_API::OptimizedDraw::IndirectCall;

    /* TODO: Add more MultiDraw* support */

    if(data.dc.instanced())
    {

        /* TODO: Implement this using the buffer type */
        if(indirectBuf == 0)
            glGenBuffers(1, &indirectBuf);

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuf);
        glBufferData(
                    GL_DRAW_INDIRECT_BUFFER,
                    data.indirectCalls.size() * sizeof(IndirectCall),
                    data.indirectCalls.data(), GL_STATIC_DRAW);

        /* TODO: Implement fallbacks using the pseudo-implementations */

//        szptr elsize = 4;
//        if(data.etype==TypeEnum::UShort)
//            elsize = 2;
//        if(data.etype==TypeEnum::UInt)
//            elsize = 4;

//        for(auto i : Range(data.indirectCalls.size()))
//        {
//            auto& dc = data.indirectCalls[i];
//            CGL43::DrawElementsInstancedBaseVertexBaseInstance(
//            {
//                            Prim::Triangle, PrimCre::Explicit
//                        },
//                        dc.count,
//                        data.etype,
//                        dc.firstIndex * elsize,
//                        dc.instanceCount,
//                        dc.baseVertex,
//                        dc.baseInstance
//                        );
//        }

        CGL43::DrawMultiElementsIndirect(
        {
                        Prim::Triangle, PrimCre::Explicit
                    }, data.etype,
                    (c_cptr)0,
                    data.indirectCalls.size(),
                    sizeof(data.indirectCalls[0])
                    );
    }
    else

        CGL33::DrawMultiElementsBaseVertex(
                    GL_TRIANGLES, data.counts.data(), data.etype,
                    data.offsets.data(),
                    data.counts.size(), data.baseVertex.data());

    return true;
}
#endif

void GLEAM_API::MultiDraw(
        const GLEAM_API::PIP &pipeline,
        const GLEAM_API::OPT_DRAW &draws)
{
    /* In debug mode, display the entire draw call.
     *  This is the true verbose mode. */
    if(GL_DEBUG_MODE && PrintingVerbosityLevel >= 12)
    {
        cVerbose(12, "- Pipeline:{0}", pipeline.m_handle);
        for(auto& cmd : draws.cmdBufs)
        {
            cVerbose(12, "-- Vertices:{0} + UState:{1}",
                     &cmd.vertices, &cmd.state);
            for(auto& call : cmd.commands)
            {
                cVerbose(12, "--- Draw call:{0}", &call);
            }
        }
    }

    /* The important limiting factor is the speed of
     *  binding new shader pipelines. */
    pipeline.bind();

    /* If using a platform without instancing, cheat by using a
     *  uniform in its place. Preprocessor macros will handle the rest. */
    i32 instanceID_loc = glGetUniformLocation(pipeline.m_handle,
                                              "InstanceID");
    i32 instanceID_val = 0;

    /* TODO: Add fallback support for gl_DrawID */

    /* We will use these to allow use of existing state.
     * We will treat program uniform state as atomic,
     *  because it can be a lot to compare */
    V_DESC* p_vertices = nullptr;
    PSTATE* p_state = nullptr;

#if defined(COFFEE_GLEAM_DESKTOP)
    /* We assume that, if no multiDrawData is available,
     *  we can't use it */
    if(draws.multiDrawData.size())
    {
        for(auto& mdData : draws.multiDrawData)
        {
            auto& buffer = *mdData.first;

            if(&buffer.vertices != p_vertices)
            {
                buffer.vertices.bind();
                p_vertices = &buffer.vertices;
            }

            if(&buffer.state != p_state)
            {
                for(auto const& s : buffer.state)
                    SetShaderUniformState(pipeline, s.first, s.second);
                p_state = &buffer.state;
            }

            InternalMultiDraw(mdData.second);
        }
    }else
#endif
        /* This method minimizes the amount of state changes only,
         *  draw calls are still a couple, but it's faster than
         *  multiple calls to Draw() */
        for(auto& buffer : draws.cmdBufs)
        {
            if(&buffer.vertices != p_vertices)
            {
                buffer.vertices.bind();
                p_vertices = &buffer.vertices;
            }

            if(&buffer.state != p_state)
            {
                for(auto const& s : buffer.state)
                    SetShaderUniformState(pipeline, s.first, s.second);
                p_state = &buffer.state;
            }

            for(auto& cmd : buffer.commands)
            {
                glUniform1i(instanceID_loc, instanceID_val);
                InternalDraw(pipeline.m_handle, {
                                 Prim::Triangle, PrimCre::Explicit
                             }, cmd.call, cmd.data);
                instanceID_val ++;
            }
        }

    pipeline.unbind();
}

void GLEAM_API::Draw(const GLEAM_Pipeline &pipeline,
                     PipelineState const& ustate,
                     V_DESC& vertices,
                     const DrawCall &d, const DrawInstanceData &i,
                     OccludeQuery* query)
{
    C_UNUSED(vertices);

    DrwMd mode = {Prim::Triangle,PrimCre::Explicit};

    if(query)
        query->begin();

    pipeline.bind();

    for(auto const& s : ustate)
    {
        SetShaderUniformState(pipeline, s.first, s.second);
    }

    vertices.bind();

    InternalDraw(pipeline.m_handle, mode, d, i);

    if(query)
        query->end();

    vertices.unbind();
    pipeline.unbind();
}

void GLEAM_API::DrawConditional(const GLEAM_Pipeline &pipeline,
                                const PipelineState &ustate,
                                V_DESC &vertices,
                                const DrawCall &d,
                                const DrawInstanceData &i,
                                OccludeQuery &c)
{
#if !defined(COFFEE_ONLY_GLES20)
    /*TODO: Implement use of GL_QUERY_RESULT_AVAILABLE for GLES path */
    CGL33::ConditionalRenderBegin(c.m_handle, Delay::Wait);
    Draw(pipeline, ustate, vertices, d, i);
    CGL33::ConditionalRenderEnd();
#else
    C_UNUSED(pipeline);
    C_UNUSED(ustate);
    C_UNUSED(vertices);
    C_UNUSED(d);
    C_UNUSED(i);
    C_UNUSED(c);
#endif
}

GLEAM_API::FB_T &GLEAM_API::DefaultFramebuffer()
{
    return m_store->DefaultFramebuffer;
}

APILevel GLEAM_API::Level()
{
    return GL_CURR_API;
}

}
}

namespace Strings{
cstring to_string(RHI::GLEAM::APILevel lev)
{
    using LEV = RHI::GLEAM::APILevel;

#if defined(COFFEE_GLEAM_DESKTOP)
    switch(lev)
    {
    case LEV::GL_3_3:
        return "Desktop GL 3.3";
    case LEV::GL_4_3:
        return "Desktop GL 4.3";
    case LEV::GL_4_5:
        return "Desktop GL 4.5";
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
}
}
