#ifndef CTEXTURE
#define CTEXTURE

#include "copengl_types.h"
#include "coffee/core/coffee.h"
#include "coffee/image/cimage.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CTexture
{
    CTexture();

    CTexType textureType; /*!< Texture type, defines texture target */
    CGhnd handle; /*!< Texture handle */
    CGsize levels; /*!< Mipmap levels */
    CTexFormat format; /*!< Texture format */
    int32 unit; /*!< GL_TEXTURE* unit */
    CGuint64 bhandle; /*!< Bindless texture handle */
};

namespace CTextureTools{
/*!
 * \brief Describes the data for a texture object that is not necessary later
 */
struct CTextureData
{
    const void* data; /*!< Pointer to data */
    szptr* lengths; /*!< The stored dimensions of the image */
    CTexIntFormat format; /*!< The format of the image, often GL_RGBA */
    CDataType datatype; /*!< Which datatype, most likely unsigned byte */
    uint8 dimensions;       /*!< Amount of dimensions in lengths array */
};

}

/*!
 * \brief Load a texture for rendering
 */
typedef void (*TexLoadFun)(const CTexture* tex);
/*!
 * \brief Unload a texture after finished rendering
 */
typedef void (*TexUnloadFun)(const CTexture* tex);

/*!
 * \brief Define texture storage, mutable storage
 */
typedef bool (*TexDefineFun)(
        const CTexture* tex,
        const CTextureTools::CTextureData* data);
/*!
 * \brief Store texture data
 */
typedef bool (*TexStoreFun)(
        const CTexture* tex,
        const CTextureTools::CTextureData* data,
        CGint level);

typedef void (*TexParamEnumFun)(const CTexture*,GLenum param,GLenum val);
typedef void (*TexParamFun)(const CTexture*,GLenum param,CGint val);

typedef void (*TexMipmapFun)(const CTexture* tex);

struct CTextureFunctionBinds
{
    TexLoadFun      load    = nullptr;
    TexUnloadFun    unload  = nullptr;

    TexDefineFun    define  = nullptr;
    TexStoreFun     store   = nullptr;

    TexParamEnumFun param_e = nullptr;
    TexParamFun     param   = nullptr;

    TexMipmapFun    mipmap  = nullptr;
};

namespace CTextureTools{
/*!
 * \brief Frees data in CTextureData
 * \param tex Structure to free
 */
extern void     coffee_graphics_tex_free_texdata(CTextureData* tex);

//The following functions help to set a size for a texture.
//The template form makes it possible to use this for 1D, 2D and 3D alike
template<typename arg, typename... sizes>
static void _coffee_create_texturelist(
        szptr* list, int index, arg subject)
{
    list[index] = subject;
}
template<typename arg, typename... sizes>
static void _coffee_create_texturelist(
        szptr* list, int index, arg subject, sizes... sizelist)
{
    list[index] = subject;
    _coffee_create_texturelist(list,index+1,sizelist...);
}

template<typename... Sizes>
/*!
 * \brief Set dimensions in CTextureData object, allows setting 1D, 2D and 3D lengths in one function
 * \param val Target to set values to
 * \param sizes Variadic template of lengths
 */
static void coffee_create_texturesize(
        CTextureData* val, Sizes... sizes)
{
    const int n = sizeof...(sizes);
    if(n<1)
        return;
    val->dimensions = n;
    val->lengths = (szptr*)malloc(sizeof(szptr)*n);
    _coffee_create_texturelist(val->lengths,0,sizes...);
}

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
        const CTexture* tex, GLenum param, CGint val);
/*!
 * \brief Set texture parameter, GLenum variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param(
        const CTexture* tex, GLenum param, GLenum val);

/*!
 * \brief Set texture parameter, GLint variant, old variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param_safe(
        const CTexture* tex, GLenum param, CGint val);
/*!
 * \brief Set texture parameter, GLenum variant, old variant
 * \param tex
 * \param param
 * \param val
 */
extern void     coffee_graphics_tex_param_safe(
        const CTexture* tex, GLenum param, GLenum val);

/*!
 * \brief Binds and unbinds texture, used for bindless textures among others, only for convenience
 * \param tex
 */
extern void     coffee_graphics_tex_activate(
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
 * \brief Download GL texture to an image, sets image's properties
 * \param tex Texture to download
 * \param level Mipmap level to download
 * \param size Expected size of buffer (is allocated)
 * \param format Format of data
 * \param img Target image
 */
extern void     coffee_graphics_tex_download_texture(
        const CTexture* tex, CGint level,
        CGsize size, GLenum format, CStbImageLib::CStbImage* img);

/*!
 * \brief Debug function for dumping texture to file
 * \param tex
 * \param filename
 */
extern void     coffee_graphics_tex_dump(
        const CTexture* tex, cstring filename);

}
}

#endif // CTEXTURE
