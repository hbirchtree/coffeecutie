#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_TYPES_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_TYPES_H

#include "coffee/core/types/vector_types.h"
#include "coffee/core/types/composite_types.h"
#include "copengl_types.h"

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

struct CTextureRegion : _cbasic_size_3d<int32>,CVectors::_cbasic_tvector<int32,3>
{
};

struct CTextureSize : _cbasic_size_3d<int32>
{
    size_t dimensions() const
    {
        if(w == 0)
            return 0;
        if(h == 0)
            return 1;
        if(d == 0)
            return 2;
        return 3;
    }
};

struct CTexture
{
    CTexture();

    CTexType textureType; /*!< Texture type, defines texture target */
    CGhnd handle; /*!< Texture handle */
    CGsize levels; /*!< Mipmap levels */
    CTexIntFormat format; /*!< Texture format */
    int32 unit; /*!< GL_TEXTURE* unit */
    CGuint64 bhandle; /*!< Bindless texture handle */
    CTextureSize size;
};

/*!
 * \brief Describes the data for a texture object that is not necessary later
 */
struct CTextureData
{
    void* data; /*!< Pointer to data */
    CTexFormat format; /*!< The format of the image data, often GL_RGBA8 */
    CDataType datatype; /*!< Which datatype, most likely unsigned byte */
    CTextureSize size;
};

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
        const CTexture* tex);
/*!
 * \brief Store texture data
 */
typedef bool (*TexStoreFun)(
        const CTexture* tex,
        const CTextureData* data,
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

}
}

#endif
