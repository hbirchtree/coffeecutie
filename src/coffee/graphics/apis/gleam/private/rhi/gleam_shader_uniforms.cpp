#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>

#include <coffee/core/CProfiling>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

template<typename T>
STATICINLINE Span<T> get_uniform_span(const T* d, szptr size)
{
    return Span<T>::Unsafe(C_CCAST<T*>(d), size, size / sizeof(T));
}

template<typename T>
void SetUniform_wrapf(
    glhnd const& prog, uint32 idx, const T* data, szptr arr_size, gleam_error& ec)
{
    if(!data)
    {
        ec = APIError::UniformDataNullptr;
        return;
    }

    C_USED(prog);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(GLEAM_FEATURES.separable_programs)
        CGL43::Uniffv(prog, C_CAST<i32>(idx), get_uniform_span(data, arr_size));
    else
#endif
        CGL33::Uniffv(C_CAST<i32>(idx), get_uniform_span(data, arr_size));
}

template<typename T>
void SetUniform_wrapf_m(
    glhnd const& prog, uint32 idx, const T* data, szptr arr_size, gleam_error& ec)
{
    if(!data)
    {
        ec = APIError::UniformDataNullptr;
        return;
    }

    C_USED(prog);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(GLEAM_FEATURES.separable_programs)
        CGL43::Uniffv(
            prog, C_CAST<i32>(idx), false, get_uniform_span(data, arr_size));
    else
#endif
        CGL33::Uniffv(
            C_CAST<i32>(idx), false, get_uniform_span(data, arr_size));
}

template<typename T>
void SetUniform_wrapi(
    glhnd const& prog, uint32 idx, const T* data, szptr arr_size, gleam_error& ec)
{
    if(!data)
    {
        ec = APIError::UniformDataNullptr;
        return;
    }

    C_USED(prog);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(GLEAM_FEATURES.separable_programs)
        CGL43::Unifiv(prog, C_CAST<i32>(idx), get_uniform_span(data, arr_size));
    else
#endif
        CGL33::Unifiv(C_CAST<i32>(idx), get_uniform_span(data, arr_size));
}

#if GL_VERSION_VERIFY(0x330, 0x300)
template<typename T>
void SetUniform_wrapui(
    glhnd const& prog, uint32 idx, const T* data, szptr arr_size, gleam_error& ec)
{
    if(!data)
    {
        ec = APIError::UniformDataNullptr;
        return;
    }

#if GL_VERSION_VERIFY(0x330, 0x320)
    if(GLEAM_FEATURES.separable_programs)
        CGL43::Unifuiv(
            prog, C_CAST<i32>(idx), get_uniform_span(data, arr_size));
    else
#endif
        CGL33::Unifuiv(C_CAST<i32>(idx), get_uniform_span(data, arr_size));
}
#endif

