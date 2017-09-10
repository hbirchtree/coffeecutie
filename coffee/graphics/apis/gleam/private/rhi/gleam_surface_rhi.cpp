#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>

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
    if(GL_CURR_API != GL_4_3
            && GL_CURR_API != GLES_3_0
            && GL_CURR_API != GLES_3_2)
    {
        /* We must set this to register a proper mipmap level */
#if !defined(COFFEE_ONLY_GLES20)
//        int32 min_lev = 0;
        int32 max_lev = mips - 1;
        CGL33::TexBind(type,m_handle);
        CGL33::TexParameteriv(type,GL_TEXTURE_MAX_LEVEL,&max_lev);
        CGL33::TexBind(type,0);
#endif
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

CGhnd GLEAM_Surface::handle()
{
    return m_handle;
}

void GLEAM_Surface::upload_info(PixCmp comp, uint32 mip, uint32 d)
{
    C_USED(comp);
    C_USED(mip);
    C_USED(d);

#if !defined(COFFEE_ONLY_GLES20)
    if(GL_DEBUG_MODE){
        uint32 w,h,d_;
        szptr size;
        CGL33::TexGetImageSize(m_type,comp,w,h,d_,&size,mip);
        if(m_type == Texture::T2DArray)
            size /= d;
        cVerbose(5,"Texture allocation size ({0}): {1}",m_handle,size);

        /* TODO: Add information about mipmaps */
    }
#endif
}

GLEAM_Surface2D::GLEAM_Surface2D(PixelFormat fmt,uint32 mips,uint32 texflags):
    GLEAM_Surface(Texture::T2D,fmt,mips,texflags),
    m_size(0,0)
{
}

void GLEAM_Surface2D::allocate(CSize size, PixelComponents c)
{
    CGL33::TexBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_2_0 || GL_CURR_API==GLES_3_0)
    {
        CGL33::TexImage2D(Texture::T2D,0,m_pixfmt,
                          size.w,size.h,0,c,
                          BitFormat::UByte,nullptr);
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        CGL43::TexStorage2D(Texture::T2D,m_mips,m_pixfmt,
                            size.w,size.h);
    }
#endif
    m_size = size;
}

void GLEAM_Surface2D::upload(BitFormat fmt, PixelComponents comp,
                             CSize size, c_cptr data,
                             CPoint offset, uint32 mip)
{
    c_cptr data_ptr = data;
    auto msz = size.convert<u32>();

#if !defined(COFFEE_ONLY_GLES20)
    if(m_flags&GLEAM_API::TextureDMABuffered)
    {
        GLEAM_PboQueue::Pbo& pbo = GLEAM_API_INSTANCE_DATA->pboQueue.current();

        if((GL_CURR_API==GL_4_3 || GL_CURR_API==GL_4_5) && CGL43::BufferStorageSupported())
        {
            data_ptr = 0x0;
            CGL43::BufBind(BufType::PixelUData,pbo.buf);
            if(pbo.flags != 0)
                CGL43::BufInvalidateData(pbo.buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }else
        {
        data_ptr = 0x0;
        CGL33::BufBind(BufType::PixelUData,pbo.buf);
        CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.area()),data,
                       ResourceAccess::WriteOnly|ResourceAccess::Streaming);
        }
    }
#endif

    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_2_0)
    {
        CGL33::TexBind(m_type,m_handle);

        if(m_size.area()==0)
        {
            m_size = size;
            CGL33::TexImage2D(m_type,m_mips,m_pixfmt,msz.w,msz.h,0,comp,fmt,data_ptr);
        }else{
            CGL33::TexSubImage2D(m_type,mip,offset.x,offset.y,
                                 msz.w,msz.h,comp,fmt,data_ptr);
        }
        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(BufType::PixelUData,0);

        CGL33::TexGenMipmap(m_type);
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        CGL43::TexBind(m_type,m_handle);

        CGL43::TexStorage2D(m_type,m_mips,m_pixfmt,size.w,size.h);
        CGL43::TexSubImage2D(m_type,mip,offset.x,offset.y,
                             size.w,size.h,comp,fmt,data_ptr);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::BufBind(BufType::PixelUData,0);
    }
