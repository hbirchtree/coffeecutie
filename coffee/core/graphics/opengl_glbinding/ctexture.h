#ifndef CTEXTURE
#define CTEXTURE

#include "ctexture_pbo.h"
#include "ctexture_types.h"
#include "ctexture_storage.h"
#include "coffee/core/plat/cmemory.h"

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
 * \brief Set texture parameter, GLint variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param(
        const CTexture* tex, CTexParam param, CGint val);

/*!
 * \brief Set texture parameter, GLfloat variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_paramf(
        const CTexture* tex, CTexParam param, scalar val);
/*!
 * \brief Set texture parameter, GLenum variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param(
        const CTexture* tex, CTexParam param, CTexParamOpt val);

/*!
 * \brief Set texture parameter, GLint variant, old variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param_safe(
        const CTexture* tex, CTexParam param, CGint val);

/*!
 * \brief Set texture parameter, GLfloat variant, old variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_paramf_safe(
        const CTexture* tex, CTexParam param, scalar val);
/*!
 * \brief Set texture parameter, GLenum variant, old variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param_safe(
        const CTexture* tex, CTexParam param, CTexParamOpt val);

/*!
 * \brief Binds and unbinds texture, used for bindless textures among others, only for convenience
 * \param tex
 */
extern void     coffee_graphics_activate(
        const CTexture* tex);

/*!
 * \brief Acquire a bindless texture handle for a texture
 * \param tex
 * \return
 */
extern CGuint64 coffee_graphics_tex_get_handle(CTexture* tex);
/*!
 * \brief Ask the GL to load the texture into memory
 * \param tex
 */
extern void     coffee_graphics_tex_make_resident(const CTexture* tex);
/*!
 * \brief Ask the GL to unload the texture from memory
 * \param tex
 */
extern void     coffee_graphics_tex_make_nonresident(const CTexture* tex);

/*!
 * \brief Set up texture for rendering
 * \param tex
 */
extern void     coffee_graphics_tex_use(const CTexture* tex);
/*!
 * \brief Set up texture for rendering, old variant with GL_TEXTURE* units
 * \param tex
 */
extern void     coffee_graphics_tex_use_safe(const CTexture* tex);

/*!
 * \brief Unload texture
 * \param tex
 */
extern void     coffee_graphics_tex_unload(const CTexture* tex);
/*!
 * \brief Unload texture, old variant with GL_TEXTURE* units
 */
extern void     coffee_graphics_tex_unload_safe(const CTexture*);

/*!
 * \brief Make a given texture sparse, allowing virtual addressing of texture space
 * \param tex
 * \param enable
 */
extern void coffee_graphics_tex_sparsify(CTexture* tex, bool enable);

}
}

#endif // CTEXTURE
