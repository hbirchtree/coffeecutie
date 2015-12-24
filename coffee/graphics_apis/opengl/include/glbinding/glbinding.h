#ifndef COFFEE_GRAPHICS_GLBINDING_H
#define COFFEE_GRAPHICS_GLBINDING_H

#include <coffee/graphics_apis/COpenGL>
#include <coffee/core/CTypes>

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

#ifdef COFFEE_WINDOWS

C_FLAGS(gl::BufferAccessMask,uint32);
C_FLAGS(gl::BufferStorageMask,uint32);

C_FLAGS(gl::ClearBufferMask,uint32);
C_FLAGS(gl::MemoryBarrierMask,uint32);
C_FLAGS(gl::UseProgramStageMask,uint32);

#endif

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

constexpr _cbasic_static_map<CBufferType,GLenum,11> cbuffertype_map = {
    {CBufferType::Array, GL_ARRAY_BUFFER},
    {CBufferType::Index, GL_ELEMENT_ARRAY_BUFFER},
    {CBufferType::Uniform, GL_UNIFORM_BUFFER},
    {CBufferType::ShaderStorage, GL_SHADER_STORAGE_BUFFER},
    {CBufferType::DrawIndirect, GL_DRAW_INDIRECT_BUFFER},
    {CBufferType::TransformFeedback, GL_TRANSFORM_FEEDBACK_BUFFER},
    {CBufferType::Texture, GL_TEXTURE_BUFFER},
    {CBufferType::Query, GL_QUERY_BUFFER},

    {CBufferType::ComputeIndirect, GL_DISPATCH_INDIRECT_BUFFER},

    {CBufferType::PixelUnpack, GL_PIXEL_UNPACK_BUFFER},
    {CBufferType::PixelPack, GL_PIXEL_PACK_BUFFER},
};

static const _cbasic_static_map<CBufferAccess,BufferAccessMask,5> cbufferaccess_map = {
    {CBufferAccess::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferAccess::ReadBit, GL_MAP_READ_BIT},
    {CBufferAccess::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferAccess::Persistent, GL_MAP_PERSISTENT_BIT},
    {CBufferAccess::Invalidate, GL_MAP_INVALIDATE_BUFFER_BIT},
};

static const _cbasic_static_map<CBufferStorage,BufferStorageMask,6> cbufferstore_map = {
    {CBufferStorage::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferStorage::ReadBit, GL_MAP_READ_BIT},
    {CBufferStorage::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferStorage::Persistent, GL_MAP_PERSISTENT_BIT},
    {CBufferStorage::Dynamic, GL_DYNAMIC_STORAGE_BIT},
    {CBufferStorage::ClientStorage, GL_CLIENT_STORAGE_BIT},
};

constexpr _cbasic_static_map<CProgramStage,UseProgramStageMask,6> cprogmask_map = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER_BIT},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER_BIT},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER_BIT},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER_BIT},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER_BIT},

    {CProgramStage::Compute,GL_COMPUTE_SHADER_BIT},
};

constexpr _cbasic_static_map<CProgramStage,GLenum,6> cshader_map = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER},

    {CProgramStage::Compute,GL_COMPUTE_SHADER},
};

constexpr _cbasic_static_map<CDataType,GLenum,22> cdtypes_map = {
    {CDataType::Scalar,GL_FLOAT},
    {CDataType::BigScalar,GL_DOUBLE},

    {CDataType::UByte,GL_UNSIGNED_BYTE},
    {CDataType::Byte,GL_BYTE},

    {CDataType::UShort,GL_UNSIGNED_SHORT},
    {CDataType::Short,GL_SHORT},

    {CDataType::UInt,GL_UNSIGNED_INT},
    {CDataType::Int,GL_INT},

    {CDataType::UInt64,GL_UNSIGNED_INT64_ARB},
    #ifdef COFFEE_UNIXPLAT
    //Why? Because Windows is worse than I thought.
    {CDataType::Int64,GL_INT64_ARB},
    #endif

    //Texture data types below

    {CDataType::UByte3_3_2,GL_UNSIGNED_BYTE_3_3_2},
    {CDataType::UByte2_3_3_Rev,GL_UNSIGNED_BYTE_2_3_3_REV},

    {CDataType::UShort5_6_5,GL_UNSIGNED_SHORT_5_6_5},
    {CDataType::UShort5_6_5_Rev,GL_UNSIGNED_SHORT_5_6_5_REV},

    {CDataType::UShort5_5_5_1,GL_UNSIGNED_SHORT_5_5_5_1},
    {CDataType::UShort1_5_5_5_Rev,GL_UNSIGNED_SHORT_1_5_5_5_REV},

    {CDataType::UShort4_4_4_4,GL_UNSIGNED_SHORT_4_4_4_4},
    {CDataType::UShort4_4_4_4_Rev,GL_UNSIGNED_SHORT_4_4_4_4_REV},

    {CDataType::UInt8_8_8_8,GL_UNSIGNED_INT_8_8_8_8},
    {CDataType::UInt8_8_8_8_Rev,GL_UNSIGNED_INT_8_8_8_8_REV},

    {CDataType::UInt10_10_10_2,GL_UNSIGNED_INT_10_10_10_2},
    {CDataType::UInt2_10_10_10_Rev,GL_UNSIGNED_INT_2_10_10_10_REV},
};

