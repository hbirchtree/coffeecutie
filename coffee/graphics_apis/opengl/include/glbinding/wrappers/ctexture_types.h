#ifndef COFFEE_GRAPHICS_OPENGL_TEXTURE_TYPES_H
#define COFFEE_GRAPHICS_OPENGL_TEXTURE_TYPES_H

#include <types/vector_types.h>
#include <types/composite_types.h>
#include "copengl_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

enum class CTexParam : uint16
{
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
    None,
    RGBA8,
    RED8,
    Depth,
    Stencil,

    RG8,
    RGB8,

    RGBA32F,

    DepthStencil,

    DXT1,
    DXT1_NoAlpha,
    DXT3,
    DXT5,

    BPTC_RGBA_UNORM,
    BPTC_SRGB_ALPHA_UNORM,
    BPTC_RGB_SFLOAT,
    BPTC_RGB_UFLOAT,
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

struct CTextureRegion : _cbasic_size_3d<int32>,_cbasic_vec3<int32>
{
};

struct CTextureSize : _cbasic_size_3d<int32>
{
    size_t dimensions() const
    {
        if(width == 0)
            return 0;
        if(height == 0)
            return 1;
        if(depth<2)
            return 2;
        return 3;
    }
};

struct CTexture
{
    CTexture();

    CTexType type; /*!< Texture type, defines texture target */
    CGhnd handle; /*!< Texture handle */
    CGsize levels; /*!< Mipmap levels */
    CTexIntFormat format; /*!< Texture format */
    CTextureSize size;
};

/*!
 * \brief An OpenGL sampler for textures
 */
struct CTextureSampler
{
    CGhnd64 bhandle;
    CGhnd handle;
    int32 unit; /*!< GL_TEXTURE* unit */
};

/*!
 * \brief Describes the data for a texture object that is not necessary later
 */
struct CTextureData
{
    const void* data;   /*!< Pointer to data */
    CTexFormat format;  /*!< The format of the image data, often GL_RGBA8 */
    CDataType datatype; /*!< Which datatype, most likely unsigned byte */
    CTextureSize size;
};

using CTextureSingletonPFN = void(*)(CTexture&);

using CTextureSubstorePFN = void(*)(
CTexture&,CTextureData const&,CGint const&,CTextureRegion const&);

using CTextureStorePFN = void(*)(
CTexture&,CTextureData const&,CGint const&);

using CTextureReadTexelRegionPFN = void(*)(
CTexture const&,CTextureRegion const&,CGint const&,CTexFormat const&,CDataType const&,c_ptr);

using CTextureReadTexelsPFN = void(*)(
CTexture const&,CGint const&,CTexFormat const&,CDataType const&,c_ptr);

using CTextureLoadPFN = void(*)(
CTextureSampler const&,CTexture const&);

struct CTexturePFN
{
    CTextureSingletonPFN mipmap;

    CTextureSingletonPFN define;

    CTextureStorePFN store;
    CTextureSubstorePFN substore;

    CTextureReadTexelRegionPFN read_region;
    CTextureReadTexelsPFN read_all;

    CTextureLoadPFN load;
    CTextureLoadPFN unload;
};

}
}

#endif
