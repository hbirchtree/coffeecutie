#pragma once

#include "../gleam.h"
#include <glad_es/glad.h>
#include <glad_es/KHR/khrplatform.h>
#include <coffee/core/types/basetypes.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include "desktop/glbase.h"
#else
#include "es/glbase.h"
#endif

namespace Coffee{
namespace CGL{

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(
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
#endif

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(
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
#endif

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(
        Object t)
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
#endif

inline CGenum to_enum(
        Feature f, uint32 offset)
{
    switch(f)
    {
    case Feature::Blend:
        return GL_BLEND;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::DebugOutput:
        return GL_DEBUG_OUTPUT;
    case Feature::DebugOutputSync:
        return GL_DEBUG_OUTPUT_SYNCHRONOUS;
#endif
    case Feature::DepthTest:
        return GL_DEPTH_TEST;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::DepthClamp:
        return GL_DEPTH_CLAMP;
#endif
    case Feature::Dither:
        return GL_DITHER;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::FramebufferSRGB:
        return GL_FRAMEBUFFER_SRGB;
    case Feature::LineSmooth:
        return GL_LINE_SMOOTH;
    case Feature::Multisample:
        return GL_MULTISAMPLE;
    case Feature::PointSize:
        return GL_PROGRAM_POINT_SIZE;
#endif
    case Feature::PolygonOffsetFill:
        return GL_POLYGON_OFFSET_FILL;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::PolygonOffsetLine:
        return GL_POLYGON_OFFSET_LINE;
    case Feature::PolygonOffsetPoint:
        return GL_POLYGON_OFFSET_POINT;
    case Feature::PolygonSmooth:
        return GL_POLYGON_SMOOTH;
    case Feature::PrimitiveRestart:
        return GL_PRIMITIVE_RESTART;
#endif
    case Feature::PrimitiveRestartFixedIdx:
        return GL_PRIMITIVE_RESTART_FIXED_INDEX;
    case Feature::RasterizerDiscard:
        return GL_RASTERIZER_DISCARD;
    case Feature::SampleAlphaToCoverage:
        return GL_SAMPLE_ALPHA_TO_COVERAGE;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::SampleAlphaToOne:
        return GL_SAMPLE_ALPHA_TO_ONE;
#endif
    case Feature::SampleCoverage:
        return GL_SAMPLE_COVERAGE;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::SampleMask:
        return GL_SAMPLE_MASK;
    case Feature::SampleShading:
        return GL_SAMPLE_SHADING;
#endif
    case Feature::ScissorTest:
        return GL_SCISSOR_TEST;
    case Feature::StencilTest:
        return GL_STENCIL_TEST;
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::SeamlessCubemap:
        return GL_TEXTURE_CUBE_MAP_SEAMLESS;
#endif
#ifdef COFFEE_GLEAM_DESKTOP
    case Feature::ClipDist:
        return GL_CLIP_DISTANCE0+offset;
#endif
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
    case Prim::Line:
#ifdef COFFEE_GLEAM_DESKTOP
        if(feval(c&(PrimCre::Adjacency|PrimCre::Strip)))
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
    case Prim::Point:
        return GL_POINTS;
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
    switch(f)
    {
#ifdef COFFEE_GLEAM_DESKTOP
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
#endif
#ifdef COFFEE_GLEAM_DESKTOP
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
	if(feval(e&(PixelFlags::R|PixelFlags::Unsigned)))
            return GL_COMPRESSED_RED_RGTC1;
	if(feval(e&(PixelFlags::R|PixelFlags::Signed)))
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
	if(feval(e&(PixelFlags::RG|PixelFlags::Unsigned)))
            return GL_COMPRESSED_RG_RGTC2;
	if(feval(e&(PixelFlags::RG|PixelFlags::Signed)))
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
#endif

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

#ifdef COFFEE_GLEAM_DESKTOP
    case PixelFormat::R3G3B2UI:
        return GL_R3_G3_B2;
    case PixelFormat::RGB4UI:
        return GL_RGB4;
    case PixelFormat::RGB5UI:
        return GL_RGB5;
#endif
    case PixelFormat::RGB565UI:
        return GL_RGB565;
    case PixelFormat::RGB9E5UI:
        return GL_RGB9_E5;
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelFormat::RGB10:
        return GL_RGB10;
    case PixelFormat::RGB12:
        return GL_RGB12;
#endif
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelFormat::RGBA2:
        return GL_RGBA2;
#endif
    case PixelFormat::RGB10A2I:
        return GL_RGB10_A2;
    case PixelFormat::RGB10A2UI:
        return GL_RGB10_A2UI;
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelFormat::RGBA12:
        return GL_RGBA12;
#endif
    case PixelFormat::RGB5A1UI:
        return GL_RGB5_A1;

    case PixelFormat::SRGB8A8:
        return GL_SRGB8_ALPHA8;
    case PixelFormat::SRGB8:
        return GL_SRGB8;
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
#ifdef COFFEE_GLEAM_DESKTOP
    case ShaderStage::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case ShaderStage::TessEval:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderStage::Geometry:
        return GL_GEOMETRY_SHADER;
#endif
    case ShaderStage::Fragment:
        return GL_FRAGMENT_SHADER;
#ifdef COFFEE_GLEAM_DESKTOP
    case ShaderStage::Compute:
        return GL_COMPUTE_SHADER;
#endif
    default:
        return GL_NONE;
    }
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum2(
        ShaderStage f)
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

    if(f==ShaderStage::All)
        o = GL_ALL_SHADER_BITS;

    return o;
}
#endif

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
    case Operator::Min:
        return GL_MIN;
    case Operator::Max:
        return GL_MAX;
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

inline CGenum to_enum1(ResourceAccess acc)
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

inline CGenum to_enum2(ResourceAccess acc)
{
    CGenum f = 0;
#ifdef COFFEE_GLEAM_DESKTOP
    if(feval(acc&ResourceAccess::Persistent))
        f |= GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT;
#endif
    if(feval(acc&ResourceAccess::ReadOnly))
        f |= GL_MAP_READ_BIT;
    if(feval(acc&ResourceAccess::WriteOnly))
        f |= GL_MAP_WRITE_BIT;
    if(feval(acc&ResourceAccess::ReadWrite))
        f |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
#ifdef COFFEE_GLEAM_DESKTOP
    if(feval(acc&ResourceAccess::Streaming))
        f |= GL_DYNAMIC_STORAGE_BIT;
#endif
    return f;
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum3(ResourceAccess acc)
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
#endif

inline CGenum to_enum(PixelComponents f)
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
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelComponents::BGR:
        return GL_BGR;
#endif
    case PixelComponents::RGBA:
        return GL_RGBA;
#ifdef COFFEE_GLEAM_DESKTOP
    case PixelComponents::BGRA:
        return GL_BGRA;
#endif
    case PixelComponents::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelComponents::DepthStencil:
        return GL_DEPTH_STENCIL;
    case PixelComponents::Stencil:
        return GL_STENCIL;
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
    case BitFormat::Byte:
        return GL_BYTE;
    case BitFormat::Short:
        return GL_SHORT;
    case BitFormat::Int:
        return GL_INT;

    case BitFormat::UByte:
        return GL_UNSIGNED_BYTE;
#ifdef COFFEE_GLEAM_DESKTOP
    case BitFormat::UByte_233R:
        return GL_UNSIGNED_BYTE_2_3_3_REV;
    case BitFormat::UByte_332:
        return GL_UNSIGNED_BYTE_3_3_2;
#endif

    case BitFormat::UShort:
        return GL_UNSIGNED_SHORT;
    case BitFormat::UShort_565:
        return GL_UNSIGNED_SHORT_5_6_5;
#ifdef COFFEE_GLEAM_DESKTOP
    case BitFormat::UShort_565R:
        return GL_UNSIGNED_SHORT_5_6_5_REV;
    case BitFormat::UShort_1555R:
        return GL_UNSIGNED_SHORT_1_5_5_5_REV;
#endif
    case BitFormat::UShort_5551:
        return GL_UNSIGNED_SHORT_5_5_5_1;

    case BitFormat::UInt:
        return GL_UNSIGNED_INT;
    case BitFormat::UInt_5999R:
        return GL_UNSIGNED_INT_5_9_9_9_REV;
#ifdef COFFEE_GLEAM_DESKTOP
    case BitFormat::UInt_1010102:
        return GL_UNSIGNED_INT_10_10_10_2;
#endif
    case BitFormat::UInt_2101010R:
        return GL_UNSIGNED_INT_2_10_10_10_REV;

    case BitFormat::Scalar_16:
        return GL_HALF_FLOAT;
    case BitFormat::Scalar_32:
        return GL_FLOAT;
    default:
        return GL_NONE;
    }
}

#ifdef COFFEE_GLEAM_DESKTOP
inline CGenum to_enum(QueryT f)
{
    return (CGenum)f;
}
#endif

inline CGenum to_enum(FramebufferT f)
{
    switch(f)
    {
    case FramebufferT::Draw:
        return GL_DRAW_FRAMEBUFFER;
    case FramebufferT::Read:
        return GL_READ_FRAMEBUFFER;
    case FramebufferT::All:
        return GL_FRAMEBUFFER;
    }
}

inline CGenum to_enum(AttribMode f)
{
    return (CGenum)f;
}

inline CGpixfmt get_fmt(PixelFormat e, bool rev)
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
#ifdef COFFEE_GLEAM_DESKTOP
        if(rev)
            return {GL_UNSIGNED_INT_8_8_8_8_REV,GL_RGBA};
        else
            return {GL_UNSIGNED_INT_8_8_8_8,GL_RGBA};
#endif
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

#ifdef COFFEE_GLEAM_DESKTOP
    case PixelFormat::R3G3B2UI:
        if(rev)
            return {GL_UNSIGNED_BYTE_2_3_3_REV,GL_RGB};
        else
            return {GL_UNSIGNED_BYTE_3_3_2,GL_RGB};
#endif
    case PixelFormat::RGB4UI:
#ifdef COFFEE_GLEAM_DESKTOP
        if(rev)
            return {GL_UNSIGNED_SHORT_4_4_4_4_REV,GL_RGB};
        else
#endif
            return {GL_UNSIGNED_SHORT_4_4_4_4,GL_RGB};
    case PixelFormat::RGB565UI:
#ifdef COFFEE_GLEAM_DESKTOP
        if(rev)
            return {GL_UNSIGNED_SHORT_5_6_5_REV,GL_RGB};
        else
#endif
            return {GL_UNSIGNED_SHORT_5_6_5,GL_RGB};

    case PixelFormat::RGB5A1UI:
#ifdef COFFEE_GLEAM_DESKTOP
        if(rev)
            return {GL_UNSIGNED_SHORT_1_5_5_5_REV,GL_RGBA};
        else
#endif
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
#ifdef COFFEE_GLEAM_DESKTOP
        if(rev)
#endif
            return {GL_UNSIGNED_INT_2_10_10_10_REV,GL_RGBA};
#ifdef COFFEE_GLEAM_DESKTOP
        else
            return {GL_UNSIGNED_INT_10_10_10_2,GL_RGBA};
#endif
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
