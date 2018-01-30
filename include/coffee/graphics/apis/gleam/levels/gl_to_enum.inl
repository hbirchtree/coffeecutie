#pragma once

#include "shared/gl_shared_include.h"
#include "shared/gl_shared_types.h"
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/logicenum.h>

namespace Coffee{
namespace CGL{

inline CGenum to_enum(
        Severity s)
{
    switch(s)
    {
#if !defined(COFFEE_ONLY_GLES20)
    case Severity::High:
        return GL_DEBUG_SEVERITY_HIGH;
    case Severity::Medium:
        return GL_DEBUG_SEVERITY_MEDIUM;
    case Severity::Low:
        return GL_DEBUG_SEVERITY_LOW;
    case Severity::Information:
        return GL_DEBUG_SEVERITY_NOTIFICATION;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(
        DebugType t)
{
    switch(t)
    {
#if !defined(COFFEE_ONLY_GLES20)
    case DebugType::Compatibility:
        return GL_DEBUG_TYPE_PORTABILITY;
    case DebugType::Compliance:
        return GL_DEBUG_TYPE_PORTABILITY;
    case DebugType::Deprecated:
        return GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
    case DebugType::Performance:
        return GL_DEBUG_TYPE_PERFORMANCE;
    case DebugType::Marker:
        return GL_DEBUG_TYPE_MARKER;
    case DebugType::UndefinedBehavior:
        return GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;

    case DebugType::Other:
    case DebugType::Information:
        return GL_DEBUG_TYPE_OTHER;
#else
    default:
        return GL_NONE;
#endif
    }
}

inline CGenum to_enum(
        Object t)
{
    CGenum type;
    switch(t)
    {
#if !defined(COFFEE_ONLY_GLES20)
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
#else
    default:
        return GL_NONE;
#endif
    }
    return type;
}

inline CGenum to_enum(
        Feature f, uint32 offset)
{
    (void)offset;

    switch(f)
    {
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::DepthClamp:
        return GL_DEPTH_CLAMP;
    case Feature::FramebufferSRGB:
        return GL_FRAMEBUFFER_SRGB;
    case Feature::LineSmooth:
        return GL_LINE_SMOOTH;
    case Feature::Multisample:
        return GL_MULTISAMPLE;
    case Feature::PointSize:
        return GL_PROGRAM_POINT_SIZE;
    case Feature::PolygonOffsetLine:
        return GL_POLYGON_OFFSET_LINE;
    case Feature::PolygonOffsetPoint:
        return GL_POLYGON_OFFSET_POINT;
    case Feature::PolygonSmooth:
        return GL_POLYGON_SMOOTH;
    case Feature::PrimitiveRestart:
        return GL_PRIMITIVE_RESTART;
    case Feature::SampleAlphaToOne:
        return GL_SAMPLE_ALPHA_TO_ONE;
    case Feature::SampleShading:
        return GL_SAMPLE_SHADING;
    case Feature::SeamlessCubemap:
        return GL_TEXTURE_CUBE_MAP_SEAMLESS;
    case Feature::ClipDist:
        return GL_CLIP_DISTANCE0+((offset>7) ? 7 : offset);
#endif
#if !defined(COFFEE_ONLY_GLES20)
    case Feature::DebugOutput:
        return GL_DEBUG_OUTPUT;
    case Feature::DebugOutputSync:
        return GL_DEBUG_OUTPUT_SYNCHRONOUS;
    case Feature::PrimitiveRestartFixedIdx:
        return GL_PRIMITIVE_RESTART_FIXED_INDEX;
    case Feature::RasterizerDiscard:
        return GL_RASTERIZER_DISCARD;
    case Feature::SampleMask:
        return GL_SAMPLE_MASK;
#endif
    case Feature::Blend:
        return GL_BLEND;
    case Feature::Culling:
        return GL_CULL_FACE;
    case Feature::DepthTest:
        return GL_DEPTH_TEST;
    case Feature::Dither:
        return GL_DITHER;
    case Feature::PolygonOffsetFill:
        return GL_POLYGON_OFFSET_FILL;
    case Feature::SampleAlphaToCoverage:
        return GL_SAMPLE_ALPHA_TO_COVERAGE;
    case Feature::SampleCoverage:
        return GL_SAMPLE_COVERAGE;
    case Feature::ScissorTest:
        return GL_SCISSOR_TEST;
    case Feature::StencilTest:
        return GL_STENCIL_TEST;
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(
        Face f)
{
    switch(f)
    {
    case Face::Front:
        return GL_FRONT;
    case Face::Back:
        return GL_BACK;
    case Face::Both:
        return GL_FRONT_AND_BACK;
    default:
        return GL_NONE;
    }
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(
        BufBit f)
{
    switch(f)
    {
    case BufBit::Color:
    {
        return GL_COLOR_BUFFER_BIT;
    }
    case BufBit::Depth:
    {
        return GL_DEPTH_BUFFER_BIT;
    }
    case BufBit::Stencil:
    {
        return GL_STENCIL_BUFFER_BIT;
    }
    }
}
#endif

inline CGenum to_enum(
        Prim p,
        PrimCre c)
{
    switch(p)
    {

    /* LINES */

    case Prim::Line:
#ifdef COFFEE_GLEAM_DESKTOP
        if(feval(c, PrimCre::Adjacency|PrimCre::Strip))
            return GL_LINE_STRIP_ADJACENCY;
#endif
        switch(c)
        {
        case PrimCre::Explicit:
            return GL_LINES;
#ifdef COFFEE_GLEAM_DESKTOP
        case PrimCre::Adjacency:
            return GL_LINE_STRIP_ADJACENCY;
#endif
        case PrimCre::Strip:
            return GL_LINE_STRIP;
        case PrimCre::Loop:
            return GL_LINE_LOOP;
        default:
            return GL_NONE;
        }

        /* POINTS */

    case Prim::Point:
        return GL_POINTS;

        /* TRIANGLES */

    case Prim::Triangle:
#ifdef COFFEE_GLEAM_DESKTOP
        if(feval(c&(PrimCre::Adjacency|PrimCre::Strip)))
            return GL_TRIANGLE_STRIP_ADJACENCY;
#endif
        switch(c)
        {
        case PrimCre::Explicit:
            return GL_TRIANGLES;
#ifdef COFFEE_GLEAM_DESKTOP
        case PrimCre::Adjacency:
            return GL_TRIANGLES_ADJACENCY;
#endif
        case PrimCre::Strip:
            return GL_TRIANGLE_STRIP;
        case PrimCre::Fan:
            return GL_TRIANGLE_FAN;
        default:
            return GL_TRIANGLES;
        }

        /* PATCHES */

#ifdef COFFEE_GLEAM_DESKTOP
    case Prim::Patch:
        if(c!=PrimCre::Explicit)
            return GL_NONE;
        else
            return GL_PATCHES;
#endif
    default:
        return GL_NONE;
    }
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(
        DrawMode f)
{
    switch(f)
    {
    case DrawMode::Fill:
        return GL_FILL;
    case DrawMode::Line:
        return GL_LINE;
    case DrawMode::Point:
        return GL_POINT;
    }
}
#endif

inline CGenum to_enum(
        bool pack, PixelOperation f)
{
    switch(f)
    {
    case PixelOperation::Alignment:
        if(pack)
            return GL_PACK_ALIGNMENT;
        else
            return GL_UNPACK_ALIGNMENT;
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelOperation::SwapEndiannes:
        if(pack)
            return GL_PACK_SWAP_BYTES;
        else
            return GL_UNPACK_SWAP_BYTES;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(
        PixelFormat f, PixelFlags e,
        CompFlags d)
{
    using P = PixFmt;

    (void)f;
    (void)e;
    (void)d;

    switch(f)
    {
#ifdef COFFEE_GLEAM_DESKTOP
    case P::ASTC:{
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
            out += 0x0;
            break;
        case ASTC_5x4:
            out += 0x1;
            break;
        case ASTC_5x5:
            out += 0x2;
            break;
        case ASTC_6x5:
            out += 0x3;
            break;
        case ASTC_6x6:
            out += 0x4;
            break;
        case ASTC_8x5:
            out += 0x5;
            break;
        case ASTC_8x6:
            out += 0x6;
            break;
        case ASTC_8x8:
            out += 0x7;
            break;
        case ASTC_10x5:
            out += 0x8;
            break;
        case ASTC_10x6:
            out += 0x9;
            break;
        case ASTC_10x8:
            out += 0xA;
            break;
        case ASTC_10x10:
            out += 0xB;
            break;
        case ASTC_12x10:
            out += 0xC;
            break;
        case ASTC_12x12:
            out += 0xD;
            break;
        default:
            return GL_NONE;
        }
        return out;
    }
    case P::BPTC:
        if(feval(e,PixelFlags::RGBA|PixelFlags::Unormalized))
            return GL_COMPRESSED_RGBA_BPTC_UNORM;
        if(feval(e,PixelFlags::SRGBA|PixelFlags::Unormalized))
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        if(feval(e,PixelFlags::RGB|PixelFlags::FloatingPoint|PixelFlags::Unsigned))
            return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
        if(feval(e,PixelFlags::RGB|PixelFlags::FloatingPoint))
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        return GL_NONE;

    case P::RGTC:
        if(feval(e, PixelFlags::R|PixelFlags::Unsigned))
            return GL_COMPRESSED_RED_RGTC1;
        if(feval(e, PixelFlags::R|PixelFlags::Signed))
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        if(feval(e, PixelFlags::RG|PixelFlags::Unsigned))
            return GL_COMPRESSED_RG_RGTC2;
        if(feval(e, PixelFlags::RG|PixelFlags::Signed))
            return GL_COMPRESSED_SIGNED_RG_RGTC2;
        return GL_NONE;

    case P::S3TC:
        if(feval(e, PixelFlags::RGB)&& d == S3TC_1)
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        if(feval(e, PixelFlags::RGBA)&& d == S3TC_1)
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        if(feval(e, PixelFlags::RGBA)&&d == S3TC_3)
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        if(feval(e, PixelFlags::RGBA)&&d == S3TC_5)
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        return GL_NONE;

    case P::RGBA2:
        return GL_RGBA2;
    case P::R3G3B2:
        return GL_R3_G3_B2;
    case P::RGB4:
        return GL_RGB4;
    case P::RGBA4:
        return GL_RGBA4;
    case P::RGB5:
        return GL_RGB5;
    case P::RGB10:
        return GL_RGB10;
    case P::RGB12:
        return GL_RGB12;
    case P::RGBA12:
        return GL_RGBA12;

    case PixFmt::R16:
        return GL_R16;
    case PixFmt::RG16:
        return GL_RG16;
    case P::RGB16:
        return GL_RGB16;
    case P::RGBA16:
        return GL_RGBA16;
#endif

//    case P::ETC1:
//        return GL_ETC1_RGB8_OES;

    /* Requires to be used with GL_UNSIGNED_INT */
#if !defined(COFFEE_ONLY_GLES20)
    case P::Depth16:
        return GL_DEPTH_COMPONENT16;
    /* Depth/stencil buffers */
    case P::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    case P::Depth32F:
        return GL_DEPTH_COMPONENT32F;
    case P::Depth32FStencil8:
        return GL_DEPTH32F_STENCIL8;

    case P::R8I:
        return GL_R8I;
    case P::R8UI:
        return GL_R8UI;
    case P::R16I:
        return GL_R16I;
    case P::R16UI:
        return GL_R16UI;
    case P::R32I:
        return GL_R32I;
    case P::R32UI:
        return GL_R32UI;
    case P::R16F:
        return GL_R16F;
    case P::R32F:
        return GL_R32F;

    case P::RG8I:
        return GL_RG8I;
    case P::RG8UI:
        return GL_RG8UI;
    case P::RG16I:
        return GL_RG16I;
    case P::RG16UI:
        return GL_RG16UI;
    case P::RG32I:
        return GL_RG32I;
    case P::RG32UI:
        return GL_RG32UI;
    case P::RG16F:
        return GL_RG16F;
    case P::RG32F:
        return GL_RG32F;

    case P::RGB8I:
        return GL_RGB8I;
    case P::RGB8UI:
        return GL_RGB8UI;
    case P::RGB16I:
        return GL_RGB16I;
    case P::RGB16UI:
        return GL_RGB16UI;
    case P::RGB32I:
        return GL_RGB32I;
    case P::RGB32UI:
        return GL_RGB32UI;
    case P::RGB16F:
        return GL_RGB16F;
    case P::RGB32F:
        return GL_RGB32F;

    case P::RGBA8I:
        return GL_RGBA8I;
    case P::RGBA8UI:
        return GL_RGBA8UI;
    case P::RGBA16I:
        return GL_RGBA16I;
    case P::RGBA16UI:
        return GL_RGBA16UI;
    case P::RGBA32I:
        return GL_RGBA32I;
    case P::RGBA32UI:
        return GL_RGBA32UI;
    case P::RGBA16F:
        return GL_RGBA16F;
    case P::RGBA32F:
        return GL_RGBA32F;

    /* Because GL_RGBA8I cannot be used with GL_UNSIGNED_BYTE */
    case P::RGB565:
        return GL_RGB565;
    case P::RGB5A1:
        return GL_RGB5_A1;
    case P::RGB9E5:
        return GL_RGB9_E5;
    case P::RGB10A2:
        return GL_RGB10_A2;
    case P::RGB10A2UI:
        return GL_RGB10_A2UI;

    case PixFmt::R8:
        return GL_R8;
    case PixFmt::RG8:
        return GL_RG8;
    case P::RGB8:
        return GL_RGB8;
    case P::RGBA8:
        return GL_RGBA8;

    case P::R11G11B10F:
        return GL_R11F_G11F_B10F;

#if !defined(COFFEE_DISABLE_SRGB_SUPPORT)
    case P::SRGB8A8:
        return GL_SRGB8_ALPHA8;
    case P::SRGB8:
        return GL_SRGB8;
#endif

#else
    case P::RGBA8:
        return GL_RGBA;
#endif

    default:
        return GL_NONE;
    }
}

inline CGenum to_enum1(
        ShaderStage f)
{
    switch(f)
    {
    case ShaderStage::Vertex:
        return GL_VERTEX_SHADER;
#if !defined(COFFEE_ONLY_GLES20)
    case ShaderStage::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case ShaderStage::TessEval:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderStage::Geometry:
        return GL_GEOMETRY_SHADER;
#endif
    case ShaderStage::Fragment:
        return GL_FRAGMENT_SHADER;
#if !defined(COFFEE_ONLY_GLES20)
    case ShaderStage::Compute:
        return GL_COMPUTE_SHADER;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum2(
        ShaderStage f)
{
    (void)f;

    CGenum o = 0;

#if !defined(COFFEE_ONLY_GLES20)
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

    if(f==ShaderStage::All)
        o = GL_ALL_SHADER_BITS;
#endif

    return o;
}

inline CGenum to_enum(
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

inline CGenum to_enum(
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
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(
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
#if !defined(COFFEE_ONLY_GLES20)
    case Operator::Min:
        return GL_MIN;
    case Operator::Max:
        return GL_MAX;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(Texture f)
{
    return (CGenum)f;
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(LogicOp op)
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
#endif

inline CGenum to_enum(BufType f)
{
    return (CGenum)f;
}

inline CGenum to_enum1(RSCA acc)
{
    CGenum f = GL_NONE;

#if !defined(COFFEE_ONLY_GLES20)
    if(feval(acc, RSCA::ReadOnly|RSCA::Persistent))
        f = GL_DYNAMIC_READ;
    if(feval(acc, RSCA::WriteOnly|RSCA::Persistent))
        f = GL_DYNAMIC_DRAW;
    if(feval(acc, RSCA::ReadWrite|RSCA::Persistent))
        f = GL_DYNAMIC_COPY;

    if(f != GL_NONE)
        return f;

    if(feval(acc, RSCA::ReadOnly|RSCA::Streaming))
        f = GL_STREAM_READ;
    if(feval(acc, RSCA::WriteOnly|RSCA::Streaming))
        f = GL_STREAM_DRAW;
    if(feval(acc, RSCA::ReadWrite|RSCA::Streaming))
        f = GL_STREAM_COPY;

    if(f != GL_NONE)
        return f;

    if(feval(acc&RSCA::ReadOnly))
        f = GL_STATIC_READ;
    if(feval(acc&RSCA::WriteOnly))
        f = GL_STATIC_DRAW;
    if(feval(acc&RSCA::ReadWrite))
        f = GL_STATIC_COPY;
#else
    if(feval(acc&(RSCA::Persistent)))
        f = GL_DYNAMIC_DRAW;
    else if(feval(acc&(RSCA::Streaming)))
        f = GL_STREAM_DRAW;
    else
        f = GL_STATIC_DRAW;
#endif

    return f;
}

inline CGenum to_enum2(RSCA acc)
{
    (void)acc;

    CGenum f = 0;
#ifdef COFFEE_GLEAM_DESKTOP
    if(feval(acc,RSCA::Persistent))
        f |= GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT;
#endif
#if !defined(COFFEE_ONLY_GLES20)
    if(feval(acc,RSCA::ReadOnly))
        f |= GL_MAP_READ_BIT;
    if(feval(acc,RSCA::WriteOnly))
        f |= GL_MAP_WRITE_BIT;
#endif
#ifdef COFFEE_GLEAM_DESKTOP
    if(feval(acc,RSCA::Streaming))
        f |= GL_CLIENT_STORAGE_BIT;
#endif
    return f;
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum3(RSCA acc)
{
    switch(acc)
    {
    case RSCA::WriteOnly:
        return GL_WRITE_ONLY;
    case RSCA::ReadOnly:
        return GL_READ_ONLY;
    case RSCA::ReadWrite:
        return GL_READ_WRITE;
    default:
        return GL_NONE;
    }
}
#endif

inline CGenum to_enum(PixelComponents f, PixFmt hint)
{
#if !defined(COFFEE_ONLY_GLES20)
    PixFlg flags = GetPixSampleType(hint);
#endif

    switch(f)
    {
    case PixelComponents::RGB:
#if !defined(COFFEE_ONLY_GLES20)
        if(feval(flags & (PixFlg::Signed|PixFlg::Unsigned)))
            return GL_RGB_INTEGER;
        else
#endif
            return GL_RGB;
    case PixelComponents::RGBA:
#if !defined(COFFEE_ONLY_GLES20)
        if(feval(flags & (PixFlg::Signed|PixFlg::Unsigned)))
            return GL_RGBA_INTEGER;
        else
#endif
            return GL_RGBA;
#if !defined(COFFEE_ONLY_GLES20)
    case PixelComponents::R:
        if(feval(flags & (PixFlg::Signed|PixFlg::Unsigned)))
            return GL_RED_INTEGER;
        else
            return GL_RED;
    case PixelComponents::G:
        return GL_GREEN;
    case PixelComponents::B:
        return GL_BLUE;
    case PixelComponents::RG:
        if(feval(flags & (PixFlg::Signed|PixFlg::Unsigned)))
            return GL_RG_INTEGER;
        else
            return GL_RG;

    case PixelComponents::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelComponents::DepthStencil:
        return GL_DEPTH_STENCIL;
    case PixelComponents::Stencil:
        return GL_STENCIL;
#else
    case PixCmp::R:
    case PixCmp::G:
    case PixCmp::B:
        return GL_ALPHA;
    case PixCmp::RG:
        return GL_LUMINANCE_ALPHA;
#endif
        /* Endian-flipped formats */
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelComponents::BGR:
        return GL_BGR;
    case PixelComponents::BGRA:
        return GL_BGRA;
#else
    case PixelComponents::BGR:
        return GL_RGB;
    case PixelComponents::BGRA:
        return GL_RGBA;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum_swizz(PixCmp f)
{
    switch(f)
    {
#if !defined(COFFEE_ONLY_GLES20)
    case PixCmp::R:
        return GL_TEXTURE_SWIZZLE_R;
    case PixCmp::G:
        return GL_TEXTURE_SWIZZLE_G;
    case PixCmp::B:
        return GL_TEXTURE_SWIZZLE_B;
    case PixCmp::A:
        return GL_TEXTURE_SWIZZLE_A;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(TypeEnum f)
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
#ifdef COFFEE_GLEAM_DESKTOP
    case TypeEnum::BigScalar:
        return GL_DOUBLE;
#endif
    case TypeEnum::Scalar:
        return GL_FLOAT;

    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(BitFormat f)
{
    switch(f)
    {
    /* Apparently, this is as far as the OpenGL ES 2.0 standard goes */
    case BitFormat::UByte:
        return GL_UNSIGNED_BYTE;
    case BitFormat::UShort_565:
        return GL_UNSIGNED_SHORT_5_6_5;
    case BitFormat::UShort_5551:
        return GL_UNSIGNED_SHORT_5_5_5_1;
    case BitFormat::UShort_4444:
        return GL_UNSIGNED_SHORT_4_4_4_4;

#if !defined(COFFEE_ONLY_GLES20)
    case BitFormat::Byte:
        return GL_BYTE;
    case BitFormat::Short:
        return GL_SHORT;
    case BitFormat::Int:
        return GL_INT;
    case BitFormat::UShort:
        return GL_UNSIGNED_SHORT;
    case BitFormat::UInt:
        return GL_UNSIGNED_INT;

#ifdef COFFEE_GLEAM_DESKTOP
    case BitFormat::UByte_233R:
        return GL_UNSIGNED_BYTE_2_3_3_REV;
    case BitFormat::UByte_332:
        return GL_UNSIGNED_BYTE_3_3_2;
    case BitFormat::UShort_4444R:
        return GL_UNSIGNED_SHORT_4_4_4_4_REV;
    case BitFormat::UShort_565R:
        return GL_UNSIGNED_SHORT_5_6_5_REV;
    case BitFormat::UShort_1555R:
        return GL_UNSIGNED_SHORT_1_5_5_5_REV;
    case BitFormat::UIntR:
        return GL_UNSIGNED_INT_8_8_8_8_REV;
    case BitFormat::UInt_1010102:
        return GL_UNSIGNED_INT_10_10_10_2;
#endif


    case BitFormat::UInt_5999R:
        return GL_UNSIGNED_INT_5_9_9_9_REV;
    case BitFormat::UInt_2101010R:
        return GL_UNSIGNED_INT_2_10_10_10_REV;

    case BitFormat::Scalar_16:
        return GL_HALF_FLOAT;
    case BitFormat::Scalar_32:
        return GL_FLOAT;

    case BitFmt::Scalar_11_11_10:
        return GL_UNSIGNED_INT_10F_11F_11F_REV;

    case BitFormat::UInt24_8:
        return GL_UNSIGNED_INT_24_8;
    case BitFormat::Scalar_32_Int_24_8:
        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
#else
        /* In order to keep compatibility, we fall back to normal format,
         *  and OpenGL ES 2.0 does not support depth+stencil formats. */
    case BitFormat::UInt24_8:
        return GL_UNSIGNED_BYTE;
#endif


    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(QueryT f)
{
    return (CGenum)f;
}

inline CGenum to_enum(FramebufferT f)
{
    switch(f)
    {
#if !defined(COFFEE_ONLY_GLES20)
    case FramebufferT::Draw:
        return GL_DRAW_FRAMEBUFFER;
    case FramebufferT::Read:
        return GL_READ_FRAMEBUFFER;
#else
    case FramebufferT::Draw:
    case FramebufferT::Read:
#endif
    case FramebufferT::All:
        return GL_FRAMEBUFFER;
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(AttribMode f)
{
    return C_CAST<CGenum>(f);
}

inline CGenum to_enum(Filtering p, Filtering s)
{
    switch(p)
    {
    case Filtering::Nearest:
    {
        switch(s)
        {
        case Filtering::Nearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case Filtering::Linear:
            return GL_NEAREST_MIPMAP_LINEAR;
        default:
            return GL_NEAREST;
        }
    }
    case Filtering::Linear:
    {
        switch(s)
        {
        case Filtering::Nearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case Filtering::Linear:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_LINEAR;
        }
    }
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(WrapPolicy p)
{
    switch(p)
    {
    case WrapPolicy::Clamp:
        return GL_CLAMP_TO_EDGE;
    case WrapPolicy::MirrorRepeat:
        return GL_MIRRORED_REPEAT;
    case WrapPolicy::Repeat:
        return GL_REPEAT;
#ifdef COFFEE_GLEAM_DESKTOP
    case WrapPolicy::MirrorClamp:
        return GL_MIRROR_CLAMP_TO_EDGE;
#endif
    default:
        return GL_NONE;
    }
}

inline CGenum to_enum(VertexWinding e)
{
    switch(e)
    {
    case VertexWinding::CCW:
        return GL_CCW;
    case VertexWinding::CW:
        return GL_CW;
    }
}

inline CGenum to_enum1(DBuffers buf)
{
    if(feval(buf,DBuffers::Color))
        return GL_COLOR_ATTACHMENT0;
#if !defined(COFFEE_ONLY_GLES20)
    if(feval(buf,DBuffers::DepthStencil))
        return GL_DEPTH_STENCIL_ATTACHMENT;
#endif
    if(feval(buf,DBuffers::Depth))
        return GL_DEPTH_ATTACHMENT;
    if(feval(buf,DBuffers::Stencil))
        return GL_STENCIL_ATTACHMENT;

    return GL_NONE;
}

inline CGenum to_enum2(DBuffers buf)
{
    if(feval(buf,DBuffers::Color))
        return GL_COLOR_BUFFER_BIT;
    if(feval(buf,DBuffers::DepthStencil))
        return GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT;
    if(feval(buf,DBuffers::Depth))
        return GL_DEPTH_BUFFER_BIT;
    if(feval(buf,DBuffers::Stencil))
        return GL_STENCIL_BUFFER_BIT;

    return GL_NONE;
}

inline uint32 to_enum_shtype(CGenum f)
{
    using namespace ShaderTypes;

    /* We use sdt_* template to allow simpler verification of
     *  these types later on. */

    switch(f)
    {
#if !defined(COFFEE_ONLY_GLES20)
    /* Depth buffer samplers */
    case GL_SAMPLER_2D_SHADOW:
        return sdt_sampf<S2|Depth>::value;
    case GL_SAMPLER_2D_ARRAY_SHADOW:
        return sdt_sampf<S2|Depth>::value;
    case GL_SAMPLER_CUBE_SHADOW:
        return sdt_sampf<SCube|Depth>::value;
#endif
#ifdef COFFEE_GLEAM_DESKTOP
    case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
        return sdt_sampf<SCubeA|Depth>::value;
#endif

    /* Normal samplers */
    case GL_SAMPLER_2D:
        return sdt_sampf<S2>::value;
    case GL_SAMPLER_CUBE:
        return sdt_sampf<SCube>::value;
#if !defined(COFFEE_ONLY_GLES20)
    case GL_SAMPLER_CUBE_MAP_ARRAY:
        return sdt_sampf<SCubeA>::value;
    case GL_SAMPLER_3D:
        return sdt_sampf<S3>::value;
    case GL_SAMPLER_2D_ARRAY:
        return sdt_sampf<S2A>::value;

    case GL_UNSIGNED_INT_SAMPLER_2D:
        return sdt_samp<UInt_t,S2>::value;
    case GL_UNSIGNED_INT_SAMPLER_3D:
        return sdt_samp<UInt_t,S3>::value;
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        return sdt_samp<UInt_t,S2A>::value;
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
        return sdt_samp<UInt_t,SCube>::value;
    case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
        return sdt_samp<UInt_t,SCubeA>::value;

    case GL_INT_SAMPLER_2D:
        return sdt_samp<Int_t,S2>::value;
    case GL_INT_SAMPLER_3D:
        return sdt_samp<Int_t,S3>::value;
    case GL_INT_SAMPLER_2D_ARRAY:
        return sdt_samp<Int_t,S2A>::value;
    case GL_INT_SAMPLER_CUBE:
        return sdt_samp<Int_t,SCube>::value;
    case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
        return sdt_samp<Int_t,SCubeA>::value;
        break;
#endif

    case GL_FLOAT:
        return sdt_uniff<S1>::value;
    case GL_FLOAT_VEC2:
        return sdt_uniff<Vec_d | S2>::value;
    case GL_FLOAT_VEC3:
        return sdt_uniff<Vec_d | S3>::value;
    case GL_FLOAT_VEC4:
        return sdt_uniff<Vec_d | S4>::value;

    case GL_FLOAT_MAT2:
        return sdt_uniff<Mat_d | S2>::value;
    case GL_FLOAT_MAT3:
        return sdt_uniff<Mat_d | S3>::value;
    case GL_FLOAT_MAT4:
        return sdt_uniff<Mat_d | S4>::value;

#ifdef COFFEE_GLEAM_DESKTOP
    case GL_DOUBLE:
        return sdt_unifd<S1>::value;
    case GL_DOUBLE_VEC2:
        return sdt_unifd<Vec_d | S2>::value;
    case GL_DOUBLE_VEC3:
        return sdt_unifd<Vec_d | S3>::value;
    case GL_DOUBLE_VEC4:
        return sdt_unifd<Vec_d | S4>::value;

    case GL_DOUBLE_MAT2:
        return sdt_unifd<Mat_d | S2>::value;
    case GL_DOUBLE_MAT3:
        return sdt_unifd<Mat_d | S3>::value;
    case GL_DOUBLE_MAT4:
        return sdt_unifd<Mat_d | S4>::value;
#endif

    case GL_UNSIGNED_INT:
        return sdt_unifu<S1>::value;
#if !defined(COFFEE_ONLY_GLES20)
    case GL_UNSIGNED_INT_VEC2:
        return sdt_unifu<Vec_d | S2>::value;
    case GL_UNSIGNED_INT_VEC3:
        return sdt_unifu<Vec_d | S3>::value;
    case GL_UNSIGNED_INT_VEC4:
        return sdt_unifu<Vec_d | S4>::value;
#endif

    case GL_INT:
        return sdt_unifi<S1>::value;
    case GL_INT_VEC2:
        return sdt_unifi<Vec_d | S2>::value;
    case GL_INT_VEC3:
        return sdt_unifi<Vec_d | S3>::value;
    case GL_INT_VEC4:
        return sdt_unifi<Vec_d | S4>::value;

    default:
        return ShaderTypes::None;
    }
}

inline CGenum to_enum1(Delay d)
{
    switch(d)
    {
#ifdef COFFEE_GLEAM_DESKTOP
    case Delay::NoWait:
        return GL_QUERY_NO_WAIT;
    default:
        return GL_QUERY_WAIT;
#else
    default:
        return GL_NONE;
#endif
    }
}

}
}
