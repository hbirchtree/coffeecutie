#pragma once

#include "gl_shared_include.h"

#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/types/edef/enumfun.h>
#include <coffee/core/types/edef/graphicsenum.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<size_t Major, size_t Minor, bool ES = false>
struct gl_ver_t
{
    static constexpr size_t major = Major;
    static constexpr size_t minor = Minor;
    static constexpr bool   es    = ES;
};

template<typename ReqVer, typename MinVer>
struct gl_version_at_least_ver
{
    template<size_t V1, size_t V2>
    struct ge_to
    {
        static constexpr bool value = V1 >= V2;
    };

    using major_test = ge_to<ReqVer::major, MinVer::major>;
    using minor_test = ge_to<ReqVer::minor, MinVer::minor>;

    static constexpr bool value =
            (major_test::value && minor_test::value) &&
            (ReqVer::es == MinVer::es);
};

/* Functions that require a certain OpenGL version */
#define GL_VERSION_REQ(MinVer) \
    template<\
        typename RequestedVer = ReqVer,\
        typename MinimumVer = MinVer,\
        typename std::enable_if<gl_version_at_least_ver<RequestedVer, MinimumVer>::value>::type* = nullptr\
        >

#define GL_VERSION_REQ_COMBO(MinVerDesktop, MinVerES) \
    template<\
        typename RequestedVer = ReqVer,\
        typename MinimumVerDesktop = MinVerDesktop,\
        typename MinimumVerES = MinVerES,\
        typename std::enable_if<gl_version_at_least_ver<RequestedVer, MinimumVerDesktop>::value || gl_version_at_least_ver<RequestedVer, MinimumVerES>::value, bool>::type* = nullptr\
        >

/* Functions that require a certain OpenGL version and are desktop-only */
#define GL_VERSION_REQ_DESKTOP(MinVer) \
    template<\
        typename RequestedVer = ReqVer,\
        typename MinimumVer = MinVer,\
        typename std::enable_if<gl_version_at_least_ver<RequestedVer, MinimumVer>::value, bool>::type* = nullptr\
        >
/* Functions that require a certain OpenGL version and are ES-only */
#define GL_VERSION_REQ_ES(MinVer) \
    template<\
        typename RequestedVer = ReqVer,\
        typename MinimumVer = MinVer,\
        typename std::enable_if<gl_version_at_least_ver<RequestedVer, MinimumVer>::value>::type* = nullptr\
        >

/* For requirement of an extension */
#define GL_EXTENSION_REQ(extname) /* Intentionally empty */

/* -- Danger zone -- */
#define GLVER_20 gl_ver_t<2,0>
#define GLVER_21 gl_ver_t<2,1>

/* Standardized OpenGL versions */
#define GLVER_30 gl_ver_t<3,0>
#define GLVER_31 gl_ver_t<3,1>
#define GLVER_32 gl_ver_t<3,2>
#define GLVER_33 gl_ver_t<3,3>
#define GLVER_40 gl_ver_t<4,0>
#define GLVER_41 gl_ver_t<4,1>
#define GLVER_42 gl_ver_t<4,2>
#define GLVER_43 gl_ver_t<4,3>
#define GLVER_44 gl_ver_t<4,4>
#define GLVER_45 gl_ver_t<4,5>
#define GLVER_46 gl_ver_t<4,6>

/* OpenGL ES versions */
#define GLESVER_20 gl_ver_t<2,0,true>
#define GLESVER_30 gl_ver_t<3,0,true>
#define GLESVER_31 gl_ver_t<3,1,true>
#define GLESVER_32 gl_ver_t<3,2,true>

#define GL_EXT_CHECK(name, ext_name) bool name ## Supported() \
    {\
        return Debug::CheckExtensionSupported(ext_name);\
    }

#define GL_VERSION_NONE 0xFFFFF

#define GL_VERSION_VERIFY(core_ver, es_ver) \
    (GL_VERSION_BASE_CORE >= core_ver) || (GL_VERSION_BASE_ES >= es_ver)

/*
 * The above macro is used as such:
 *
 * GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
 * static void ClearBuffer(...)
 * {
 *     ...;
 * }
 */


/* Type definitions */
using CGenum = uint32;
using CGflag = uint32;
#if GL_VERSION_VERIFY(0x410, 0x320)
using CGcallback = GLDEBUGPROC;
#else
using CGcallback = void(*)();
#endif
using CGsync = void*;

/* Shorthand for GL object handles, we will treat them differently */
using CGhnd = uint32;
using CGhnd64 = uint64;
struct CGpixfmt
{
    CGenum type;
    CGenum fmt;
};

struct CGDbgMsg
{
    CString msg;
    Severity sev;
    DebugComponent comp;
    DebugType type;
    uint32 id;
};

enum class BufBit
{
    Color = 0x1,
    Depth = 0x2,
    Stencil = 0x4,
};

enum class Face
{
    None = 0x0,
    Front = 0x1,
    Back = 0x2,
    Both = 0x3,
    FaceMask = Both,
};

enum class VertexWinding
{
    CCW, CW
};

enum class AttribMode
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    Interleaved = GL_INTERLEAVED_ATTRIBS,
    Separate = GL_SEPARATE_ATTRIBS,
