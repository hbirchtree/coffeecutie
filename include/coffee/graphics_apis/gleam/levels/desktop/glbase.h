#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include "../../../gltypes.h"
#include <coffee/core/coffee_strings.h>

#include <coffee/core/CDebug>
#include <coffee/core/types/composite_types.h>
#include <coffee/CImage>

#include <glad/glad.h>
#include <glad/KHR/khrplatform.h>

#include "glxml.h"

namespace Coffee{
namespace CGL{

struct CGL_Implementation
{
    /* Type definitions */
    using CGenum = uint32;
    using CGflag = uint32;
    using CGcallback = GLDEBUGPROC;
    using CGsync = void*;

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

    /* Shorthand for GL object handles, we will treat them differently */
    using CGhnd = uint32;

    enum class Prim
    {
        Point,
        Line,
        Triangle,
        Patch,
    };

    enum class DrawMode
    {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL,
    };

    enum class PrimCre
    {
        Explicit = 0x0,
        Strip = 0x1,
        Fan = 0x2,
        Loop = 0x4,
        Adjacency = 0x8,
    };

    enum class BufBit
    {
        Color = 0x1,
        Depth = 0x2,
        Stencil = 0x3,
    };

    enum class Face
    {
        Front = 0x1,
        Back = 0x2,
        Both = 0x3,
    };

    enum class AttribMode
    {
        Interleaved = GL_INTERLEAVED_ATTRIBS,
        Separate = GL_SEPARATE_ATTRIBS,
    };

    enum class ShaderStage
    {
        Vertex = 0x1,
        Fragment = 0x2,

        Compute = 0x4,
        Geometry = 0x8,

        TessEval = 0x10,
        TessControl = 0x20,

        All = 0x40,
    };

    enum CompFlags
    {
        CompressionNone,

        ASTC_4x4,
        ASTC_5x4,
        ASTC_5x5,
        ASTC_6x5,
        ASTC_6x6,
        ASTC_8x5,
        ASTC_8x6,
        ASTC_8x8,
        ASTC_10x5,
        ASTC_10x6,
        ASTC_10x8,
        ASTC_10x10,
        ASTC_12x10,
        ASTC_12x12,

        S3TC_1,
        S3TC_3,
        S3TC_5,
    };

    enum class BufType
    {
        ArrayData = GL_ARRAY_BUFFER,
        ElementData = GL_ELEMENT_ARRAY_BUFFER,
        UniformData = GL_UNIFORM_BUFFER,
        ShaderData = GL_SHADER_STORAGE_BUFFER,
        AtomicData = GL_ATOMIC_COUNTER_BUFFER,
        QueryData = GL_QUERY_BUFFER,
        XFBData = GL_TRANSFORM_FEEDBACK_BUFFER,
        DrawcallData = GL_DRAW_INDIRECT_BUFFER,
        ComputecallData = GL_DISPATCH_INDIRECT_BUFFER,
        PixelUData = GL_PIXEL_UNPACK_BUFFER,
        PixelPData = GL_PIXEL_PACK_BUFFER,
    };

    enum class Texture
    {
        T2D = GL_TEXTURE_2D,
        T3D = GL_TEXTURE_3D,
        Cubemap = GL_TEXTURE_CUBE_MAP,
        Rect = GL_TEXTURE_RECTANGLE,

        T2DArray = GL_TEXTURE_2D_ARRAY,
        CubemapArray = GL_TEXTURE_CUBE_MAP_ARRAY,

        CubeX_P = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        CubeX_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        CubeY_P = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        CubeY_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        CubeZ_P = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        CubeZ_N = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

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

        ClipDistance,

        DebugOutput,
        DebugOutputSynchronous,

        DepthClamp,
        DepthTest,
        Dither,

        FramebufferSRGB,

        LineSmooth,
        PolygonSmooth,

        PrimitiveRestart,
        PrimitiveRestartFixedIdx,

        SampleAlphaToCoverage,
        SampleAlphaToOne,
        SampleCoverage,
        SampleShading,
        SampleMask,

        RasterizerDiscard,

        ScissorTest,
        StencilTest,

        PointSize,

        SeamlessCubemap,

        Multisample,

        PolygonOffsetFill,
        PolygonOffsetPoint,
        PolygonOffsetLine,
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

    enum class FramebufferT
    {
        Draw = 0x1,
        Read = 0x2,
        All = 0x3,
    };

