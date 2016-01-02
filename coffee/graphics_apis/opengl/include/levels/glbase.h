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

    /* Type definitions */
    using CGenum = uint32;
    using CGflag = uint32;
    using CGcallback = GLDEBUGPROC;
    using CGsync = void*;

    /* Shorthand for GL object handles, we will treat them differently */
    using CGhnd = uint32;

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

    struct Debug
    {
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

CString CGL_Implementation::Debug::s_ExtensionList = "";

}
}

#endif
