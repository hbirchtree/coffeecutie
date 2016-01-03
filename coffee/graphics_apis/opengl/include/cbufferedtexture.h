#ifndef COFFEE_GRAPHICS_OPENGL_CONVENIENCE_H
#define COFFEE_GRAPHICS_OPENGL_CONVENIENCE_H

#include <glbinding/wrappers/cbuffer.h>
#include <glbinding/wrappers/ctexture.h>

namespace Coffee{
namespace CGraphicsWrappers{

template<size_t Size>
class CBufferedTexture
{
public:
    /*!
     * \brief Create texture as well as buffers associated with it
     * \param size The size of the texture to be created
     * \param fmt The internal format of this texture
     * \param type The texture type, most likely 2D
     * \param levels The amount of mip levels
     * \param initData Initial data for pixel buffers
     * \param dFmt Format of initial data
     */
    void createTexture(
            const CTextureSize& size, const CTexIntFormat& fmt,
            const CTexType& type, const CGint& levels,
            const CByteData& initData, const CTexFormat& dFmt)
    {
        m_texture.type = type;
        m_texture.format = fmt;
        m_texture.levels = levels;
        m_texture.size = size;

        coffee_graphics_alloc(m_texture);
        coffee_graphics_activate(m_texture);
        coffee_graphics_alloc(m_sampler);
        coffee_graphics_alloc(m_buffers.size,m_buffers.data,CBufferType::PixelUnpack);

        switch(m_texture.size.dimensions())
        {
        case 1:
            coffee_graphics_tex_defimmutable_1d(m_texture);
            break;
        case 2:
            coffee_graphics_tex_defimmutable_2d(m_texture);
            break;
        case 3:
            coffee_graphics_tex_defimmutable_3d(m_texture);
            break;
        }

        for(size_t i=0;i<m_buffers.size;i++)
        {
            coffee_graphics_activate(m_buffers.current());
            coffee_graphics_buffer_store_immutable(m_buffers.current(),
                                                   initData.data,initData.size,
                                                   CBufferConstants::PersistentStorageFlags());
            coffee_graphics_buffer_map(m_buffers.current(),
                                       CBufferConstants::PersistentAccessFlags());
            m_buffers.advance();
        }
        uploadData(dFmt,0);

        coffee_graphics_tex_get_handle(m_texture,m_sampler);
    }

    void prefillBuffer(const CByteData& data)
    {
        c_memcpy(m_buffers.next().data,data.data,
                 CMath::min<CGsize>(data.size,m_buffers.next().size));
    }

    void uploadData(const CTexFormat& fmt, const CGint& level)
    {
        m_buffers.awaitCurrent();
        coffee_graphics_tex_pbo_upload(m_texture,m_buffers.current(),fmt,
                                       CDataType::UByte,level);
    }

    void advance()
    {
        m_buffers.lockCurrent();
        m_buffers.advance();
    }

    CTextureSampler& sampler()
    {
        return m_sampler;
    }
    CTexture& texture()
    {
        return m_texture;
    }
    CNBuffer<Size>& buffers()
    {
        return m_buffers;
    }
private:
    CTextureSampler m_sampler;
    CTexture m_texture;
    CNBuffer<Size> m_buffers;
};

}
}

#endif
