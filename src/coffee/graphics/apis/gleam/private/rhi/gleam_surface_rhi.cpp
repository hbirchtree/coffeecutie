#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>

#include <coffee/interfaces/cgraphics_pixops.h>

#include "gleam_internal_types.h"

#define TEX_SQUARE_GRID_SIZE(depth) C_CAST<u32>(CMath::ceil(CMath::sqrt(depth)))

namespace Coffee {
namespace RHI {
namespace GLEAM {

/* TODO: Apply templates to simplify readability of this file, and for less code
 */

namespace detail {

void surface_allocate(
    u32 m_flags, glhnd& m_handle, u32 mips, PixFmt fmt, TexComp::tex_flag type)
{
    GLEAM_API::DBG::SCOPE _(GLM_API "Surface allocation");

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
        gl::v45::TexAllocEx(type, Span<CGhnd>::From(m_handle));
    else
#endif
        gl::v33::TexAlloc(Span<CGhnd>::From(m_handle));

    if(!feval(m_flags & GLEAM_API::TextureImmutable) ||
       properties::get<properties::is_compressed>(fmt))
    {
        /* We must set this to register a proper mipmap level */
#if GL_VERSION_VERIFY(0x300, 0x300)
        //        int32 min_lev = 0;
        i32 max_lev  = C_FCAST<i32>(mips - 1);
        i32 base_lev = 0;
        gl::v33::TexBind(type, m_handle);
        gl::v33::TexParameteriv(type, GL_TEXTURE_BASE_LEVEL, &base_lev);
        gl::v33::TexParameteriv(type, GL_TEXTURE_MAX_LEVEL, &max_lev);
        gl::v33::TexBind(type, glhnd());
#endif
    }
}

void surface_flag(u32& m_flags, PixFmt fmt)
{
    if(!GLEAM_FEATURES.texture_storage ||
       properties::get<properties::is_compressed>(fmt))
        m_flags = m_flags & (~GLEAM_API::TextureImmutable);
}

void surface_dealloc(glhnd& m_handle)
{
    gl::v33::TexFree(Span<CGhnd>::From(m_handle));
    m_handle.release();
}

/*
 *
 * Surface sampling levels
 *
 * Will opt for direct_state_access when possible
 * Does not support negative levels
 */

Tup<u32, u32> surface_get_levels(
    glhnd const& m_handle, TexComp::tex_flag m_type, gleam_error& ec)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    i32 base = 0, max = 0;

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
    {
        gl::v45::TexGetParameteriv(m_handle, GL_TEXTURE_MAX_LEVEL, &max);
        gl::v45::TexGetParameteriv(m_handle, GL_TEXTURE_BASE_LEVEL, &base);
    } else
#endif
    {
        gl::v33::TexBind(m_type, m_handle);

        gl::v33::TexGetParameteriv(m_type, GL_TEXTURE_MAX_LEVEL, &max);
        gl::v33::TexGetParameteriv(m_type, GL_TEXTURE_BASE_LEVEL, &base);
    }

    return std::make_tuple(C_FCAST<u32>(base), C_FCAST<u32>(max));

#else

