#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>

#include <coffee/interfaces/cgraphics_pixops.h>

#include "gleam_internal_types.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

/* TODO: If a texture format is reversed, eg. BGRA, BGR
 *  or simply reversed (RGBA -> ABGR), use swizzling
 *  as fallback on OpenGL ES */
C_MAYBE_UNUSED STATICINLINE void texture_swizzle(
    Texture type, CGhnd tex_hnd, PixCmp cmp, PixCmp target)
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::TexBind(type, tex_hnd);
    i32 val = C_FCAST<i32>(CGL::to_enum(target, PixFmt::None));
    CGL33::TexParameteriv(type, to_enum_swizz(cmp), &val);
#endif
}

STATICINLINE void texture_pbo_upload(
    szptr pixSize, Bytes const& data, u32 m_flags, c_cptr& data_ptr)
{
#if !defined(COFFEE_ONLY_GLES20)
    if(pixSize > data.size)
        throw undefined_behavior("memory access would go out of bounds");

    if(m_flags & GLEAM_API::TextureDMABuffered)
    {
        data_ptr = 0x0;
        CGL33::BufBind(
            BufType::PixelUData,
            GLEAM_API_INSTANCE_DATA->pboQueue.current().buf);
        CGL33::BufData(
            BufType::PixelUData,
            Bytes::From(data.data, pixSize),
            ResourceAccess::WriteOnly | ResourceAccess::Persistent);
    }
#endif
}

STATICINLINE bool texture_check_bounds(
    Bytes const& texdata, PixCmp pix, BitFmt bit, szptr pixels)
{
    bool r = texdata.size >= GetPixSize(bit, pix, pixels);

    if(!r)
        cWarning(
            GLM_API "Texture failed bounds test: {0} < {1}",
            texdata.size,
            GetPixSize(bit, pix, pixels));

    return r;
}

GLEAM_Surface::GLEAM_Surface(
    Texture type, PixelFormat fmt, uint32 mips, uint32 texflags) :
    Surface(fmt, false, 0, mips, texflags),
    m_type(type), m_handle(0)
{
    if(!GLEAM_FEATURES.texture_storage)
        m_flags = m_flags & (~GLEAM_API::TextureImmutable);

    allocate();
    if(!feval(m_flags & GLEAM_API::TextureImmutable))
    {
    /* We must set this to register a proper mipmap level */
#if !defined(COFFEE_ONLY_GLES20)
        //        int32 min_lev = 0;
        int32 max_lev = C_FCAST<i32>(mips - 1);
        CGL33::TexBind(type, m_handle);
        CGL33::TexParameteriv(type, GL_TEXTURE_MAX_LEVEL, &max_lev);
        CGL33::TexBind(type, 0);
#endif
    }
}

void GLEAM_Surface::allocate()
{
#if defined(COFFEE_GLEAM_DESKTOP)
    if(GLEAM_FEATURES.direct_state)
        CGL45::TexAlloc(m_type, Span<CGhnd>::From(m_handle));
    else
#endif
        CGL33::TexAlloc(Span<CGhnd>::From(m_handle));
}

void GLEAM_Surface::dealloc()
{
    CGL33::TexFree(Span<CGhnd>::From(m_handle));
}

CGhnd GLEAM_Surface::handle()
{
    return m_handle;
}

void GLEAM_Surface::upload_info(PixCmp comp, uint32 mip, uint32 d)
{
    C_USED(comp);
    C_USED(mip);
    C_USED(d);

    if(d == 0)
        d = 1;

#if !defined(COFFEE_ONLY_GLES20)
    if(GL_DEBUG_MODE)
    {
        uint32 w, h, d_;
        szptr  size;
        CGL33::TexGetImageSize(m_type, comp, w, h, d_, &size, mip);
        if(m_type == Texture::T2DArray)
            size /= d;
        cVerbose(5, "Texture allocation size ({0}): {1}", m_handle, size);

        /* TODO: Add information about mipmaps */
    }
#endif
}

GLEAM_Surface2D::GLEAM_Surface2D(
    PixelFormat fmt, uint32 mips, uint32 texflags) :
    GLEAM_Surface(Texture::T2D, fmt, mips, texflags),
    m_size(0, 0)
{
}