    enum class QueryT
    {
        AnySamples = GL_ANY_SAMPLES_PASSED,
        AnySamplesCon = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
        Samples = GL_SAMPLES_PASSED,
        PrimGen = GL_PRIMITIVES_GENERATED,
        XFGen = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
        TimeElapsed = GL_TIME_ELAPSED,
    };

    STATICINLINE CGenum to_enum(Severity s);
    STATICINLINE CGenum to_enum(DebugType t);
    STATICINLINE CGenum to_enum(Object t);
    STATICINLINE CGenum to_enum(Feature f, uint32 offset = 0);
    STATICINLINE CGenum to_enum(Face f);
    STATICINLINE CGenum to_enum(BufBit f);
    STATICINLINE CGenum to_enum(Prim p,PrimCre c);
    STATICINLINE CGenum to_enum(DrawMode f);
    STATICINLINE CGenum to_enum(bool pack, PixelOperation f);
    STATICINLINE CGenum to_enum(PixelFormat f, PixelFlags e = PixelFlags::None,
                          CompFlags d = CompressionNone);
    /*!
     * \brief Returns GL_*_SHADER enum
     * \param f
     * \return
     */
    STATICINLINE CGenum to_enum1(ShaderStage f);
    /*!
     * \brief Returns GL_*_SHADER_BIT enum
     * \param f
     * \return
     */
    STATICINLINE CGenum to_enum2(ShaderStage f);
    STATICINLINE CGenum to_enum(ValueHandling f);
    STATICINLINE CGenum to_enum(ValueComparison f);
    STATICINLINE CGenum to_enum(Operator f);
    STATICINLINE CGenum to_enum(Texture f);
    STATICINLINE CGenum to_enum(LogicOp op);
    STATICINLINE CGenum to_enum(BufType f);

    STATICINLINE CGenum to_enum1(ResourceAccess acc);
    STATICINLINE CGenum to_enum2(ResourceAccess acc);
    STATICINLINE CGenum to_enum3(ResourceAccess acc);

    STATICINLINE CGenum to_enum(PixelComponents f);
    STATICINLINE CGenum to_enum(TypeEnum f);
    STATICINLINE CGenum to_enum(BitFormat f);

    STATICINLINE CGenum to_enum(QueryT f);
    STATICINLINE CGenum to_enum(FramebufferT f);

    STATICINLINE CGenum to_enum(AttribMode f);

    STATICINLINE CGpixfmt get_fmt(PixelFormat e, bool rev);

    /* Base OpenGL, all implemented in GL3.3 */
    STATICINLINE void Enable(Feature e,uint32 o = 0){glEnable(to_enum(e,o));}
    STATICINLINE void Disable(Feature e,uint32 o = 0){glDisable(to_enum(e,o));}

    STATICINLINE void Enablei(Feature e,uint32 i,uint32 o = 0){glEnablei(to_enum(e,o),i);}
    STATICINLINE void Disablei(Feature e,uint32 i,uint32 o = 0){glDisablei(to_enum(e,o),i);}

    STATICINLINE void ClearDepth(scalar f){glClearDepthf(f);}
    STATICINLINE void ClearStencil(int32 f){glClearStencil(f);}

    STATICINLINE void FrontFace(Face v){glFrontFace(to_enum(v));}
    STATICINLINE void CullMode(Face v){glCullFace(to_enum(v));}

    STATICINLINE void Flush(){glFlush();}
    STATICINLINE void Finish(){glFinish();}

    STATICINLINE void ClearBufferiv(const int32* d){glClearBufferiv(GL_STENCIL,0,d);}
//    STATICINLINE void ClearBufferuiv(CGenum f,int32 i,const uint32* d){glClearBufferuiv(f,i,d);}
    STATICINLINE void ClearBufferfv(bool color,int32 i,const scalar* d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,d);}
    STATICINLINE void ClearBufferfv(bool color,int32 i,const CVec4& d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,(scalar*)&d);}
    STATICINLINE void ClearBufferfi(scalar d,int32 s){glClearBufferfi(GL_DEPTH_STENCIL,0,d,s);}

    STATICINLINE void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    STATICINLINE void BlendEqSep(Operator v1,Operator v2)
    {glBlendEquationSeparate(to_enum(v1),to_enum(v2));}

    STATICINLINE void ColorMaski(uint32 i, CColorMask op){glColorMaski(i,op.r,op.g,op.b,op.a);}
    STATICINLINE void ColorLogicOp(LogicOp op){glLogicOp(to_enum(op));}

