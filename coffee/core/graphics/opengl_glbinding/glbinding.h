#ifndef COFFEE_GRAPHICS_GLBINDING_H
#define COFFEE_GRAPHICS_GLBINDING_H

#include "coffee/core/CTypes"
#include "coffee/core/graphics_apis/COpenGL"

#include <glbinding/gl/gl.h>

//We are using macros here because it preserves the size of static arrays
#define CG_GET(val,arr) coffee_get_value(val,arr)
#define CG_GETI(val,arr,offset) coffee_get_value_texindex(val,arr,offset)
#define CG_GETF(val,arr) coffee_get_flags(val,arr)

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

C_FLAGS(UseProgramStageMask,uint32)
C_FLAGS(BufferAccessMask,uint32)
C_FLAGS(BufferStorageMask,uint32)
C_FLAGS(ClearBufferMask,uint32)

/*!
 * \brief Contains a GL message from the binding layer
 */
struct CGLReport
{
    CGLReport();

    GLenum source; /*!< Source of message*/
    GLenum type; /*!< Type of message*/
    GLuint id; /*!< ID of message*/
    GLenum severity; /*!< Severity of message*/
    const char* message; /*!< Textual message*/
};

extern CString _glbinding_get_string(GLenum e);
extern CString _glbinding_get_string(GLextension e);

template<typename KeyType, typename ValueType, size_t Size>
ValueType coffee_get_value_texindex(
        const KeyType& key,
        const _cbasic_static_map<KeyType,ValueType,Size>& map,
        uint32 offset)
{
    for(size_t i=0;i<Size;i++)
        if(map[i].key == key)
            return (ValueType)(((uint32)map[i].value)+offset);

    return ValueType();
}

static const _cbasic_static_map<CClearFlag,ClearBufferMask,4> cclearflag_map = {
    {CClearFlag::Accum, GL_ACCUM_BUFFER_BIT},
    {CClearFlag::Color, GL_COLOR_BUFFER_BIT},
    {CClearFlag::Depth, GL_DEPTH_BUFFER_BIT},
    {CClearFlag::Stencil, GL_STENCIL_BUFFER_BIT},
};

static const _cbasic_static_map<CBufferType,GLenum,8> cbuffertype_map = {
    {CBufferType::Array, GL_ARRAY_BUFFER},
    {CBufferType::Index, GL_ELEMENT_ARRAY_BUFFER},
    {CBufferType::Uniform, GL_UNIFORM_BUFFER},
    {CBufferType::ShaderStorage, GL_SHADER_STORAGE_BUFFER},
    {CBufferType::DrawIndirect, GL_DRAW_INDIRECT_BUFFER},
    {CBufferType::TransformFeedback, GL_TRANSFORM_FEEDBACK_BUFFER},
    {CBufferType::Texture, GL_TEXTURE_BUFFER},
    {CBufferType::Query, GL_QUERY_BUFFER},
};

static const _cbasic_static_map<CBufferAccess,BufferAccessMask,4> cbufferaccess_map = {
    {CBufferAccess::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferAccess::ReadBit, GL_MAP_READ_BIT},
    {CBufferAccess::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferAccess::Persistent, GL_MAP_PERSISTENT_BIT},
};

static const _cbasic_static_map<CBufferStorage,BufferStorageMask,6> cbufferstore_map = {
    {CBufferStorage::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferStorage::ReadBit, GL_MAP_READ_BIT},
    {CBufferStorage::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferStorage::Persistent, GL_MAP_PERSISTENT_BIT},
    {CBufferStorage::Dynamic, GL_DYNAMIC_STORAGE_BIT},
    {CBufferStorage::ClientStorage, GL_CLIENT_STORAGE_BIT},
};

static const _cbasic_static_map<CProgramStage,UseProgramStageMask,5> cprogmask_map = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER_BIT},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER_BIT},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER_BIT},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER_BIT},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER_BIT},
};