#else
    Interleaved = GL_NONE,
    Separate = GL_NONE,
#endif
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
    RenderBuffer,
    Buffer,
    Query,
    XFB,
    VAO,
    Sampler,

    Shader,
    Program,
};

enum class OpType
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

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    ClipDist,
#endif
    Culling,

    DebugOutput,
    DebugOutputSync,

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    DepthClamp,
#endif
    DepthTest,
    Dither,

    FramebufferSRGB,

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    LineSmooth,
    PolygonSmooth,
#endif

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    PrimitiveRestart,
#endif
    PrimitiveRestartFixedIdx,

    SampleAlphaToCoverage,
    SampleCoverage,
    SampleMask,
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    SampleAlphaToOne,
    SampleShading,
#endif

    RasterizerDiscard,

    ScissorTest,
    StencilTest,

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    PointSize,
#endif

    SeamlessCubemap,

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    Multisample,
#endif

    PolygonOffsetFill,
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    PolygonOffsetPoint,
    PolygonOffsetLine,
#endif
};

enum class BufType
{

    ArrayData = GL_ARRAY_BUFFER,
    ElementData = GL_ELEMENT_ARRAY_BUFFER,
#if GL_VERSION_VERIFY(0x300, 0x300)
    UniformData = GL_UNIFORM_BUFFER,
#else
    UniformData = GL_NONE,
#endif

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    ShaderData = GL_SHADER_STORAGE_BUFFER,
    AtomicData = GL_ATOMIC_COUNTER_BUFFER,
    QueryData = GL_QUERY_BUFFER,
#else
    ShaderData = 0,
    AtomicData = 0,
    QueryData = 0,
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
    XFBData = GL_TRANSFORM_FEEDBACK_BUFFER,
#else
    XFBData = GL_NONE,
#endif
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    DrawcallData = GL_DRAW_INDIRECT_BUFFER,
    ComputecallData = GL_DISPATCH_INDIRECT_BUFFER,
#else
    DrawcallData = 0,
    ComputecallData = 0,
#endif

#if GL_VERSION_VERIFY(0x300, 0x300)
    PixelUData = GL_PIXEL_UNPACK_BUFFER,
    PixelPData = GL_PIXEL_PACK_BUFFER,
#else
    PixelUData = GL_NONE,
    PixelPData = GL_NONE,
#endif
};

enum class Texture
{
    T2D = GL_TEXTURE_2D,
#if GL_VERSION_VERIFY(0x300, 0x300)
    T3D = GL_TEXTURE_3D,
#else
    T3D = GL_NONE,
#endif
    Cubemap = GL_TEXTURE_CUBE_MAP,
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    Rect = GL_TEXTURE_RECTANGLE,
#endif
#if GL_VERSION_VERIFY(0x300, 0x300)
    T2DArray = GL_TEXTURE_2D_ARRAY,
#else
    T2DArray = GL_NONE,
#endif
#if GL_VERSION_VERIFY(0x330, 0x320)
    CubemapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
#endif

    CubeX_P = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    CubeX_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    CubeY_P = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    CubeY_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    CubeZ_P = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    CubeZ_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    Proxy2D = GL_PROXY_TEXTURE_2D,
    Proxy3D = GL_PROXY_TEXTURE_3D,
    ProxyCubemap = GL_PROXY_TEXTURE_CUBE_MAP,
    ProxyRect = GL_PROXY_TEXTURE_RECTANGLE,

    Proxy2DArray = GL_PROXY_TEXTURE_2D_ARRAY,
    ProxyCubemapArray = GL_PROXY_TEXTURE_CUBE_MAP_ARRAY,

    T2DMS = GL_TEXTURE_2D_MULTISAMPLE,
    T2DArrayMS = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,

    Proxy2DMS = GL_PROXY_TEXTURE_2D_MULTISAMPLE,
    Proxy2DArrayMS = GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
};

enum class DrawMode
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    Point = GL_POINT,
    Line = GL_LINE,
    Fill = GL_FILL,
#else
    Point = 0,
    Line = 0,
    Fill = 0,
#endif
};

enum class QueryT
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    AnySamples = GL_ANY_SAMPLES_PASSED,
    AnySamplesCon = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
    XFGen = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
#else
    AnySamples = GL_NONE,
    AnySamplesCon = GL_NONE,
    XFGen = GL_NONE,
#endif

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    TimeElapsed = GL_TIME_ELAPSED,

    Samples = GL_SAMPLES_PASSED,
    PrimGen = GL_PRIMITIVES_GENERATED,

#else
    TimeElapsed = GL_NONE,
    Samples = GL_NONE,
    PrimGen = GL_NONE,
#endif
};

enum class PatchProperty
{
#if GL_VERSION_VERIFY(0x330, 0x320)
    Vertices = GL_PATCH_VERTICES,
#else
    Vertices = GL_NONE,
#endif
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    DefOuterLevel = GL_PATCH_DEFAULT_OUTER_LEVEL,
    DefInnerLevel = GL_PATCH_DEFAULT_INNER_LEVEL,
#endif
};

C_FLAGS(BufBit,u32);
C_FLAGS(ShaderStage,u32);
C_FLAGS(Face,u32);

}
}