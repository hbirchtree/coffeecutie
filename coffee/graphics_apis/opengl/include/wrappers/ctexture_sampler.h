#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_SAMPLER_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_SAMPLER_H

#include "copengl_types.h"
#include "ctexture_types.h"

namespace Coffee{
namespace CGraphicsWrappers{
/*!
 * \brief Allocate texture sampler
 * \param sampler
 */
extern void coffee_graphics_alloc(size_t count, CTextureSampler* sampler);
/*!
 * \brief Free texture sampler
 * \param sampler
 */
extern void coffee_graphics_free(size_t count, CTextureSampler* sampler);

inline void coffee_graphics_alloc(CTextureSampler& sampler)
{
    coffee_graphics_alloc(1,&sampler);
}

inline void coffee_graphics_free(CTextureSampler& sampler)
{
    coffee_graphics_free(1,&sampler);
}

/*!
 * \brief Get texture handle for a texture-sampler pair
 * \param texture Texture target
 * \param samp Sampler target
 * \return The handle of the created pair. Will return the same handle if it is already acquired.
 */
extern CGhnd64 coffee_graphics_tex_get_handle(const CTexture &texture, CTextureSampler &samp);
/*!
 * \brief Make a texture resident for sampling
 * \param samp
 */
extern void coffee_graphics_tex_make_resident(CTextureSampler const& samp);
/*!
 * \brief Move texture out of residency
 * \param samp
 */
extern void coffee_graphics_tex_make_nonresident(CTextureSampler const& samp);

/*!
 * \brief Set texture parameter, GLint variant
 * \param tex
 * \param param
 * \param val
 */
extern void coffee_graphics_tex_sampler_parami(
        CTextureSampler &tex, CTexParam const& param, CGint const& val);

/*!
 * \brief Set texture parameter, GLfloat variant
 * \param tex
 * \param param
 * \param val
 */
extern void coffee_graphics_tex_sampler_paramf(
        CTextureSampler &tex, CTexParam const& param, scalar const& val);
/*!
 * \brief Set texture parameter, GLenum variant
 * \param tex
 * \param param
 * \param val
 */
extern void coffee_graphics_tex_sampler_parame(
        CTextureSampler &tex, CTexParam const& param, CTexParamOpt const& val);

extern void coffee_graphics_tex_load(
        CTextureSampler const& tex, CTexture const&);

extern void coffee_graphics_tex_unload(
        CTextureSampler const& tex, CTexture const&);

extern void coffee_graphics_tex_load_safe(
        CTextureSampler const& sampler, CTexture const& tex);

extern void coffee_graphics_tex_unload_safe(
        CTextureSampler const&, CTexture const&);

}
}

#endif
