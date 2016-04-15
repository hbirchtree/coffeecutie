#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_buffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

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
    GLEAM_Surface::allocate();
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

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            CGL33::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                           ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        if(m_size.area()==0)
        {
            m_size = size;
            if(m_flags&GLEAM_API::TextureDMABuffered)
                CGL33::TexImage2D(m_type,m_mips,m_pixfmt,size.w,size.h,0,comp,fmt,0x0);
            else
                CGL33::TexImage2D(m_type,m_mips,m_pixfmt,size.w,size.h,0,comp,fmt,data);
        }else{
            if(m_flags&GLEAM_API::TextureDMABuffered)
                CGL33::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                     size.w,size.h,comp,fmt,0x0);
            else
                CGL33::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                     size.w,size.h,comp,fmt,data);
        }
        CGL33::BufBind(BufType::PixelUData,0);
    }else if(GL_CURR_API==GL_4_3)
    {
        CGL43::TexBind(m_type,m_handle);

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            CGL43::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        CGL43::TexStorage2D(m_type,m_mips,m_pixfmt,size.w,size.h);
        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                 size.w,size.h,comp,fmt,0x0);
        else
            CGL43::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                 size.w,size.h,comp,fmt,data);

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
    GLEAM_Surface::allocate();
    CGL33::TexBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexImage3D(Texture::T2DArray,0,m_pixfmt,
                          size.width,size.height,size.depth,0,c,
                          BitFormat::UByte,nullptr);
    }else if(GL_CURR_API==GL_4_3)
    {
        CGL43::TexStorage3D(Texture::T2DArray,m_mips,m_pixfmt,
                            size.width,size.height,size.depth);
    }
    m_size = size;
}

void GLEAM_Surface2DArray::upload(BitFormat fmt, PixelComponents comp,
                                  CSize3 size, c_cptr data, CPoint3 offset, uint32 mip)
{
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexBind(m_type,m_handle);

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            CGL33::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.volume()),data,
                           ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        if(m_size.volume()==0)
        {
            m_size = size;
            if(m_flags&GLEAM_API::TextureDMABuffered)
                CGL33::TexImage3D(m_type,m_mips,m_pixfmt,size.width,size.height,size.depth,0,comp,fmt,0x0);
            else
                CGL33::TexImage3D(m_type,m_mips,m_pixfmt,size.width,size.height,size.depth,0,comp,fmt,data);
        }else{
            if(m_flags&GLEAM_API::TextureDMABuffered)
                CGL33::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                                     size.width,size.height,size.depth,comp,fmt,0x0);
            else
                CGL33::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.x,
                                     size.width,size.height,size.depth,comp,fmt,data);
        }
        CGL33::BufBind(BufType::PixelUData,0);
    }else if(GL_CURR_API==GL_4_3)
    {
        CGL43::TexBind(m_type,m_handle);

        if(m_flags&GLEAM_API::TextureDMABuffered)
        {
            CGL43::BufBind(BufType::PixelUData,GLEAM_API::instance_data->pboQueue.current().buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.volume()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }

        CGL43::TexStorage3D(m_type,m_mips,m_pixfmt,size.width,size.height,size.depth);
        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                                 size.width,size.height,size.depth,comp,fmt,0x0);
        else
            CGL43::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                                 size.width,size.height,size.depth,comp,fmt,data);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::BufBind(BufType::PixelUData,0);
    }
}


void GLEAM_Sampler::alloc()
{
    CGL33::SamplerAlloc(1,&m_handle);
}

void GLEAM_Sampler::dealloc()
{
    CGL33::SamplerFree(1,&m_handle);
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

    h.texture = m_surface->m_handle;
    h.m_sampler = m_handle;

    return h;
}

}
}
}
