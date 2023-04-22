#include <coffee/gexxo/gexxo_api.h>

#include <gccore.h>
#include <malloc.h>
#include <ogc/tpl.h>

#define GEXXO_API "gexxo::"

namespace gexxo {

struct Context
{
    RenderTarget defaultFramebuffer;

    libc_types::u32            currentFb   = 0;
    stl_types::Array<void*, 2> framebuffer = {{nullptr, nullptr}};
    GXRModeObj*                rmode       = nullptr;

    void* gxFifo = nullptr;
};

static std::shared_ptr<Context> currentContext;

void initialize()
{
    currentContext = std::make_shared<Context>();

    VIDEO_Init();

    currentContext->rmode = VIDEO_GetPreferredMode(NULL);

    for(int i = 0; i < 2; i++)
        currentContext->framebuffer[i] =
            MEM_K0_TO_K1(SYS_AllocateFramebuffer(currentContext->rmode));

    VIDEO_Configure(currentContext->rmode);
    VIDEO_SetNextFramebuffer(currentContext->framebuffer[0]);

    VIDEO_SetBlack(FALSE);

    VIDEO_Flush();
    VIDEO_WaitVSync();

    if(currentContext->rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    console_init(
        currentContext->framebuffer[0],
        60,
        60,
        currentContext->rmode->fbWidth,
        currentContext->rmode->xfbHeight,
        currentContext->rmode->fbWidth * 2);
}

void infiniteLoop()
{
    while(1)
    {
        VIDEO_WaitVSync();
    }
}

void swapBuffers()
{
    GX_DrawDone();

    GX_CopyDisp(
        currentContext->framebuffer[currentContext->currentFb], GX_TRUE);
    VIDEO_SetNextFramebuffer(
        currentContext->framebuffer[currentContext->currentFb]);

    VIDEO_Flush();
    VIDEO_WaitVSync();

    (++currentContext->currentFb) %= 2;
}

size_2d framebufferSize()
{
    return {currentContext->rmode->fbWidth, currentContext->rmode->efbHeight};
}

RenderTarget* GXAPI::DefaultFramebuffer()
{
    return &currentContext->defaultFramebuffer;
}

GXAPI::API_CONTEXT GXAPI::GetLoadAPI(GXAPI::OPTS const&)
{
    static constexpr libc_types::u32 default_fifo_size = 256 * 1024;

    if(!currentContext)
        initialize();

    currentContext->gxFifo = memalign(32, default_fifo_size);
    memset(currentContext->gxFifo, 0, default_fifo_size);

    GX_Init(currentContext->gxFifo, default_fifo_size);

    GXColor clearColor = {0x0, 0x0, 0x0, 0xff};
    GX_SetCopyClear(clearColor, 0x00ffffff);

    GX_SetViewport(
        0,
        0,
        currentContext->rmode->fbWidth,
        currentContext->rmode->efbHeight,
        0.f,
        1.f);
    auto yscale = GX_GetYScaleFactor(
        currentContext->rmode->efbHeight, currentContext->rmode->xfbHeight);
    auto xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetDispCopySrc(
        0, 0, currentContext->rmode->fbWidth, currentContext->rmode->efbHeight);
    GX_SetDispCopyDst(currentContext->rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(
        currentContext->rmode->aa,
        currentContext->rmode->sample_pattern,
        GX_TRUE,
        currentContext->rmode->vfilter);
    GX_SetFieldMode(
        currentContext->rmode->field_rendering,
        (currentContext->rmode->viHeight ==
         currentContext->rmode->xfbHeight * 2)
            ? GX_ENABLE
            : GX_DISABLE);

    GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    console_init(
        currentContext->framebuffer[0],
        60,
        60,
        currentContext->rmode->fbWidth,
        currentContext->rmode->xfbHeight,
        currentContext->rmode->fbWidth * 2);

    currentContext->defaultFramebuffer.mode = currentContext->rmode;
    currentContext->defaultFramebuffer.framebuffer =
        currentContext->framebuffer[0];
    currentContext->defaultFramebuffer.depth_bits = 24;

    GX_SetCullMode(GX_CULL_NONE);

    return [](bool debugMode) { return true; };
}

void RenderTarget::clear(u32, const Vecf4& inColor)
{
    vec4_u8 newColor = {libc_types::convert_f32<u8>(inColor.r()),
                        libc_types::convert_f32<u8>(inColor.g()),
                        libc_types::convert_f32<u8>(inColor.b()),
                        libc_types::convert_f32<u8>(inColor.a())};

    if(newColor == color)
        return;

    color = newColor;

    GXColor clearColor = {color.r(), color.g(), color.b(), color.a()};
    GX_SetCopyClear(clearColor, depth);
}

void RenderTarget::clear(f32 inDepth)
{
    u32 newDepth = std::numeric_limits<u32>::max();
    newDepth >>= (32 - depth_bits);
    newDepth *= inDepth;

    if(newDepth == depth)
        return;

    depth = newDepth;

    GXColor clearColor = {color.r(), color.g(), color.b(), color.a()};
    GX_SetCopyClear(clearColor, depth);
}

STATICINLINE u32 index_to_attr(u32 idx)
{
    switch(idx)
    {
    case 0:
        return GX_VA_POS;
    case 1:
        return GX_VA_CLR0;
    case 2:
        return GX_VA_CLR1;
    case 3:
        return GX_VA_NRM;
    default:
        break;
    }

    if(idx > 3 && idx < 11)
        return GX_VA_TEX0 + idx - 4;

    Throw(undefined_behavior(GEXXO_API "invalid attribute index"));

    return 0;
}

STATICINLINE u32 type_to_layout(u32 attr, semantic::TypeEnum type, u32 size)
{
    switch(attr)
    {
    case GX_VA_POS:
        return size == 2 ? GX_POS_XY : GX_POS_XYZ;
    case GX_VA_NRM:
        return GX_NRM_XYZ;
    case GX_VA_CLR0:
    case GX_VA_CLR1:
        return size == 3 ? GX_CLR_RGB : GX_CLR_RGBA;
    case GX_VA_TEX0:
    case GX_VA_TEX1:
    case GX_VA_TEX2:
    case GX_VA_TEX3:
    case GX_VA_TEX4:
    case GX_VA_TEX5:
    case GX_VA_TEX6:
    case GX_VA_TEX7:
        return size == 1 ? GX_TEX_S : GX_TEX_ST;
    }

    Throw(undefined_behavior(GEXXO_API "unhandled layout"));

    return 0;
}

STATICINLINE u32 type_to_format(u32 attr, semantic::TypeEnum type, u32 size)
{
    using T = semantic::TypeEnum;

    switch(attr)
    {
    case GX_VA_POS:
    case GX_VA_NRM:
    case GX_VA_TEX0:
    case GX_VA_TEX1:
    case GX_VA_TEX2:
    case GX_VA_TEX3:
    case GX_VA_TEX4:
    case GX_VA_TEX5:
    case GX_VA_TEX6:
    case GX_VA_TEX7:
    {
        switch(type)
        {
        case T::UByte:
            return GX_U8;
        case T::Byte:
            return GX_S8;
        case T::UShort:
            return GX_U16;
        case T::Short:
            return GX_S16;
        case T::Scalar:
            return GX_F32;
        }
        break;
    }
    case GX_VA_CLR0:
    case GX_VA_CLR1:
    {
        switch(type)
        {
        case T::UByte:
            return size == 3 ? GX_RGB8 : GX_RGBA8;
        default:
            break;
        }
        break;
    }
    }

    Throw(undefined_behavior(GEXXO_API "unhandled format"));

    return 0;
}

void VertexDescriptor::bind()
{
    GX_ClearVtxDesc();

    u32 mode = indexBuffer ? GX_INDEX8 : GX_DIRECT;

    for(auto attr : attribs)
    {
        u32 attr_i = index_to_attr(attr.m_idx);

        GX_SetArray(attr_i, nullptr, attr.m_stride);
        GX_SetVtxDesc(attr_i, mode);
        GX_SetVtxAttrFmt(
            GX_VTXFMT0,
            attr_i,
            type_to_layout(attr_i, attr.m_type, attr.m_size),
            type_to_format(attr_i, attr.m_type, attr.m_size),
            0);

        auto buffer = buffers.find(attr.m_bassoc);

        if(buffer == buffers.end())
            continue;

        auto& buf = buffer->second->data;

        GX_SetArray(attr_i, buf.data + attr.m_off, attr.m_stride);
    }
}

void Pipeline::bind()
{
    for(auto const& shader : shaders)
    {
        std::string variant = shader.second->variant
                ? shader.second->variant
                : "nullptr";

        switch(shader.second->stage)
        {
        case typing::graphics::ShaderStage::Vertex:
        {
            if(variant == "matrix_transform")
                continue;

            break;
        }
        case typing::graphics::ShaderStage::Fragment:
        {
            if(variant == "vertex_color")
            {
                GX_SetNumChans(1);
                GX_SetNumTexGens(0);

                GX_SetTevOrder(
                    GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
                GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

                continue;
            }

            break;
        }
        default:
            break;
        }

        Throw(undefined_behavior(
            std::string(GEXXO_API "unhandled shader variant: ") +
            shader.second->variant));
    }
}

STATICINLINE u32 call_to_primitive(GXAPI::D_CALL const& call)
{
    using Prim = typing::graphics::primitive::base;
    using Cre  = typing::graphics::primitive::creation;

    switch(call.primitive())
    {
    case Prim::Triangle:
    {
        switch(call.primitiveMode())
        {
        case Cre::Explicit:
            return GX_TRIANGLES;
        case Cre::Strip:
            return GX_TRIANGLESTRIP;
        case Cre::Fan:
            return GX_TRIANGLEFAN;
        default:
            break;
        }
        break;
    }
    case Prim::Line:
    {
        switch(call.primitiveMode())
        {
        case Cre::Explicit:
            return GX_LINES;
        case Cre::Strip:
            return GX_LINESTRIP;
        default:
            break;
        }
        break;
    }
    case Prim::Point:
        return GX_POINTS;
    }

    Throw(undefined_behavior(GEXXO_API "unhandled primitive"));

    return 0;
}

static void set_attribs_x8(
    stl_types::Vector<VertexAttrib> const& attribs, u16 idx)
{
    for(auto const& attr : attribs)
    {
        switch(index_to_attr(attr.m_idx))
        {
        case GX_VA_POS:
            GX_Position1x8(idx);
            break;
        case GX_VA_CLR0:
            GX_Color1x8(idx);
            break;
        case GX_VA_NRM:
            GX_Normal1x8(idx);
            break;
        case GX_VA_TEX0:
        case GX_VA_TEX1:
        case GX_VA_TEX2:
        case GX_VA_TEX3:
        case GX_VA_TEX4:
        case GX_VA_TEX5:
        case GX_VA_TEX6:
        case GX_VA_TEX7:
            GX_TexCoord1x8(idx);
            break;
        default:
            break;
        }
    }
}

static void set_attribs_x16(
    stl_types::Vector<VertexAttrib> const& attribs, u16 idx)
{
    for(auto const& attr : attribs)
    {
        switch(index_to_attr(attr.m_idx))
        {
        case GX_VA_POS:
            GX_Position1x16(idx);
            break;
        case GX_VA_CLR0:
            GX_Color1x16(idx);
            break;
        case GX_VA_NRM:
            GX_Normal1x16(idx);
            break;
        case GX_VA_TEX0:
        case GX_VA_TEX1:
        case GX_VA_TEX2:
        case GX_VA_TEX3:
        case GX_VA_TEX4:
        case GX_VA_TEX5:
        case GX_VA_TEX6:
        case GX_VA_TEX7:
            GX_TexCoord1x16(idx);
            break;
        default:
            break;
        }
    }
}

void GXAPI::Draw(
    GXAPI::PIP const&,
    GXAPI::PSTATE const&,
    GXAPI::V_DESC&       desc,
    GXAPI::D_CALL const& call,
    GXAPI::D_DATA const& data)
{
    using T = semantic::TypeEnum;

    desc.bind();

    auto primitive = call_to_primitive(call);

    if(desc.indexBuffer)
    {
        GX_Begin(primitive, GX_VTXFMT0, data.elements());

        auto indexMap =
            data.elementType() == T::UByte ? set_attribs_x8 : set_attribs_x16;

        auto indexMask   = data.elementType() == T::UByte ? 0xFF : 0xFFFF;
        auto indexStride = data.elementType() == T::UByte ? 1 : 2;

        auto baseIndex = *desc.indexBuffer->data.at(data.indexOffset());

        for(auto i : stl_types::Range<>(data.elements()))
        {
            u16 idx = (*baseIndex.at(i * indexStride)).as<u16 const>()[0];
            indexMap(desc.attribs, idx);
        }
    } else
    {
        GX_Begin(primitive, GX_VTXFMT0, data.vertices());

        for(auto i : stl_types::Range<>(data.vertices()))
        {
            for(auto const& attr : desc.attribs)
            {
                auto buf_it = desc.buffers.find(attr.m_bassoc);

                if(buf_it == desc.buffers.end())
                    continue;

                auto buf = *buf_it->second->data.at(
                    attr.m_boffset + i * attr.m_stride + attr.m_off);

                auto buf_u8  = buf.as<u8>();
                auto buf_s8  = buf.as<i8>();
                auto buf_u16 = buf.as<u16>();
                auto buf_s16 = buf.as<i16>();
                auto buf_f32 = buf.as<f32>();

                switch(index_to_attr(attr.m_idx))
                {
                case GX_VA_POS:
                {
                    switch(attr.m_size)
                    {
                    case 2:
                    {
                        switch(attr.m_type)
                        {
                        case T::Byte:
                            GX_Position2s8(buf_s8[0], buf_s8[1]);
                            break;
                        case T::UByte:
                            GX_Position2u8(buf_u8[0], buf_u8[1]);
                            break;
                        case T::Short:
                            GX_Position2s16(buf_s16[0], buf_s16[1]);
                            break;
                        case T::UShort:
                            GX_Position2u16(buf_u16[0], buf_u16[1]);
                            break;
                        case T::Scalar:
                            GX_Position2f32(buf_f32[0], buf_f32[1]);
                            break;
                        }
                        break;
                    }
                    case 3:
                    {
                        switch(attr.m_type)
                        {
                        case T::Byte:
                            GX_Position3s8(buf_s8[0], buf_s8[1], buf_s8[2]);
                            break;
                        case T::UByte:
                            GX_Position3u8(buf_u8[0], buf_u8[1], buf_u8[2]);
                            break;
                        case T::Short:
                            GX_Position3s16(buf_s16[0], buf_s16[1], buf_s16[2]);
                            break;
                        case T::UShort:
                            GX_Position3u16(buf_u16[0], buf_u16[1], buf_u16[2]);
                            break;
                        case T::Scalar:
                            GX_Position3f32(buf_f32[0], buf_f32[1], buf_f32[2]);
                            break;
                        }
                        break;
                    }
                    }
                    break;
                }
                case GX_VA_NRM:
                {
                    switch(attr.m_type)
                    {
                    case T::Byte:
                        GX_Normal3s8(buf_s8[0], buf_s8[1], buf_s8[2]);
                        break;
                    case T::Short:
                        GX_Normal3s16(buf_s16[0], buf_s16[1], buf_s16[2]);
                        break;
                    case T::Scalar:
                        GX_Normal3f32(buf_f32[0], buf_f32[1], buf_f32[2]);
                        break;
                    }

                    break;
                }
                case GX_VA_CLR0:
                case GX_VA_CLR1:
                {
                    switch(attr.m_size)
                    {
                    case 3:
                    {
                        GX_Color3u8(buf_u8[0], buf_u8[1], buf_u8[2]);
                        break;
                    }
                    case 4:
                    {
                        GX_Color4u8(buf_u8[0], buf_u8[1], buf_u8[2], buf_u8[3]);
                        break;
                    }
                    }

                    break;
                }
                case GX_VA_TEX0:
                case GX_VA_TEX1:
                case GX_VA_TEX2:
                case GX_VA_TEX3:
                case GX_VA_TEX4:
                case GX_VA_TEX5:
                case GX_VA_TEX6:
                case GX_VA_TEX7:
                {
                    switch(attr.m_size)
                    {
                    case 1:
                    {
                        switch(attr.m_type)
                        {
                        case T::Byte:
                            GX_TexCoord1s8(buf_s8[0]);
                            break;
                        case T::UByte:
                            GX_TexCoord1u8(buf_u8[0]);
                            break;
                        case T::Short:
                            GX_TexCoord1s16(buf_s16[0]);
                            break;
                        case T::UShort:
                            GX_TexCoord1u16(buf_u16[0]);
                            break;
                        case T::Scalar:
                            GX_TexCoord1f32(buf_f32[0]);
                            break;
                        }
                        break;
                    }
                    case 2:
                    {
                        switch(attr.m_type)
                        {
                        case T::Byte:
                            GX_TexCoord2s8(buf_s8[0], buf_s8[1]);
                            break;
                        case T::UByte:
                            GX_TexCoord2u8(buf_u8[0], buf_u8[1]);
                            break;
                        case T::Short:
                            GX_TexCoord2s16(buf_s16[0], buf_s16[1]);
                            break;
                        case T::UShort:
                            GX_TexCoord2u16(buf_u16[0], buf_u16[1]);
                            break;
                        case T::Scalar:
                            GX_TexCoord2f32(buf_f32[0], buf_f32[1]);
                            break;
                        }
                        break;
                    }
                    }

                    break;
                }
                }
            }
        }
    }

    GX_End();
}

} // namespace gexxo