void GLEAM_API::SetShaderUniformState(
    const GLEAM_Pipeline&           pipeline,
    ShaderStage const&              stage,
    const GLEAM_ShaderUniformState& ustate,
    gleam_error&                    ec)
{
    DPROF_CONTEXT_FUNC(GLM_API);

    using namespace ShaderTypes;

    /* TODO: Tie uniforms to their applicable stages */

    glhnd prog;

    if(!GLEAM_FEATURES.separable_programs)
        prog = pipeline.m_handle.hnd;
    else if(GLEAM_FEATURES.separable_programs)
    {
        /*TODO: Find better way of doing this */
        for(auto s : pipeline.m_programs)
            if(feval(s.stages & stage))
            {
                prog = s.shader->m_handle.hnd;
                break;
            }
    }

    if(!prog)
        ec = APIError::UniformNoProgram;

    for(auto u : ustate.m_uniforms)
    {
        if(!feval(u.second.stages & stage))
            continue;
        Bytes const* db = u.second.value->data;
        if(!db)
            continue;

        /* TODO: Cache uniform state changes, only set them when necessary */

        uint32 const& idx = u.first;
        uint32 const& fgs = u.second.value->flags;

        using Matf2_t = sdt_uniff<Mat_d | S2>;
        using Matf3_t = sdt_uniff<Mat_d | S3>;
        using Matf4_t = sdt_uniff<Mat_d | S4>;

        using Vecf2_t = sdt_uniff<Vec_d | S2>;
        using Vecf3_t = sdt_uniff<Vec_d | S3>;
        using Vecf4_t = sdt_uniff<Vec_d | S4>;

        using Veci2_t = sdt_unifi<Vec_d | S2>;
        using Veci3_t = sdt_unifi<Vec_d | S3>;
        using Veci4_t = sdt_unifi<Vec_d | S4>;

        using Vecu2_t = sdt_unifu<Vec_d | S2>;
        using Vecu3_t = sdt_unifu<Vec_d | S3>;
        using Vecu4_t = sdt_unifu<Vec_d | S4>;

        using Valuef_t = sdt_uniff<S1>;
        using Valuei_t = sdt_unifi<S1>;
        using Valueu_t = sdt_unifu<S1>;

        union
        {
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
            i32*    i;
            u32*    u;
        } ptr;

        ptr.data = db->data;

        switch(fgs)
        {
        case Matf2_t::value:
            SetUniform_wrapf_m(prog, idx, ptr.m2, db->size, ec);
            break;
        case Matf3_t::value:
            SetUniform_wrapf_m(prog, idx, ptr.m3, db->size, ec);
            break;
        case Matf4_t::value:
            SetUniform_wrapf_m(prog, idx, ptr.m4, db->size, ec);
            break;

        case Vecf2_t::value:
            SetUniform_wrapf(prog, idx, ptr.v2, db->size, ec);
            break;
        case Vecf3_t::value:
            SetUniform_wrapf(prog, idx, ptr.v3, db->size, ec);
            break;
        case Vecf4_t::value:
            SetUniform_wrapf(prog, idx, ptr.v4, db->size, ec);
            break;

        case Veci2_t::value:
            SetUniform_wrapi(prog, idx, ptr.vi2, db->size, ec);
            break;
        case Veci3_t::value:
            SetUniform_wrapi(prog, idx, ptr.vi3, db->size, ec);
            break;
        case Veci4_t::value:
            SetUniform_wrapi(prog, idx, ptr.vi4, db->size, ec);
            break;

#if GL_VERSION_VERIFY(0x300, 0x300)
        case Vecu2_t::value:
            SetUniform_wrapui(prog, idx, ptr.vu2, db->size, ec);
            break;
        case Vecu3_t::value:
            SetUniform_wrapui(prog, idx, ptr.vu3, db->size, ec);
            break;
        case Vecu4_t::value:
            SetUniform_wrapui(prog, idx, ptr.vu4, db->size, ec);
            break;
#endif

        case Valuef_t::value:
            SetUniform_wrapf(prog, idx, ptr.s, db->size, ec);
            break;
        case Valuei_t::value:
            SetUniform_wrapi(prog, idx, ptr.i, db->size, ec);
            break;
#if GL_VERSION_VERIFY(0x300, 0x300)
        case Valueu_t::value:
            SetUniform_wrapui(prog, idx, ptr.u, db->size, ec);
            break;
#endif
        default:
            ec = APIError::UniformTypeUnhandled;
            prog.release();
            return;
        }

        if(ec != APIError::None)
        {
            prog.release();
            return;
        }
    }

    for(auto s : ustate.m_samplers)
    {
        if(!feval(s.second.stages & stage))
            continue;

        auto& handle = s.second.value;

        {
            /* Set up texture state */
            glhnd ghnd(handle->texture);

            CGL33::TexActive(GL_TEXTURE0 + handle->m_unit);
            CGL33::TexBind(handle->m_type, ghnd);

            ghnd.release();
#if GL_VERSION_VERIFY(0x330, 0x300)
            glhnd shnd(handle->m_sampler);
            CGL33::SamplerBind(handle->m_unit, shnd);
            shnd.release();

#if GL_VERSION_VERIFY(0x410, 0x320)
            if(GLEAM_FEATURES.separable_programs)
            {
                /* Set texture handle in shader */
                CGL43::Unifiv(prog, s.first, handle->m_unit);
            } else
#endif
            {
                /* Set texture handle in shader */
                CGL33::Unifiv(s.first, handle->m_unit);
            }
#endif
        }
        /*TODO: Add bindless textures */
        /*TODO: Add optimized path where BindSamplers is used */
    }

#if GL_VERSION_VERIFY(0x310, 0x300)
    for(auto b : ustate.m_ubuffers)
    {
        auto&  det    = b.second;
        auto&  buf    = det.buff;
        uint32 bindex = b.first;
        /* Bind uniform block to binding point */
        buf->bindrange(bindex, det.sec.offset, det.sec.size, ec);
        CGL33::UnifBlockBinding(prog, b.first, bindex);
    }
#endif

#if GL_VERSION_VERIFY(0x430, 0x310)
    if(Extensions::ShaderStorageSupported())
        for(auto b : ustate.m_sbuffers)
        {
            auto&  det    = b.second;
            auto&  buf    = det.buff;
            uint32 bindex = b.first;
            /* Bind SSBO to binding point */
            buf->bindrange(bindex, det.sec.offset, det.sec.size, ec);
#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
            CGL43::SSBOBinding(prog, b.first, bindex);
#endif
        }
#endif

    prog.release();
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
