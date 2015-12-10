#ifndef CTEXTURE
#define CTEXTURE

#include <coffee/core/plat/cmemory.h>
#include <coffee/image/cimage.h>

#include "ctexture_pbo.h"
#include "ctexture_types.h"
#include "ctexture_storage.h"
#include "ctexture_sampler.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Allocate texture
 * \param tex
 */
extern void     coffee_graphics_alloc(CTexture* tex);
/*!
 * \brief Free texture
 * \param tex
 */
extern void     coffee_graphics_free(CTexture* tex);

/*!
 * \brief Bind texture to its texture target
 * \param tex
 */
extern void     coffee_graphics_bind(
        const CTexture* tex);
/*!
 * \brief Unbind texture from its texture target
 * \param tex
 */
extern void     coffee_graphics_unbind(
        const CTexture* tex);
/*!
 * \brief Generate mipmapping for texture
 * \param tex
 */
extern void     coffee_graphics_tex_mipmap(
        const CTexture* tex);
/*!
 * \brief Generate mipmapping for texture, old variant
 * \param tex
 */
extern void     coffee_graphics_tex_mipmap_safe(
        const CTexture* tex);

/*!
 * \brief Binds and unbinds texture, used for bindless textures among others, only for convenience
 * \param tex
 */
extern void     coffee_graphics_activate(
        const CTexture* tex);

/*!
 * \brief Make a given texture sparse, allowing virtual addressing of texture space
 * \param tex
 * \param enable
 */
extern void coffee_graphics_tex_sparsify(CTexture* tex, bool enable);

/*!
 * \brief Set all data of texture to the specified data
 * \param tex
 * \param level
 * \param region
 * \param type
 * \param data
 */
extern void coffee_graphics_tex_memset(
        CTexture &tex, CGint const& level,
        CDataType const& type, c_cptr data);

/*!
 * \brief Set all data of texture to the specified data
 * \param tex
 * \param level
 * \param region
 * \param type
 * \param data
 */
extern void coffee_graphics_tex_memset_region(
        CTexture &tex, CGint const& level,
        CTextureRegion const& region,
        CDataType const& type, c_cptr data);




/*
 * Below is some CImage-bindings to simplify dumping/loading of textures
 */


/*!
 * \brief Debug function for dumping texture to file
 * \param tex
 * \param filename
 */
extern void     coffee_graphics_tex_dump(
        const CTexture* tex, cstring filename);

/*!
 * \brief Download GL texture to an image, sets image's properties
 * \param tex Texture to download
 * \param level Mipmap level to download
 * \param size Expected size of buffer (is allocated)
 * \param format Format of data
 * \param img Target image
 */
extern void     coffee_graphics_tex_download_texture(
        const CTexture* tex, CGint level,
        CGsize size, CTexFormat format, CStbImageLib::CStbImage* img);

/*!
 * \brief The simple way to load a texture
 * \param resource
 * \param location
 * \return Null if operation failed, valid pointer if success
 */
extern CTextureData* coffee_graphics_tex_create_texdata(
        const CResources::CResource &resource, c_ptr location);
/*!
 * \brief Free the texture data (correctly)
 * \param texd
 */
extern void coffee_graphics_tex_free_texdata(CTextureData* texd);

class CImportedTexture : public _cbasic_raii_container<CTextureData>
{
public:
    using _cbasic_raii_container::_cbasic_raii_container;
    virtual ~CImportedTexture()
    {
        coffee_graphics_tex_free_texdata(m_data);
    }
};

extern CImportedTexture coffee_graphics_tex_create_rtexdata(
        const CResources::CResource &resource);
}
}

#endif // CTEXTURE
