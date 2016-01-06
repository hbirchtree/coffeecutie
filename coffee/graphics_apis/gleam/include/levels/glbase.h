#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/graphics_apis/include/gltypes.h>
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
    struct CGpixfmt
    {
        CGenum type;
        CGenum fmt;
    };

    /* Shorthand for GL object handles, we will treat them differently */
    using CGhnd = uint32;

    enum class Primitive
    {
        Point,
        Line,
        Triangle,
        Patch,
    };

    enum class PaintingMode
    {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL,
    };

    enum class PrimitiveCreation
    {
        Explicit = 0x0,
        Strip = 0x1,
        Fan = 0x2,
        Loop = 0x4,
        Adjacency = 0x8,
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

    enum CompressionFlags
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

    enum class DataType
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
        constexpr static _cbasic_static_map<BufferBit,GLenum,3> bufferbit_map = {
            {BufferBit::Color,GL_COLOR_BUFFER_BIT},
            {BufferBit::Depth,GL_DEPTH_BUFFER_BIT},
            {BufferBit::Stencil,GL_STENCIL_BUFFER_BIT},
        };
    };

    static CGenum to_enum(Severity s);
    static CGenum to_enum(DebugType t);
    static CGenum to_enum(Object t);
    static CGenum to_enum(Feature f);
    static CGenum to_enum(Face f);
    static CGenum to_enum(BufferBit f);
    static CGenum to_enum(Primitive p,PrimitiveCreation c);
    static CGenum to_enum(PaintingMode f);
    static CGenum to_enum(bool pack, PixelOperation f);
    static CGenum to_enum(PixelFormat f, PixelFlags e = PixelFlags::None,
                          CompressionFlags d = CompressionNone);
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
    static CGenum to_enum(DataType f);

    static CGenum to_enum1(ResourceAccess acc);
    static CGenum to_enum2(ResourceAccess acc);
    static CGenum to_enum3(ResourceAccess acc);

    static CGenum to_enum(PixelComponents f);
    static CGenum to_enum(TypeEnum f);

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

    static void PolyMode(Face f1,PaintingMode f2){glPolygonMode(to_enum(f1),to_enum(f2));}
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
            return c_strstr(s_ExtensionList.c_str(),id);
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
    };

    static void LoadBinding(){}
};