void GLEAM_Surface2D::allocate(CSize size, PixelComponents c)
{
    auto bitformat = GetPreferredBitFmt(m_pixfmt);

    if(IsPixFmtCompressed(m_pixfmt))
        return;

    if(!GLEAM_FEATURES.direct_state ||
       !feval(m_flags & GLEAM_API::TextureImmutable))
        CGL33::TexBind(m_type, m_handle);

    if(!feval(m_flags & GLEAM_API::TextureImmutable))
    {
        CGL33::TexImage2D(
            Texture::T2D,
            0,
            m_pixfmt,
            size.w,
            size.h,
            0,
            c,
            bitformat,
            nullptr);
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.texture_storage)
    {
#if defined(COFFEE_GLEAM_DESKTOP)
        if(GLEAM_FEATURES.direct_state)
            CGL45::TexStorage2D(m_handle, m_mips, m_pixfmt, size.w, size.h);
        else
#endif
            CGL43::TexStorage2D(Texture::T2D, m_mips, m_pixfmt, size.w, size.h);
    } else
        throw implementation_error(GLM_API "failed to allocate texture!");
#endif
    m_size = size;
}

void GLEAM_Surface2D::upload(
    BitFormat       fmt,
    PixelComponents comp,
    CSize           size,
    Bytes const&    data,
    CPoint          offset,
    uint32          mip)
{
    c_cptr data_ptr = data.data;
    auto   msz      = size.convert<u32>();

    if(!texture_check_bounds(data, comp, fmt, size.area()))
        return;

#if !defined(COFFEE_ONLY_GLES20)
    texture_pbo_upload(
        GetPixSize(fmt, comp, size.convert<u32>().area()),
        data,
        m_flags,
        data_ptr);
#endif

    if(IsPixFmtCompressed(m_pixfmt))
    {
        CGL33::TexBind(m_type, m_handle);

        auto pflags = PixelFlags::None;

        switch(comp)
        {
        case PixCmp::R:
            pflags = PixelFlags::R;
            break;
        case PixCmp::RG:
            pflags = PixelFlags::RG;
            break;
        case PixCmp::RGB:
            pflags = PixelFlags::RGB;
            break;
        case PixCmp::RGBA:
            pflags = PixelFlags::RGBA;
            break;
        default:
            cWarning(GLM_API "Invalid pixel components specified");
            return;
        }

        CGL33::TexImageCompressed2D(
            m_type,
            mip,
            m_pixfmt,
            pflags,
            (CompFlags)(m_flags >> 10),
            size.w,
            size.h,
            0,
            data.size,
            data.data);
    } else
    {
        CGL33::TexBind(m_type, m_handle);

        if(m_size.area() == 0)
        {
            m_size = size;
        } else
        {
            CGL33::TexSubImage2D(
                m_type,
                mip,
                offset.x,
                offset.y,
                msz.w,
                msz.h,
                m_pixfmt,
                fmt,
                data_ptr);
        }
        if(m_flags & GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(BufType::PixelUData, 0);

        CGL33::TexGenMipmap(m_type);
    }

    if(GL_DEBUG_MODE)
        upload_info(comp, mip, 1);
}

CSize Coffee::RHI::GLEAM::GLEAM_Surface2D::texSize() const
{
    return m_size;
}

GLEAM_SurfaceCube::GLEAM_SurfaceCube(PixelFormat fmt, u32 mips, u32 texflags) :
    GLEAM_Surface2D(fmt, mips, texflags)
{
}

GLEAM_Surface3D_Base::GLEAM_Surface3D_Base(
    Texture t, PixelFormat fmt, uint32 mips, uint32 texflags) :
    GLEAM_Surface(
        (GL_CURR_API == GLES_2_0) ? Texture::T2D : t, fmt, mips, texflags),
    m_size(0, 0, 0)
{
    C_USED(t);
}

#define TEX_SQUARE_GRID_SIZE(depth) C_CAST<u32>(CMath::ceil(CMath::sqrt(depth)))

void GLEAM_Surface3D_Base::allocate(CSize3 size, PixelComponents c)
{
    auto bitformat = GetPreferredBitFmt(m_pixfmt);

    m_size = size;

    auto msz = size.convert<u32>();

#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        if(!feval(m_flags & GLEAM_API::TextureImmutable))
            CGL33::TexBind(m_type, m_handle);

        if(GLEAM_FEATURES.texture_storage &&
           feval(m_flags & GLEAM_API::TextureImmutable))
        {
#if defined(COFFEE_GLEAM_DESKTOP)
            if(GLEAM_FEATURES.direct_state)
                CGL45::TexStorage3D(
                    m_handle,
                    m_mips,
                    m_pixfmt,
                    size.width,
                    size.height,
                    size.depth);
            else
#endif
                CGL43::TexStorage3D(
                    m_type, m_mips, m_pixfmt, msz.width, msz.height, msz.depth);
        } else
        {
            CGL33::TexImage3D(
                m_type,
                0,
                m_pixfmt,
                msz.width,
                msz.height,
                msz.depth,
                0,
                c,
                bitformat,
                nullptr);
        }
    } else
#endif
    {
        CGL33::TexBind(m_type, m_handle);

        u32 square_size = TEX_SQUARE_GRID_SIZE(size.depth);

        CGL33::TexImage2D(
            m_type,
            0,
            m_pixfmt,
            square_size * msz.width,
            square_size * msz.height,
            0,
            c,
            bitformat,
            nullptr);
    }
}

