#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_query_rhi.h>

#include <coffee/core/platform_data.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

using GLC = CGL_Implementation;

void GLEAM_API::LoadAPI(bool debug)
{
    instance_data = new GLEAM_Instance_Data;

#ifndef NDEBUG
    GL_DEBUG_MODE = debug;
#endif

    {
        const szptr num_pbos = 5;

        CGhnd* bufs = new CGhnd[num_pbos];
        CGL33::BufAlloc(num_pbos,bufs);

        instance_data->pboQueue.buffers.reserve(num_pbos);
        for(uint32 i=0;i<num_pbos;i++)
        {
            GLEAM_PboQueue::Pbo pbo;
            pbo.buf = bufs[i];
            pbo.flags = 0;
            instance_data->pboQueue.buffers.push_back(pbo);
        }
        delete[] bufs;
    }

    auto ver = CGL_Implementation::Debug::ContextVersion();

    if(!PlatformData::IsGLES())
    {
        const Display::CGLVersion ver33(3,3);
        const Display::CGLVersion ver43(4,3);
        const Display::CGLVersion ver45(4,5);

        /* If higher level of API is not achieved, stay at the lower one */
        if(ver>=ver45&& /* DISABLES CODE */ (false))
            /* Unimplemented both on CGL level and here */
            GL_CURR_API = GL_4_5;
        else if(ver>=ver43)
            GL_CURR_API = GL_4_3;
        else if(ver>=ver33)
            GL_CURR_API = GL_3_3;
    }else
    {
        const Display::CGLVersion ver30es(3,0);
        const Display::CGLVersion ver32es(3,2);

        if(ver>=ver32es)
            GL_CURR_API = GLES_3_2;
        else
            if(ver>=ver30es)
            GL_CURR_API = GLES_3_0;
    }

    if(GL_CURR_API == GL_Nothing)
    {
        cWarning("Totally failed to create a GLEAM context, got version: {0}",ver);
//        RUNOUTTHEWINDOW();
    }

    if(CGL33::Tex_SRGB_Supported())
    {
        cVerbose(5,"Enabling SRGB color for framebuffers");
        CGL33::Enable(Feature::FramebufferSRGB);
    }

    cVerbose(4,"Initialized API level {0}",(const void* const&)GL_CURR_API);
}

/* Future improvement: cache changes, or maybe rely on driver for that */

void GLEAM_API::SetRasterizerState(const RasterizerState &rstate, uint32 i)
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
        GLC::CullMode(((Face)rstate.culling())&Face::FaceMask);
}

void GLEAM_API::SetTessellatorState(const TessellatorState& tstate)
{
    if(CGL43::TessellationSupported())
    {
        CGL43::PatchParameteri(PatchProperty::Vertices,tstate.patchCount());
        /*TODO: Add configurability for inner and outer levels in place of TCS */
    }
}

void GLEAM_API::SetViewportState(const ViewportState& vstate, uint32 i)
{
#ifdef COFFEE_GLEAM_DESKTOP
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

        CGL33::Enable(Feature::ClipDist,0);

        }else if(CGL33::ClipDistanceSupported())
        {
            /* TODO: Expand on this feature */
            CGL33::Enable(Feature::ClipDist,0);
        }
    }else
#endif
    {
        {
            auto sview = vstate.view(0);
            CRect64 tview(sview.x,sview.y,sview.w,sview.h);
            GLC::ViewportSet(tview);
        }
//        GLC::DepthSet(&vstate.depth(0));
        {
            auto sview = vstate.scissor(0);
            CRect64 tview(sview.x,sview.y,sview.w,sview.h);
//            GLC::ScissorSet(&tview);
        }
    }
}

void GLEAM_API::SetBlendState(const BlendState& bstate, uint32 i)
{
    if(GL_CURR_API==GLES_3_0)
    {
        if(bstate.blend())
            GLC::Enable(Feature::Blend);
        else
            GLC::Disable(Feature::Blend);
    }else if(GL_CURR_API==GL_3_3
             || GL_CURR_API==GL_4_3
             || GL_CURR_API==GLES_3_2)
    {
        if(bstate.blend())
            GLC::Enable(Feature::Blend,i);
        else
            GLC::Disable(Feature::Blend,i);
    }

    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        if(bstate.additive())
        {
            GLC::BlendFunc(GL_SRC_ALPHA,GL_ONE);
            /*TODO: Add indexed alternative, BlendFunci */
        }else
            GLC::BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        if(bstate.additive())
        {
            CGL43::BlendFunci(i,GL_SRC_ALPHA,GL_ONE);
            /*TODO: Add indexed alternative, BlendFunci */
        }else
            CGL43::BlendFunci(i,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

    /*TODO: Add more advanced blending options*/

    /*TODO: Find semantics for SampleCoverage*/
}

void GLEAM_API::SetDepthState(const DepthState& dstate, uint32 i)
{
    if(GL_CURR_API==GLES_3_0)
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

    GLC::DepthMask(dstate.mask());

    GLC::DepthFunc((ValueComparison)dstate.fun());

    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GL_4_3)
    {
        if(dstate.clampDepth())
            GLC::Enable(Feature::DepthClamp,i);
        else
            GLC::Disable(Feature::DepthClamp,i);
    }

    /*TODO: Implement clamping*/
}

