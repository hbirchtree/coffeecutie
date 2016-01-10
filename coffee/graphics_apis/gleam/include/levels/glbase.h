#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/graphics_apis/include/gltypes.h>
#include <coffee/core/coffee_strings.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/core/base/cdisplay.h>
#include <coffee/core/CTypes>

#include <glad/glad.h>
#include <KHR/khrplatform.h>

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

    enum class BufBit : int32
    {
        Color = 0x1,
        Depth = 0x2,
        Stencil = 0x4,
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

        All = 0x3F,
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

    enum class PatchProperty
    {
        Vertices = GL_PATCH_VERTICES,
        DefOuterLevel = GL_PATCH_DEFAULT_OUTER_LEVEL,
        DefInnerLevel = GL_PATCH_DEFAULT_INNER_LEVEL,
    };

    struct CGL_TypeMapBase
    {
        constexpr static _cbasic_static_map<BufBit,GLenum,3> bufferbit_map = {
            {BufBit::Color,GL_COLOR_BUFFER_BIT},
            {BufBit::Depth,GL_DEPTH_BUFFER_BIT},
            {BufBit::Stencil,GL_STENCIL_BUFFER_BIT},
        };
    };

    static CGenum to_enum(Severity s);
    static CGenum to_enum(DebugType t);
    static CGenum to_enum(Object t);
    static CGenum to_enum(Feature f);
    static CGenum to_enum(Face f);
    static CGenum to_enum(BufBit f);
    static CGenum to_enum(Prim p,PrimCre c);
    static CGenum to_enum(DrawMode f);
    static CGenum to_enum(bool pack, PixelOperation f);
    static CGenum to_enum(PixelFormat f, PixelFlags e = PixelFlags::None,
                          CompFlags d = CompressionNone);
    /*!
     * \brief Returns GL_*_SHADER enum
     * \param f
     * \return
     */
    static CGenum to_enum1(ShaderStage f);
    /*!
     * \brief Returns GL_*_SHADER_BIT enum
     * \param f
     * \return
     */
    static CGenum to_enum2(ShaderStage f);
    static CGenum to_enum(ValueHandling f);
    static CGenum to_enum(ValueComparison f);
    static CGenum to_enum(Operator f);
    static CGenum to_enum(Texture f);
    static CGenum to_enum(LogicOp op);
    static CGenum to_enum(BufType f);

    static CGenum to_enum1(ResourceAccess acc);
    static CGenum to_enum2(ResourceAccess acc);
    static CGenum to_enum3(ResourceAccess acc);

    static CGenum to_enum(PixelComponents f);
    static CGenum to_enum(TypeEnum f);
    static CGenum to_enum(BitFormat f);

    static CGenum to_enum(QueryT f);
    static CGenum to_enum(FramebufferT f);

    static CGenum to_enum(AttribMode f);
    static CGenum to_enum(PatchProperty f);

    static CGpixfmt get_fmt(PixelFormat e, bool rev);

    /* Base OpenGL, all implemented in GL3.3 */
    static void Enable(Feature e){glEnable(to_enum(e));}
    static void Disable(Feature e){glDisable(to_enum(e));}

    static void Enablei(Feature e,uint32 i){glEnablei(to_enum(e),i);}
    static void Disablei(Feature e,uint32 i){glDisablei(to_enum(e),i);}

    static void ClearDepth(scalar f){glClearDepthf(f);}
    static void ClearStencil(int32 f){glClearStencil(f);}

    static void FrontFace(Face v){glFrontFace(to_enum(v));}
    static void CullMode(Face v){glCullFace(to_enum(v));}

    static void Flush(){glFlush();}
    static void Finish(){glFinish();}

    static void ClearBufferiv(const int32* d){glClearBufferiv(GL_STENCIL,0,d);}
//    static void ClearBufferuiv(CGenum f,int32 i,const uint32* d){glClearBufferuiv(f,i,d);}
    static void ClearBufferfv(bool color,int32 i,const scalar* d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,d);}
    static void ClearBufferfv(bool color,int32 i,const CVec4& d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,(scalar*)&d);}
    static void ClearBufferfi(scalar d,int32 s){glClearBufferfi(GL_DEPTH_STENCIL,0,d,s);}

    static void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    static void BlendEqSep(Operator v1,Operator v2)
    {glBlendEquationSeparate(to_enum(v1),to_enum(v2));}

    static void ColorMaski(uint32 i, CColorMask op){glColorMaski(i,op.r,op.g,op.b,op.a);}
    static void ColorLogicOp(LogicOp op){glLogicOp(to_enum(op));}

    static void DepthFunc(ValueComparison f){glDepthFunc(to_enum(f));}
    static void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}

    static void StencilFuncSep(Face v1,ValueComparison v2,int32 d1,uint32 d2)
    {glStencilFuncSeparate(to_enum(v1),to_enum(v2),d1,d2);}
    static void StencilOpSep(Face v1,ValueHandling sfail,ValueHandling dfail,ValueHandling dsfail)
    {glStencilOpSeparate(to_enum(v1),to_enum(sfail),to_enum(dfail),to_enum(dsfail));}
    static void StencilMaskSep(Face f,uint32 d){glStencilMaskSeparate(to_enum(f),d);}

    static void ViewportSet(CRect64 const* r){glViewport(r->x,r->y,r->w,r->h);}
    static void ScissorSet(CRect64 const* r){glScissor(r->x,r->y,r->w,r->h);}
    static void DepthSet(CZField64 const* d){glDepthRange(d->near,d->far);}

    static void PointParameteriv(CGenum f,const int32* d){glPointParameteriv(f,d);}
    static void PointParameterfv(CGenum f,const scalar* d){glPointParameterfv(f,d);}
    static void PointSize(scalar f){glPointSize(f);}

    static void LineWidth(scalar f){glLineWidth(f);}

    static void PolyMode(Face f1,DrawMode f2){glPolygonMode(to_enum(f1),to_enum(f2));}
    static void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    static void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}
    static void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}

    static void PixelStore(bool pack, PixelOperation op, int32 v)
    {glPixelStorei(to_enum(pack,op),v);}

    static bool DebuggingSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_debug");}

    struct Debug
    {
        static void GetExtensions()
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

        static bool CheckExtensionSupported(cstring id)
        {
            return CStrFind(s_ExtensionList.c_str(),id);
        }

        //Variables
        static bool b_isDebugging;
        static CString s_ExtensionList;

        /* Strictly debugging */

        static void InitDebugFlag(){b_isDebugging = false;}

        static void SetDebugMode(bool enabled)
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

        static void SetDebugLevel(Severity s,bool e)
        {
            glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,
                                  to_enum(s),0,nullptr,(e)?GL_TRUE:GL_FALSE);
        }

        static void SetObjectLabel(Object t,CGhnd h,cstring s)
        {
            glObjectLabel(to_enum(t),h,-1,s);
        }

        static void SetDebugGroup(cstring n, uint32 id)
        {
            glPushDebugGroup(GL_DEBUG_TYPE_PUSH_GROUP,id,-1,n);
        }
        static void UnsetDebugGroup(){glPopDebugGroup();}

        static void DebugMessage(Severity s,DebugType t,cstring n)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                                 to_enum(t),0,
                                 to_enum(s),
                                 -1,n);
        }

        static void DebugSetCallback(CGcallback c, void* param)
        {
            glDebugMessageCallback(c,param);
        }

        static void IsEnabledi(Feature f,int32 i){
            glIsEnabledi(CGL::CGL_Implementation::to_enum(f),i);}

        static cstring GetString(CGenum e){return (cstring)glGetString(e);}
        static cstring GetStringi(CGenum e,uint32 i){return (cstring)glGetStringi(e,i);}

        /* Get*v */

        static int32 GetInteger(CGenum e)
        {
            int32 i;
            glGetIntegerv(e,&i);
            return i;
        }

        static int64 GetIntegerLL(CGenum e)
        {
            int64 i;
            glGetInteger64v(e,&i);
            return i;
        }

        static scalar GetScalar(CGenum e)
        {
            scalar i;
            glGetFloatv(e,&i);
            return i;
        }
        static bigscalar GetScalarLL(CGenum e)
        {
            bigscalar i;
            glGetDoublev(e,&i);
            return i;
        }

        static bool GetBooleanv(CGenum e)
        {
            GLboolean i;
            glGetBooleanv(e,&i);
            return i==GL_TRUE;
        }

        /* Get*i_v */

        static int32 GetIntegerI(CGenum e,uint32 i)
        {
            int32 v;
            glGetIntegeri_v(e,i,&v);
            return v;
        }

        static int64 GetIntegerLLI(CGenum e,uint32 i)
        {
            int64 v;
            glGetInteger64i_v(e,i,&v);
            return v;
        }

        static scalar GetScalarI(CGenum e,uint32 i)
        {
            scalar v;
            glGetFloati_v(e,i,&v);
            return v;
        }
        static bigscalar GetScalarLLI(CGenum e,uint32 i)
        {
            bigscalar v;
            glGetDoublei_v(e,i,&v);
            return v;
        }

        static bool GetBooleanvI(CGenum e,uint32 i)
        {
            GLboolean v;
            glGetBooleani_v(e,i,&v);
            return v==GL_TRUE;
        }

        /* Is* */

        static bool IsBuffer(CGhnd h){return glIsBuffer(h)==GL_TRUE;}
        static bool IsVAO(CGhnd h){return glIsVertexArray(h)==GL_TRUE;}

        static bool IsFramebuffer(CGhnd h){return glIsFramebuffer(h)==GL_TRUE;}
        static bool IsRenderbuffer(CGhnd h){return glIsRenderbuffer(h)==GL_TRUE;}

        static bool IsPipeline(CGhnd h){return glIsProgramPipeline(h)==GL_TRUE;}
        static bool IsShader(CGhnd h){return glIsShader(h)==GL_TRUE;}
        static bool IsProgram(CGhnd h){return glIsProgram(h)==GL_TRUE;}

        static bool IsSync(CGsync h){return glIsSync((GLsync)h)==GL_TRUE;}
        static bool IsQuery(CGhnd h){return glIsQuery(h)==GL_TRUE;}

        static bool IsTexture(CGhnd h){return glIsTexture(h)==GL_TRUE;}
        static bool IsSampler(CGhnd h){return glIsSampler(h)==GL_TRUE;}

        static bool IsXF(CGhnd h){return glIsTransformFeedback(h)==GL_TRUE;}

        /* Internal format information */

        static int32* InternalFormatSupport(CGenum tt, CGenum t, CGenum prop,int32 n)
        {
            int32* i = new int32[n];
            glGetInternalformativ(tt,t,prop,n*sizeof(i[0]),i);
            return i;
        }

        static CDisplay::CGLVersion ContextVersion()
        {
            CDisplay::CGLVersion ver;

            ver.major = GetInteger(GL_MAJOR_VERSION);
            ver.minor = GetInteger(GL_MINOR_VERSION);
            ver.revision = 0;

            return ver;
        }
    };

    static void LoadBinding(){}
};

C_FLAGS(CGL_Implementation::BufBit,uint32);
C_FLAGS(CGL_Implementation::PrimCre,uint32);
C_FLAGS(CGL_Implementation::ShaderStage,uint32);

#include "gl_to_enum.inl"

}
}

#endif
