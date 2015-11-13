#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"
#include "coffee/core/graphics/copengl_types.h"

#include <glbinding/gl/gl.h>

//We are using macros here because it preserves the size of static arrays
#define CG_GET(val,arr) coffee_get(val,arr,sizeof(arr)/sizeof(arr[0]))
#define CG_GETI(val,arr,offset) coffee_get(val,arr,sizeof(arr)/sizeof(arr[0]),offset)
#define CG_GETF(val,arr) coffee_get_flags(val,arr,sizeof(arr)/sizeof(arr[0]))

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

template<typename T1, typename T2>
struct coffeetype_mapping {T1 k; T2 v;};

template<typename T1, typename T2>
T2 coffee_get(T1 v, const coffeetype_mapping<T1,T2>* list, size_t size)
{
    for(size_t i=0;i<size;i++)
        if(list[i].k == v)
            return list[i].v;
    return T2();
}

template<typename T1, typename T2>
T2 coffee_get(T1 v, const coffeetype_mapping<T1,T2>* list, size_t size, size_t offset)
{
    T2 out;
    for(size_t i=0;i<size;i++)
        if(list[i].k == v)
            out = list[i].v;
    out = (T2)((uint32)out+offset);
    return out;
}

template<typename T1, typename T2>
T2 coffee_get_flags(T1 v, const coffeetype_mapping<T1,T2>* list, size_t size)
{
    T2 flag = GL_NONE_BIT;
    for(size_t i=0;i<size;i++)
        if(flag_eval(list[i].k & v))
            flag = flag|list[i].v;
    return flag;
}

static const coffeetype_mapping<CClearFlag,ClearBufferMask> cclearflag_map[4] = {
    {CClearFlag::Accum, GL_ACCUM_BUFFER_BIT},
    {CClearFlag::Color, GL_COLOR_BUFFER_BIT},
    {CClearFlag::Depth, GL_DEPTH_BUFFER_BIT},
    {CClearFlag::Stencil, GL_STENCIL_BUFFER_BIT},
};

constexpr coffeetype_mapping<CBufferType,GLenum> cbuffertype_map[8] = {
    {CBufferType::Array, GL_ARRAY_BUFFER},
    {CBufferType::Index, GL_ELEMENT_ARRAY_BUFFER},
    {CBufferType::Uniform, GL_UNIFORM_BUFFER},
    {CBufferType::ShaderStorage, GL_SHADER_STORAGE_BUFFER},
    {CBufferType::DrawIndirect, GL_DRAW_INDIRECT_BUFFER},
    {CBufferType::TransformFeedback, GL_TRANSFORM_FEEDBACK_BUFFER},
    {CBufferType::Texture, GL_TEXTURE_BUFFER},
    {CBufferType::Query, GL_QUERY_BUFFER},
};

static const coffeetype_mapping<CBufferAccess,BufferAccessMask> cbufferaccess_map[4] = {
    {CBufferAccess::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferAccess::ReadBit, GL_MAP_READ_BIT},
    {CBufferAccess::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferAccess::Persistent, GL_MAP_PERSISTENT_BIT},
};

static const coffeetype_mapping<CBufferStorage,BufferStorageMask> cbufferstore_map[6] = {
    {CBufferStorage::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferStorage::ReadBit, GL_MAP_READ_BIT},
    {CBufferStorage::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferStorage::Persistent, GL_MAP_PERSISTENT_BIT},
    {CBufferStorage::Dynamic, GL_DYNAMIC_STORAGE_BIT},
    {CBufferStorage::ClientStorage, GL_CLIENT_STORAGE_BIT},
};

static const coffeetype_mapping<CProgramStage,UseProgramStageMask> cprogmask_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER_BIT},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER_BIT},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER_BIT},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER_BIT},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER_BIT},
};

constexpr coffeetype_mapping<CProgramStage,GLenum> cshader_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER},
};

constexpr coffeetype_mapping<CDataType,GLenum> cdtypes_map[7] = {
    {CDataType::Scalar,GL_FLOAT},
    {CDataType::BigScalar,GL_DOUBLE},

    {CDataType::UInt,GL_UNSIGNED_INT},
    {CDataType::Int,GL_INT},

    {CDataType::UInt64,GL_UNSIGNED_INT64_ARB},

    {CDataType::UByte,GL_UNSIGNED_BYTE},
    {CDataType::Byte,GL_BYTE},
};

constexpr coffeetype_mapping<CBufferUsage,GLenum> cbufusage_map[10] = {
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

constexpr coffeetype_mapping<CPrimitiveMode,GLenum> cpritype_map[9] = {
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

constexpr coffeetype_mapping<CTexIntFormat,GLenum> ctexint_map[10] = {
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

constexpr coffeetype_mapping<CTexFormat,GLenum> ctexfmt_map[9] = {
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

constexpr coffeetype_mapping<CTexType,GLenum> ctextp_map[9] = {
    {CTexType::Tex2D,GL_TEXTURE_2D},
    {CTexType::Tex3D,GL_TEXTURE_3D},
    {CTexType::CubeMap,GL_TEXTURE_CUBE_MAP},
};

constexpr coffeetype_mapping<CTexParam,GLenum> ctexparm_map[15] = {
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

constexpr coffeetype_mapping<CTexParamOpt,GLenum> ctexparmopt_map[11] = {
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

constexpr coffeetype_mapping<CFBFilter,GLenum> cfbfilt_map[2] = {
    {CFBFilter::Nearest,GL_NEAREST},
    {CFBFilter::Linear,GL_LINEAR},
};

constexpr coffeetype_mapping<CFBType,GLenum> cfbtype_map[3] = {
    {CFBType::All,GL_FRAMEBUFFER},
    {CFBType::Draw,GL_DRAW_FRAMEBUFFER},
    {CFBType::Read,GL_READ_FRAMEBUFFER},
};

constexpr coffeetype_mapping<CFBAttachment,GLenum> cfbattch_map[4] = {
    {CFBAttachment::Color,GL_COLOR_ATTACHMENT0},
    {CFBAttachment::Depth,GL_DEPTH_ATTACHMENT},
    {CFBAttachment::Stencil,GL_STENCIL_ATTACHMENT},
    {CFBAttachment::DepthStencil,GL_DEPTH_STENCIL_ATTACHMENT},
};

}
}

#endif // GLBINDING