C_FLAGS(CGL_Implementation::BufferBit,uint32);
C_FLAGS(CGL_Implementation::PrimitiveCreation,uint32);
C_FLAGS(CGL_Implementation::ShaderStage,uint32);

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        Severity s)
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

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        DebugType t)
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

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::Object t)
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

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::Feature f)
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

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::Face f)
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

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::BufferBit f)
{
    return coffee_get_flags(f,CGL_TypeMapBase::bufferbit_map);
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::Primitive p,
        CGL_Implementation::PrimitiveCreation c)
{
    switch(p)
    {
    case Primitive::Line:
        if(feval(c&(PrimitiveCreation::Adjacency|PrimitiveCreation::Strip)))
            return GL_LINE_STRIP_ADJACENCY;
        switch(c)
        {
        case PrimitiveCreation::Explicit:
            return GL_LINES;
        case PrimitiveCreation::Adjacency:
            return GL_LINE_STRIP_ADJACENCY;
        case PrimitiveCreation::Strip:
            return GL_LINE_STRIP;
        case PrimitiveCreation::Loop:
            return GL_LINE_LOOP;
        default:
            return GL_NONE;
        }
    case Primitive::Point:
        return GL_POINTS;
    case Primitive::Triangle:
        if(feval(c&(PrimitiveCreation::Adjacency|PrimitiveCreation::Strip)))
            return GL_TRIANGLE_STRIP_ADJACENCY;
        switch(c)
        {
        case PrimitiveCreation::Explicit:
            return GL_TRIANGLES;
        case PrimitiveCreation::Adjacency:
            return GL_TRIANGLES_ADJACENCY;
        case PrimitiveCreation::Strip:
            return GL_TRIANGLE_STRIP;
        case PrimitiveCreation::Fan:
            return GL_TRIANGLE_FAN;
        default:
            return GL_TRIANGLES;
        }
    case Primitive::Patch:
        if(c!=PrimitiveCreation::Explicit)
            return GL_NONE;
        else
            return GL_PATCHES;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        CGL_Implementation::PaintingMode f)
{
    switch(f)
    {
    case PaintingMode::Fill:
        return GL_FILL;
    case PaintingMode::Line:
        return GL_LINE;
    case PaintingMode::Point:
        return GL_POINT;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        bool pack, PixelOperation f)
{
    switch(f)
    {
    case PixelOperation::Alignment:
        if(pack)
            return GL_PACK_ALIGNMENT;
        else
            return GL_UNPACK_ALIGNMENT;
    case PixelOperation::SwapEndiannes:
        if(pack)
            return GL_PACK_SWAP_BYTES;
        else
            return GL_PACK_SWAP_BYTES;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        PixelFormat f, PixelFlags e,
        CGL_Implementation::CompressionFlags d)
{
    switch(f)
    {
    case PixelFormat::ASTC:{
        CGenum out = 0;
        if(feval(e&PixelFlags::SRGBA))
            out = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        else if(feval(e&PixelFlags::RGBA))
            out = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        else
            return GL_NONE;
        switch(d)
        {
        case ASTC_4x4:
            out += 0;
        case ASTC_5x4:
            out += 1;
        case ASTC_5x5:
            out += 2;
        case ASTC_6x5:
            out += 3;
        case ASTC_6x6:
            out += 4;
        case ASTC_8x5:
            out += 5;
        case ASTC_8x6:
            out += 6;
        case ASTC_8x8:
            out += 7;
        case ASTC_10x5:
            out += 8;
        case ASTC_10x6:
            out += 9;
        case ASTC_10x8:
            out += 10;
        case ASTC_10x10:
            out += 11;
        case ASTC_12x10:
            out += 12;
        case ASTC_12x12:
            out += 13;
        default:
            return GL_NONE;
        }
        return out;
    }
    case PixelFormat::BPTC:
        if(feval(e&(PixelFlags::RGBA|PixelFlags::Unormalized)))
            return GL_COMPRESSED_RGBA_BPTC_UNORM;
        if(feval(e&(PixelFlags::SRGBA|PixelFlags::Unormalized)))
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        if(feval(e&(PixelFlags::RGB|PixelFlags::FloatingPoint)))
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        if(feval(e&(PixelFlags::RGB|PixelFlags::FloatingPoint|PixelFlags::Unsigned)))
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        return GL_NONE;
    case PixelFormat::RGTC:
        if(feval(e&PixelFlags::R|PixelFlags::Unsigned))
            return GL_COMPRESSED_RED_RGTC1;
        if(feval(e&PixelFlags::R|PixelFlags::Signed))
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        if(feval(e&PixelFlags::RG|PixelFlags::Unsigned))
            return GL_COMPRESSED_RG_RGTC2;
        if(feval(e&PixelFlags::RG|PixelFlags::Signed))
            return GL_COMPRESSED_SIGNED_RG_RGTC2;
        return GL_NONE;
    case PixelFormat::S3TC:
        if(feval(e&(PixelFlags::RGB))&&feval(d&S3TC_1))
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        if(feval(e&(PixelFlags::RGBA))&&feval(d&S3TC_1))
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        if(feval(e&(PixelFlags::RGBA))&&feval(d&S3TC_3))
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        if(feval(e&(PixelFlags::RGBA))&&feval(d&S3TC_5))
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    case PixelFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::DepthStencil:
        return GL_DEPTH_STENCIL;
    case PixelFormat::Stencil:
        return GL_NONE; //TODO: Fix stencil stuff

    case PixelFormat::R8I:
        return GL_R8I;
    case PixelFormat::R8UI:
        return GL_R8UI;
    case PixelFormat::R16I:
        return GL_R16I;
    case PixelFormat::R16UI:
        return GL_R16UI;
    case PixelFormat::R32I:
        return GL_R32I;
    case PixelFormat::R32UI:
        return GL_R32UI;
    case PixelFormat::R16F:
        return GL_R16F;
    case PixelFormat::R32F:
        return GL_R32F;

    case PixelFormat::RG8I:
        return GL_RG8I;
    case PixelFormat::RG8UI:
        return GL_RG8UI;
    case PixelFormat::RG16I:
        return GL_RG16I;
    case PixelFormat::RG16UI:
        return GL_RG16UI;
    case PixelFormat::RG32I:
        return GL_RG32I;
    case PixelFormat::RG32UI:
        return GL_RG32UI;
    case PixelFormat::RG16F:
        return GL_RG16F;
    case PixelFormat::RG32F:
        return GL_RG32F;

    case PixelFormat::RGB8I:
        return GL_RGB8I;
    case PixelFormat::RGB8UI:
        return GL_RGB8UI;
    case PixelFormat::RGB16I:
        return GL_RGB16I;
    case PixelFormat::RGB16UI:
        return GL_RGB16UI;
    case PixelFormat::RGB32I:
        return GL_RGB32I;
    case PixelFormat::RGB32UI:
        return GL_RGB32UI;
    case PixelFormat::RGB16F:
        return GL_RGB16F;
    case PixelFormat::RGB32F:
        return GL_RGB32F;

    case PixelFormat::RGBA8I:
        return GL_RGBA8I;
    case PixelFormat::RGBA8UI:
        return GL_RGBA8UI;
    case PixelFormat::RGBA16I:
        return GL_RGBA16I;
    case PixelFormat::RGBA16UI:
        return GL_RGBA16UI;
    case PixelFormat::RGBA32I:
        return GL_RGBA32I;
    case PixelFormat::RGBA32UI:
        return GL_RGBA32UI;
    case PixelFormat::RGBA16F:
        return GL_RGBA16F;
    case PixelFormat::RGBA32F:
        return GL_RGBA32F;

    case PixelFormat::R11G11B10F:
        return GL_R11F_G11F_B10F;

    case PixelFormat::R3G3B2UI:
        return GL_R3_G3_B2;
    case PixelFormat::RGB4UI:
        return GL_RGB4;
    case PixelFormat::RGB5UI:
        return GL_RGB5;
    case PixelFormat::RGB565UI:
        return GL_RGB565;
    case PixelFormat::RGB9E5UI:
        return GL_RGB9_E5;
    case PixelFormat::RGB10:
        return GL_RGB10;
    case PixelFormat::RGB12:
        return GL_RGB12;

    case PixelFormat::RGBA2:
        return GL_RGBA2;
    case PixelFormat::RGB10A2I:
        return GL_RGB10_A2;
    case PixelFormat::RGB10A2UI:
        return GL_RGB10_A2UI;
    case PixelFormat::RGBA12:
        return GL_RGBA12;
    case PixelFormat::RGB5A1UI:
        return GL_RGB5_A1;

    case PixelFormat::SRGB8A8:
        return GL_SRGB8_ALPHA8;
    case PixelFormat::SRGB8:
        return GL_SRGB8;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum1(
        CGL_Implementation::ShaderStage f)
{
    switch(f)
    {
    case ShaderStage::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderStage::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case ShaderStage::TessEval:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderStage::Geometry:
        return GL_GEOMETRY_SHADER;
    case ShaderStage::Fragment:
        return GL_FRAGMENT_SHADER;
    case ShaderStage::Compute:
        return GL_COMPUTE_SHADER;
    default:
        return GL_NONE;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum2(
        CGL_Implementation::ShaderStage f)
{
    CGenum o = 0;

    if(feval(f&ShaderStage::Vertex))
        o |= GL_VERTEX_SHADER_BIT;
    if(feval(f&ShaderStage::TessControl))
        o |= GL_TESS_CONTROL_SHADER_BIT;
    if(feval(f&ShaderStage::TessEval))
        o |= GL_TESS_EVALUATION_SHADER_BIT;
    if(feval(f&ShaderStage::Geometry))
        o |= GL_GEOMETRY_SHADER_BIT;
    if(feval(f&ShaderStage::Fragment))
        o |= GL_FRAGMENT_SHADER_BIT;
    if(feval(f&ShaderStage::Compute))
        o |= GL_COMPUTE_SHADER_BIT;
    if(feval(f&ShaderStage::All))
        o = GL_ALL_SHADER_BITS;

    return o;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        ValueHandling f)
{
    switch(f)
    {
    case ValueHandling::Keep:
        return GL_KEEP;
    case ValueHandling::Increment:
        return GL_INCR;
    case ValueHandling::Decrement:
        return GL_DECR;
    case ValueHandling::Invert:
        return GL_INVERT;
    case ValueHandling::Replace:
        return GL_REPLACE;
    case ValueHandling::WrapIncrement:
        return GL_INCR_WRAP;
    case ValueHandling::WrapDecrement:
        return GL_DECR_WRAP;
    case ValueHandling::Zero:
        return GL_ZERO;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        ValueComparison f)
{
    switch(f)
    {
    case ValueComparison::Always:
        return GL_ALWAYS;
    case ValueComparison::Equal:
        return GL_EQUAL;
    case ValueComparison::GEqual:
        return GL_GEQUAL;
    case ValueComparison::Greater:
        return GL_GREATER;
    case ValueComparison::LEqual:
        return GL_LEQUAL;
    case ValueComparison::Less:
        return GL_LESS;
    case ValueComparison::NEqual:
        return GL_NOTEQUAL;
    case ValueComparison::Never:
        return GL_NEVER;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(
        Operator f)
{
    switch(f)
    {
    case Operator::Add:
        return GL_FUNC_ADD;
    case Operator::Sub:
        return GL_FUNC_SUBTRACT;
    case Operator::RevSub:
        return GL_FUNC_REVERSE_SUBTRACT;
    case Operator::Min:
        return GL_MIN;
    case Operator::Max:
        return GL_MAX;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(CGL_Implementation::Texture f)
{
    return (CGenum)f;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(LogicOp op)
{
    if(feval(op&(LogicOp::COPY)))
        return GL_COPY;
    if(feval(op&(LogicOp::COPY|LogicOp::SRC_INVERSE)))
        return GL_COPY_INVERTED;
    if(feval(op&(LogicOp::CLEAR0)))
        return GL_CLEAR;
    if(feval(op&(LogicOp::CLEAR1)))
        return GL_SET;

    if(feval(op&(LogicOp::AND)))
        return GL_AND;
    if(feval(op&(LogicOp::NAND)))
        return GL_NAND;
    if(feval(op&(LogicOp::AND|LogicOp::DST_INVERSE)))
        return GL_AND_REVERSE;
    if(feval(op&(LogicOp::AND|LogicOp::SRC_INVERSE)))
        return GL_AND_INVERTED;

    if(feval(op&(LogicOp::OR)))
        return GL_OR;
    if(feval(op&(LogicOp::NOR)))
        return GL_NOR;
    if(feval(op&(LogicOp::XOR)))
        return GL_XOR;
    if(feval(op&(LogicOp::OR|LogicOp::DST_INVERSE)))
        return GL_OR_REVERSE;
    if(feval(op&(LogicOp::AND|LogicOp::SRC_INVERSE)))
        return GL_OR_INVERTED;

    if(feval(op&(LogicOp::NOOP|LogicOp::DST_INVERSE)))
        return GL_INVERT;

    if(feval(op&(LogicOp::XOR|LogicOp::SRC_INVERSE|LogicOp::DST_INVERSE)))
        return GL_XOR;

    return GL_NONE;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(CGL_Implementation::DataType f)
{
    return (CGenum)f;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum1(ResourceAccess acc)
{
    CGenum f = GL_NONE;
    if(feval(acc&(ResourceAccess::ReadOnly|ResourceAccess::Persistent)))
        f = GL_DYNAMIC_READ;
    if(feval(acc&(ResourceAccess::WriteOnly|ResourceAccess::Persistent)))
        f = GL_DYNAMIC_DRAW;
    if(feval(acc&(ResourceAccess::ReadWrite|ResourceAccess::Persistent)))
        f = GL_DYNAMIC_COPY;

    if(f != GL_NONE)
        return f;

    if(feval(acc&(ResourceAccess::ReadOnly|ResourceAccess::Streaming)))
        f = GL_STREAM_READ;
    if(feval(acc&(ResourceAccess::WriteOnly|ResourceAccess::Streaming)))
        f = GL_STREAM_DRAW;
    if(feval(acc&(ResourceAccess::ReadWrite|ResourceAccess::Streaming)))
        f = GL_STREAM_COPY;

    if(f != GL_NONE)
        return f;

    if(feval(acc&(ResourceAccess::ReadOnly)))
        f = GL_STATIC_READ;
    if(feval(acc&(ResourceAccess::WriteOnly)))
        f = GL_STATIC_DRAW;
    if(feval(acc&(ResourceAccess::ReadWrite)))
        f = GL_STATIC_COPY;

    return f;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum2(ResourceAccess acc)
{
    CGenum f = 0;
    if(feval(acc&ResourceAccess::Persistent))
        f |= GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT;
    if(feval(acc&ResourceAccess::ReadOnly))
        f |= GL_MAP_READ_BIT;
    if(feval(acc&ResourceAccess::WriteOnly))
        f |= GL_MAP_WRITE_BIT;
    if(feval(acc&ResourceAccess::ReadWrite))
        f |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    if(feval(acc&ResourceAccess::Streaming))
        f |= GL_DYNAMIC_STORAGE_BIT;
    return f;
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum3(ResourceAccess acc)
{
    switch(acc)
    {
    case ResourceAccess::WriteOnly:
        return GL_WRITE_ONLY;
    case ResourceAccess::ReadOnly:
        return GL_READ_ONLY;
    case ResourceAccess::ReadWrite:
        return GL_READ_WRITE;
    default:
        return GL_NONE;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(PixelComponents f)
{
    switch(f)
    {
    case PixelComponents::R:
        return GL_RED;
    case PixelComponents::G:
        return GL_GREEN;
    case PixelComponents::B:
        return GL_BLUE;
    case PixelComponents::RG:
        return GL_RG;
    case PixelComponents::RGB:
        return GL_RGB;
    case PixelComponents::BGR:
        return GL_BGR;
    case PixelComponents::RGBA:
        return GL_RGBA;
    case PixelComponents::BGRA:
        return GL_BGRA;
    case PixelComponents::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelComponents::DepthStencil:
        return GL_DEPTH_STENCIL;
    case PixelComponents::Stencil:
        return GL_STENCIL;
    }
}

inline CGL_Implementation::CGenum CGL_Implementation::to_enum(TypeEnum f)
{
    switch(f)
    {
    case TypeEnum::Byte:
        return GL_BYTE;
    case TypeEnum::UByte:
        return GL_UNSIGNED_BYTE;

    case TypeEnum::Short:
        return GL_SHORT;
    case TypeEnum::UShort:
        return GL_UNSIGNED_SHORT;

    case TypeEnum::Int:
        return GL_INT;
    case TypeEnum::UInt:
        return GL_UNSIGNED_INT;

    case TypeEnum::BigScalar:
        return GL_DOUBLE;
    case TypeEnum::Scalar:
        return GL_FLOAT;

    default:
        return GL_NONE;
    }
}

inline CGL_Implementation::CGpixfmt CGL_Implementation::get_fmt(PixelFormat e, bool rev)
{
    switch(e)
    {
    case PixelFormat::RGB8UI:
            return {GL_UNSIGNED_BYTE,GL_RGB};
    case PixelFormat::RGB8I:
            return {GL_BYTE,GL_RGB};
    case PixelFormat::RGB16UI:
            return {GL_UNSIGNED_SHORT,GL_RGB};
    case PixelFormat::RGB16I:
            return {GL_SHORT,GL_RGB};
    case PixelFormat::RGB32UI:
            return {GL_UNSIGNED_INT,GL_RGB};
    case PixelFormat::RGB32I:
            return {GL_INT,GL_RGB};

    case PixelFormat::RGBA8UI:
        if(rev)
            return {GL_UNSIGNED_INT_8_8_8_8_REV,GL_RGBA};
        else
            return {GL_UNSIGNED_INT_8_8_8_8,GL_RGBA};
    case PixelFormat::RGBA8I:
            return {GL_BYTE,GL_RGBA};
    case PixelFormat::RGBA16UI:
            return {GL_UNSIGNED_SHORT,GL_RGBA};
    case PixelFormat::RGBA16I:
            return {GL_SHORT,GL_RGBA};
    case PixelFormat::RGBA32UI:
            return {GL_UNSIGNED_INT,GL_RGBA};
    case PixelFormat::RGBA32I:
            return {GL_INT,GL_RGBA};

    case PixelFormat::R3G3B2UI:
        if(rev)
            return {GL_UNSIGNED_BYTE_2_3_3_REV,GL_RGB};
        else
            return {GL_UNSIGNED_BYTE_3_3_2,GL_RGB};
    case PixelFormat::RGB4UI:
        if(rev)
            return {GL_UNSIGNED_SHORT_4_4_4_4_REV,GL_RGB};
        else
            return {GL_UNSIGNED_SHORT_4_4_4_4,GL_RGB};
    case PixelFormat::RGB565UI:
        if(rev)
            return {GL_UNSIGNED_SHORT_5_6_5_REV,GL_RGB};
        else
            return {GL_UNSIGNED_SHORT_5_6_5,GL_RGB};

    case PixelFormat::RGB5A1UI:
        if(rev)
            return {GL_UNSIGNED_SHORT_1_5_5_5_REV,GL_RGBA};
        else
            return {GL_UNSIGNED_SHORT_5_5_5_1,GL_RGBA};
    case PixelFormat::RGB9E5UI:
        if(rev)
            return {GL_UNSIGNED_INT_5_9_9_9_REV,GL_RGBA};
        else
            break;

    case PixelFormat::RGB10A2I:
        if(rev)
            return {GL_INT_2_10_10_10_REV,GL_RGBA};
        else
            break;
    case PixelFormat::RGB10A2UI:
        if(rev)
            return {GL_UNSIGNED_INT_2_10_10_10_REV,GL_RGBA};
        else
            return {GL_UNSIGNED_INT_10_10_10_2,GL_RGBA};
    case PixelFormat::R11G11B10F:
        return {GL_UNSIGNED_INT_10F_11F_11F_REV,GL_RGB};

    case PixelFormat::R32F:
        return {GL_FLOAT,GL_RED};
    case PixelFormat::RG32F:
        return {GL_FLOAT,GL_RG};
    case PixelFormat::RGB32F:
        return {GL_FLOAT,GL_RGB};
    case PixelFormat::RGBA32F:
        return {GL_FLOAT,GL_RGBA};

    default:
        break;
    }
    return {GL_NONE,GL_NONE};
}

}
}

#endif