void GLEAM_Surface3D_Base::upload(
    BitFormat       fmt,
    PixelComponents comp,
    CSize3          size,
    Bytes const&    data,
    CPoint3         offset,
    uint32          mip)
{
    auto msz = size.convert<u32>();
    auto mof = offset.convert<u32>();

    if(!texture_check_bounds(data, comp, fmt, size.volume()))
        return;

#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        /* If we want to use DMA transfer */
        c_cptr data_ptr = data.data;

        texture_pbo_upload(
            GetPixSize(fmt, comp, size.convert<u32>().volume()),
            data,
            m_flags,
            data_ptr);

#if defined(COFFEE_GLEAM_DESKTOP)
        if(GLEAM_FEATURES.direct_state)
        {
            CGL45::TexSubImage3D(
                m_handle,
                mip,
                offset.x,
                offset.y,
                offset.z,
                size.width,
                size.height,
                size.depth,
                m_pixfmt,
                fmt,
                data_ptr);

            CGL45::TexGenMipmap(m_handle);
        } else
#endif
        {
            CGL33::TexBind(m_type, m_handle);

            CGL33::TexSubImage3D(
                m_type,
                mip,
                offset.x,
                offset.y,
                offset.z,
                size.width,
                size.height,
                size.depth,
                m_pixfmt,
                fmt,
                data_ptr);

            CGL33::TexGenMipmap(m_type);
        }

        if(m_flags & GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(BufType::PixelUData, 0);

        if(GL_DEBUG_MODE)
            upload_info(comp, mip, size.depth);
    } else
#endif
    {
        // Packs all 2D textures into one 2D texture atlas, limited size

        CGL33::TexBind(m_type, m_handle);

        u32 g_size = TEX_SQUARE_GRID_SIZE(m_size.depth);

        for(u32 i = 0; i < C_FCAST<u32>(size.depth); i++)
        {
            auto mofi = mof;

            auto x_coord = (mof.z + i) % g_size;
            auto y_coord = (mof.z + i) / g_size;

            mofi.x += m_size.width * x_coord;
            mofi.y += m_size.height * y_coord;

            CGL33::TexSubImage2D(
                m_type,
                mip,
                mofi.x,
                mofi.y,
                msz.width,
                msz.height,
                m_pixfmt,
                fmt,
                data.data);
        }
    }
}

CSize3 GLEAM_Surface3D_Base::texSize() const
{
    return m_size;
}

GLEAM_SurfaceCubeArray::GLEAM_SurfaceCubeArray(
    PixelFormat fmt, u32 mips, u32 texflags) :
    GLEAM_Surface2DArray(fmt, mips, texflags)
{
}