    ec = APIError::UnimplementedPath;
    return std::make_tuple(u32(), u32());
#endif
}

void surface_set_levels(
    glhnd const& m_handle, TexComp::tex_flag m_type, u32 base, u32 max)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    i32 _base = C_FCAST<i32>(base), _max = C_FCAST<i32>(max);

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
    if(GLEAM_FEATURES.direct_state)
    {
        gl::v45::TexParameteriv(m_handle, GL_TEXTURE_MAX_LEVEL, &_max);
        gl::v45::TexParameteriv(m_handle, GL_TEXTURE_BASE_LEVEL, &_base);
    } else
#endif
    {
        gl::v33::TexBind(m_type, m_handle);

        gl::v33::TexParameteriv(m_type, GL_TEXTURE_MAX_LEVEL, &_max);
        gl::v33::TexParameteriv(m_type, GL_TEXTURE_BASE_LEVEL, &_base);
    }
#endif
}

enum SurfaceQuirks
{
    Dim3D_Texture     = 0x1,
    Immutable_Texture = 0x2,
    DirectState_Impl  = 0x4,
    Compression_Impl  = 0x8,
};

/*
 *
 * Ordering of implementations:
 *  - "Old" implementation
 *  - Compression
 *  - Immutable texture
 *  - Direct-state immutable texture
 */

/*
 *
 * 2D texture implementations
 *
 */

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        0,
        Dim3D_Texture | Compression_Impl | Immutable_Texture>::value>::type* =
        nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface,
    SizeT const&                  size,
    PixDesc const&                c,
    i32                           i)
{
    gl::v33::TexImage2D(
        surface.m_type, i, surface.m_pixfmt, size, c.comp, c.bfmt, nullptr);
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Compression_Impl,
        Dim3D_Texture | Immutable_Texture>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface,
    SizeT const&                  size,
    CompFmt const&                c,
    Vector<byte_t> const&         data,
    i32                           i)
{
    gl::v33::TexCompressedImage2D(
        surface.m_type, i, c, size, C_FCAST<i32>(data.size()), data.data());
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Immutable_Texture,
        Dim3D_Texture | DirectState_Impl>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v43::TexStorage2D(
        surface.m_type, C_FCAST<i32>(surface.m_mips), surface.m_pixfmt, size);
#endif
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Immutable_Texture | DirectState_Impl,
        Dim3D_Texture>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v45::TexStorage2D(
        surface.m_handle, C_FCAST<i32>(surface.m_mips), surface.m_pixfmt, size);
#endif
}

/*
 *
 * 3D texture implementations
 *
 */

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Dim3D_Texture,
        Immutable_Texture | Compression_Impl |
            DirectState_Impl>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface,
    SizeT const&                  size,
    PixDesc const&                c,
    i32                           i)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v33::TexImage3D(
        surface.m_type, i, surface.m_pixfmt, size, c.comp, c.bfmt, nullptr);
#endif
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Dim3D_Texture | Compression_Impl,
        Immutable_Texture | DirectState_Impl>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface,
    SizeT const&                  size,
    CompFmt const&                c,
    Vector<byte_t> const&         data,
    i32                           i)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v33::TexCompressedImage3D(
        surface.m_type, i, c, size, C_FCAST<i32>(data.size()), data.data());
#endif
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals_except<
        u32,
        SurfaceQuirks,
        Dim3D_Texture | Immutable_Texture,
        DirectState_Impl>::value>::type* = nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v43::TexStorage3D(
        surface.m_type, C_FCAST<i32>(surface.m_mips), surface.m_pixfmt, size);
#endif
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<or_equals<
        u32,
        SurfaceQuirks,
        Dim3D_Texture | Immutable_Texture | DirectState_Impl>::value>::type* =
        nullptr>
