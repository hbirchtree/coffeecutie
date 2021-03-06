#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

namespace detail {

GLEAM_API_LINKAGE void surface_allocate(
    u32 m_flags, glhnd& m_handle, u32 mips, PixFmt fmt, TexComp::tex_flag type);

GLEAM_API_LINKAGE void surface_dealloc(glhnd& m_handle);

GLEAM_API_LINKAGE void surface_flag(u32& m_flags, PixFmt fmt);

GLEAM_API_LINKAGE Tup<u32, u32> surface_get_levels(
    glhnd const& m_handle, TexComp::tex_flag m_type, gleam_error& ec);

GLEAM_API_LINKAGE void surface_set_levels(
    glhnd const& m_handle, TexComp::tex_flag m_type, u32 base, u32 max);

} // namespace detail

template<typename SizeT, typename PointT>
struct GLEAM_Surface : GraphicsAPI::Surface<SizeT, PointT>
{
    friend struct GLEAM_RenderTarget;

    GLEAM_Surface(
        TexComp::tex_flag type, PixFmt fmt, u32 mips, u32 texflags = 0) :
        GraphicsAPI::Surface<SizeT, PointT>(fmt, false, 0, mips, texflags),
        m_type(type), m_handle(0)
    {
        detail::surface_flag(this->m_flags, fmt);
        detail::surface_allocate(texflags, m_handle, mips, fmt, type);
    }

    ~GLEAM_Surface()
    {
        dealloc();
    }

    void dealloc()
    {
        detail::surface_dealloc(m_handle);
    }

    u32 handle()
    {
        return m_handle.hnd;
    }

    u32& glTexHandle()
    {
        return m_handle.hnd;
    }

    SizeT const& texSize() const
    {
        return m_size;
    }

    Tup<u32, u32> levels() const
    {
        gleam_error ec;
        auto        out = detail::surface_get_levels(m_handle, m_type, ec);
        C_ERROR_CHECK(ec);
        return out;
    }

    void setLevels(u32 base, u32 max)
    {
        return detail::surface_set_levels(m_handle, m_type, base, max);
    }

    SizeT m_size;

  public:
    const TexComp::tex_flag m_type;
    glhnd                   m_handle;
};

struct GLEAM_Surface2D : GLEAM_Surface<Size, Point>
{
    using sampler_type = GLEAM_Sampler2D;

    friend struct GLEAM_Sampler2D;

    GLEAM_Surface2D(PixFmt fmt, u32 mips = 1, u32 texflags = 0);

    void allocate(Size size, PixCmp c);

    void upload(
        PixDesc           pfmt,
        Size const&       size,
        BytesConst const& data,
        gleam_error&      ec,
        Point             offset = {0, 0},
        u32               mip    = 0);
};

struct GLEAM_SurfaceCube : GLEAM_Surface2D
{
    using sampler_type = GLEAM_SamplerCube;

    friend struct GLEAM_SamplerCube;

    GLEAM_SurfaceCube(PixFmt fmt, u32 mips = 1, u32 texflags = 0);
};

struct GLEAM_Surface3D_Base : GLEAM_Surface<Size3, Point3>
{
    friend struct GLEAM_Sampler3D;

    GLEAM_Surface3D_Base(tex::flag t, PixFmt fmt, u32 mips, u32 texflags);

    GLEAM_API_CLASS_LINKAGE void allocate(Size3 size, PixCmp c);

    GLEAM_API_CLASS_LINKAGE void upload(
        PixDesc           pfmt,
        Size3 const&      size,
        BytesConst const& data,
        gleam_error&      ec,
        Point3 const&     offset = {0, 0, 0},
        u32               mip    = 0,
        u32               stride = 0);
};

struct GLEAM_Surface3D : GLEAM_Surface3D_Base
{
    using sampler_type = GLEAM_Sampler3D;

    friend struct GLEAM_Sampler3D;
    GLEAM_Surface3D(PixFmt fmt, u32 mips = 1, u32 texflags = 0) :
        GLEAM_Surface3D_Base(tex::t3d::value, fmt, mips, texflags)
    {
    }
};

struct GLEAM_Surface2DArray : GLEAM_Surface3D_Base
{
    using sampler_type = GLEAM_Sampler2DArray;

