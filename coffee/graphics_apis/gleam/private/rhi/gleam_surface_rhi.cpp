#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_buffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

GLEAM_Surface::GLEAM_Surface(Texture type, PixelFormat fmt, uint32 mips, uint32 texflags):
    Surface(fmt,false,0,mips,texflags),
    m_type(type),
    m_handle(0)
{
    allocate();
    {
        /* We must set this to register a proper mipmap level */

        int32 min_lev = 0;
        int32 max_lev = mips - 1;

        CGL33::TexBind(type,m_handle);
        CGL33::TexParameteriv(type,GL_TEXTURE_BASE_LEVEL,&min_lev);
        CGL33::TexParameteriv(type,GL_TEXTURE_MAX_LEVEL,&max_lev);
        CGL33::TexBind(type,0);
    }
}

void GLEAM_Surface::allocate()
{
    CGL33::TexAlloc(1,&m_handle);
}

void GLEAM_Surface::dealloc()
{
    CGL33::TexFree(1,&m_handle);
}

GLEAM_Surface2D::GLEAM_Surface2D(PixelFormat fmt,uint32 mips,uint32 texflags):
    GLEAM_Surface(Texture::T2D,fmt,mips,texflags),
    m_size(0,0)
{
}

void GLEAM_Surface2D::allocate(CSize size, PixelComponents c)
{
    CGL33::TexBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexImage2D(Texture::T2D,0,m_pixfmt,
                          size.w,size.h,0,c,
                          BitFormat::UByte,nullptr);
    }else if(GL_CURR_API==GL_4_3)
    {
        CGL43::TexStorage2D(Texture::T2D,m_mips,m_pixfmt,
                            size.w,size.h);
    }
    m_size = size;
}

void GLEAM_Surface2D::upload(BitFormat fmt, PixelComponents comp,
                             CSize size, c_cptr data,
                             CPoint offset, uint32 mip)
{
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexBind(m_type,m_handle);
        c_cptr data_ptr = data;

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            data_ptr = 0x0;
            CGL33::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                           ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        if(m_size.area()==0)
        {
            m_size = size;
            CGL33::TexImage2D(m_type,m_mips,m_pixfmt,size.w,size.h,0,comp,fmt,data_ptr);
        }else{
            CGL33::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                 size.w,size.h,comp,fmt,data_ptr);
        }
        CGL33::BufBind(BufType::PixelUData,0);
    }else if(GL_CURR_API==GL_4_3)
    {
        CGL43::TexBind(m_type,m_handle);
        c_cptr data_ptr = data;

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            data_ptr = 0x0;
            CGL43::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        CGL43::TexStorage2D(m_type,m_mips,m_pixfmt,size.w,size.h);
        CGL43::TexSubImage2D(m_type,mip,offset.x,offset.y,
                             size.w,size.h,comp,fmt,data_ptr);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::BufBind(BufType::PixelUData,0);
    }
}

GLEAM_Surface2DArray::GLEAM_Surface2DArray(PixelFormat fmt, uint32 mips, uint32 texflags):
    GLEAM_Surface(Texture::T2DArray,fmt,mips,texflags),
    m_size(0,0,0)
{
}

void GLEAM_Surface2DArray::allocate(CSize3 size, PixelComponents c)
{
    CGL33::TexBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexImage3D(Texture::T2DArray,0,m_pixfmt,
                          size.width,size.height,size.depth,0,c,
                          BitFormat::UByte,nullptr);
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API == GLES_3_0)
    {
        CGL43::TexStorage3D(Texture::T2DArray,m_mips,m_pixfmt,
                            size.width,size.height,size.depth);
    }
    m_size = size;
}

void GLEAM_Surface2DArray::upload(BitFormat fmt, PixelComponents comp,
                                  CSize3 size, c_cptr data, CPoint3 offset, uint32 mip)
{
    /* If we want to use DMA transfer */
    c_cptr data_ptr = data;

    if(m_flags&GLEAM_API::TextureDMABuffered)
    {
        data_ptr = 0x0;
        if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
        {
            CGL33::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.volume()),data,
                           ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }else if (GL_CURR_API == GL_4_3)
        {
            CGL43::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.volume()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }
    }


    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexBind(m_type,m_handle);

        CGL33::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                             size.width,size.height,size.depth,comp,fmt,data_ptr);

        CGL33::BufBind(BufType::PixelUData,0);

    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
    {
        CGL43::TexBind(m_type,m_handle);

        CGL43::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                             size.width,size.height,size.depth,comp,fmt,data_ptr);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::BufBind(BufType::PixelUData,0);
    }
}


void GLEAM_Sampler::alloc()
{
    if(GL_CURR_API == GL_3_3 || GL_CURR_API == GL_4_3
            || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
    {
        CGL33::SamplerAlloc(1,&m_handle);
    }
#ifdef COFFEE_GLEAM_DESKTOP
    else if(GL_CURR_API==GL_4_5)
        CGL45::SamplerAlloc(1,&m_handle);
#endif
}

void GLEAM_Sampler::dealloc()
{
    CGL33::SamplerFree(1,&m_handle);
}

void GLEAM_Sampler::setLODRange(const Vecf2 &range)
{
        CGL33::SamplerParameterfv(m_handle,GL_TEXTURE_MIN_LOD,&range.x());
        CGL33::SamplerParameterfv(m_handle,GL_TEXTURE_MAX_LOD,&range.y());
}

void GLEAM_Sampler::setLODBias(scalar bias)
{
#ifdef COFFEE_GLEAM_DESKTOP
    if(GL_DEBUG_MODE)
    {
        scalar max = CGL33::Debug::GetScalar(GL_MAX_TEXTURE_LOD_BIAS);
        if(bias > max)
        {
            cWarning("GL_TEXTURE_LOD_BIAS: {0} > {1}",bias,max);
            return;
        }
    }
    CGL33::SamplerParameterfv(m_handle,GL_TEXTURE_LOD_BIAS,&bias);
#endif
}

void GLEAM_Sampler::setEdgePolicy(uint8 dim, WrapPolicy p)
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

    CGL33::SamplerParameteri(m_handle,d,p);
}

void GLEAM_Sampler::setFiltering(Filtering mag, Filtering min, Filtering mip)
{
    CGL33::SamplerParameteri(m_handle,GL_TEXTURE_MAG_FILTER,mag);
    CGL33::SamplerParameteri(m_handle,GL_TEXTURE_MIN_FILTER,min,mip);
}

void GLEAM_Sampler2D::bind(uint32 i)
{
    CGL33::TexActive(i);
    CGL33::SamplerBind(i,m_handle);
    CGL33::TexBind(m_surface->m_type,m_surface->m_handle);
}

GLEAM_SamplerHandle GLEAM_Sampler2D::handle()
{
    GLEAM_SamplerHandle h = {};

    /* TODO: Add bindless */
    h.m_type = m_surface->m_type;
    h.texture = m_surface->m_handle;
    h.m_sampler = m_handle;

    return h;
}

void GLEAM_Sampler2DArray::bind(uint32 i)
{
    CGL33::TexActive(i);
    CGL33::SamplerBind(i,m_handle);
    CGL33::TexBind(m_surface->m_type,m_surface->m_handle);
}

GLEAM_SamplerHandle GLEAM_Sampler2DArray::handle()
{
    GLEAM_SamplerHandle h = {};

    h.m_type = m_surface->m_type;
    h.texture = m_surface->m_handle;
    h.m_sampler = m_handle;

    return h;
}

}
}
}