void GLEAM_API::SetStencilState(const StencilState& sstate, uint32 i)
{
    if(GL_CURR_API==GLES_3_0)
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

void GLEAM_API::SetPixelProcessState(const PixelProcessState& pstate, bool unpack)
{
    if(pstate.alignment())
        GLC::PixelStore(unpack,PixelOperation::Alignment,pstate.alignment());
    if(pstate.swapEndianness())
        GLC::PixelStore(unpack,PixelOperation::SwapEndiannes,
                        (pstate.swapEndianness()) ? GL_TRUE : GL_FALSE);

    /*TODO: Implement more processing switches */
}

template<typename T>
void SetUniform_wrapf(CGhnd prog, uint32 idx, const T* data, szptr arr_size)
{
    if(GL_CURR_API == GL_4_3)
        CGL43::Uniformfv(prog,idx,arr_size,data);
    else
        CGL33::Uniformfv(idx,arr_size,data);
}

template<typename T>
void SetUniform_wrapf_m(CGhnd prog, uint32 idx, const T* data, szptr arr_size)
{
    if(GL_CURR_API == GL_4_3)
        CGL43::Uniformfv(prog,idx,arr_size,false,data);
    else
        CGL33::Uniformfv(idx,arr_size,false,data);
}

template<typename T>
void SetUniform_wrapi(CGhnd prog, uint32 idx, const T* data, szptr arr_size)
{
    if(GL_CURR_API == GL_4_3)
        CGL43::Uniformiv(prog,idx,arr_size,data);
    else
        CGL33::Uniformiv(idx,arr_size,data);
}

template<typename T>
void SetUniform_wrapui(CGhnd prog, uint32 idx, const T* data, szptr arr_size)
{
    if(GL_CURR_API == GL_4_3)
        CGL43::Uniformuiv(prog,idx,arr_size,data);
    else
        CGL33::Uniformuiv(idx,arr_size,data);
}

void GLEAM_API::SetShaderUniformState(const GLEAM_Pipeline &pipeline,
                                      ShaderStage const& stage,
                                      const GLEAM_ShaderUniformState &ustate)
{
    using namespace ShaderTypes;

    /* TODO: Tie uniforms to their applicable stages */

    /* Skip uniform application on 3.3 fragment stage */
    if((GL_CURR_API == GL_3_3 || GL_CURR_API==GLES_3_0) && stage == ShaderStage::Fragment)
        return;

    CGhnd prog = 0;

    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
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

        if(fgs == (Mat_d|S2|Scalar_t))
            SetUniform_wrapf_m(prog,idx,(Matf2*)db->data,db->size/sizeof(Matf2));
        else if(fgs == (Mat_d|S3|Scalar_t))
            SetUniform_wrapf_m(prog,idx,(Matf3*)db->data,db->size/sizeof(Matf3));
        else if(fgs == (Mat_d|S4|Scalar_t))
            SetUniform_wrapf_m(prog,idx,(Matf4*)db->data,db->size/sizeof(Matf4));

        else if(fgs == (Vec_d|S2|Scalar_t))
            SetUniform_wrapf(prog,idx,(Vecf2*)db->data,db->size/sizeof(Vecf2));
        else if(fgs == (Vec_d|S3|Scalar_t))
            SetUniform_wrapf(prog,idx,(Vecf3*)db->data,db->size/sizeof(Vecf3));
        else if(fgs == (Vec_d|S4|Scalar_t))
            SetUniform_wrapf(prog,idx,(Vecf4*)db->data,db->size/sizeof(Vecf4));

        else if(fgs == (Vec_d|S2|Int_t))
            SetUniform_wrapi(prog,idx,(Veci2*)db->data,db->size/sizeof(Veci2));
        else if(fgs == (Vec_d|S3|Int_t))
            SetUniform_wrapi(prog,idx,(Veci3*)db->data,db->size/sizeof(Veci3));
        else if(fgs == (Vec_d|S4|Int_t))
            SetUniform_wrapi(prog,idx,(Veci4*)db->data,db->size/sizeof(Veci4));

        else if(fgs == (Vec_d|S2|UInt_t))
            SetUniform_wrapui(prog,idx,(Vecui2*)db->data,db->size/sizeof(Vecui2));
        else if(fgs == (Vec_d|S3|UInt_t))
            SetUniform_wrapui(prog,idx,(Vecui3*)db->data,db->size/sizeof(Vecui3));
        else if(fgs == (Vec_d|S4|UInt_t))
            SetUniform_wrapui(prog,idx,(Vecui4*)db->data,db->size/sizeof(Vecui4));

        else if(fgs==Scalar_t)
            SetUniform_wrapf(prog,idx,(scalar*)db->data,db->size/sizeof(scalar));
        else if(fgs==Int_t)
            SetUniform_wrapi(prog,idx,(int32*)db->data,db->size/sizeof(int32));
        else if(fgs==UInt_t)
            SetUniform_wrapui(prog,idx,(uint32*)db->data,db->size/sizeof(uint32));
    }

    for(auto s : ustate.m_samplers)
    {
        if(!feval(s.second.stages&stage))
            continue;

        auto& handle = s.second.value;
        if(GL_CURR_API==GL_3_3 || GL_CURR_API==GL_4_3 ||
                GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
        {
            /* Set up texture state */
            CGL33::TexActive(handle->m_unit);
            CGL33::TexBind(handle->m_type,handle->texture);
            CGL33::SamplerBind(handle->m_unit,handle->m_sampler);
            if(GL_CURR_API != GL_4_3)
            {
                /* Set texture handle in shader */
                CGL33::Uniformi(s.first,handle->m_unit);
            }else
            {
                /* Set texture handle in shader */
                CGL43::Uniformi(prog,s.first,handle->m_unit);
            }
        }
        /*TODO: Add bindless textures */
        /*TODO: Add optimized path where BindSamplers is used */
    }

    for(auto b : ustate.m_ubuffers)
    {
        auto& det = b.second;
        auto& buf = det.buff;
        uint32 bindex = b.first;
        /* Bind uniform block to binding point */
        buf->bindrange(bindex,det.sec.offset,det.sec.size);
        CGL33::ProgramUnifBlockBind(prog,b.first,bindex);
    }

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
}

void GLEAM_API::PreDrawCleanup()
{
    CGL::CGL_ES2Compatibility::ShaderReleaseCompiler();
}

void GLEAM_API::Draw(const DrawCall &d, const DrawInstanceData &i)
{
    DrwMd mode = {Prim::Triangle,PrimCre::Explicit};

    if(d.indexed())
    {
        szptr elsize = 1;
        if(i.elementType()==TypeEnum::UShort)
            elsize = 2;
        if(i.elementType()==TypeEnum::UInt)
            elsize = 4;

        if(d.instanced())
        {
            /* TODO: Implement the disabled drawcalls using other means */
#ifdef COFFEE_GLEAM_DESKTOP
            if(GL_CURR_API==GL_4_3&&i.instanceOffset()>0&&i.vertexOffset()!=0)

                CGL43::DrawElementsInstancedBaseVertexBaseInstance(
                            mode,i.elements(),i.elementType(),
                            i.indexOffset()*elsize,i.instances(),
                            i.vertexOffset(),i.instanceOffset());

            else if(GL_CURR_API==GL_4_3&&i.instanceOffset()>0)

                CGL43::DrawElementsInstancedBaseInstance(
                            mode,
                            i.elements(),i.elementType(),
                            i.indexOffset()*elsize,i.instanceOffset(),
                            i.instances());

            else
#endif
                CGL33::DrawElementsInstanced(mode,i.elements(),i.elementType(),
                                             i.indexOffset()*elsize,i.instances());
        }else

            CGL33::DrawElements(mode,i.elements(),i.elementType(),
                                i.indexOffset()*elsize);

    }else{
        if(d.instanced())

            CGL33::DrawArraysInstanced(mode,i.vertexOffset(),i.vertices(),i.instances());

        else

            CGL33::DrawArrays(mode,i.vertexOffset(),i.vertices());
    }
}

void GLEAM_API::DrawConditional(const DrawCall &d,
                                const DrawInstanceData &i,
                                const OccludeQuery &c)
{
    /*TODO: Implement use of GL_QUERY_RESULT_AVAILABLE for GLES path */
#ifdef COFFEE_GLEAM_DESKTOP
    CGL33::ConditionalRenderBegin(c.m_handle, Delay::Wait);
#else
    uint32 v = 0;
    CGL33::QueryGetObjectuiv(c.m_handle,GL_QUERY_RESULT,&v);
    if(v > 0)
    {
#endif
    Draw(d,i);
#ifdef COFFEE_GLEAM_DESKTOP
    CGL33::ConditionalRenderEnd();
#else
    }
#endif
}

}
}
}
