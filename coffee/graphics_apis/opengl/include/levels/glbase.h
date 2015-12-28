#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include "gltypes.h"
#include <coffee/core/base/cdebug.h>
#include <coffee/core/CTypes>

#include <glad/glad.h>
#include <KHR/khrplatform.h>

namespace Coffee{
namespace CGL{

struct CGL_Implementation
{
    enum class Primitive
    {
        Point,
        Line,
        Triangle,
    };

    enum class PrimitiveCreation
    {
        Explicit = 0x1,
        Strip = 0x2,
        Fan = 0x4,
        Loop = 0x8,
        Adjacency = 0x10,
    };

    enum class Face
    {
        Front,
        Back,
        Both,
    };

    enum class BufferBit
    {
        Color,
        Depth,
        Stencil,
    };

    enum class AttribMode
    {
        Interleaved,
        Separate,
    };

    enum class ShaderStage
    {
        Vertex,
        Fragment,

        Compute,
        Geometry,

        TessEval,
        TessControl,
    };

    enum class DataType
    {
        ArrayData,
        ElementData,
        UniformData,
        ShaderData,
        AtomicData,
        QueryData,
        XFBData,
        DrawcallData,
        ComputecallData,
        PixelData,
    };

    enum class Texture
    {
        T2D,
        T3D,
        Cubemap,
        Rect,

        T2DArray,
        T3DArray,
        CubemapArray,

        CubeX_P,
        CubeX_N,
        CubeY_P,
        CubeY_N,
        CubeZ_P,
        CubeZ_N,

        Proxy2D,
        Proxy3D,
        ProxyCube,
        ProxyRect,
    };

    enum class InternalFormat
    {
        RGBA8,
        RGBA32F,
    };

    enum class OperationType
    {
        PixelTransfer,
        ShaderBufferTransfer,

        BufferDataTransfer,

        TextureTransfer,

        CommandCompletion,
        XFBCompletion,

        ImageCompletion,

        UniformCompletion,
        VertexAttributeCompletion,
        VertexElementCompletion,

        FramebufferCompletion,
    };

    enum class Feature
    {
        Blend,

        DepthClamp,
        DepthTest,
        Dither,

        FramebufferSRGB,

        LineSmooth,
        PolygonSmooth,

        PrimitiveRestart,
        PrimitiveRestartFixedIdx,

        SampleAlphaToCoverage,
        SampleShading,
        SampleMask,

        ScissorTest,
        StencilTest,

        PointSize,
    };

    enum class RetrieveStrategy
    {
        Await,
        NoWait,
        Value,
    };

    enum class Object
    {
        Framebuffer,
        Texture,
        Buffer,
        Query,
        XFB,
        VAO,
        Sampler,

        Shader,
        Program,
    };

    /* Type definitions */

    using CGenum = uint32;
    using CGflag = uint32;

    /* Shorthand for GL object handles, we will treat them differently */
    using CGhnd = uint32;

    /* Feature support */

    struct Debug
    {
        //Variables
        static bool b_isDebugging;

        /* Strictly debugging */

        static void InitDebugFlag(){b_isDebugging = false;}
        static void SetDebugLevel(Severity){}

        static void SetObjectLabel(Object,uint32,cstring){}

        static void SetDebugGroup(cstring){}
        static void UnsetDebugGroup(){}

        static void DebugMessage(cstring){}

        static void DebugSetCallback(){}

        static void IsEnabledi(){}

        static cstring GetString(CGenum){return nullptr;}
        static cstring GetStringi(CGenum,uint32){return nullptr;}

        /* Get*v */

        static int32 GetInteger(CGenum){return 0;}
        static uint32 GetUInteger(CGenum){return 0;}

        static int64 GetIntegerLL(CGenum){return 0;}
        static uint64 GetUIntegerLL(CGenum){return 0;}

        static scalar GetScalar(CGenum){return 0;}
        static bigscalar GetScalarLL(CGenum){return 0;}

        static bool GetBooleanv(CGenum){return false;}

        /* Get*i_v */

        static int32 GetIntegerI(CGenum,uint32){return 0;}
        static uint32 GetUIntegerI(CGenum,uint32){return 0;}

        static int64 GetIntegerLLI(CGenum,uint32){return 0;}
        static uint64 GetUIntegerLLI(CGenum,uint32){return 0;}

        static scalar GetScalarI(CGenum,uint32){return 0;}
        static bigscalar GetScalarLLI(CGenum,uint32){return 0;}

        static bool GetBooleanvI(CGenum,uint32){return false;}

        /* Is* */

        static bool IsBuffer(CGhnd){return false;}
        static bool IsVAO(CGhnd){return false;}

        static bool IsFramebuffer(CGhnd){return false;}
        static bool IsRenderbuffer(CGhnd){return false;}

        static bool IsPipeline(CGhnd){return false;}
        static bool IsShader(CGhnd){return false;}
        static bool IsProgram(CGhnd){return false;}

        static bool IsSync(CGhnd){return false;}
        static bool IsQuery(CGhnd&){return false;}

        static bool IsTexture(CGhnd){return false;}
        static bool IsSampler(CGhnd){return false;}

        static bool IsXF(CGhnd){return false;}

        /* Internal format information */

        static bool InternalFormatSupport(CGenum){return false;}
    };

    static void LoadBinding()
    {
    }
};

}
}

#endif