static const _cbasic_static_map<CProgramStage,GLenum,5> cshader_map = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER},
};

static const _cbasic_static_map<CDataType,GLenum,7> cdtypes_map = {
    {CDataType::Scalar,GL_FLOAT},
    {CDataType::BigScalar,GL_DOUBLE},

    {CDataType::UInt,GL_UNSIGNED_INT},
    {CDataType::Int,GL_INT},

    {CDataType::UInt64,GL_UNSIGNED_INT64_ARB},

    {CDataType::UByte,GL_UNSIGNED_BYTE},
    {CDataType::Byte,GL_BYTE},
};

static const _cbasic_static_map<CBufferUsage,GLenum,10> cbufusage_map = {
    {CBufferUsage::Default,GL_NONE},

    {CBufferUsage::DynamicCopy,GL_DYNAMIC_COPY},
    {CBufferUsage::DynamicRead,GL_DYNAMIC_READ},
    {CBufferUsage::DynamicDraw,GL_DYNAMIC_DRAW},

    {CBufferUsage::StaticCopy,GL_STATIC_COPY},
    {CBufferUsage::StaticRead,GL_STATIC_READ},
    {CBufferUsage::StaticDraw,GL_STATIC_DRAW},

    {CBufferUsage::StreamCopy,GL_STREAM_COPY},
    {CBufferUsage::StreamCopy,GL_STREAM_READ},
    {CBufferUsage::StreamCopy,GL_STREAM_DRAW},
};

static const _cbasic_static_map<CPrimitiveMode,GLenum,9> cpritype_map = {
    {CPrimitiveMode::Triangles,GL_TRIANGLES},
    {CPrimitiveMode::Points,GL_POINTS},
    {CPrimitiveMode::Line,GL_LINES},
    {CPrimitiveMode::TriangleFan,GL_TRIANGLE_FAN},
    {CPrimitiveMode::TriangleStrip,GL_TRIANGLE_STRIP},
    {CPrimitiveMode::TrianglesAdjacency,GL_TRIANGLES_ADJACENCY},
    {CPrimitiveMode::TriangleStripAdjacency,GL_TRIANGLE_STRIP_ADJACENCY},
    {CPrimitiveMode::LineStrip,GL_LINE_STRIP},
    {CPrimitiveMode::LineLoop,GL_LINE_LOOP},
};

