#ifndef COFFEE_OPENGL_ENUMS_H
#define COFFEE_OPENGL_ENUMS_H

#include "coffee/core/types/basetypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

enum class CMemoryBarrier : uint8
{
    ElementArray,
    VertexAttribArray,
    Uniform,

    ShaderImage,

    Command,

    PixelBuffer,
    TextureUpdate,
    TextureFetch,

    BufferUpdate,

    Framebuffer,

    TransformFeedback,

    AtomicCounter,

    ShaderStorage,
};

C_FLAGS(CMemoryBarrier,uint8);

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
    None = 0,
    Array = 1,
    Index = 2,
    Uniform = 3,
    ShaderStorage = 4,
    DrawIndirect = 5,
    TransformFeedback = 6,
    Texture = 7,
    Query = 8,

    PixelUnpack = 9,
    PixelPack = 10,
};

enum class CTexType : uint16
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
    None = 0x0,
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
    Coherent = 0x8, /*!< Make buffer contents coherent across draws*/
};

C_FLAGS(CBufferAccess,uint16);

enum class CBufferStorage : uint16
{
    WriteBit = 0x1,
    ReadBit = 0x2,
    Persistent = 0x4, /*!< Make the buffer mapping persistent*/
    Coherent = 0x8, /*!< Make buffer contents coherent across draws and etc., performance impact*/
    Dynamic = 0x10, /*!< Allow use of coffee_graphics_buffer_substore()*/
    ClientStorage = 0x20, /*!< Read from client memory*/
};

C_FLAGS(CBufferStorage,uint16);

/*!
 * \brief Specifies buffer storage parameters in coffee_graphics_buffer_store(). Stream* specifies data modified once and drawn few times, static is modified once and drawn many times, dynamic is modified often and drawn many times. *Draw specifies data used for GL drawing, *Read specifies data read from the GL, *Copy specifies data is used for both reading and drawing operations.
 */
enum class CBufferUsage : uint16
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

enum class CDataType : uint16
{
    None = 0,
    Scalar = 1,
    BigScalar = 2,
    UInt = 3,
    Int = 4,
    UInt64 = 5,
    Byte = 7,
    UByte = 8,
};

enum class CDebugSeverity : uint8
{
    Notification = 1,
    Low = 2,
    Medium = 3,
    High = 4,
};

}
}

#endif
