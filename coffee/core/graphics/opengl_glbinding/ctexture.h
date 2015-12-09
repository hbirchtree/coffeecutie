#ifndef CTEXTURE
#define CTEXTURE

#include "ctexture_pbo.h"
#include "ctexture_types.h"
#include "ctexture_storage.h"
#include "coffee/core/plat/cmemory.h"

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

}
}

#endif // CTEXTURE