inline void surface_initialize(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v45::TexStorage3D(
        surface.m_handle, C_FCAST<i32>(surface.m_mips), surface.m_pixfmt, size);
#endif
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<std::is_same<SizeT, Size3>::value>::type* = nullptr>
inline void surface_gles20_3d_initialize(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size, PixDesc const& c)
{
    u32 square_size = TEX_SQUARE_GRID_SIZE(size.depth);

    gl::v33::TexImage2D(
        surface.m_type,
        0,
        surface.m_pixfmt,
        typing::geometry::size_2d<u32>(
            square_size * size.width, square_size * size.height)
            .convert<i32>(),
        c.comp,
        c.bfmt,
        nullptr);
}

template<
    u32 SurfaceQuirks,
    typename SizeT,
    typename PointT,
    typename std::enable_if<!std::is_same<SizeT, Size3>::value>::type* =
        nullptr>
inline void surface_gles20_3d_initialize(
    GLEAM_Surface<SizeT, PointT>&, SizeT const&, PixDesc const&)
{
}

template<u32 SurfaceQuirks, typename SizeT, typename PointT>
inline void surface_internal_alloc(
    GLEAM_Surface<SizeT, PointT>& surface, SizeT const& size, PixCmp c)
{
    constexpr bool is_3d_texture = std::is_same<SizeT, size_3d<i32>>::value;
    const bool     is_immutable_texture =
        feval(surface.m_flags & GLEAM_API::TextureImmutable);

    auto    bit_fmt = convert::to<BitFmt>(surface.m_pixfmt);
    CompFmt c_fmt(
        surface.m_pixfmt,
        convert::to<PixFlg>(c),
        (CompFlags)(surface.m_flags >> 10));
    PixDesc p_fmt(surface.m_pixfmt, bit_fmt, c);

    surface.m_size = size;

    if(!GLEAM_FEATURES.direct_state || !is_immutable_texture)
        gl::v33::TexBind(surface.m_type, surface.m_handle);

    if(properties::get<properties::is_compressed>(surface.m_pixfmt))
    {
        Vector<byte_t> empty;

        u32 num_layers = 1;
        if(is_3d_texture)
            num_layers = C_FCAST<u32>(size[2]);

        SizeT c_size = size;
        for(auto i : Range<>(surface.m_mips))
        {
            empty.resize(
                GetPixCompressedSize(
                    c_fmt, size_2d<u32>(c_size[0], c_size[1])) *
                num_layers);

            surface_initialize<SurfaceQuirks | Compression_Impl>(
                surface, c_size, c_fmt, empty, C_FCAST<i32>(i));

            c_size[0] /= 2;
            c_size[1] /= 2;
        }

        return;
    }

    if(!is_immutable_texture && GLEAM_FEATURES.gles20 && is_3d_texture)
    {
        /* 3D texture implementation for GLES 2.0 */
        surface_gles20_3d_initialize<SurfaceQuirks>(surface, size, p_fmt);
    } else if(!is_immutable_texture)
    {
        /* Traditional TexImage*D */
        SizeT c_size = size;
        for(auto i : Range<>(surface.m_mips))
        {
            /* "Old" texture loading, needs full initialization */
            surface_initialize<SurfaceQuirks>(
                surface, c_size, p_fmt, C_FCAST<i32>(i));

            c_size[0] /= 2;
            c_size[1] /= 2;
        }
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    else if(GLEAM_FEATURES.texture_storage)
    {
        /* Immutable textures, optionally using direct_state_access */
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(GLEAM_FEATURES.direct_state)
            surface_initialize<
                SurfaceQuirks | Immutable_Texture | DirectState_Impl>(
                surface, size);
        else
#endif
            surface_initialize<SurfaceQuirks | Immutable_Texture>(
                surface, size);
    }
#endif
    else
        Throw(implementation_error("no fallback allocation found"));
}

template<u32 SurfaceQuirks, typename SizeT, typename PointT>
inline void surface_internal_upload(GLEAM_Surface<SizeT, PointT>& surface)
{
}

} // namespace detail

/* TODO: If a texture format is reversed, eg. BGRA, BGR
 *  or simply reversed (RGBA -> ABGR), use swizzling
 *  as fallback on OpenGL ES */
C_UNUSED(STATICINLINE void texture_swizzle(
    TexComp::tex_flag type, glhnd const& tex_hnd, PixCmp cmp, PixCmp target))
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    gl::v33::TexBind(type, tex_hnd);
    i32 val = C_FCAST<i32>(CGL::to_enum(target, PixFmt::None));
    gl::v33::TexParameteriv(type, to_enum_swizz(cmp), &val);
#endif
}

STATICINLINE void texture_pbo_upload(
    szptr pixSize, BytesConst const& data, u32 m_flags, c_cptr& data_ptr)
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    if(pixSize > data.size)
        Throw(undefined_behavior("memory access would go out of bounds"));

    if(m_flags & GLEAM_API::TextureDMABuffered)
    {
        data_ptr = nullptr;
        glhnd hand(GLEAM_API_INSTANCE_DATA->pboQueue.current().buf);
        gl::v33::BufBind(buf::pixel_unpack::value, hand);
        gl::v33::BufData(
            buf::pixel_unpack::value,
            BytesConst::From(data.data, pixSize),
            RSCA::WriteOnly | RSCA::Persistent);

        hand.release();
    }
#endif
}

