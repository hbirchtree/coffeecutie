#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Surface : GraphicsAPI::Surface
{
    GLEAM_Surface(Texture type, PixelFormat fmt,uint32 mips,uint32 texflags = 0):
        Surface(fmt,false,0,mips,texflags),
        m_type(type),
        m_handle(0)
    {
    }

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

struct GLEAM_Surface2DArray : GLEAM_Surface
{
    friend struct GLEAM_Sampler2DArray;

    GLEAM_Surface2DArray(PixelFormat fmt,uint32 mips = 1,uint32 texflags = 0);

    void allocate(CSize3 size, PixelComponents c);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize3 size, c_cptr data,
                CPoint3 offset = {}, uint32 mip = 0);

    /*TODO: Add download function */

protected:
    CSize3 m_size;
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

struct GLEAM_Sampler2DArray : GLEAM_Sampler_Base<GLEAM_Surface2DArray>
{
    void bind(uint32 i);
    GLEAM_SamplerHandle handle();
};

}
}
}