    STATICINLINE void DepthFunc(ValueComparison f){glDepthFunc(to_enum(f));}
    STATICINLINE void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}

    STATICINLINE void StencilFuncSep(Face v1,ValueComparison v2,int32 d1,uint32 d2)
    {glStencilFuncSeparate(to_enum(v1),to_enum(v2),d1,d2);}
    STATICINLINE void StencilOpSep(Face v1,ValueHandling sfail,ValueHandling dfail,ValueHandling dsfail)
    {glStencilOpSeparate(to_enum(v1),to_enum(sfail),to_enum(dfail),to_enum(dsfail));}
    STATICINLINE void StencilMaskSep(Face f,uint32 d){glStencilMaskSeparate(to_enum(f),d);}

    STATICINLINE void ViewportSet(CRect64 const* r){glViewport(r->x,r->y,r->w,r->h);}
    STATICINLINE void ScissorSet(CRect64 const* r){glScissor(r->x,r->y,r->w,r->h);}
    STATICINLINE void DepthSet(ZField64 const* d){glDepthRange(d->near,d->far);}

    STATICINLINE void PointParameteriv(CGenum f,const int32* d){glPointParameteriv(f,d);}
    STATICINLINE void PointParameterfv(CGenum f,const scalar* d){glPointParameterfv(f,d);}
    STATICINLINE void PointSize(scalar f){glPointSize(f);}

    STATICINLINE void LineWidth(scalar f){glLineWidth(f);}

