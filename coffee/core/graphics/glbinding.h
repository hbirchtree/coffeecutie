#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"
#include "coffee/core/graphics/copengl_types.h"

#include <glbinding/gl/gl.h>

#define CG_GET(val,arr) coffee_get(val,arr,sizeof(arr)/sizeof(arr[0]))
#define CG_GETF(val,arr) coffee_get_flags(val,arr,sizeof(arr)/sizeof(arr[0]))

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

C_FLAGS(UseProgramStageMask,uint32)
C_FLAGS(BufferAccessMask,uint32)
C_FLAGS(BufferStorageMask,uint32)

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
T2 coffee_get_flags(T1 v, const coffeetype_mapping<T1,T2>* list, size_t size)
{
    T2 flag = GL_NONE_BIT;
    for(size_t i=0;i<size;i++)
        if(flag_eval(list[i].k & v))
            flag = flag|list[i].v;
    return flag;
}

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

constexpr size_t cbuffertype_size = 8;

static const coffeetype_mapping<CBufferAccess,BufferAccessMask> cbufferaccess_map[4] = {
    {CBufferAccess::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferAccess::ReadBit, GL_MAP_READ_BIT},
    {CBufferAccess::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferAccess::Persistent, GL_MAP_PERSISTENT_BIT},
};

constexpr size_t cbufferaccess_size = 4;

static const coffeetype_mapping<CBufferStorage,BufferStorageMask> cbufferstore_map[6] = {
    {CBufferStorage::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferStorage::ReadBit, GL_MAP_READ_BIT},
    {CBufferStorage::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferStorage::Persistent, GL_MAP_PERSISTENT_BIT},
    {CBufferStorage::Dynamic, GL_DYNAMIC_STORAGE_BIT},
    {CBufferStorage::ClientStorage, GL_CLIENT_STORAGE_BIT},
};

constexpr size_t cbufferstore_size = 6;

static const coffeetype_mapping<CProgramStage,UseProgramStageMask> cprogmask_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER_BIT},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER_BIT},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER_BIT},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER_BIT},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER_BIT},
};

constexpr size_t cprogmask_size = 5;

constexpr coffeetype_mapping<CProgramStage,GLenum> cshader_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER},

    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER},
};

constexpr size_t cshader_size = 5;

constexpr coffeetype_mapping<CDataType,GLenum> cdtypes_map[7] = {
    {CDataType::Scalar,GL_FLOAT},
    {CDataType::BigScalar,GL_DOUBLE},

    {CDataType::UInt,GL_UNSIGNED_INT},
    {CDataType::Int,GL_INT},

    {CDataType::UInt64,GL_UNSIGNED_INT64_ARB},

    {CDataType::UByte,GL_UNSIGNED_BYTE},
    {CDataType::Byte,GL_BYTE},
};

constexpr size_t cdtypes_size = 7;

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

constexpr size_t cbufusage_size = 10;

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

constexpr size_t cpritype_size = 9;

}
}

#endif // GLBINDING