static const _cbasic_static_map<CTexIntFormat,GLenum,10> ctexint_map = {
    {CTexIntFormat::Depth,GL_DEPTH_COMPONENT24},
    {CTexIntFormat::DepthStencil,GL_DEPTH24_STENCIL8},
    {CTexIntFormat::Stencil,GL_STENCIL_INDEX8},

    {CTexIntFormat::RGBA8,GL_RGBA8},
    {CTexIntFormat::RGB8,GL_RGB8},
    {CTexIntFormat::RG8,GL_RG8},
    {CTexIntFormat::RED8,GL_RED},

    {CTexIntFormat::DXT1,GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
    {CTexIntFormat::DXT3,GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
    {CTexIntFormat::DXT5,GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},
};

static const _cbasic_static_map<CTexFormat,GLenum,9> ctexfmt_map = {
    {CTexFormat::Depth,GL_DEPTH_COMPONENT},
    {CTexFormat::DepthStencil,GL_DEPTH_STENCIL},
    {CTexFormat::Stencil,GL_STENCIL_INDEX},

    {CTexFormat::RGBA,GL_RGBA},
    {CTexFormat::RGB,GL_RGB},
    {CTexFormat::RG,GL_RG},

    {CTexFormat::RED,GL_RED},
    {CTexFormat::GREEN,GL_GREEN},
    {CTexFormat::BLUE,GL_BLUE},
};

static const _cbasic_static_map<CTexType,GLenum,9> ctextp_map = {
    {CTexType::Tex2D,GL_TEXTURE_2D},
    {CTexType::Tex3D,GL_TEXTURE_3D},
    {CTexType::CubeMap,GL_TEXTURE_CUBE_MAP},
};

static const _cbasic_static_map<CTexParam,GLenum,15> ctexparm_map = {
    {CTexParam::MipmapBaseLevel,GL_TEXTURE_BASE_LEVEL},
    {CTexParam::MipmapLodBias,GL_TEXTURE_LOD_BIAS},
    {CTexParam::MipmapMinLod,GL_TEXTURE_MIN_LOD},
    {CTexParam::MipmapMaxLod,GL_TEXTURE_MAX_LOD},
    {CTexParam::MipmapMinFilter,GL_TEXTURE_MIN_FILTER},
    {CTexParam::MipmapMagFilter,GL_TEXTURE_MAG_FILTER},
    {CTexParam::MipmapMaxLevel,GL_TEXTURE_MAX_LEVEL},

    {CTexParam::WrapS,GL_TEXTURE_WRAP_S},
    {CTexParam::WrapT,GL_TEXTURE_WRAP_T},
    {CTexParam::WrapR,GL_TEXTURE_WRAP_R},

    {CTexParam::SwizzleR,GL_TEXTURE_SWIZZLE_R},
    {CTexParam::SwizzleG,GL_TEXTURE_SWIZZLE_G},
    {CTexParam::SwizzleB,GL_TEXTURE_SWIZZLE_B},
    {CTexParam::SwizzleA,GL_TEXTURE_SWIZZLE_A},
    {CTexParam::SwizzleRGBA,GL_TEXTURE_SWIZZLE_RGBA},
};

static const _cbasic_static_map<CTexParamOpt,GLenum,11> ctexparmopt_map = {
    {CTexParamOpt::Nearest,GL_NEAREST},
    {CTexParamOpt::Linear,GL_LINEAR},
    {CTexParamOpt::LinearMipmapLinear,GL_LINEAR_MIPMAP_LINEAR},
    {CTexParamOpt::NearestMipmapLinear,GL_NEAREST_MIPMAP_LINEAR},
    {CTexParamOpt::NearestMipmapNearest,GL_NEAREST_MIPMAP_NEAREST},
    {CTexParamOpt::LinearMipmapNearest,GL_LINEAR_MIPMAP_NEAREST},

    {CTexParamOpt::ClampEdge,GL_CLAMP_TO_EDGE},
    {CTexParamOpt::ClampBorder,GL_CLAMP_TO_BORDER},
    {CTexParamOpt::Repeat,GL_REPEAT},
    {CTexParamOpt::RepeatMirror,GL_MIRRORED_REPEAT},
    {CTexParamOpt::ClampEdgeMirror,GL_MIRROR_CLAMP_TO_EDGE},
};

static const _cbasic_static_map<CFBFilter,GLenum,2> cfbfilt_map = {
    {CFBFilter::Nearest,GL_NEAREST},
    {CFBFilter::Linear,GL_LINEAR},
};

static const _cbasic_static_map<CFBType,GLenum,3> cfbtype_map = {
    {CFBType::All,GL_FRAMEBUFFER},
    {CFBType::Draw,GL_DRAW_FRAMEBUFFER},
    {CFBType::Read,GL_READ_FRAMEBUFFER},
};

static const _cbasic_static_map<CFBAttachment,GLenum,4> cfbattch_map = {
    {CFBAttachment::Color,GL_COLOR_ATTACHMENT0},
    {CFBAttachment::Depth,GL_DEPTH_ATTACHMENT},
    {CFBAttachment::Stencil,GL_STENCIL_ATTACHMENT},
    {CFBAttachment::DepthStencil,GL_DEPTH_STENCIL_ATTACHMENT},
};

static const _cbasic_static_map<CCullMode,GLenum,3> cculling_map = {
    {CCullMode::Back,GL_BACK},
    {CCullMode::Front,GL_FRONT},
    {CCullMode::Both,GL_FRONT_AND_BACK},
};

}
}

#endif // COFFEE_GRAPHICS_GLBINDING_H

