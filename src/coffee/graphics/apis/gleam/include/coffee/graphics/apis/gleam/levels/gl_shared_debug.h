#pragma once

#include "gl_shared_enum_convert.h"

#include <coffee/core/base.h>
#include <coffee/core/string_ops.h>
#include <coffee/core/types/display/gl_properties.h>
#include <coffee/core/types/hardware_info.h>
#include <coffee/core/types/software_info.h>

namespace Coffee {
namespace CGL {

struct CGL_Shared_Limits
{
    enum ShaderVariant
    {
        Vertex_Base   = 50,
        Fragment_Base = 100,

        Total_Base = 150,

        Geometry_Base    = 200,
        TessControl_Base = 250,
        TessEval_Base    = 300,
        Compute_Base     = 350,
    };

    enum MaxLimit
    {
        None,

        Outputs = 1,
        Inputs,
        UniformVals,    /* number of scalars allowed */
        ImageUnits,     /* aka. texture units */
        ImageUniforms,  /* read/write images */
        UniformsBlocks, /* UBO */
        UniformVectors, /* scalar, vec2, vec3, vec4 uniforms */
        AtomicCounters,
        AtomicBufs,
        SSBO,

        Max_Shader_Property,

        Vertex_Attribs = 1000,
        Vertex_AttrRelativeOff,
        Vertex_AttrStride,
        Vertex_AttrBindings,
        Vertex_Streams,

        Vertex_ClipDists,
        Vertex_CullDists,
        Vertex_CombClipCullDists,

        Geom_InComps,
        Geom_OutComps,
        Geom_OutVerts,

        Tess_Patches,

        Tex_Size2D,
        Tex_Size3D,
        Tex_SizeArray,
        Tex_SizeCube,
        Tex_SizeBuf,
        Tex_SizeSparse2D,
        Tex_SizeSparse3D,
        Tex_SizeSparseArray,
        Tex_LODBias,

        VAO_ElementVerts,
        VAO_ElementIndices,
        VAO_ElementIndex,

        FBO_DrawBufs,
        FBO_RendBufs,
        FBO_RendBufSize,
        FBO_ColorAtt,
        FBO_Width,
        FBO_Height,
        FBO_Layers,
        FBO_Samples,

        XF_SeparateComps,
        XF_InterleavComps,
        XF_SeparateAttrs,

        UniformLocs,

        UBO_Size,
        SSBO_Size,
        AtomicBuf_Size,

        UBO_Bindings,
        SSBO_Bindings,
        AtomicBuf_Bindings,

        View_Dimensions,

        Dbg_LabelLen,
        Dbg_MessageLen,
        Dbg_LoggedMessages,

        Compile_Threads,

        Max_Property,
    };

    static i32     Max(u32 v);
    static cstring MaxName(u32 v);

    static size_2d<i32> MaxSize(u32 v);

    template<u32 ShaderBase>
    STATICINLINE i32 ShaderMax(MaxLimit v)
    {
        if(v >= Max_Shader_Property)
            return 0;
        return Max(ShaderBase + v);
    }
};

struct CGL_Shared_Debug
{
    struct Context
    {
        CString extensionList;

        /* Stores information on pixel formats */
        Vector<i32> internalFormats;
    };

    /* Verifying loader results */
    STATICINLINE bool VerifyInit()
    {
        return C_CAST<bool>(glEnable);
    }

    /* GL_KHR_debug */
    C_DEPRECATED
    STATICINLINE void InitDebugFlag()
    {
    }

    static void SetDebugMode(bool enabled);

    STATICINLINE void SetDebugLevel(
        C_UNUSED(debug::Severity s), C_UNUSED(bool enabled))
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glDebugMessageControl(
            GL_DONT_CARE,
            GL_DONT_CARE,
            to_enum(s),
            0,
            nullptr,
            (enabled) ? GL_TRUE : GL_FALSE);
#endif
    }

    STATICINLINE void SetObjectLabel(
        C_UNUSED(Object t), C_UNUSED(CGhnd h), C_UNUSED(cstring s))
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glObjectLabel(to_enum(t), h, -1, s);
#endif
    }

    STATICINLINE void SetDebugGroup(C_UNUSED(cstring n), C_UNUSED(u32 id) = 0)
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, id, -1, n);
#endif
    }
    STATICINLINE void UnsetDebugGroup()
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glPopDebugGroup();
#endif
    }

    STATICINLINE void DebugMessage(
        debug::Severity s, debug::Type t, CString const& n)
    {
        DebugMessage(s, t, n.c_str());
    }

    STATICINLINE void DebugMessage(
        C_UNUSED(debug::Severity s),
        C_UNUSED(debug::Type t),
        C_UNUSED(cstring n))
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glDebugMessageInsert(
            GL_DEBUG_SOURCE_APPLICATION, to_enum(t), 0, to_enum(s), -1, n);