    STATICINLINE void PolyMode(Face f1,DrawMode f2){glPolygonMode(to_enum(f1),to_enum(f2));}
    STATICINLINE void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    STATICINLINE void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}
    STATICINLINE void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}

    STATICINLINE void PixelStore(bool pack, PixelOperation op, int32 v)
    {glPixelStorei(to_enum(pack,op),v);}

    STATICINLINE bool DebuggingSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_debug");}

    struct Debug
    {
        STATICINLINE bool VerifyInit()
	{
	    return (bool)glEnable;
	}

        STATICINLINE void GetExtensions()
        {
            int32 numExtensions = GetInteger(GL_NUM_EXTENSIONS);
            s_ExtensionList = std::string();
            s_ExtensionList.reserve(numExtensions*20);
            for(int32 i=0;i<numExtensions;i++)
            {
                cstring str = GetStringi(GL_EXTENSIONS,i);
                s_ExtensionList.append(str);
                if(i<numExtensions-1)
                s_ExtensionList.push_back(' ');
            }
        }

        STATICINLINE bool CheckExtensionSupported(cstring id)
        {
            return StrFind(s_ExtensionList.c_str(),id);
        }

        /*Variables*/
        static bool b_isDebugging;
        static CString s_ExtensionList;

        /* Strictly debugging */

        STATICINLINE void InitDebugFlag(){b_isDebugging = false;}

        STATICINLINE void SetDebugMode(bool enabled)
        {
            if(enabled == b_isDebugging)
                return;
            if(enabled)
            {
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                b_isDebugging = true;
            }
            else
            {
                glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                b_isDebugging = true;
            }
        }

        STATICINLINE void SetDebugLevel(Severity s,bool enabled)
        {
            glDebugMessageControl(
                        GL_DONT_CARE,GL_DONT_CARE,
                        to_enum(s),0,nullptr,
                        (enabled)?GL_TRUE:GL_FALSE);
        }

        STATICINLINE void SetObjectLabel(Object t,CGhnd h,cstring s)
        {
            glObjectLabel(to_enum(t),h,-1,s);
        }

        STATICINLINE void SetDebugGroup(cstring n, uint32 id)
        {
            glPushDebugGroup(GL_DEBUG_TYPE_PUSH_GROUP,id,-1,n);
        }
        STATICINLINE void UnsetDebugGroup(){glPopDebugGroup();}

        STATICINLINE void DebugMessage(Severity s,DebugType t,cstring n)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                                 to_enum(t),0,
                                 to_enum(s),
                                 -1,n);
        }

        STATICINLINE void DebugSetCallback(CGcallback c, void* param)
        {
            glDebugMessageCallback(c,param);
        }

        STATICINLINE bool IsEnabledi(Feature f,int32 i){
            return glIsEnabledi(CGL::CGL_Implementation::to_enum(f),i)==GL_TRUE;}

        STATICINLINE cstring GetString(CGenum e){return (cstring)glGetString(e);}
        STATICINLINE cstring GetStringi(CGenum e,uint32 i){return (cstring)glGetStringi(e,i);}

        /* Get*v */

        STATICINLINE int32 GetInteger(CGenum e)
        {
            int32 i;
            glGetIntegerv(e,&i);
            return i;
        }

        STATICINLINE int64 GetIntegerLL(CGenum e)
        {
            int64 i;
            glGetInteger64v(e,&i);
            return i;
        }

        STATICINLINE scalar GetScalar(CGenum e)
        {
            scalar i;
            glGetFloatv(e,&i);
            return i;
        }
        STATICINLINE bigscalar GetScalarLL(CGenum e)
        {
            bigscalar i;
            glGetDoublev(e,&i);
            return i;
        }

        STATICINLINE bool GetBooleanv(CGenum e)
        {
            GLboolean i;
            glGetBooleanv(e,&i);
            return i==GL_TRUE;
        }

        /* Get*i_v */

        STATICINLINE int32 GetIntegerI(CGenum e,uint32 i)
        {
            int32 v;
            glGetIntegeri_v(e,i,&v);
            return v;
        }

        STATICINLINE int64 GetIntegerLLI(CGenum e,uint32 i)
        {
            int64 v;
            glGetInteger64i_v(e,i,&v);
            return v;
        }

        STATICINLINE scalar GetScalarI(CGenum e,uint32 i)
        {
            scalar v;
            glGetFloati_v(e,i,&v);
            return v;
        }
        STATICINLINE bigscalar GetScalarLLI(CGenum e,uint32 i)
        {
            bigscalar v;
            glGetDoublei_v(e,i,&v);
            return v;
        }

        STATICINLINE bool GetBooleanvI(CGenum e,uint32 i)
        {
            GLboolean v;
            glGetBooleani_v(e,i,&v);
            return v==GL_TRUE;
        }

        /* Is* */

        STATICINLINE bool IsBuffer(CGhnd h){return glIsBuffer(h)==GL_TRUE;}
        STATICINLINE bool IsVAO(CGhnd h){return glIsVertexArray(h)==GL_TRUE;}

        STATICINLINE bool IsFramebuffer(CGhnd h){return glIsFramebuffer(h)==GL_TRUE;}
        STATICINLINE bool IsRenderbuffer(CGhnd h){return glIsRenderbuffer(h)==GL_TRUE;}

        STATICINLINE bool IsPipeline(CGhnd h){return glIsProgramPipeline(h)==GL_TRUE;}
        STATICINLINE bool IsShader(CGhnd h){return glIsShader(h)==GL_TRUE;}
        STATICINLINE bool IsProgram(CGhnd h){return glIsProgram(h)==GL_TRUE;}

        STATICINLINE bool IsSync(CGsync h){return glIsSync((GLsync)h)==GL_TRUE;}
        STATICINLINE bool IsQuery(CGhnd h){return glIsQuery(h)==GL_TRUE;}

        STATICINLINE bool IsTexture(CGhnd h){return glIsTexture(h)==GL_TRUE;}
        STATICINLINE bool IsSampler(CGhnd h){return glIsSampler(h)==GL_TRUE;}

        STATICINLINE bool IsXF(CGhnd h){return glIsTransformFeedback(h)==GL_TRUE;}

        /* Internal format information */

        STATICINLINE int32* InternalFormatSupport(CGenum tt, CGenum t, CGenum prop,int32 n)
        {
            int32* i = new int32[n];
            glGetInternalformativ(tt,t,prop,n*sizeof(i[0]),i);
            return i;
        }

        STATICINLINE CDisplay::CGLVersion ContextVersion()
        {
            CDisplay::CGLVersion ver;

            ver.major = GetInteger(GL_MAJOR_VERSION);
            ver.minor = GetInteger(GL_MINOR_VERSION);
            ver.revision = 0;

            return ver;
        }

        STATICINLINE HWDeviceInfo Renderer()
        {
            cstring vendor = GetString(GL_VENDOR);
            cstring device = GetString(GL_RENDERER);
            cstring ver = GetString(GL_VERSION);

            return HWDeviceInfo(vendor,device,ver);
        }
    };

    STATICINLINE void LoadBinding(){}
};

C_FLAGS(CGL_Implementation::BufBit,uint32);
C_FLAGS(CGL_Implementation::PrimCre,uint32);
C_FLAGS(CGL_Implementation::ShaderStage,uint32);

}
}

#endif

#include "../gl_to_enum.inl"