    friend struct GLEAM_Sampler2DArray;
    GLEAM_Surface2DArray(PixFmt fmt, u32 mips = 1, u32 texflags = 0) :
        GLEAM_Surface3D_Base(tex::t2d_array::value, fmt, mips, texflags)
    {
    }
};

struct GLEAM_SurfaceCubeArray : GLEAM_Surface3D_Base
{
    using sampler_type = GLEAM_SamplerCubeArray;

    friend struct GLEAM_SamplerCube;

    GLEAM_API_CLASS_LINKAGE GLEAM_SurfaceCubeArray(
        PixFmt fmt, u32 mips = 1, u32 texflags = 0);

    GLEAM_API_CLASS_LINKAGE void allocate(Size3 size, PixCmp c);

    GLEAM_API_CLASS_LINKAGE void upload(
        PixDesc const&             pfmt,
        typing::graphics::CubeFace face,
        Size const&                size,
        BytesConst const&          data,
        Point3 const&              offset,
        gleam_error&               ec,
        u32                        mipmap = 0);
};

struct GLEAM_SamplerHandle
{
    union
    {
        struct
        {
            u32 texture;
            u32 m_unit;
        };
        u64 texture64;
    };
    u32       m_sampler;
    tex::flag m_type;
    u32       arraySize;

    GLEAM_API_CLASS_LINKAGE u32& glTexHandle()
    {
        return texture;
    }
    GLEAM_API_CLASS_LINKAGE u32& glSamplerHandle()
    {
        return m_sampler;
    }

    inline GLEAM_SamplerHandle bind(u32 i = 0)
    {
        m_unit = i;
        return *this;
    }
};

struct GLEAM_Sampler : GraphicsAPI::Sampler
{
    GLEAM_Sampler();

    ~GLEAM_Sampler()
    {
        dealloc();
    }

    GLEAM_API_CLASS_LINKAGE void alloc();
    GLEAM_API_CLASS_LINKAGE void dealloc();

    GLEAM_API_CLASS_LINKAGE void setLODRange(Vecf2 const& range);
    GLEAM_API_CLASS_LINKAGE void setLODBias(scalar bias);
    GLEAM_API_CLASS_LINKAGE void setEdgePolicy(u8 dim, WrapPolicy p);
    GLEAM_API_CLASS_LINKAGE void setFiltering(
        Filtering mag, Filtering min, Filtering mip = Filtering::None);
    GLEAM_API_CLASS_LINKAGE void setAnisotropic(f32 samples);

    GLEAM_API_CLASS_LINKAGE void enableShadowSampler();

  protected:
    glhnd m_handle;
};

template<typename Surf>
struct GLEAM_Sampler_Base : GLEAM_Sampler
{
    using surface_type = Surf;

    void attach(Surf* surf)
    {
        m_surface = surf;
    }

  protected:
    Surf* m_surface;
};

struct GLEAM_Sampler2D : GLEAM_Sampler_Base<GLEAM_Surface2D>
{
    using GLEAM_Sampler_Base<GLEAM_Surface2D>::GLEAM_Sampler_Base;

    GLEAM_API_CLASS_LINKAGE void bind(u32 i);
    GLEAM_API_CLASS_LINKAGE GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler3D : GLEAM_Sampler_Base<GLEAM_Surface3D>
{
    GLEAM_API_CLASS_LINKAGE void bind(u32 i);
    GLEAM_API_CLASS_LINKAGE GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler2DArray : GLEAM_Sampler_Base<GLEAM_Surface2DArray>
{
    friend struct GLEAM_API;

    GLEAM_API_CLASS_LINKAGE void bind(u32 i);
    GLEAM_API_CLASS_LINKAGE GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCube : GLEAM_Sampler_Base<GLEAM_SurfaceCube>
{
    GLEAM_API_CLASS_LINKAGE void bind(u32 i);
    GLEAM_API_CLASS_LINKAGE GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCubeArray : GLEAM_Sampler_Base<GLEAM_SurfaceCubeArray>
{
    GLEAM_API_CLASS_LINKAGE void bind(u32 i);
    GLEAM_API_CLASS_LINKAGE GLEAM_SamplerHandle handle();
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
