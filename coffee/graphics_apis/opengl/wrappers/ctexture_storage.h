#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_STORAGE_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_STORAGE_H

#include "ctexture_storage_fallback.h"

namespace Coffee{
namespace CGraphicsWrappers{
/*!
 * \brief Define immutable texture storage for a 1D texture
 * \param tex
 */
extern void coffee_graphics_tex_defimmutable_1d(CTexture& tex);
/*!
 * \brief Define immutable texture storage for a 2D texture
 * \param tex
 */
extern void coffee_graphics_tex_defimmutable_2d(CTexture& tex);
/*!
 * \brief Define immutable texture storage for a 3D texture
 * \param tex
 */
extern void coffee_graphics_tex_defimmutable_3d(CTexture& tex);

/*!
 * \brief Store texture data in a region of a 1D texture
 * \param tex
 * \param data
 * \param level
 * \param reg
 */
extern void coffee_graphics_tex_substore_1d(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);
/*!
 * \brief Store texture data in a region of a 2D texture
 * \param tex
 * \param data
 * \param level
 * \param reg
 */
extern void coffee_graphics_tex_substore_2d(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);
/*!
 * \brief Store texture data in a region of a 3D texture
 * \param tex
 * \param data
 * \param level
 * \param reg
 */
extern void coffee_graphics_tex_substore_3d(
        CTexture& tex, const CTextureData& data, const CGint& level, const CTextureRegion& reg);

/*!
 * \brief Store texture data in a 1D texture
 * \param tex
 * \param data
 * \param level
 */
inline void coffee_graphics_tex_store_1d(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.w = tex.size.w;
    coffee_graphics_tex_substore_1d(tex,data,level,reg);
}
/*!
 * \brief Store texture data in a 1D texture
 * \param tex
 * \param data
 * \param level
 */
inline void coffee_graphics_tex_store_2d(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.w = tex.size.w;
    reg.h = tex.size.h;
    coffee_graphics_tex_substore_2d(tex,data,level,reg);
}
/*!
 * \brief Store texture data in a 1D texture
 * \param tex
 * \param data
 * \param level
 */
inline void coffee_graphics_tex_store_3d(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    CTextureRegion reg = {};
    reg.w = tex.size.w;
    reg.h = tex.size.h;
    reg.d = tex.size.d;
    coffee_graphics_tex_substore_3d(tex,data,level,reg);
}

inline void coffee_graphics_tex_define(
        CTexture& tex)
{
    switch(tex.size.dimensions())
    {
    case 1:
        coffee_graphics_tex_defimmutable_1d(tex);
        break;
    case 2:
        coffee_graphics_tex_defimmutable_2d(tex);
        break;
    case 3:
        coffee_graphics_tex_defimmutable_3d(tex);
        break;
    }
}

inline void coffee_graphics_tex_store(
        CTexture& tex, const CTextureData& data, const CGint& level)
{
    switch(tex.size.dimensions())
    {
    case 1:
        coffee_graphics_tex_store_1d(tex,data,level);
        break;
    case 2:
        coffee_graphics_tex_store_2d(tex,data,level);
        break;
    case 3:
        coffee_graphics_tex_store_3d(tex,data,level);
        break;
    }
}

/*!
 * \brief Store a part of a texture in a designated buffer, either GPU-side or CPU-side. (Please use PBOs!)
 * \param tex
 * \param reg
 * \param level
 * \param fmt
 * \param dType
 * \param dPtr
 */
extern void coffee_graphics_tex_readtexelregion(
        const CTexture& tex, const CTextureRegion& reg,
        const CGint& level, const CTexFormat& fmt,
        const CDataType& dType, c_ptr dPtr);
/*!
 * \brief Store entire texture in a designated buffer.
 * \param tex
 * \param level
 * \param fmt
 * \param dType
 * \param dPtr
 */
inline void coffee_graphics_tex_readtexels(
        const CTexture& tex, const CGint& level,
        const CTexFormat& fmt, const CDataType& dType,
        c_ptr dPtr)
{
    CTextureRegion reg = {};
    reg.w = tex.size.w;
    reg.h = tex.size.h;
    reg.d = tex.size.d;
    coffee_graphics_tex_readtexelregion(tex,reg,level,fmt,dType,dPtr);
}

}
}

#endif
