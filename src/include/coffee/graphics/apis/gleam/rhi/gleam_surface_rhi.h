#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_Surface : GraphicsAPI::Surface<CSize, CPoint>
{
    friend struct GLEAM_RenderTarget;

    GLEAM_Surface(
        TexComp::tex_flag type,
        PixelFormat       fmt,
        uint32            mips,
        uint32            texflags = 0);

    void allocate();
    void dealloc();

    u32 handle();

    u32& glTexHandle()
    {
        return m_handle.hnd;
    }

  protected:
    void upload_info(PixCmp comp, uint32 mip, uint32 d);

    TexComp::tex_flag m_type;

  public:
    glhnd m_handle;
};

struct GLEAM_Surface2D : GLEAM_Surface
{
    friend struct GLEAM_Sampler2D;

    GLEAM_Surface2D(PixelFormat fmt, uint32 mips = 1, uint32 texflags = 0);

    void allocate(CSize size, PixCmp c);

    void upload(
        PixDesc      pfmt,
        CSize const& size,
        const Bytes& data,
        gleam_error& ec,
        CPoint       offset = {0, 0},
        uint32       mip    = 0);

    void upload(
        PixDesc      pfmt,
        CSize const& size,
        const Bytes& data,
        CPoint       offset = {0, 0},
        uint32       mip    = 0)
    {
        gleam_error ec;
        return upload(pfmt, size, data, ec, offset, mip);
    }

    CSize texSize() const;

    /*TODO: Add download function */

  protected:
    CSize m_size;
};

struct GLEAM_SurfaceCube : GLEAM_Surface2D
{
    friend struct GLEAM_SamplerCube;

    GLEAM_SurfaceCube(PixelFormat fmt, u32 mips = 1, u32 texflags = 0);
};

struct GLEAM_Surface3D_Base : GLEAM_Surface
{
    friend struct GLEAM_Sampler3D;

    GLEAM_Surface3D_Base(
        tex::flag t, PixelFormat fmt, uint32 mips, uint32 texflags);

    void allocate(CSize3 size, PixCmp c);

    void upload(
        PixDesc        pfmt,
        CSize3 const&  size,
        Bytes const&   data,
        CPoint3 const& offset = {0, 0, 0},
        uint32         mip    = 0);

    void upload(
        BitFmt         fmt,
        PixCmp         comp,
        CSize3 const&  size,
        Bytes const&   data,
        CPoint3 const& offset = {0, 0, 0},
        uint32         mip    = 0);

    void upload(
        BitFmt         fmt,
        PixCmp         comp,
        CSize3 const&  size,
        c_cptr         data,
        CPoint3 const& offset = {0, 0, 0},
        uint32         mip    = 0)
    {
        Bytes dataS;
        dataS.data = C_RCAST<byte_t*>(C_CCAST<c_ptr>(data));
        dataS.size = GetPixSize(fmt, comp, size.volume());
        upload(fmt, comp, size, dataS, offset, mip);
    }

    CSize3 texSize() const;

    /*TODO: Add download function */

    CSize3 m_size;
};

struct GLEAM_Surface3D : GLEAM_Surface3D_Base
{
    friend struct GLEAM_Sampler3D;
    GLEAM_Surface3D(PixelFormat fmt, uint32 mips = 1, uint32 texflags = 0) :
        GLEAM_Surface3D_Base(tex::t3d::value, fmt, mips, texflags)
    {
    }
};

struct GLEAM_Surface2DArray : GLEAM_Surface3D_Base
{
    friend struct GLEAM_Sampler2DArray;
    GLEAM_Surface2DArray(
        PixelFormat fmt, uint32 mips = 1, uint32 texflags = 0) :
        GLEAM_Surface3D_Base(tex::t2d_array::value, fmt, mips, texflags)
    {
    }
};

struct GLEAM_SurfaceCubeArray : GLEAM_Surface2DArray
{
    friend struct GLEAM_SamplerCube;

    GLEAM_SurfaceCubeArray(PixelFormat fmt, u32 mips = 1, u32 texflags = 0);
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

    u32& glTexHandle()
    {
        return texture;
    }
    u32& glSamplerHandle()
    {
        return m_sampler;
    }
};

struct GLEAM_Sampler : GraphicsAPI::Sampler
{
    GLEAM_Sampler();

    void alloc();
    void dealloc();

    void setLODRange(Vecf2 const& range);
    void setLODBias(scalar bias);
    void setEdgePolicy(uint8 dim, WrapPolicy p);
    void setFiltering(
        Filtering mag, Filtering min, Filtering mip = Filtering::None);

    void enableShadowSampler();

  protected:
    glhnd m_handle;
};

template<typename Surf>
struct GLEAM_Sampler_Base : GLEAM_Sampler
{
    void attach(Surf* surf)
    {
        m_surface = surf;
    }

  protected:
    Surf* m_surface;
};

struct GLEAM_Sampler2D : GLEAM_Sampler_Base<GLEAM_Surface2D>
{
    void                bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler3D : GLEAM_Sampler_Base<GLEAM_Surface3D>
{
    void                bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler2DArray : GLEAM_Sampler_Base<GLEAM_Surface2DArray>
{
    friend struct GLEAM_API;

    void                bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCube : GLEAM_Sampler_Base<GLEAM_SurfaceCube>
{
    void                bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCubeArray : GLEAM_Sampler_Base<GLEAM_SurfaceCubeArray>
{
    void                bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
