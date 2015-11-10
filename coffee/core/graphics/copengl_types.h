#ifndef COPENGL_TYPES_H
#define COPENGL_TYPES_H

#include <bitset>

#include "coffee/core/coffee_basetypes.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief A handle for a GL resource
 */
struct CGHndl;
/*!
 * \brief An index retrieved from GL or to be used in GL
 */
typedef uint32 CGIdx;

//Misc. flags

enum class CClearFlag : uint8
{
    Depth = 0x1,
    Color = 0x2,
    Stencil = 0x4,
    Accum = 0x8,
};

C_FLAGS(CClearFlag,uint8);

enum class CBufferType : uint16
{
    Array = 1,
    Index = 2,
    Uniform = 3,
    ShaderStorage = 4,
    DrawIndirect = 5,
    TransformFeedback = 6,
    Texture = 7,
    Query = 8,
};

enum class CTextureType : uint16
{
    Array1D = 1,
    Array2D = 2,
    Array3D = 3,
    ArrayCubeMap = 4,
    CubeMap = 5,
    Tex2D = 6,
    Tex3D = 7,
    TexBuffer = 8,
};

enum class CProgramStage : uint16
{
    Vertex = 0x1,
    Fragment = 0x2,
    Geometry = 0x4,
    TessellationEvaluation = 0x8,
    TessellationControl = 0x10,
};

C_FLAGS(CProgramStage,uint16);

enum class CBufferAccess : uint16
{
    WriteBit = 0x1, /*!< Enable writing to the buffer*/
    ReadBit = 0x2, /*!< Enable reading from the buffer*/
    Persistent = 0x4, /*!< Make the buffer mapping persistent*/
    Coherent = 0x8, /*!< Make buffer contents coherent across draws and etc., performance impact*/
    Dynamic = 0x10, /*!< Allow use of coffee_graphics_buffer_substore()*/
    ClientStorage = 0x20, /*!< Read from client memory*/
};

C_FLAGS(CBufferAccess,uint16);

/*!
 * \brief Specifies buffer storage parameters in coffee_graphics_buffer_store(). Stream* specifies data modified once and drawn few times, static is modified once and drawn many times, dynamic is modified often and drawn many times. *Draw specifies data used for GL drawing, *Read specifies data read from the GL, *Copy specifies data is used for both reading and drawing operations.
 */
enum class CBufferStorage : uint16
{
    Default = 0,

    StreamDraw = 1,
    StreamRead = 2,
    StreamCopy = 3,

    StaticDraw = 4,
    StaticRead = 5,
    StaticCopy = 6,

    DynamicDraw = 7,
    DynamicRead = 8,
    DynamicCopy = 9,
};

//Misc. data structures

struct _cbasic_graphics_resource
{
    GLuint handle;
};

struct _cbasic_graphics_shader_program
{
    _cbasic_graphics_shader_program();

    GLuint handle;
    union{
        UseProgramStageMask stages;
        UseProgramStageMask stage;
    };
};

struct _cbasic_graphics_buffer_mappable
{
    void* data;
    BufferStorageMask flags;
    BufferAccessMask mapflags;
    GLsizeiptr size;
    GLuint handle;
    GLenum type;
};

struct _cbasic_graphics_buffer_section
{
    _cbasic_graphics_buffer_mappable* parent;
    GLsizeiptr offset;
    GLsizeiptr size;
    GLuint handle;
    GLenum type;
};

struct _cbasic_graphics_resource_desc
{
    _cbasic_graphics_resource_desc();

    cstring object_name;
    GLuint index;
};

struct _cbasic_graphics_buffer_resource_desc
{
    _cbasic_graphics_buffer_resource_desc();

    cstring object_name;
    _cbasic_graphics_buffer_section* buffer;
    GLuint index;
};

template<typename T>
struct _cbasic_graphics_resource_binding
{
    _cbasic_graphics_resource_binding():
        object(nullptr),
        binding(0)
    {
    }

    T* object;
    GLuint binding;
};

typedef _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_resource_desc>
    _cbasic_graphics_resource_binding_generic;

}
}

#endif