#endif
    }

    STATICINLINE void DebugSetCallback(
        C_UNUSED(CGcallback c), C_UNUSED(void* param))
    {
#if GL_VERSION_VERIFY(0x330, 0x320)
        glDebugMessageCallback(c, param);
#endif
    }

    /* Extensions */

    static void GetExtensions(Context& ctxt);

    STATICINLINE bool CheckExtensionSupported(Context& ctxt, cstring id)
    {
        if(ctxt.extensionList.size() > 0)
            return ctxt.extensionList.find(id) != CString::npos;
        else
            return false;
    }

    /* Context information */

    static Display::GL::Version ContextVersion();

    /* GLSL information */

    static Display::GL::Version ShaderLanguageVersion();

    /* Rendering device info */

    static HWDeviceInfo Renderer();

    /* Texture format support */
    STATICINLINE void InitInternalFormats()
    {
    }
    STATICINLINE void FreeInternalFormats()
    {
    }

    STATICINLINE void InitCompressedFormats(Context& ctxt)
    {
        ctxt.internalFormats.resize(
            C_FCAST<u32>(GetInteger(GL_NUM_COMPRESSED_TEXTURE_FORMATS)));
        GetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, ctxt.internalFormats.data());
    }

    C_DEPRECATED
    STATICINLINE void FreeCompressedFormats()
    {
    }

    static bool CompressedFormatSupport(Context& ctxt, PixFmt t);

    STATICINLINE bool InternalFormatSupport(PixFmt)
    {
#if GL_VERSION_VERIFY(0x300, 0x300)
        return true;
#else
        return false;
#endif
    }
    STATICINLINE Size InternalFormatMaxResolution2D()
    {
        Size sz;
        sz.w = sz.h = GetInteger(GL_MAX_TEXTURE_SIZE);
        return sz;
    }

    /* GetString */

    STATICINLINE cstring GetString(CGenum e)
    {
        return C_RCAST<cstring>(glGetString(e));
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE cstring GetStringi(CGenum e, uint32 i)
    {
        return (cstring)glGetStringi(e, i);
    }
#endif

    /* Get*v */

    STATICINLINE Size GetViewport()
    {
        rect<i32> r;
        GetIntegerv(GL_VIEWPORT, r.data);
        return r.convert<u32>().size();
    }

    STATICINLINE void GetIntegerv(CGenum e, i32* v)
    {
        glGetIntegerv(e, v);
    }
    STATICINLINE i32 GetInteger(CGenum e)
    {
        i32 i = 0;
        glGetIntegerv(e, &i);
        return i;
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE int64 GetIntegerLL(CGenum e)
    {
        int64 i = 0;
        glGetInteger64v(e, &i);
        return i;
    }
#endif

    STATICINLINE scalar GetScalar(CGenum e)
    {
        scalar i = 0.f;
        glGetFloatv(e, &i);
        return i;
    }

    STATICINLINE bool GetBooleanv(CGenum e)
    {
        GLboolean i = GL_FALSE;
        glGetBooleanv(e, &i);
        return i == GL_TRUE;
    }

        /* Get*i_v */
#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE int32 GetIntegerI(CGenum e, uint32 i)
    {
        int32 v = 0;
        glGetIntegeri_v(e, i, &v);
        return v;
    }

    STATICINLINE int64 GetIntegerLLI(CGenum e, uint32 i)
    {
        int64 v = 0;
        glGetInteger64i_v(e, i, &v);
        return v;
    }
#endif

    /* Object validity */

    STATICINLINE
    bool IsBuffer(CGhnd h)
    {
        return glIsBuffer(h) == GL_TRUE;
    }
#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE
    bool IsVAO(CGhnd h)
    {
        return glIsVertexArray(h) == GL_TRUE;
    }
#endif

    STATICINLINE
    bool IsFramebuffer(CGhnd h)
    {
        return glIsFramebuffer(h) == GL_TRUE;
    }
    STATICINLINE
    bool IsRenderbuffer(CGhnd h)
    {
        return glIsRenderbuffer(h) == GL_TRUE;
    }

    STATICINLINE
    bool IsShader(CGhnd h)
    {
        return glIsShader(h) == GL_TRUE;
    }
    STATICINLINE
    bool IsProgram(CGhnd h)
    {
        return glIsProgram(h) == GL_TRUE;
    }

    STATICINLINE
    bool IsTexture(CGhnd h)
    {
        return glIsTexture(h) == GL_TRUE;
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE
    bool IsSampler(CGhnd h)
    {
        return glIsSampler(h) == GL_TRUE;
    }

    STATICINLINE
    bool IsSync(CGsync h)
    {
        return glIsSync((GLsync)h) == GL_TRUE;
    }
    STATICINLINE
    bool IsQuery(CGhnd h)
    {
        return glIsQuery(h) == GL_TRUE;
    }

    STATICINLINE
    bool IsXF(CGhnd h)
    {
        return glIsTransformFeedback(h) == GL_TRUE;
    }

#if GL_VERSION_VERIFY(0x330, 0x310)
    STATICINLINE
    bool IsPipeline(CGhnd h)
    {
        return glIsProgramPipeline(h) == GL_TRUE;
    }

#endif
#if GL_VERSION_VERIFY(0x330, 0x320)
    /* IsEnabled */
    STATICINLINE
    bool IsEnabledi(Feature f, int32 i)
    {
        return glIsEnabledi(to_enum(f), i) == GL_TRUE;
    }
#endif

#endif
};

} // namespace CGL
} // namespace Coffee