#endif

    if(GL_DEBUG_MODE)
        upload_info(comp,mip,1);
}

GLEAM_Surface3D_Base::GLEAM_Surface3D_Base(Texture t, PixelFormat fmt, uint32 mips, uint32 texflags):
#if !defined(COFFEE_ONLY_GLES20)
    GLEAM_Surface(t,fmt,mips,texflags),
#else
    GLEAM_Surface(Texture::T2D,fmt,mips,texflags),
#endif
    m_size(0,0,0),
#if defined(COFFEE_ONLY_GLES20)
    m_type(Texture::T2D)
#else
    m_type(t)
#endif
{
    C_USED(t);
}

#define TEX_SQUARE_GRID_SIZE(depth) \
    C_CAST<u32>(CMath::ceil(CMath::sqrt(depth)))

void GLEAM_Surface3D_Base::allocate(CSize3 size, PixelComponents c)
{
    m_size = size;

    auto msz = size.convert<u32>();

#if !defined(COFFEE_ONLY_GLES20)
    CGL33::TexBind(m_type,m_handle);
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexImage3D(m_type, 0,m_pixfmt,
                          msz.width,msz.height,msz.depth,
                          0,c,
                          BitFormat::UByte,nullptr);
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API == GLES_3_0 || GL_CURR_API==GLES_3_2)
    {
        CGL43::TexStorage3D(m_type, m_mips,m_pixfmt,
                            msz.width,msz.height,msz.depth);
    }
#else
    CGL33::TexBind(m_type, m_handle);
    
    u32 square_size = TEX_SQUARE_GRID_SIZE(size.depth);
    
    CGL33::TexImage2D(m_type, 0, m_pixfmt,
                      square_size * msz.width, square_size * msz.height,
                      0, c, BitFormat::UByte, nullptr);
#endif
}

void GLEAM_Surface3D_Base::upload(BitFormat fmt, PixelComponents comp,
                                  CSize3 size, c_cptr data, CPoint3 offset, uint32 mip)
{
    auto msz = size.convert<u32>();
    auto mof = offset.convert<u32>();
#if !defined(COFFEE_ONLY_GLES20)
    /* If we want to use DMA transfer */
    c_cptr data_ptr = data;

    if(m_flags&GLEAM_API::TextureDMABuffered)
    {
        data_ptr = 0x0;
        if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0 || GL_CURR_API==GLES_3_2)
        {
            CGL33::BufBind(BufType::PixelUData,GLEAM_API_INSTANCE_DATA->pboQueue.current().buf);
            CGL33::BufData(BufType::PixelUData,GetPixSize(fmt,comp,size.convert<u32>().volume()),data,
                           ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }else if (GL_CURR_API == GL_4_3)
        {
            CGL43::BufBind(BufType::PixelUData,GLEAM_API_INSTANCE_DATA->pboQueue.current().buf);
            CGL43::BufStorage(BufType::PixelUData,GetPixSize(fmt,comp,size.convert<u32>().volume()),data,
                              ResourceAccess::WriteOnly|ResourceAccess::Persistent);
        }
    }


    if(GL_CURR_API==GL_3_3)
    {
        CGL33::TexBind(m_type,m_handle);

        CGL33::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                             size.width,size.height,size.depth,comp,fmt,data_ptr);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL33::BufBind(BufType::PixelUData,0);
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
    {
        CGL43::TexBind(m_type,m_handle);

        CGL43::TexSubImage3D(m_type,mip,offset.x,offset.y,offset.z,
                             size.width,size.height,size.depth,comp,fmt,data_ptr);

        if(m_flags&GLEAM_API::TextureDMABuffered)
            CGL43::BufBind(BufType::PixelUData,0);
    }

    if(GL_DEBUG_MODE)
        upload_info(comp,mip,size.depth);
#else
    // Packs all 2D textures into one 2D texture atlas, limited size
    
    CGL33::TexBind(m_type, m_handle);
    
    u32 g_size = TEX_SQUARE_GRID_SIZE(m_size.depth);
    
    for(u32 i=0;i < C_FCAST<u32>(size.depth); i++)
    {
        auto mofi = mof;
        
        auto x_coord = (mof.z + i) % g_size;
        auto y_coord = (mof.z + i) / g_size;
        
        mofi.x += m_size.width * x_coord;
        mofi.y += m_size.height * y_coord;
        
        CGL33::TexSubImage2D(m_type, mip,
                             mofi.x, mofi.y, msz.width, msz.height,
                             comp, fmt, data);
    }
#endif
}