STATICINLINE bool texture_check_bounds(
    BytesConst const& texdata, PixDesc const& fmt, szptr pixels, Size size = {})
{
    using namespace typing::pixels::properties;

    bool r = false;

    if(get<is_compressed>(fmt.pixfmt))
    {
        auto expected = GetPixCompressedSize(fmt.c, size);
        auto real     = texdata.size;

        r = expected == real;
    } else
    {
        r = texdata.size >= GetPixSize(fmt.bfmt, fmt.comp, pixels);
    }

    // TODO: Return error

    return r;
}

GLEAM_Surface2D::GLEAM_Surface2D(PixFmt fmt, u32 mips, u32 texflags) :
    GLEAM_Surface(TexComp::tex_2d::value, fmt, mips, texflags)
{
}

void GLEAM_Surface2D::allocate(size_type size, PixCmp c)
{
    detail::surface_internal_alloc<0>(*this, size.convert<i32>(), c);
}

void GLEAM_Surface2D::upload(
    PixDesc                     pfmt,
    size_type const&            size,
    const semantic::BytesConst& data,
    gleam_error&                ec,
    Point                       offset,
    u32                         mip)
{
    c_cptr data_ptr = data.data;
    auto   msz      = size.convert<u32>();
    auto   comp     = pfmt.comp;
    auto   fmt      = pfmt.bfmt;
    auto   pxfmt    = pfmt.pixfmt;

    if(!texture_check_bounds(data, pfmt, msz.area(), size))
        Throw(undefined_behavior("not enough texture data"));

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!properties::get<properties::is_compressed>(pxfmt))
        texture_pbo_upload(
            GetPixSize(fmt, comp, size.convert<u32>().area()),
            data,
            m_flags,
            data_ptr);
#endif

    gl::v33::TexBind(m_type, m_handle);

    if(properties::get<properties::is_compressed>(pxfmt))
    {
        auto pflags = convert::to<PixFlg>(comp);

        gl::v33::TexCompressedImage2D(
            m_type,
            C_FCAST<i32>(mip),
            CompFmt(m_pixfmt, pflags, (CompFlags)(m_flags >> 10)),
            size,
            data.size,
            data.data);
    } else
    {
        if(m_size.area() == 0)
        {
            m_size = size;
        } else
        {
            gl::v33::TexSubImage2D(
                m_type,
                C_FCAST<i32>(mip),
                offset,
                size,
                convert::to<PixCmp>(m_pixfmt),
                fmt,
                data_ptr);
        }

        if(m_flags & GLEAM_API::TextureDMABuffered)
            gl::v33::BufBind(buf::pixel::value, glhnd());
    }
}

GLEAM_SurfaceCube::GLEAM_SurfaceCube(PixFmt fmt, u32 mips, u32 texflags) :
    GLEAM_Surface2D(fmt, mips, texflags)
{
}

GLEAM_Surface3D_Base::GLEAM_Surface3D_Base(
    tex::flag t, PixFmt fmt, u32 mips, u32 texflags) :
    GLEAM_Surface(
        (GL_CURR_API == GLES_2_0) ? tex::t2d::value : t, fmt, mips, texflags)
{
}

void GLEAM_Surface3D_Base::allocate(size_type size, PixCmp c)
{
    detail::surface_internal_alloc<detail::Dim3D_Texture>(
        *this, size.convert<i32>(), c);
}

