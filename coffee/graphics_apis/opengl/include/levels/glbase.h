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
    /* Type definitions */
    using CGenum = uint32;
    using CGflag = uint32;
    using CGcallback = GLDEBUGPROC;
    using CGsync = void*;

    /* Shorthand for GL object handles, we will treat them differently */
    using CGhnd = uint32;

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

    enum class BufferBit : int32
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
        Interleaved,
        Separate,
    };

    enum class ShaderStage
    {
        Vertex = 0x1,
        Fragment = 0x2,

        Compute = 0x4,
        Geometry = 0x8,

        TessEval = 0x10,
        TessControl = 0x20,
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

    enum InternalFormat
    {
        //R formats
        CG_RED8I = GL_R8I,
        CG_RED8UI = GL_R8UI,
        CG_RED16I = GL_R16I,
        CG_RED16UI = GL_R16UI,
        CG_RED16F = GL_R16F,
        CG_RED32I = GL_R32I,
        CG_RED32UI = GL_R32UI,
        CG_RED32F = GL_R32F,

        //RG formats
        CG_RG8I = GL_RG8I,
        CG_RG8UI = GL_RG8UI,
        CG_RG16I = GL_RG16I,
        CG_RG16UI = GL_RG16UI,
        CG_RG16F = GL_RG16F,

        //RGB formats
        CG_R3G3B2 = GL_R3_G3_B2,
        CG_RGB5 = GL_RGB5,
        CG_RGB8 = GL_RGB8,
        CG_RGB10 = GL_RGB10,
        CG_RGB12 = GL_RGB12,
        CG_RGB16I = GL_RGB16I,
        CG_RGB16UI = GL_RGB16UI,
        CG_RGB16F = GL_RGB16F,
        CG_RGB32I = GL_RGB32I,
        CG_RGB32UI = GL_RGB32UI,
        CG_RGB32F = GL_RGB32F,

        //Special RGB formats
        CG_RGB9E5 = GL_RGB9_E5,
        CG_R11G11B10F = GL_R11F_G11F_B10F,

        //SRGB formats
        CG_SRGB8 = GL_SRGB8,
        CG_SRGB8A8 = GL_SRGB8_ALPHA8,

        //RGBA formats
        CG_RGBA2 = GL_RGBA2,
        CG_RGBA4 = GL_RGBA4,
        CG_RGBA8I = GL_RGBA8I,
        CG_RGBA8UI = GL_RGBA8UI,
        CG_RGBA12 = GL_RGBA12,
        CG_RGBA16I = GL_RGBA16I,
        CG_RGBA16UI = GL_RGBA16UI,
        CG_RGBA16F = GL_RGBA16F,
        CG_RGBA32I = GL_RGBA32I,
        CG_RGBA32UI = GL_RGBA32UI,
        CG_RGBA32F = GL_RGBA32F,

        //Special RGBA formats
        CG_RGB5A1 = GL_RGB5_A1,
        CG_RGB10A2 = GL_RGB10_A2,
        CG_RGB10A2UI = GL_RGB10_A2UI,

        //Compressed formats
        CG_RGTC_RUI = GL_COMPRESSED_RED_RGTC1,
        CG_RGTC_RI = GL_COMPRESSED_SIGNED_RED_RGTC1,
        CG_RGTC_RGUI = GL_COMPRESSED_RG_RGTC2,
        CG_RGTC_RGI = GL_COMPRESSED_SIGNED_RG_RGTC2,

#ifdef GL_EXT_texture_compression_s3tc
        CG_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        CG_DXT1NoAlpha = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
        CG_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        CG_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
#endif

#ifdef GL_ARB_texture_compression_bptc
        CG_BPTC_RGBA_UNORM = GL_COMPRESSED_RGBA_BPTC_UNORM,
        CG_BPTC_SRGBA_UNORM = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
        CG_BPTC_RGB_SFLOAT = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
        CG_BPTC_RGB_UFLOAT = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
#endif

#ifdef GL_KHR_texture_compression_astc_hdr
        CG_ASTC_RGBA_4x4 = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
        CG_ASTC_RGBA_5x4 = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
        CG_ASTC_RGBA_5x5 = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
        CG_ASTC_RGBA_6x5 = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
        CG_ASTC_RGBA_6x6 = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
        CG_ASTC_RGBA_8x5 = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
        CG_ASTC_RGBA_8x6 = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
        CG_ASTC_RGBA_8x8 = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
        CG_ASTC_RGBA_10x5 = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
        CG_ASTC_RGBA_10x6 = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
        CG_ASTC_RGBA_10x8 = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
        CG_ASTC_RGBA_10x10 = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
        CG_ASTC_RGBA_12x10 = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
        CG_ASTC_RGBA_12x12 = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,

        CG_ASTC_SRGB8A8_4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
        CG_ASTC_SRGB8A8_5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
        CG_ASTC_SRGB8A8_5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
        CG_ASTC_SRGB8A8_6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
        CG_ASTC_SRGB8A8_6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
        CG_ASTC_SRGB8A8_8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
        CG_ASTC_SRGB8A8_8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
        CG_ASTC_SRGB8A8_8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
        CG_ASTC_SRGB8A8_10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
        CG_ASTC_SRGB8A8_10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
        CG_ASTC_SRGB8A8_10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
        CG_ASTC_SRGB8A8_10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
        CG_ASTC_SRGB8A8_12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
        CG_ASTC_SRGB8A8_12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
#endif
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

    struct CGL_TypeMapBase
    {
        static _cbasic_static_map<BufferBit,GLenum,3> bufferbit_map;
    };

    static CGenum to_enum(Severity s)
    {
        switch(s)
        {
        case Severity::High:
            return GL_DEBUG_SEVERITY_HIGH;
        case Severity::Medium:
            return GL_DEBUG_SEVERITY_MEDIUM;
        case Severity::Low:
            return GL_DEBUG_SEVERITY_LOW;
        case Severity::Information:
            return GL_DEBUG_SEVERITY_NOTIFICATION;
        default:
            return GL_NONE;
        }
    }
    static CGenum to_enum(DebugType t)
    {
        switch(t)
        {
        case DebugType::Compatibility:
            return GL_DEBUG_TYPE_PORTABILITY;
            break;
        case DebugType::Compliance:
            return GL_DEBUG_TYPE_PORTABILITY;
            break;
        case DebugType::Deprecated:
            return GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
            break;
        case DebugType::Performance:
            return GL_DEBUG_TYPE_PERFORMANCE;
            break;
        case DebugType::Marker:
            return GL_DEBUG_TYPE_MARKER;
            break;
        case DebugType::UndefinedBehavior:
            return GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
            break;
        default:
            return GL_NONE;
        }
    }
    static CGenum to_enum(Object t)
    {
        CGenum type;
        switch(t)
        {
        case Object::Shader:
            type = GL_SHADER;
            break;
        case Object::Program:
            type = GL_PROGRAM;
            break;
        case Object::VAO:
            type = GL_VERTEX_ARRAY;
            break;
        case Object::Buffer:
            type = GL_BUFFER;
            break;
        case Object::Texture:
            type = GL_TEXTURE;
            break;
        case Object::XFB:
            type = GL_TRANSFORM_FEEDBACK;
            break;
        case Object::Sampler:
            type = GL_SAMPLER;
            break;
        case Object::Query:
            type = GL_QUERY;
            break;
        case Object::Framebuffer:
            type = GL_FRAMEBUFFER;
            break;
        case Object::RenderBuffer:
            type = GL_RENDERBUFFER;
            break;
        }
        return type;
    }
    static CGenum to_enum(Feature f)
    {
        switch(f)
        {
        case Feature::Blend:
            return GL_BLEND;
        case Feature::DepthTest:
            return GL_DEPTH_TEST;
        case Feature::DepthClamp:
            return GL_DEPTH_CLAMP;
        case Feature::Dither:
            return GL_DITHER;
        case Feature::FramebufferSRGB:
            return GL_FRAMEBUFFER_SRGB;
        case Feature::LineSmooth:
            return GL_LINE_SMOOTH;
        case Feature::Multisample:
            return GL_MULTISAMPLE;
        case Feature::PointSize:
            return GL_PROGRAM_POINT_SIZE;
        case Feature::PolygonOffsetFill:
            return GL_POLYGON_OFFSET_FILL;
        case Feature::PolygonOffsetLine:
            return GL_POLYGON_OFFSET_LINE;
        case Feature::PolygonOffsetPoint:
            return GL_POLYGON_OFFSET_POINT;
        case Feature::PolygonSmooth:
            return GL_POLYGON_SMOOTH;
        case Feature::PrimitiveRestart:
            return GL_PRIMITIVE_RESTART;
        case Feature::PrimitiveRestartFixedIdx:
            return GL_PRIMITIVE_RESTART_FIXED_INDEX;
        case Feature::RasterizerDiscard:
            return GL_RASTERIZER_DISCARD;
        case Feature::SampleAlphaToCoverage:
            return GL_SAMPLE_ALPHA_TO_COVERAGE;
        case Feature::SampleAlphaToOne:
            return GL_SAMPLE_ALPHA_TO_ONE;
        case Feature::SampleCoverage:
            return GL_SAMPLE_COVERAGE;
        case Feature::SampleMask:
            return GL_SAMPLE_MASK;
        case Feature::SampleShading:
            return GL_SAMPLE_SHADING;
        case Feature::ScissorTest:
            return GL_SCISSOR_TEST;
        case Feature::StencilTest:
            return GL_STENCIL_TEST;
        case Feature::SeamlessCubemap:
            return GL_TEXTURE_CUBE_MAP_SEAMLESS;
        }
    }
    static CGenum to_enum(Face f)
    {
        switch(f)
        {
        case Face::Front:
            return GL_FRONT;
        case Face::Back:
            return GL_BACK;
        case Face::Both:
            return GL_FRONT_AND_BACK;
        }
    }
    static CGenum to_enum(BufferBit f)
    {
        return coffee_get_flags(f,CGL_TypeMapBase::bufferbit_map);
    }

    /* Base OpenGL, all implemented in GL3.3 */
    static void Enable(Feature e){glEnable(to_enum(e));}
    static void Disable(Feature e){glDisable(to_enum(e));}

    static void Enablei(Feature e,uint32 i){glEnablei(to_enum(e),i);}
    static void Disablei(Feature e,uint32 i){glDisablei(to_enum(e),i);}

    static void Clear(BufferBit f){glClear(to_enum(f));}
    static void ClearColor(CVec4 c){glClearColor(c.x(),c.y(),c.z(),c.w());}

    static void ClearDepth(scalar f){glClearDepthf(f);}
    static void ClearStencil(int32 f){glClearStencil(f);}

    static void FrontFace(Face v){glFrontFace(to_enum(v));}
    static void CullMode(Face v){glCullFace(to_enum(v));}

    static void Flush(){glFlush();}
    static void Finish(){glFinish();}

    static void ClearBufferiv(CGenum f,int32 i,const int32* d){glClearBufferiv(f,i,d);}
    static void ClearBufferuiv(CGenum f,int32 i,const uint32* d){glClearBufferuiv(f,i,d);}
    static void ClearBufferfv(CGenum f,int32 i,const scalar* d){glClearBufferfv(f,i,d);}
    static void ClearBufferfi(CGenum f,int32 i,scalar d1,int32 d2){glClearBufferfi(f,i,d1,d2);}

    static void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    static void BlendEqSep(CGenum v1,CGenum v2){glBlendEquationSeparate(v1,v2);}

    static void DepthFunc(CGenum f){glDepthFunc(f);}
    static void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}

    static void StencilFuncSep(CGenum v1,CGenum v2,int32 d1,uint32 d2)
    {glStencilFuncSeparate(v1,v2,d1,d2);}
    static void StencilOpSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glStencilOpSeparate(v1,v2,v3,v4);}
    static void StencilMaskSep(CGenum f,uint32 d){glStencilMaskSeparate(f,d);}

    static void ViewportSet(CRect64 const* r){glViewport(r->x,r->y,r->w,r->h);}
    static void ScissorSet(CRect64 const* r){glScissor(r->x,r->y,r->w,r->h);}
    static void DepthSet(CZField64 const* d){glDepthRange(d->near,d->far);}

    static void PointParameteriv(CGenum f,const int32* d){glPointParameteriv(f,d);}
    static void PointParameterfv(CGenum f,const scalar* d){glPointParameterfv(f,d);}
    static void PointSize(scalar f){glPointSize(f);}

    static void LineWidth(scalar f){glLineWidth(f);}

    static void PolyMode(CGenum f1,CGenum f2){glPolygonMode(f1,f2);}
    static void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    static void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}
    static void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}

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
            return c_strstr(s_ExtensionList.c_str(),id);
        }

        //Variables
        static bool b_isDebugging;
        static CString s_ExtensionList;

        /* Strictly debugging */

        static void InitDebugFlag(){b_isDebugging = false;}

        static void SetDebugMode(bool enabled)
        {
            if(enabled)
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            else
                glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
    };

    static void LoadBinding()
    {
    }
};

C_FLAGS(CGL_Implementation::BufferBit,int32);

CString CGL_Implementation::Debug::s_ExtensionList = "";

_cbasic_static_map<CGL_Implementation::BufferBit,GLenum,3>
CGL_Implementation::CGL_TypeMapBase::bufferbit_map = {
    {BufferBit::Color,GL_COLOR_BUFFER_BIT},
    {BufferBit::Depth,GL_DEPTH_BUFFER_BIT},
    {BufferBit::Stencil,GL_STENCIL_BUFFER_BIT},
};

}
}

#endif
