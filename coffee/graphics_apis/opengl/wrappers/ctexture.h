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
extern void coffee_graphics_alloc(size_t count, CTexture* tex);
/*!
 * \brief Free texture
 * \param tex
 */
extern void coffee_graphics_free(size_t count, CTexture* tex);

inline void coffee_graphics_alloc(CTexture& tex)
{
    coffee_graphics_alloc(1,&tex);
}

inline void coffee_graphics_free(CTexture& tex)
{
    coffee_graphics_free(1,&tex);
}

/*!
 * \brief Bind texture to its texture target
 * \param tex
 */
extern void coffee_graphics_bind(CTexture const& tex);
/*!
 * \brief Bind texture for ARB_shader_image_load_store operations
 */
extern void coffee_graphics_tex_bind_image(
        const CTexture& tex, const CGuint& unit, const CGint& level,
        bool layered, const CGint& layer, const CTextureAccess& access);
/*!
 * \brief Unbind texture from its texture target
 * \param tex
 */
extern void coffee_graphics_unbind(CTexture const& tex);
/*!
 * \brief Generate mipmapping for texture
 * \param tex
 */
extern void coffee_graphics_tex_mipmap(CTexture& tex);
/*!
 * \brief Generate mipmapping for texture, old variant
 * \param tex
 */
extern void coffee_graphics_tex_mipmap_safe(CTexture& tex);

/*!
 * \brief Binds and unbinds texture, used for bindless textures among others, only for convenience
 * \param tex
 */
extern void coffee_graphics_activate(CTexture &tex);

/*!
 * \brief Make a given texture sparse, allowing virtual addressing of texture space
 * \param tex
 * \param enable
 */
extern void coffee_graphics_tex_sparsify(CTexture& tex, bool enable);

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
extern void coffee_graphics_tex_dump(
        const CTexture& tex, cstring filename);

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