void GLEAM_Surface3D_Base::upload(
    PixDesc                          pfmt,
    size_type const&                 size,
    semantic::BytesConst const&      data,
    Coffee::RHI::GLEAM::gleam_error& ec,
    Point3 const&                    offset,
    u32                              mip,
    u32                              stride)
{
    auto msz = size.convert<u32>();
    auto mof = offset.convert<u32>();

    auto pxfmt = pfmt.pixfmt;
    auto fmt   = pfmt.bfmt;

    if(size.depth == 1 && stride == 0 &&
       !texture_check_bounds(
           data, pfmt, msz.volume(), Size(msz.width, msz.height)))
        Throw(undefined_behavior("not enough texture data"));

    if(stride != 0)
    {
#if GL_VERSION_VERIFY(0x300, 0x300)
        if(stride < size.width)
            Throw(undefined_behavior("stride is less than upload size"));

        gl::v33::PixelStorei(GL_UNPACK_ROW_LENGTH, stride);
#else
        ec = GLEAM::APIError::UnimplementedPath;
        return;
#endif
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(!GLEAM_FEATURES.gles20)
    {
        /* If we want to use DMA transfer */
        c_cptr data_ptr = data.data;

        if(!properties::get<properties::is_compressed>(pxfmt))
            texture_pbo_upload(
                PixDescSize(pfmt, Size(msz.width, msz.height)),
                data,
                m_flags,
                data_ptr);

        if(!GLEAM_FEATURES.direct_state)
            gl::v33::TexBind(m_type, m_handle);

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
        if(properties::get<properties::is_compressed>(pxfmt) &&
           GLEAM_FEATURES.direct_state)
        {
            gl::v45::TexCompressedSubImage3D(
                m_handle,
                mip,
                offset,
                size,
                pfmt.c,
                C_FCAST<i32>(PixDescSize(
                    pfmt, Size(size.width, size.height * size.depth))),
                data.data);
        } else
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
            if(properties::get<properties::is_compressed>(pxfmt))
        {
            gl::v33::TexCompressedSubImage3D(
                m_type, mip, offset, size, pfmt.c, data.size, data.data);
        } else
#endif
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
            if(GLEAM_FEATURES.direct_state)
        {
            gl::v45::TexSubImage3D(
                m_handle,
                mip,
                offset,
                size,
                convert::to<PixCmp>(m_pixfmt),
                fmt,
                data_ptr);
        } else
#endif
        {
            gl::v33::TexSubImage3D(
                m_type,
                mip,
                offset,
                size,
                convert::to<PixCmp>(pxfmt),
                pfmt.bfmt,
                data_ptr);
        }

        if(m_flags & GLEAM_API::TextureDMABuffered)
            gl::v33::BufBind(buf::pixel_unpack::value, glhnd());

    } else
#endif
    {
        // Packs all 2D textures into one 2D texture atlas, limited size

        gl::v33::TexBind(m_type, m_handle);

        u32 g_size = TEX_SQUARE_GRID_SIZE(m_size.depth);

        for(u32 i = offset.z; i < C_FCAST<u32>(size.depth + offset.z); i++)
        {
            Point mofi = {C_FCAST<i32>(mof.x), C_FCAST<i32>(mof.y)};

            auto x_coord = i % g_size;
            auto y_coord = i / g_size;

            mofi.x += m_size.width * x_coord + offset.x;
            mofi.y += m_size.height * y_coord + offset.y;

            gl::v33::TexSubImage2D(
                m_type,
                C_FCAST<i32>(mip),
                mofi,
                typing::geometry::size_2d(size.width, size.height)
                    .convert<i32>(),
                convert::to<PixCmp>(m_pixfmt),
                fmt,
                data.data);
        }
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(stride != 0)
        gl::v33::PixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
}

GLEAM_SurfaceCubeArray::GLEAM_SurfaceCubeArray(
    PixFmt fmt, u32 mips, u32 texflags) :
    GLEAM_Surface3D_Base(tex::tex_cube_array::value, fmt, mips, texflags)
{
}

void GLEAM::GLEAM_SurfaceCubeArray::allocate(size_type const& size, PixCmp c)
{
    /* Cubemap arrays have 6x the layers */
    GLEAM_Surface3D_Base::allocate(
        {size.width, size.height, size.depth * 6}, c);
}

void GLEAM::GLEAM_SurfaceCubeArray::upload(
    PixDesc const&              pfmt,
    typing::graphics::CubeFace  face,
    size_2d<i32> const&         size,
    semantic::BytesConst const& data,
    Point3 const&               offset,
    GLEAM::gleam_error&         ec,
    u32                         mipmap)
{
    GLEAM_Surface3D_Base::upload(
        pfmt,
        size_3d<i32>(size.w, size.h, 1),
        data,
        ec,
        Point3(offset.x, offset.y, offset.z * 6 + C_CAST<u32>(face)));
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