GLEAM_Sampler::GLEAM_Sampler() : m_handle(0)
{
}

void GLEAM_Sampler::alloc()
{
#if !defined(COFFEE_ONLY_GLES20)
#ifdef COFFEE_GLEAM_DESKTOP
    if(GLEAM_FEATURES.direct_state)
        CGL45::SamplerAlloc(m_handle);
    else
#endif
        if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerAlloc(m_handle);
    }
#endif
}

void GLEAM_Sampler::dealloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        CGL33::SamplerFree(m_handle);
#endif
}

void GLEAM_Sampler::setLODRange(const Vecf2& range)
{
    C_USED(range);
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_MIN_LOD, &range.x());
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_MAX_LOD, &range.y());
    }
#endif
}

void GLEAM_Sampler::setLODBias(scalar bias)
{
    C_USED(bias);
#if defined(COFFEE_GLEAM_DESKTOP)
    if(!GLEAM_FEATURES.gles20)
    {
        if(GL_DEBUG_MODE)
        {
            scalar max = CGL33::Debug::GetScalar(GL_MAX_TEXTURE_LOD_BIAS);
            if(bias > max)
            {
                cWarning("GL_TEXTURE_LOD_BIAS: {0} > {1}", bias, max);
                return;
            }
        }
        CGL33::SamplerParameterfv(m_handle, GL_TEXTURE_LOD_BIAS, &bias);
    }
#endif
}

void GLEAM_Sampler::setEdgePolicy(uint8 dim, WrapPolicy p)
{
#if !defined(COFFEE_ONLY_GLES20)
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

        CGL33::SamplerParameteri(m_handle, d, p);
    }
#else
    C_USED(dim);
    C_USED(p);
#endif
}

void GLEAM_Sampler::setFiltering(Filtering mag, Filtering min, Filtering mip)
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, mag);
        CGL33::SamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, min, mip);
    }
#else
    C_USED(mag);
    C_USED(min);
    C_USED(mip);
#endif
}

void GLEAM_Sampler::enableShadowSampler()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        i32 v = GL_COMPARE_REF_TO_TEXTURE;
        CGL33::SamplerParameteriv(m_handle, GL_TEXTURE_COMPARE_MODE, &v);
    }
#endif
}

void GLEAM_Sampler2D::bind(uint32 i)
{
    CGL33::TexActive(i);
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        CGL33::SamplerBind(i, m_handle);
#endif
    CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
}

GLEAM_SamplerHandle GLEAM_Sampler2D::handle()
{
    GLEAM_SamplerHandle h = {};

    /* TODO: Add bindless */
    h.m_type  = m_surface->m_type;
    h.texture = m_surface->m_handle;
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        h.m_sampler = m_handle;
    else
#endif
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::TexGenMipmap(m_surface->m_type);
    }

    return h;
}

void GLEAM_Sampler3D::bind(uint32 i)
{
    C_USED(i);
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::TexActive(i);
        CGL33::SamplerBind(i, m_handle);
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
    }
#endif
}

GLEAM_SamplerHandle GLEAM_Sampler3D::handle()
{
    GLEAM_SamplerHandle h = {};
    h.m_type              = m_surface->m_type;
    h.texture             = m_surface->m_handle;
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        /* TODO: Add bindless */
        h.m_sampler = m_surface->m_handle;
    } else
#endif
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::TexGenMipmap(m_surface->m_type);
    }
    return h;
}

void GLEAM_Sampler2DArray::bind(uint32 i)
{
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::TexActive(i);
        CGL33::SamplerBind(i, m_handle);
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
    } else
#endif
    {
        CGL33::TexActive(i);
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
    }
}

GLEAM_SamplerHandle GLEAM_Sampler2DArray::handle()
{
    GLEAM_SamplerHandle h = {};
    h.m_type              = m_surface->m_type;
    h.texture             = m_surface->m_handle;
#if !defined(COFFEE_ONLY_GLES20)
    if(!GLEAM_FEATURES.gles20)
        h.m_sampler = m_handle;
    else
#endif
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::TexGenMipmap(m_surface->m_type);
        h.arraySize = m_surface->m_size.depth;
    }

    return h;
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
