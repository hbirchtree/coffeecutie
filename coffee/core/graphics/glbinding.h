#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"
#include "coffee/core/graphics/copengl_types.h"

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

C_FLAGS(UseProgramStageMask,uint32)
C_FLAGS(BufferAccessMask,uint32)

typedef const GLchar* cglstring;

extern CString _glbinding_get_string(GLenum e);
extern CString _glbinding_get_string(GLextension e);

template<typename T1, typename T2>
struct coffeetype_mapping {T1 k; T2 v;};

template<typename T1, typename T2>
T2 coffee_get(T1 v, coffeetype_mapping<T1,T2>* list)
{
    for(size_t i=0;i<sizeof(list)/sizeof(list[0]);i++)
        if(list[i].k == v)
            return list[i].v;
    return T2();
}

template<typename T1, typename T2>
T2 coffee_get_flags(T1 v, coffeetype_mapping<T1,T2>* list)
{
    T2 flag = GL_NONE_BIT;
    for(size_t i=0;i<sizeof(list)/sizeof(list[0]);i++)
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

static coffeetype_mapping<CBufferAccess,BufferAccessMask> cbufferaccess_map[4] = {
    {CBufferAccess::WriteBit, GL_MAP_WRITE_BIT},
    {CBufferAccess::ReadBit, GL_MAP_READ_BIT},
    {CBufferAccess::Coherent, GL_MAP_COHERENT_BIT},
    {CBufferAccess::Persistent, GL_MAP_PERSISTENT_BIT},
};

static coffeetype_mapping<CProgramStage,UseProgramStageMask> cprogmask_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER_BIT},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER_BIT},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER_BIT},
    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER_BIT},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER_BIT},
};

static coffeetype_mapping<CProgramStage,GLenum> cshader_map[5] = {
    {CProgramStage::Vertex,GL_VERTEX_SHADER},
    {CProgramStage::Fragment,GL_FRAGMENT_SHADER},
    {CProgramStage::Geometry,GL_GEOMETRY_SHADER},
    {CProgramStage::TessellationControl,GL_TESS_CONTROL_SHADER},
    {CProgramStage::TessellationEvaluation,GL_TESS_EVALUATION_SHADER},
};

static coffeetype_mapping<CDataType,GLenum> cdtypes_map[7] = {
    {CDataType::Scalar,GL_FLOAT},
    {CDataType::BigScalar,GL_DOUBLE},
    {CDataType::UInt,GL_UNSIGNED_INT},
    {CDataType::Int,GL_INT},
    {CDataType::UInt64,GL_UNSIGNED_INT64_ARB},
    {CDataType::UByte,GL_UNSIGNED_BYTE},
    {CDataType::Byte,GL_BYTE},
};

}
}

#endif // GLBINDING

