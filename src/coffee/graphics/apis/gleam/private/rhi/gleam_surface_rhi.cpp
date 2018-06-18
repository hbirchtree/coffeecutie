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
    TexComp::tex_flag type, glhnd const& tex_hnd, PixCmp cmp, PixCmp target)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    CGL33::TexBind(type, tex_hnd);
    i32 val = C_FCAST<i32>(CGL::to_enum(target, PixFmt::None));
    CGL33::TexParameteriv(type, to_enum_swizz(cmp), &val);
#endif
}

STATICINLINE void texture_pbo_upload(
    szptr pixSize, Bytes const& data, u32 m_flags, c_cptr& data_ptr)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(pixSize > data.size)
        throw undefined_behavior("memory access would go out of bounds");

    if(m_flags & GLEAM_API::TextureDMABuffered)
    {
        data_ptr = nullptr;
        glhnd hand(GLEAM_API_INSTANCE_DATA->pboQueue.current().buf);
        CGL33::BufBind(
            buf::pixel_unpack::value,
            hand);
        CGL33::BufData(
            buf::pixel_unpack::value,
            Bytes::From(data.data, pixSize),
            ResourceAccess::WriteOnly | ResourceAccess::Persistent);

        hand.release();
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

static PixFlg component_to_flag(PixCmp cmp)
{
    switch(cmp)
    {
    case PixCmp::R:
        return PixFlg::R;
    case PixCmp::RG:
        return PixFlg::RG;
    case PixCmp::RGB:
        return PixFlg::RGB;
    case PixCmp::RGBA:
        return PixFlg::RGBA;
    default:
        return PixFlg::None;
    }
}

GLEAM_Surface::GLEAM_Surface(
    TexComp::tex_flag type, PixelFormat fmt, uint32 mips, uint32 texflags) :
    Surface(fmt, false, 0, mips, texflags),
    m_type(type), m_handle(0)
{
    if(!GLEAM_FEATURES.texture_storage || IsPixFmtCompressed(fmt))
        m_flags = m_flags & (~GLEAM_API::TextureImmutable);

    allocate();
    if(!feval(m_flags & GLEAM_API::TextureImmutable))
    {
    /* We must set this to register a proper mipmap level */
#if GL_VERSION_VERIFY(0x300, 0x300)
        //        int32 min_lev = 0;
        i32 max_lev  = C_FCAST<i32>(mips - 1);
        i32 base_lev = 0;
        CGL33::TexBind(type, m_handle);
        CGL33::TexParameteriv(type, GL_TEXTURE_BASE_LEVEL, &base_lev);
        CGL33::TexParameteriv(type, GL_TEXTURE_MAX_LEVEL, &max_lev);
        CGL33::TexBind(type, glhnd());
#endif
    }
}

void GLEAM_Surface::allocate()
{
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        CGL45::TexAllocEx(m_type, Span<CGhnd>::From(m_handle));
    else
#endif
        CGL33::TexAlloc(Span<CGhnd>::From(m_handle));
}

void GLEAM_Surface::dealloc()
{
    CGL33::TexFree(Span<CGhnd>::From(m_handle));
    m_handle.release();
}

u32 GLEAM_Surface::handle()
{
    return m_handle.hnd;
}

void GLEAM_Surface::upload_info(PixCmp comp, uint32 mip, uint32 d)
{
    C_USED(comp);
    C_USED(mip);
    C_USED(d);

    if(d == 0)
        d = 1;

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(GL_DEBUG_MODE)
    {
        uint32 w, h, d_;
        szptr  size;
        //        CGL33::TexGetImageSize(m_type, comp, w, h, d_, &size, mip);
        if(m_type & TexComp::tex_2d_array::value)
            size /= d;
        cVerbose(5, "Texture allocation size ({0}): {1}", m_handle.hnd, size);

        /* TODO: Add information about mipmaps */
    }
#endif
}

GLEAM_Surface2D::GLEAM_Surface2D(
    PixelFormat fmt, uint32 mips, uint32 texflags) :
    GLEAM_Surface(TexComp::tex_2d::value, fmt, mips, texflags),
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
        if(IsPixFmtCompressed(m_pixfmt))
        {
            Vector<byte_t> trash;
            trash.resize(size.area());
            CGL33::TexCompressedImage2D(
                m_type,
                0,
                CompFmt(m_pixfmt, PixFlg::RGBA, (CompFlags)(m_flags >> 10)),
                size,
                trash.size(),
                trash.data());
        } else
            CGL33::TexImage2D(
                tex::t2d::value,
                0,
                m_pixfmt,
                size,
                c,
                bitformat,
                nullptr);
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    else if(GLEAM_FEATURES.texture_storage)
    {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
            CGL45::TexStorage2D(m_handle, m_mips, m_pixfmt, size);
        else
#endif
            CGL43::TexStorage2D(TexComp::tex_2d::value, m_mips, m_pixfmt, size);
    } else
        throw implementation_error(GLM_API "failed to allocate texture!");
#endif
    m_size = size;
}

