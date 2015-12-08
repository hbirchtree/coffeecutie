#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_STORAGE_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_STORAGE_H

#include "ctexture_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

extern bool coffee_graphics_tex_2d_store_safe(
        const CTexture* texture, const CTextureData* data,
        CGint level);
extern bool coffee_graphics_tex_3d_store_safe(
        const CTexture* texture, const CTextureData* data,
        CGint level);
extern bool coffee_graphics_tex_cube_store_safe(
        const CTexture* texture, const CTextureData* data,
        CGint level);

extern bool coffee_graphics_tex_2d_define_safe(
        const CTexture* texture, const CTextureData* data);
extern bool coffee_graphics_tex_3d_define_safe(
        const CTexture* texture, const CTextureData* data);

//Texture storage, modern versions, bindless
extern bool coffee_graphics_tex_2d_store(
        const CTexture* texture, const CTextureData* data,
        CGint level);
extern bool coffee_graphics_tex_3d_store(
        const CTexture* texture, const CTextureData* data,
        CGint level);
extern bool coffee_graphics_tex_cube_store(
        const CTexture* texture, const CTextureData* data,
        CGint level);

extern bool coffee_graphics_tex_2d_define(
        const CTexture* texture, const CTextureData* data);
extern bool coffee_graphics_tex_3d_define(
        const CTexture* texture, const CTextureData* data);

/*!
 * \brief Defines texture storage, used before storing
 * \param tex Texture to define for
 * \param data Format to define
 * \return Whether or not the process succeeded
 */
extern bool coffee_graphics_tex_define(
        const CTexture* tex, const CTextureData* data);
/*!
 * \brief Defines texture storage, used before storing, old variant
 * \param tex Texture to define for
 * \param data Format to define
 * \return Whether or not the process succeeded
 */
extern bool coffee_graphics_tex_define_safe(
        const CTexture* tex, const CTextureData* data);

/*!
 * \brief Store texture data into texture
 * \param tex Texture to store to
 * \param data Texture data to store
 * \param level Mipmap level to store
 * \return Whether or not the process succeeded
 */
extern bool coffee_graphics_tex_store(
        const CTexture* tex, const CTextureData* data, CGint level);
/*!
 * \brief Store texture data into texture, old variant
 * \param tex Texture to store to
 * \param data Texture data to store
 * \param level Mipmap level to store
 * \return Whether or not the process succeeded
 */
extern bool coffee_graphics_tex_store_safe(
        const CTexture* tex, const CTextureData* data, CGint level);
}
}

#endif