constexpr _cbasic_static_map<CBufferUsage,GLenum,10> cbufusage_map = {
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

constexpr _cbasic_static_map<CPrimitiveMode,GLenum,9> cpritype_map = {
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

constexpr _cbasic_static_map<CTexIntFormat,GLenum,15> ctexint_map = {
    {CTexIntFormat::Depth,GL_DEPTH_COMPONENT24},
    {CTexIntFormat::DepthStencil,GL_DEPTH24_STENCIL8},
    {CTexIntFormat::Stencil,GL_STENCIL_INDEX8},

    {CTexIntFormat::RGBA8,GL_RGBA8},
    {CTexIntFormat::RGB8,GL_RGB8},
    {CTexIntFormat::RG8,GL_RG8},
    {CTexIntFormat::RED8,GL_RED},

    {CTexIntFormat::DXT1,GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
    {CTexIntFormat::DXT1_NoAlpha,GL_COMPRESSED_RGB_S3TC_DXT1_EXT},
    {CTexIntFormat::DXT3,GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
    {CTexIntFormat::DXT5,GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},

    {CTexIntFormat::BPTC_RGBA_UNORM,GL_COMPRESSED_RGBA_BPTC_UNORM},
    {CTexIntFormat::BPTC_SRGB_ALPHA_UNORM,GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM},
    {CTexIntFormat::BPTC_RGB_SFLOAT,GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT},
    {CTexIntFormat::BPTC_RGB_UFLOAT,GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT},
};

constexpr _cbasic_static_map<CTexFormat,GLenum,10> ctexfmt_map = {
    {CTexFormat::Depth,GL_DEPTH_COMPONENT},
    {CTexFormat::DepthStencil,GL_DEPTH_STENCIL},
    {CTexFormat::Stencil,GL_STENCIL_INDEX},

    {CTexFormat::RGBA,GL_RGBA},
    {CTexFormat::BGRA,GL_BGRA},
    {CTexFormat::RGB,GL_RGB},
    {CTexFormat::RG,GL_RG},

    {CTexFormat::RED,GL_RED},
    {CTexFormat::GREEN,GL_GREEN},
    {CTexFormat::BLUE,GL_BLUE},
};

constexpr _cbasic_static_map<CTexType,GLenum,8> ctextp_map = {
    {CTexType::Tex1D,GL_TEXTURE_1D},
    {CTexType::Tex2D,GL_TEXTURE_2D},
    {CTexType::Tex3D,GL_TEXTURE_3D},
    {CTexType::CubeMap,GL_TEXTURE_CUBE_MAP},

    {CTexType::Array1D,GL_TEXTURE_1D_ARRAY},
    {CTexType::Array2D,GL_TEXTURE_2D_ARRAY},
    {CTexType::ArrayCubeMap,GL_TEXTURE_CUBE_MAP_ARRAY},

    {CTexType::TexBuffer,GL_TEXTURE_BUFFER},
};

constexpr _cbasic_static_map<CTexParam,GLenum,17> ctexparm_map = {
    {CTexParam::MipmapBaseLevel,GL_TEXTURE_BASE_LEVEL},
    {CTexParam::MipmapLodBias,GL_TEXTURE_LOD_BIAS},
    {CTexParam::MipmapMinLod,GL_TEXTURE_MIN_LOD},
    {CTexParam::MipmapMaxLod,GL_TEXTURE_MAX_LOD},
    {CTexParam::MipmapMinFilter,GL_TEXTURE_MIN_FILTER},
    {CTexParam::MipmapMagFilter,GL_TEXTURE_MAG_FILTER},

    {CTexParam::WrapS,GL_TEXTURE_WRAP_S},
    {CTexParam::WrapT,GL_TEXTURE_WRAP_T},
    {CTexParam::WrapR,GL_TEXTURE_WRAP_R},

    {CTexParam::SwizzleR,GL_TEXTURE_SWIZZLE_R},
    {CTexParam::SwizzleG,GL_TEXTURE_SWIZZLE_G},
    {CTexParam::SwizzleB,GL_TEXTURE_SWIZZLE_B},
    {CTexParam::SwizzleA,GL_TEXTURE_SWIZZLE_A},
    {CTexParam::SwizzleRGBA,GL_TEXTURE_SWIZZLE_RGBA},

    {CTexParam::SparseTexture,GL_TEXTURE_SPARSE_ARB},
    {CTexParam::VirtualTexture,GL_TEXTURE_SPARSE_ARB},
};

constexpr _cbasic_static_map<CTexParamOpt,GLenum,11> ctexparmopt_map = {
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

constexpr _cbasic_static_map<CFBFilter,GLenum,2> cfbfilt_map = {
    {CFBFilter::Nearest,GL_NEAREST},
    {CFBFilter::Linear,GL_LINEAR},
};

constexpr _cbasic_static_map<CFBType,GLenum,3> cfbtype_map = {
    {CFBType::All,GL_FRAMEBUFFER},
    {CFBType::Draw,GL_DRAW_FRAMEBUFFER},
    {CFBType::Read,GL_READ_FRAMEBUFFER},
};

constexpr _cbasic_static_map<CFBAttachment,GLenum,4> cfbattch_map = {
    {CFBAttachment::Color,GL_COLOR_ATTACHMENT0},
    {CFBAttachment::Depth,GL_DEPTH_ATTACHMENT},
    {CFBAttachment::Stencil,GL_STENCIL_ATTACHMENT},
    {CFBAttachment::DepthStencil,GL_DEPTH_STENCIL_ATTACHMENT},
};

constexpr _cbasic_static_map<CCullMode,GLenum,3> cculling_map = {
    {CCullMode::Back,GL_BACK},
    {CCullMode::Front,GL_FRONT},
    {CCullMode::Both,GL_FRONT_AND_BACK},
};

constexpr _cbasic_static_map<CMemoryBarrier,MemoryBarrierMask,13> cmbarrier_map = {
    {CMemoryBarrier::AtomicCounter,GL_ATOMIC_COUNTER_BARRIER_BIT},
    {CMemoryBarrier::BufferUpdate,GL_BUFFER_UPDATE_BARRIER_BIT},
    {CMemoryBarrier::Command,GL_COMMAND_BARRIER_BIT},
    {CMemoryBarrier::ElementArray,GL_ELEMENT_ARRAY_BARRIER_BIT},
    {CMemoryBarrier::Framebuffer,GL_FRAMEBUFFER_BARRIER_BIT},
    {CMemoryBarrier::PixelBuffer,GL_PIXEL_BUFFER_BARRIER_BIT},
    {CMemoryBarrier::ShaderImage,GL_SHADER_IMAGE_ACCESS_BARRIER_BIT},
    {CMemoryBarrier::ShaderStorage,GL_SHADER_STORAGE_BARRIER_BIT},
    {CMemoryBarrier::TextureUpdate,GL_TEXTURE_UPDATE_BARRIER_BIT},
    {CMemoryBarrier::TextureFetch,GL_TEXTURE_FETCH_BARRIER_BIT},
    {CMemoryBarrier::TransformFeedback,GL_TRANSFORM_FEEDBACK_BARRIER_BIT},
    {CMemoryBarrier::Uniform,GL_UNIFORM_BARRIER_BIT},
    {CMemoryBarrier::VertexAttribArray,GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT},
};

constexpr _cbasic_static_map<GLenum,CDebugSeverity,4> cdbgsev_map = {
    {GL_DEBUG_SEVERITY_NOTIFICATION,CDebugSeverity::Notification},
    {GL_DEBUG_SEVERITY_LOW,CDebugSeverity::Low},
    {GL_DEBUG_SEVERITY_MEDIUM,CDebugSeverity::Medium},
    {GL_DEBUG_SEVERITY_HIGH,CDebugSeverity::High},
};

constexpr _cbasic_static_map<GLenum,CDebugSource,5> cdbgsrc_map = {
    {GL_DEBUG_SOURCE_API,CDebugSource::API},
    {GL_DEBUG_SOURCE_SHADER_COMPILER,CDebugSource::ShaderCompiler},
    {GL_DEBUG_SOURCE_APPLICATION,CDebugSource::Application},
    {GL_DEBUG_SOURCE_WINDOW_SYSTEM,CDebugSource::WindowManager},
    {GL_DEBUG_SOURCE_OTHER,CDebugSource::Other},
};

constexpr _cbasic_static_map<GLenum,CDebugType,4> cdbgtype_map = {
    {GL_DEBUG_TYPE_ERROR,CDebugType::Error},
    {GL_DEBUG_TYPE_PERFORMANCE,CDebugType::Performance},
    {GL_DEBUG_TYPE_MARKER,CDebugType::Marker},
    {GL_DEBUG_TYPE_OTHER,CDebugType::Other},
};

constexpr _cbasic_static_map<CTextureAccess,GLenum,3> ctexaccess_map = {
    {CTextureAccess::ReadOnly,GL_READ_ONLY},
    {CTextureAccess::WriteOnly,GL_WRITE_ONLY},
    {CTextureAccess::ReadWrite,GL_READ_WRITE},
};

constexpr _cbasic_static_map<CAttributeMode,GLenum,2> cattrmode_map = {
    {CAttributeMode::Interleaved,GL_INTERLEAVED_ATTRIBS},
    {CAttributeMode::Separate,GL_SEPARATE_ATTRIBS},
};

constexpr _cbasic_static_map<CQueryType,GLenum,5> cquerymode_map = {
    {CQueryType::AnySamplesPassed,GL_ANY_SAMPLES_PASSED},
    {CQueryType::SamplesPassed,GL_SAMPLES_PASSED},

    {CQueryType::TimeElapsed,GL_TIME_ELAPSED},

    {CQueryType::PrimitivesGenerated,GL_PRIMITIVES_GENERATED},
    {CQueryType::XFBPrimitivesWritten,GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN},
};

constexpr _cbasic_static_map<CQueryResultType,GLenum,3> cqueryresmode_map = {
    {CQueryResultType::Result,GL_QUERY_RESULT_NO_WAIT},
    {CQueryResultType::ResultWait,GL_QUERY_RESULT},
    {CQueryResultType::ResultAvailable,GL_QUERY_RESULT_AVAILABLE},
};

//Textures
inline GLenum gl_get(CTexType const& prm)
{
    return coffee_get_value(prm,ctextp_map);
}
inline GLenum gl_get(CTexParam const& prm)
{
    return coffee_get_value(prm,ctexparm_map);
}
inline GLenum gl_get(CTexParamOpt const& prm)
{
    return coffee_get_value(prm,ctexparmopt_map);
}
inline GLenum gl_get(CTexIntFormat const& f)
{
    return coffee_get_value(f,ctexint_map);
}
inline GLenum gl_get(CTexFormat const& f)
{
    return coffee_get_value(f,ctexfmt_map);
}
inline GLenum gl_get(CTextureAccess const& f)
{
    return coffee_get_value(f,ctexaccess_map);
}
inline GLenum gl_get(CFBType const& c)
{
    return coffee_get_value(c,cfbtype_map);
}
inline GLenum gl_geti(CFBAttachment const& c, int32 const& offset)
{
    return (GLenum)(((int32)coffee_get_value(c,cfbattch_map))+offset);
}



//Buffers
inline GLenum gl_get(CBufferType const& d)
{
    return coffee_get_value(d,cbuffertype_map);
}
inline GLenum gl_get(CBufferUsage const& c)
{
    return coffee_get_value(c,cbufusage_map);
}
inline BufferAccessMask gl_get(CBufferAccess const& c)
{
    return coffee_get_flags(c,cbufferaccess_map);
}
inline BufferStorageMask gl_get(CBufferStorage const& c)
{
    return coffee_get_flags(c,cbufferstore_map);
}

//General pipeline
inline GLenum gl_get(CDataType const& d)
{
    return coffee_get_value(d,cdtypes_map);
}
inline GLenum gl_get(CPrimitiveMode const& p)
{
    return coffee_get_value(p,cpritype_map);
}
inline GLenum gl_get(CCullMode const& c)
{
    return coffee_get_value(c,cculling_map);
}
inline ClearBufferMask gl_getf(CClearFlag const& c)
{
    return coffee_get_flags(c,cclearflag_map);
}
inline GLenum gl_get(CFBFilter const& c)
{
    return coffee_get_value(c,cfbfilt_map);
}
inline MemoryBarrierMask gl_getf(CMemoryBarrier const& c)
{
    return coffee_get_flags(c,cmbarrier_map);
}

//Queries
inline GLenum gl_get(CQueryType const& c)
{
    return coffee_get_value(c,cquerymode_map);
}
inline GLenum gl_get(CQueryResultType const& c)
{
    return coffee_get_value(c,cqueryresmode_map);
}

//Shaders
inline GLenum gl_get(CProgramStage const& c)
{
    return coffee_get_value(c,cshader_map);
}
inline GLenum gl_get(CAttributeMode const& c)
{
    return coffee_get_value(c,cattrmode_map);
}
inline UseProgramStageMask gl_getf(CProgramStage const& c)
{
    return coffee_get_flags(c,cprogmask_map);
}

//Debug
inline CDebugSeverity gl_get_dbg_severity(GLenum const& d)
{
    return coffee_get_value(d,cdbgsev_map);
}
inline CDebugType gl_get_dbg_type(GLenum const& d)
{
    return coffee_get_value(d,cdbgtype_map);
}
inline CDebugSource gl_get_dbg_src(GLenum const& d)
{
    return coffee_get_value(d,cdbgsrc_map);
}

}
}

#endif // COFFEE_GRAPHICS_GLBINDING_H

