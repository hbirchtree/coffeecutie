#include <coffee/gexxo/gexxo_api.h>

#include <gccore.h>
#include <malloc.h>
#include <ogc/tpl.h>

namespace gexxo {

struct Context
{
    RenderTarget defaultFramebuffer;

    libc_types::u32            currentFb   = 0;
    stl_types::Array<void*, 2> framebuffer = {{nullptr, nullptr}};
    GXRModeObj*                rmode       = nullptr;

    void* gxFifo = nullptr;
};

static stl_types::ShPtr<Context> currentContext;

void initialize()
{
    currentContext = stl_types::MkShared<Context>();

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

    Throw(undefined_behavior("invalid attribute index"));
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
}

void VertexDescriptor::bind()
{
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
            0,
            0);
    }
}

void VertexDescriptor::bindBuffer(u32 i, VertexBuffer& buf)
{
}

void VertexDescriptor::addAttribute(VertexAttrib const& attr)
{
}

void VertexDescriptor::setIndexBuffer(ElementBuffer const* buf)
{
}

} // namespace gexxo