void GLEAM_Surface2D::upload(
    PixDesc      pfmt,
    const CSize& size,
    const Bytes& data,
    gleam_error& ec,
    CPoint       offset,
    uint32       mip)
{
    c_cptr data_ptr = data.data;
    auto   msz      = size.convert<u32>();
    auto   comp     = pfmt.comp;
    auto   fmt      = pfmt.bfmt;
    auto   pxfmt    = pfmt.pixfmt;

    if(!texture_check_bounds(data, comp, fmt, msz.area()))
        return;

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!IsPixFmtCompressed(pxfmt))
        texture_pbo_upload(
            GetPixSize(fmt, comp, size.convert<u32>().area()),
            data,
            m_flags,
            data_ptr);
#endif

    CGL33::TexBind(m_type, m_handle);

    if(IsPixFmtCompressed(pxfmt))
    {
        auto pflags = component_to_flag(comp);

        CGL33::TexCompressedImage2D(
            m_type,
            C_FCAST<i32>(mip),
            CompFmt(m_pixfmt, pflags, (CompFlags)(m_flags >> 10)),
            msz,
            data.size,
            data.data);
    } else
    {
        if(m_size.area() == 0)
        {
            m_size = size;
        } else
        {
            CGL33::TexSubImage2D(
                m_type,
                C_FCAST<i32>(mip),
                offset,
                msz,
                GetPixComponent(m_pixfmt),
                fmt,
                data_ptr);
        }

        if(m_flags & GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(buf::pixel::value, glhnd());
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
    tex::flag t, PixelFormat fmt, uint32 mips, uint32 texflags) :
    GLEAM_Surface(
        (GL_CURR_API == GLES_2_0) ? tex::t2d::value : t, fmt, mips, texflags),
    m_size(0, 0, 0)
{
    C_USED(t);
}

#define TEX_SQUARE_GRID_SIZE(depth) C_CAST<i32>(CMath::ceil(CMath::sqrt(depth)))

void GLEAM_Surface3D_Base::allocate(CSize3 size, PixelComponents c)
{
    auto bitformat = GetPreferredBitFmt(m_pixfmt);

    m_size = size;

    auto msz = size.convert<u32>();

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        if(!feval(m_flags & GLEAM_API::TextureImmutable))
            CGL33::TexBind(m_type, m_handle);

        if(GLEAM_FEATURES.texture_storage &&
           feval(m_flags & GLEAM_API::TextureImmutable))
        {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
            if(GLEAM_FEATURES.direct_state)
                CGL45::TexStorage3D(m_handle, m_mips, m_pixfmt, size);
            else
#endif
                CGL43::TexStorage3D(m_type, m_mips, m_pixfmt, size);
        } else
        {
            u32 i = 0;
            while(i < m_mips)
            {
                if(!IsPixFmtCompressed(m_pixfmt))
                    CGL33::TexImage3D(
                        m_type, i, m_pixfmt, size, c, bitformat, nullptr);
                else
                {
                    CompFlags f = C_CAST<CompFlags>(m_flags >> 10);

                    Vector<byte_t> empty;
                    empty.resize(
                        GetPixCompressedSize(
                            CompFmt(m_pixfmt, component_to_flag(c), f),
                            _cbasic_size_2d<u32>(msz.width, msz.height)
                                .convert<i32>()) *
                        msz.depth);

                    CGL33::TexCompressedImage3D(
                        m_type,
                        C_FCAST<i32>(i),
                        CompFmt(m_pixfmt, component_to_flag(c), f),
                        size,
                        empty.size(),
                        empty.data());
                }

                i++;
                msz.width /= 2;
                msz.height /= 2;
            }
        }
    } else
#endif
    {
        CGL33::TexBind(m_type, m_handle);

        i32 square_size = TEX_SQUARE_GRID_SIZE(size.depth);

        CGL33::TexImage2D(
            m_type,
            0,
            m_pixfmt,
            {square_size * size.width, square_size * size.height},
            c,
            bitformat,
            nullptr);
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!feval(m_flags & GLEAM_API::TextureImmutable))
#endif
        CGL33::TexBind(m_type, glhnd());
}

