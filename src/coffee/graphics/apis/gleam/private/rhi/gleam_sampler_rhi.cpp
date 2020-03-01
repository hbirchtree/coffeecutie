#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

namespace detail {

enum GraphicsCompatibility
{
    Everything    = 0x0,
    Dim3_Textures = 0x1,
};

template<
    u32 Compatibility,
    typename SurfaceT,
    typename std::enable_if<Compatibility == Dim3_Textures>::type* = nullptr>
inline void bind_sampler_fallback(u32 i, SurfaceT* m_surface)
{
    Throw(undefined_behavior("3D textures with interpolation not supported"));
}

template<
    u32 Compatibility,
    typename SurfaceT,
    typename std::enable_if<Compatibility != Dim3_Textures>::type* = nullptr>
inline void bind_sampler_fallback(u32 i, SurfaceT* m_surface)
{
    CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
}

template<u32 Compatibility = Everything, typename SurfaceT>
inline void bind_sampler(u32 i, glhnd& m_handle, SurfaceT* m_surface)
{
    CGL33::TexActive(i);
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerBind(i, m_handle);
    } else
#endif
        bind_sampler_fallback<Compatibility>(i, m_surface);

    CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
}

} // namespace detail

GLEAM_Sampler::GLEAM_Sampler() : m_handle(0)
{
}

void GLEAM_Sampler::alloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        CGL45::SamplerAllocEx(m_handle.hnd);
    else
#endif
        if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerAlloc(m_handle.hnd);
    }
#endif
}

void GLEAM_Sampler::dealloc()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        CGL33::SamplerFree(m_handle.hnd);
    m_handle.release();
#endif
}

void GLEAM_Sampler::setAnisotropic(f32 samples)
{
    if(GLEAM_FEATURES.anisotropic)
    {
        i32 max_aniso = 0;
        CGL46::IntegerGetv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);

        if(samples > max_aniso)
            Throw(undefined_behavior(
                GLM_API
                "anisotropic filtering value out of implementation range"));

        CGL46::SamplerParameterf(m_handle, GL_TEXTURE_MAX_ANISOTROPY, samples);
    }
}

void GLEAM_Sampler::setLODRange(C_UNUSED(const Vecf2& range))
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_MIN_LOD, &range.x());
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_MAX_LOD, &range.y());
    }
#endif
}

void GLEAM_Sampler::setLODBias(C_UNUSED(scalar bias))
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    if(!GLEAM_FEATURES.gles20)
    {
        if(GL_DEBUG_MODE)
        {
            scalar max = CGL::Debug::GetScalar(GL_MAX_TEXTURE_LOD_BIAS);
            if(bias > max)
            {
                // TODO: Error
                //                cWarning("GL_TEXTURE_LOD_BIAS: {0} > {1}",
                //                bias, max);
                return;
            }
        }
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_LOD_BIAS, &bias);
    }
#endif
}

void GLEAM_Sampler::setEdgePolicy(
    UNUSED_PARAM(u8, dim), UNUSED_PARAM(WrapPolicy, p))
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGenum d;
        switch(dim)
        {
        case 0:
            d = GL_TEXTURE_WRAP_R;
            break;
        case 1:
            d = GL_TEXTURE_WRAP_S;
            break;
        case 2:
            d = GL_TEXTURE_WRAP_T;
            break;
        default:
            return;
        }

        CGL33::SamplerParameteri(m_handle, d, to_enum(p));
    }
#endif
}

void GLEAM_Sampler::setFiltering(
    UNUSED_PARAM(Filtering, mag),
    UNUSED_PARAM(Filtering, min),
    UNUSED_PARAM(Filtering, mip))
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, to_enum(mag));

        CGL33::SamplerParameteri(
            m_handle, GL_TEXTURE_MIN_FILTER, to_enum(min, mip));
    }
#endif
}

void GLEAM_Sampler::enableShadowSampler()
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        i32 v = GL_COMPARE_REF_TO_TEXTURE;
        CGL33::SamplerParameteriv(m_handle, GL_TEXTURE_COMPARE_MODE, &v);
    }
#endif
}

template<typename TextureType>
STATICINLINE GLEAM_SamplerHandle SamplerPrepareTexture(
    TextureType* m_surface, glhnd& m_handle, TexComp::tex_flag m_type)
{
    GLEAM_SamplerHandle h = {};

    h.m_type  = m_type;
    h.texture = m_surface->m_handle.hnd;

    if(m_surface->m_flags & GLEAM_API::TextureAutoMipmapped ||
       GLEAM_FEATURES.gles20)
    {
        CGL33::TexBind(m_type, m_surface->m_handle);
        CGL33::GenerateMipmap(m_type);
    }

#if GL_VERSION_VERIFY(0x300, 0x300)

    if(!GLEAM_FEATURES.gles20)
        h.m_sampler = m_handle.hnd;

#endif
#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)

    if(m_surface->m_flags & GLEAM_API::TextureBindless)
        h.texture64 =
            glGetTextureSamplerHandleARB(m_surface->m_handle.hnd, m_handle.hnd);

#endif

    return h;
}

void GLEAM_Sampler2D::bind(u32 i)
{
    detail::bind_sampler<detail::Everything>(i, m_handle, m_surface);
}

GLEAM_SamplerHandle GLEAM_Sampler2D::handle()
{
    return SamplerPrepareTexture(m_surface, m_handle, m_surface->m_type);
}

void GLEAM_Sampler3D::bind(u32 i)
{
    detail::bind_sampler<detail::Dim3_Textures>(i, m_handle, m_surface);
}

GLEAM_SamplerHandle GLEAM_Sampler3D::handle()
{
    return SamplerPrepareTexture(m_surface, m_handle, m_surface->m_type);
}

void GLEAM_Sampler2DArray::bind(u32 i)
{
    detail::bind_sampler<detail::Everything>(i, m_handle, m_surface);
}

GLEAM_SamplerHandle GLEAM_Sampler2DArray::handle()
{
    auto h = SamplerPrepareTexture(m_surface, m_handle, m_surface->m_type);

    h.arraySize = m_surface->m_size.depth;

    return h;
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
