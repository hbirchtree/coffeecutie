#ifndef CTEXTURE
#define CTEXTURE

#include "copengl_types.h"
#include "coffee/core/plat/cmemory.h"

namespace Coffee{
namespace CGraphicsWrappers{

enum class CTexParam : uint16
{
    MipmapMaxLevel = 1,
    MipmapMinFilter = 2,
    MipmapMagFilter = 3,

    MipmapMinLod = 4,
    MipmapLodBias = 5,
    MipmapMaxLod = 6,

    WrapS = 7,
    WrapT = 8,
    WrapR = 9,

    SwizzleR = 10,
    SwizzleG = 11,
    SwizzleB = 12,
    SwizzleA = 13,
    SwizzleRGBA = 14,

    MipmapBaseLevel = 15,

    SparseTexture = 16,
    VirtualTexture = 16,
};

enum class CTexParamOpt : uint16
{
    Nearest = 1,
    Linear = 2,
    LinearMipmapLinear = 3,
    LinearMipmapNearest = 4,
    NearestMipmapNearest = 5,
    NearestMipmapLinear = 6,

    ClampEdge = 7,
    ClampBorder = 8,
    RepeatMirror = 9,
    Repeat = 10,
    ClampEdgeMirror = 11,

    Enable = 12,
    Disable = 13,
};

enum class CTexIntFormat : CGint
{
    None = 0,
    RGBA8 = 1,
    RED8 = 2,
    Depth = 3,
    Stencil = 4,

    RG8 = 5,
    RGB8 = 6,

    RGBA32F = 7,

    DXT1 = 8,
    DXT3 = 9,
    DXT5 = 10,

    DepthStencil = 11,
};

enum class CTexFormat : uint16
{
    None = 0,
    RGBA = 1,
    BGRA = 2,

    RG = 3,
    RGB = 4,

    RED = 5,
    GREEN = 6,
    BLUE = 7,

    Depth = 8,
    Stencil = 9,
    DepthStencil = 10,
};

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

typedef void (*TexParamEnumFun)(const CTexture*,CTexParam param,CTexParamOpt val);
typedef void (*TexParamFun)(const CTexture*,CTexParam param,CGint val);
typedef void (*TexParamFunF)(const CTexture*,CTexParam param,scalar val);

typedef void (*TexMipmapFun)(const CTexture* tex);

struct CTextureFunctionBinds
{
    CTextureFunctionBinds();

    TexLoadFun      load;
    TexUnloadFun    unload;

    TexDefineFun    define;
    TexStoreFun     store;

    TexParamEnumFun param_e;
    TexParamFun     param;
    TexParamFunF    paramf;

    TexMipmapFun    mipmap;
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
    val->lengths = (szptr*)c_alloc(sizeof(szptr)*n);
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

extern void coffee_graphics_tex_sparsify(CTexture* tex, bool enable);

}
}

#endif // CTEXTURE