void GLEAM_Surface3D_Base::upload(
    PixDesc        pfmt,
    CSize3 const&  size,
    Bytes const&   data,
    CPoint3 const& offset,
    uint32         mip)
{
    auto msz = size.convert<u32>();
    auto mof = offset.convert<u32>();

    auto pxfmt = pfmt.pixfmt;
    auto comp  = pfmt.comp;
    auto fmt   = pfmt.bfmt;

    if(!IsPixFmtCompressed(pxfmt) &&
       !texture_check_bounds(data, comp, fmt, msz.volume()))
        return;

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        /* If we want to use DMA transfer */
        c_cptr data_ptr = data.data;

        if(!IsPixFmtCompressed(pxfmt))
            texture_pbo_upload(
                GetPixSize(fmt, comp, size.convert<u32>().volume()),
                data,
                m_flags,
                data_ptr);

        if(!GLEAM_FEATURES.direct_state)
            CGL33::TexBind(m_type, m_handle);

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
        if(IsPixFmtCompressed(pxfmt) && GLEAM_FEATURES.direct_state)
        {
            CGL45::TexCompressedSubImage3D(
                m_handle,
                mip,
                offset,
                size,
                pfmt.c,
                C_FCAST<i32>(data.size),
                data.data);
        } else if(IsPixFmtCompressed(pxfmt))
        {
            CGL33::TexCompressedSubImage3D(
                m_type, mip, offset, size, pfmt.c, data.size, data.data);
        } else if(GLEAM_FEATURES.direct_state)
        {
            CGL45::TexSubImage3D(
                m_handle,
                mip,
                offset,
                size,
                GetPixComponent(m_pixfmt),
                fmt,
                data_ptr);
        } else
#endif
        {
            CGL33::TexSubImage3D(
                m_type,
                mip,
                offset,
                size,
                GetPixComponent(pxfmt),
                pfmt.bfmt,
                data_ptr);
        }

        if(m_flags & GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(buf::pixel_unpack::value, glhnd());

        if(GL_DEBUG_MODE)
            upload_info(comp, mip, msz.depth);
    } else
#endif
    {
        // Packs all 2D textures into one 2D texture atlas, limited size

        CGL33::TexBind(m_type, m_handle);

        u32 g_size = TEX_SQUARE_GRID_SIZE(m_size.depth);

        for(u32 i = 0; i < C_FCAST<u32>(size.depth); i++)
        {
            Point mofi = {C_FCAST<i32>(mof.x), C_FCAST<i32>(mof.y)};

            auto x_coord = (mof.z + i) % g_size;
            auto y_coord = (mof.z + i) / g_size;

            mofi.x += m_size.width * x_coord;
            mofi.y += m_size.height * y_coord;

            CGL33::TexSubImage2D(
                m_type,
                C_FCAST<i32>(mip),
                mofi,
                {m_size.width, m_size.height},
                GetPixComponent(m_pixfmt),
                fmt,
                data.data);
        }
    }
}

void GLEAM_Surface3D_Base::upload(
    BitFormat       fmt,
    PixelComponents comp,
    CSize3 const&   size,
    Bytes const&    data,
    CPoint3 const&  offset,
    uint32          mip)
{
    upload({m_pixfmt, fmt, comp}, size, data, offset, mip);
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

void GLEAM_Sampler::setLODRange(const Vecf2& range)
{
    C_USED(range);
#if GL_VERSION_VERIFY(0x300, 0x300)
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
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    if(!GLEAM_FEATURES.gles20)
    {
        if(GL_DEBUG_MODE)
        {
            scalar max = CGL::Debug::GetScalar(GL_MAX_TEXTURE_LOD_BIAS);
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
#else
    C_USED(dim);
    C_USED(p);
#endif
}

void GLEAM_Sampler::setFiltering(Filtering mag, Filtering min, Filtering mip)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        CGL33::SamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, to_enum(mag));

        i32 min_filter[2] = {};
        min_filter[0]     = to_enum(min);
        min_filter[1]     = to_enum(min);
        CGL33::SamplerParameteriv(m_handle, GL_TEXTURE_MIN_FILTER, min_filter);
    }
#else
    C_USED(mag);
    C_USED(min);
    C_USED(mip);
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

void GLEAM_Sampler2D::bind(uint32 i)
{
    CGL33::TexActive(i);
#if GL_VERSION_VERIFY(0x300, 0x300)
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
    h.texture = m_surface->m_handle.hnd;

    if(m_surface->m_flags & GLEAM_API::TextureAutoMipmapped ||
       GLEAM_FEATURES.gles20)
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::GenerateMipmap(m_surface->m_type);
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
        h.m_sampler = m_handle.hnd;
#endif

    return h;
}

void GLEAM_Sampler3D::bind(uint32 i)
{
    C_USED(i);
#if GL_VERSION_VERIFY(0x300, 0x300)
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
    h.texture             = m_surface->m_handle.hnd;

    if(m_surface->m_flags & GLEAM_API::TextureAutoMipmapped ||
       GLEAM_FEATURES.gles20)
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::GenerateMipmap(m_surface->m_type);
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        /* TODO: Add bindless */
        h.m_sampler = m_surface->m_handle.hnd;
    }
#endif

    return h;
}

void GLEAM_Sampler2DArray::bind(uint32 i)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
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
    h.texture             = m_surface->m_handle.hnd;

    if(m_surface->m_flags & GLEAM_API::TextureAutoMipmapped ||
       GLEAM_FEATURES.gles20)
    {
        CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
        CGL33::GenerateMipmap(m_surface->m_type);
    }

#if GL_VERSION_VERIFY(0x300, 0x300)

    if(!GLEAM_FEATURES.gles20)
        h.m_sampler = m_handle.hnd;
    else
#endif
        h.arraySize = m_surface->m_size.depth;

    return h;
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
