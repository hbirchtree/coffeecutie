#pragma once

#include "gl_shared_include.h"

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/interfaces/cgraphics_types.h>

namespace Coffee {
namespace CGL {

using namespace RHI;

using glhnd = semantic::generic_handle_t<u32>;

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
        (major_test::value && minor_test::value) && (ReqVer::es == MinVer::es);
};

/* Functions that require a certain OpenGL version */
#define GL_VERSION_REQ(MinVer)                                                 \
    template<                                                                  \
        typename RequestedVer = ReqVer,                                        \
        typename MinimumVer   = MinVer,                                        \
        typename std::enable_if<                                               \
            gl_version_at_least_ver<RequestedVer, MinimumVer>::value>::type* = \
            nullptr>

#define GL_VERSION_REQ_COMBO(MinVerDesktop, MinVerES)                          \
    template<                                                                  \
        typename RequestedVer      = ReqVer,                                   \
        typename MinimumVerDesktop = MinVerDesktop,                            \
        typename MinimumVerES      = MinVerES,                                 \
        typename std::enable_if<                                               \
            gl_version_at_least_ver<RequestedVer, MinimumVerDesktop>::value || \
                gl_version_at_least_ver<RequestedVer, MinimumVerES>::value,    \
            bool>::type* = nullptr>

/* Functions that require a certain OpenGL version and are desktop-only */
#define GL_VERSION_REQ_DESKTOP(MinVer)                                \
    template<                                                         \
        typename RequestedVer = ReqVer,                               \
        typename MinimumVer   = MinVer,                               \
        typename std::enable_if<                                      \
            gl_version_at_least_ver<RequestedVer, MinimumVer>::value, \
            bool>::type* = nullptr>
/* Functions that require a certain OpenGL version and are ES-only */
#define GL_VERSION_REQ_ES(MinVer)                                              \
    template<                                                                  \
        typename RequestedVer = ReqVer,                                        \
        typename MinimumVer   = MinVer,                                        \
        typename std::enable_if<                                               \
            gl_version_at_least_ver<RequestedVer, MinimumVer>::value>::type* = \
            nullptr>

/* For requirement of an extension */
#define GL_EXTENSION_REQ(extname) /* Intentionally empty */

/* -- Danger zone -- */
#define GLVER_10 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_11 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_12 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_13 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_14 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_15 Coffee::CGL::gl_ver_t<1, 0>
#define GLVER_20 Coffee::CGL::gl_ver_t<2, 0>
#define GLVER_21 Coffee::CGL::gl_ver_t<2, 1>

/* Standardized OpenGL versions */

#define GLVER_30 Coffee::CGL::gl_ver_t<3, 0>
#define GLVER_31 Coffee::CGL::gl_ver_t<3, 1>
#define GLVER_32 Coffee::CGL::gl_ver_t<3, 2>
#define GLVER_33 Coffee::CGL::gl_ver_t<3, 3>
#define GLVER_40 Coffee::CGL::gl_ver_t<4, 0>
#define GLVER_41 Coffee::CGL::gl_ver_t<4, 1>
#define GLVER_42 Coffee::CGL::gl_ver_t<4, 2>
#define GLVER_43 Coffee::CGL::gl_ver_t<4, 3>
#define GLVER_44 Coffee::CGL::gl_ver_t<4, 4>
#define GLVER_45 Coffee::CGL::gl_ver_t<4, 5>
#define GLVER_46 Coffee::CGL::gl_ver_t<4, 6>

/* OpenGL ES versions */
#define GLESVER_20 Coffee::CGL::gl_ver_t<2, 0, true>
#define GLESVER_30 Coffee::CGL::gl_ver_t<3, 0, true>
#define GLESVER_31 Coffee::CGL::gl_ver_t<3, 1, true>
#define GLESVER_32 Coffee::CGL::gl_ver_t<3, 2, true>

#define GL_EXT_CHECK(name, ext_name)                        \
    bool name##Supported(Debug::Context& c)                 \
    {                                                       \
        return Debug::CheckExtensionSupported(c, ext_name); \
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
using CGenum = u32;
#if GL_VERSION_VERIFY(0x410, 0x320)
using CGcallback = GLDEBUGPROC;
#else
using CGcallback = void (*)();
#endif
using CGsync = void*;

/* Shorthand for GL object handles, we will treat them differently */
using CGhnd   = u32;
using CGhnd64 = u64;
struct CGpixfmt
{
    CGenum type;
    CGenum fmt;
};

struct CGDbgMsg
{
    CString          msg;
    debug::Severity  sev;
    debug::Component comp;
    debug::Type      type;
    u32              id;
};

enum class BufBit
{
    Color   = 0x1,
    Depth   = 0x2,
    Stencil = 0x4,
};

enum class AttribMode
{
#if GL_VERSION_VERIFY(0x300, 0x300)
    Interleaved = GL_INTERLEAVED_ATTRIBS,
    Separate    = GL_SEPARATE_ATTRIBS,
#else
    Interleaved = GL_NONE,
    Separate    = GL_NONE,
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

enum class DrawMode
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    Point = GL_POINT,
    Line  = GL_LINE,
    Fill  = GL_FILL,
#else
    Point       = 0,
    Line        = 0,
    Fill        = 0,
#endif
};

enum class PatchProperty
{
#if GL_VERSION_VERIFY(0x330, 0x320)
    Vertices = GL_PATCH_VERTICES,
#else
    Vertices    = GL_NONE,
#endif
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    DefOuterLevel = GL_PATCH_DEFAULT_OUTER_LEVEL,
    DefInnerLevel = GL_PATCH_DEFAULT_INNER_LEVEL,
#endif
};

using ShaderStage = typing::graphics::ShaderStage;

C_FLAGS(BufBit, u32);
C_FLAGS(ShaderStage, u32);
C_FLAGS(Face, u32);

} // namespace CGL
} // namespace Coffee

namespace glwrap {
namespace v {

using Coffee::CGL::gl_ver_t;

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)

using core33 = gl_ver_t<3, 3>;
using core40 = gl_ver_t<4, 0>;
using core41 = gl_ver_t<4, 1>;
using core42 = gl_ver_t<4, 2>;
using core43 = gl_ver_t<4, 3>;
using core44 = gl_ver_t<4, 4>;
using core45 = gl_ver_t<4, 5>;

using baseline = core33;

#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)

using es20 = gl_ver_t<2, 0>;
using es30 = gl_ver_t<3, 0>;
using es31 = gl_ver_t<3, 1>;
using es32 = gl_ver_t<3, 2>;

using baseline = es20;

#endif

}
}
