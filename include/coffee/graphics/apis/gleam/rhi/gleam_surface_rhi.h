#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Surface : GraphicsAPI::Surface<CSize,CPoint>
{
    friend struct GLEAM_RenderTarget;

    GLEAM_Surface(Texture type, PixelFormat fmt,
                  uint32 mips,uint32 texflags = 0);

    void allocate();
    void dealloc();

    CGhnd handle();

    CGhnd& glTexHandle() {return m_handle;}

protected:
    void upload_info(PixCmp comp, uint32 mip, uint32 d);

    Texture m_type;
public:
    CGhnd m_handle;
};

struct GLEAM_Surface2D : GLEAM_Surface
{
    friend struct GLEAM_Sampler2D;

    GLEAM_Surface2D(PixelFormat fmt,uint32 mips = 1,uint32 texflags = 0);

    void allocate(CSize size, PixelComponents c);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize size, const Bytes &data,
                CPoint offset = {0,0}, uint32 mip = 0);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize size, c_cptr data,
                CPoint offset = {0,0}, uint32 mip = 0)
    {
        Bytes dataS;
        dataS.data = C_RCAST<byte_t*>(C_CCAST<c_ptr>(data));
        dataS.size = 0;
        upload(fmt, comp, size, dataS, offset, mip);
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

    GLEAM_Surface3D_Base(Texture t, PixelFormat fmt,
                         uint32 mips,uint32 texflags);

    void allocate(CSize3 size, PixelComponents c);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize3 size, const Bytes &data,
                CPoint3 offset = {0,0,0}, uint32 mip = 0);

    void upload(BitFormat fmt, PixelComponents comp,
                CSize3 size, c_cptr data,
                CPoint3 offset = {0,0,0}, uint32 mip = 0)
    {
        Bytes dataS;
        dataS.data = C_RCAST<byte_t*>(C_CCAST<c_ptr>(data));
        dataS.size = 0;
        upload(fmt, comp, size, dataS, offset, mip);
    }

    CSize3 texSize() const;

    /*TODO: Add download function */

    CSize3 m_size;

protected:
    Texture m_type;
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
    GLEAM_Surface2DArray(PixelFormat fmt, uint32 mips = 1,
                         uint32 texflags = 0):
        GLEAM_Surface3D_Base(Texture::T2DArray,fmt,mips,texflags) {}
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
            CGhnd texture;
            CGhnd m_unit;
        };
        CGhnd64 texture64;
    };
    CGhnd m_sampler;
    Texture m_type;

#if defined(COFFEE_ONLY_GLES20)
    u32 arraySize;
#endif

    CGhnd& glTexHandle() {return texture;}
    CGhnd& glSamplerHandle() {return m_sampler;}
};

struct GLEAM_Sampler : GraphicsAPI::Sampler
{
    GLEAM_Sampler();

    void alloc();
    void dealloc();

    void setLODRange(Vecf2 const& range);
    void setLODBias(scalar bias);
    void setEdgePolicy(uint8 dim, WrapPolicy p);
    void setFiltering(Filtering mag, Filtering min,
                      Filtering mip = Filtering::None);

    void enableShadowSampler();

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
    friend struct GLEAM_API;

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