void GLEAM_Sampler::alloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    if(GL_CURR_API == GL_3_3 || GL_CURR_API == GL_4_3
            || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
    {
        CGL33::SamplerAlloc(1,&m_handle);
    }
#ifdef COFFEE_GLEAM_DESKTOP
    else if(GL_CURR_API==GL_4_5)
        CGL45::SamplerAlloc(1,&m_handle);
#endif
#endif
}

void GLEAM_Sampler::dealloc()
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::SamplerFree(1,&m_handle);
#endif
}

void GLEAM_Sampler::setLODRange(const Vecf2 &range)
{
    C_USED(range);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::SamplerParameterfv(m_handle,GL_TEXTURE_MIN_LOD,&range.x());
    CGL33::SamplerParameterfv(m_handle,GL_TEXTURE_MAX_LOD,&range.y());
#endif
}

void GLEAM_Sampler::setLODBias(scalar bias)
{
    C_USED(bias);
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
    C_USED(dim);
    C_USED(p);
#if !defined(COFFEE_ONLY_GLES20)
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
#endif
}

void GLEAM_Sampler::setFiltering(Filtering mag, Filtering min, Filtering mip)
{
    C_USED(mag);
    C_USED(min);
    C_USED(mip);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::SamplerParameteri(m_handle,GL_TEXTURE_MAG_FILTER,mag);
    CGL33::SamplerParameteri(m_handle,GL_TEXTURE_MIN_FILTER,min,mip);
#endif
}

void GLEAM_Sampler2D::bind(uint32 i)
{
    CGL33::TexActive(i);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::SamplerBind(i,m_handle);
#endif
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

void GLEAM_Sampler3D::bind(uint32 i)
{
    C_USED(i);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::TexActive(i);
    CGL33::SamplerBind(i,m_handle);
    CGL33::TexBind(m_surface->m_type,m_surface->m_handle);
#endif
}

GLEAM_SamplerHandle GLEAM_Sampler3D::handle()
{
    GLEAM_SamplerHandle h = {};
#if !defined(COFFEE_ONLY_GLES20)
    /* TODO: Add bindless */
    h.m_type = m_surface->m_type;
    h.texture = m_surface->m_handle;
    h.m_sampler = m_surface->m_handle;
#endif
    return h;
}

void GLEAM_Sampler2DArray::bind(uint32 i)
{
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::TexActive(i);
    CGL33::SamplerBind(i,m_handle);
    CGL33::TexBind(m_surface->m_type,m_surface->m_handle);
#else
    CGL33::TexActive(i);
    CGL33::TexBind(m_surface->m_type, m_surface->m_handle);
#endif
}

GLEAM_SamplerHandle GLEAM_Sampler2DArray::handle()
{
    GLEAM_SamplerHandle h = {};
    h.m_type = m_surface->m_type;
#if !defined(COFFEE_ONLY_GLES20)
    h.texture = m_surface->m_handle;
    h.m_sampler = m_handle;
#else
    CGL33::TexGenMipmap(m_surface->m_type);
    h.texture = m_surface->m_handle;
#endif

    return h;
}

}
}
}
