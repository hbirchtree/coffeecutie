#ifndef COFFEE_OPENGL_ENUMS_H
#define COFFEE_OPENGL_ENUMS_H

#include <types/basetypes.h>

namespace Coffee{
namespace CGraphicsWrappers{

enum class CPrimitiveMode : uint8
{
    Triangles,
    Points,
    Line,
    TriangleStrip,
    TriangleFan,
    TriangleStripAdjacency,
    TrianglesAdjacency,
    LineStrip,
    LineLoop,

    Patches,
};

enum class CCullMode : uint8
{
    Front,
    Back,
    Both,
};

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

enum class CBufferType : uint8
{
    None,
    Array,
    Index,
    Uniform,
    ShaderStorage,
    AtomicCounter,
    DrawIndirect,
    TransformFeedback,
    Texture,
    Query,

    ComputeIndirect,

    PixelUnpack,
    PixelPack,
};

enum class CTexType : uint8
{
    Array1D,
    Array2D,
    ArrayCubeMap,

    CubeMap,
    Tex1D,
    Tex2D,
    Tex3D,

    TexBuffer,
};

enum class CTextureAccess : uint8
{
    None,
    ReadOnly,
    ReadWrite,
    WriteOnly,
};

enum class CProgramStage : uint8
{
    None = 0x0,
    Vertex = 0x1,
    Fragment = 0x2,

    Geometry = 0x4,

    TessellationEvaluation = 0x8,
    TessellationControl = 0x10,

    Compute = 0x20,
};

C_FLAGS(CProgramStage,uint8);

enum class CBufferAccess : uint8
{
    WriteBit = 0x1, /*!< Enable writing to the buffer*/
    ReadBit = 0x2, /*!< Enable reading from the buffer*/
    Persistent = 0x4, /*!< Make the buffer mapping persistent*/
    Coherent = 0x8, /*!< Make buffer contents coherent across draws*/
    Invalidate = 0x10,
};

C_FLAGS(CBufferAccess,uint8);

enum class CBufferStorage : uint8
{
    WriteBit = 0x1,
    ReadBit = 0x2,
    Persistent = 0x4, /*!< Make the buffer mapping persistent*/
    Coherent = 0x8, /*!< Make buffer contents coherent across draws and etc., performance impact*/
    Dynamic = 0x10, /*!< Allow use of coffee_graphics_buffer_substore()*/
    ClientStorage = 0x20, /*!< Read from client memory*/
};

C_FLAGS(CBufferStorage,uint8);

/*!
 * \brief Specifies buffer storage parameters in coffee_graphics_buffer_store(). Stream* specifies data modified once and drawn few times, static is modified once and drawn many times, dynamic is modified often and drawn many times. *Draw specifies data used for GL drawing, *Read specifies data read from the GL, *Copy specifies data is used for both reading and drawing operations.
 */
enum class CBufferUsage : uint8
{
    Default,

    StreamDraw,
    StreamRead,
    StreamCopy,

    StaticDraw,
    StaticRead,
    StaticCopy,

    DynamicDraw,
    DynamicRead,
    DynamicCopy,
};

enum class CAttributeMode : uint8
{
    Interleaved, /*!< Packed in buffer*/
    Separate,    /*!< Each attribute has its own buffer*/
};

enum class CQueryType : uint8
{
    TimeElapsed,

    SamplesPassed,
    AnySamplesPassed,

    PrimitivesGenerated,
    XFBPrimitivesWritten,
};

enum class CQueryResultType : uint8
{
    ResultAvailable,
    ResultWait,
    Result,
};

enum class CDataType : uint8
{
    None,

    Scalar, BigScalar,

    Byte,   UByte,
    Short,  UShort,
    Int,    UInt,
    Int64,  UInt64,

    //Texture data types, don't worry
    UByte3_3_2,     UByte2_3_3_Rev,

    UShort5_6_5,    UShort5_6_5_Rev,
    UShort4_4_4_4,  UShort4_4_4_4_Rev,
    UShort5_5_5_1,  UShort1_5_5_5_Rev,

    UInt8_8_8_8,    UInt8_8_8_8_Rev,
    UInt10_10_10_2, UInt2_10_10_10_Rev,
};

enum class CDebugSeverity : uint8
{
    Notification,
    Low,
    Medium,
    High,
};

enum class CDebugSource : uint8
{
    API,
    ShaderCompiler,
    Application,
    WindowManager,
    Other,
};

enum class CDebugType : uint8
{
    Error,
    Performance,
    Marker,
    Other,
};

}
}

#endif
