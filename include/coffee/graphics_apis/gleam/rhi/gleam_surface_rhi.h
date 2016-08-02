#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Surface : GraphicsAPI::Surface
{
    friend struct GLEAM_RenderTarget;

    GLEAM_Surface(Texture type, PixelFormat fmt,uint32 mips,uint32 texflags = 0);

    void allocate();
    void dealloc();

    Texture m_type;

protected:
    CGhnd m_handle;
};

struct GLEAM_Surface2D : GLEAM_Surface
{
    friend struct GLEAM_Sampler2D;

    GLEAM_Surface2D(PixelFormat fmt,uint32 mips = 1,uint32 texflags = 0);

    void allocate(CSize size, PixelComponents c);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize size, c_cptr data,
                CPoint offset = {}, uint32 mip = 0);

    /*TODO: Add download function */

protected:
    CSize m_size;
};

struct GLEAM_Surface3D_Base : GLEAM_Surface
{
    friend struct GLEAM_Sampler3D;

    GLEAM_Surface3D_Base(Texture t, PixelFormat fmt,uint32 mips,uint32 texflags);

    void allocate(CSize3 size, PixelComponents c);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize3 size, c_cptr data,
                CPoint3 offset = {}, uint32 mip = 0);

    /*TODO: Add download function */

    CSize3 m_size;
};

struct GLEAM_Surface3D : GLEAM_Surface3D_Base
{
    friend struct GLEAM_Sampler3D;
    GLEAM_Surface3D(PixelFormat fmt, uint32 mips = 1,uint32 texflags = 0):
        GLEAM_Surface3D_Base(Texture::T3D,fmt,mips,texflags) {}
};

struct GLEAM_Surface2DArray : GLEAM_Surface3D_Base
{
    friend struct GLEAM_Sampler2DArray;
    GLEAM_Surface2DArray(PixelFormat fmt, uint32 mips = 1,uint32 texflags = 0):
        GLEAM_Surface3D_Base(Texture::T2DArray,fmt,mips,texflags) {}
};

struct GLEAM_SamplerHandle
{
    union
    {
        struct
        {
            CGhnd texture;
            CGhnd m_unit;
        };
        CGhnd64 texture64;
    };
    CGhnd m_sampler;
    Texture m_type;
};

struct GLEAM_Sampler : GraphicsAPI::Sampler
{
    GLEAM_Sampler():
        m_handle(0)
    {
    }

    void alloc();
    void dealloc();

    void setLODRange(Vecf2 const& range);
    void setLODBias(scalar bias);
    void setEdgePolicy(uint8 dim, WrapPolicy p);
    void setFiltering(Filtering mag, Filtering min, Filtering mip = Filtering::None);

protected:
    CGhnd m_handle;
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
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler3D : GLEAM_Sampler_Base<GLEAM_Surface3D>
{
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_Sampler2DArray : GLEAM_Sampler_Base<GLEAM_Surface2DArray>
{
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCube : GLEAM_Sampler_Base<GLEAM_SurfaceCube>
{
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

struct GLEAM_SamplerCubeArray : GLEAM_Sampler_Base<GLEAM_SurfaceCubeArray>
{
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

}
}
}
