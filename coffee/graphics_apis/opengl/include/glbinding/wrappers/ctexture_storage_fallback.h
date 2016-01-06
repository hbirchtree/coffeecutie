#ifndef COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_TEXTURE_STORAGE_FALLBACK_H
#define COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_TEXTURE_STORAGE_FALLBACK_H

#include "ctexture_types.h"

namespace Coffee{
namespace CGraphicsWrappers{
/*!
 * \brief Define mutable texture storage for a 1D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_defmutable_1d(CTexture& tex);
/*!
 * \brief Define mutable texture storage for a 2D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_defmutable_2d(CTexture& tex);
/*!
 * \brief Define mutable texture storage for a 3D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_defmutable_3d(CTexture& tex);

/*!
 * \brief Store texture data in a region of a 1D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_substore_1d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);
/*!
 * \brief Store texture data in a region of a 2D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_substore_2d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);
/*!
 * \brief Store texture data in a region of a 3D texture. This is a fallback.
 * \param tex
 */
extern void coffee_graphics_tex_substore_3d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);

/*!
 * \brief Define page commitment of sparse texture
 * \param tex
 * \param commit
 * \param level
 * \param reg
 */
extern void coffee_graphics_tex_sparse_commit_safe(
        CTexture& tex, bool commit, const CGint& level, const CTextureRegion& reg);

/*!
 * \brief Store texture data in a 1D texture. This is a fallback.
 * \param tex
 */
inline void coffee_graphics_tex_store_1d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.width = tex.size.width;
    coffee_graphics_tex_substore_1d_safe(tex,data,level,reg);
}
/*!
 * \brief Store texture data in a 2D texture. This is a fallback.
 * \param tex
 */
inline void coffee_graphics_tex_store_2d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.width = tex.size.width;
    reg.height = tex.size.height;
    coffee_graphics_tex_substore_2d_safe(tex,data,level,reg);
}
/*!
 * \brief Store texture data in a 3D texture. This is a fallback.
 * \param tex
 */
inline void coffee_graphics_tex_store_3d_safe(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.width = tex.size.width;
    reg.height = tex.size.height;
    reg.depth = tex.size.depth;
    coffee_graphics_tex_substore_3d_safe(tex,data,level,reg);
}

inline void coffee_graphics_tex_define_safe(
        CTexture& tex)
{
    switch(tex.size.dimensions())
    {
    case 1:
        coffee_graphics_tex_defmutable_1d(tex);
        break;
    case 2:
        coffee_graphics_tex_defmutable_2d(tex);
        break;
    case 3:
        coffee_graphics_tex_defmutable_3d(tex);
        break;
    }
}

inline void coffee_graphics_tex_store_safe(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    switch(tex.size.dimensions())
    {
    case 1:
        coffee_graphics_tex_store_1d_safe(tex,data,level);
        break;
    case 2:
        coffee_graphics_tex_store_2d_safe(tex,data,level);
        break;
    case 3:
        coffee_graphics_tex_store_3d_safe(tex,data,level);
        break;
    }
}

}
}

#endif
