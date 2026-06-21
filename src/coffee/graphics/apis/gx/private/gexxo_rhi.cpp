#include <coffee/graphics/apis/gexxo/rhi.h>

#include <coffee/comp_app/services.h>
#include <coffee/gexxo/gexxo_api.h>

#include <gccore.h>
#include <ogc/gu.h>

namespace gexxo {

/* --- rendertarget_t : the clear path -------------------------------------- */

namespace {

inline libc_types::u8 to_u8(f32 v)
{
    if(v < 0.f)
        v = 0.f;
    if(v > 1.f)
        v = 1.f;
    return static_cast<libc_types::u8>(v * 255.f);
}

void apply_copy_clear(Vecf4 const& color, f64 depth)
{
    GXColor c = {to_u8(color[0]), to_u8(color[1]), to_u8(color[2]),
                 to_u8(color[3])};
    if(depth < 0.0)
        depth = 0.0;
    if(depth > 1.0)
        depth = 1.0;
    u32 z = static_cast<u32>(depth * static_cast<f64>(0x00ffffff));
    /* The clear is applied during the EFB->XFB copy (GX_CopyDisp at swap). */
    GX_SetCopyClear(c, z);
}

} // namespace

void rendertarget_t::clear(Vecf4 const& color, u32 /*i*/)
{
    m_clear_color = color;
    apply_copy_clear(m_clear_color, m_clear_depth);
}

void rendertarget_t::clear(f64 depth)
{
    m_clear_depth = depth;
    apply_copy_clear(m_clear_color, m_clear_depth);
}

void rendertarget_t::clear(i32 /*stencil*/)
{
    /* No separate stencil clear on the GX copy pipeline. */
}

void rendertarget_t::clear(
    Vecf4 const& color, f64 depth, i32 /*stencil*/, u32 /*i*/)
{
    m_clear_color = color;
    m_clear_depth = depth;
    apply_copy_clear(m_clear_color, m_clear_depth);
}

void rendertarget_t::resize(rect<i32> const& size, u32)
{
    m_size = size;
}

size_2d<i32> rendertarget_t::size()
{
    auto fb = gexxo::framebufferSize();
    return {fb.w, fb.h};
}

/* --- api : lifecycle + introspection -------------------------------------- */

std::optional<error> api::load(load_options_t /*options*/)
{
    gexxo::gxInitialize();
    return std::nullopt;
}

void api::unload()
{
}

std::tuple<u32, u32> api::query_native_version()
{
    return {1, 0};
}

extensions_set api::query_native_extensions()
{
    return {};
}

api_type_t api::query_native_api()
{
    return api_type_t::none;
}

std::string_view api::api_name()
{
    return "GX";
}

api_type_t api::api_type()
{
    return api_type_t::none;
}

std::tuple<u32, u32> api::api_version()
{
    return {1, 0};
}

std::string api::shaderlang_name()
{
    return "TEV";
}

api_type_t api::shaderlang_type()
{
    return api_type_t::none;
}

std::tuple<u32, u32> api::shaderlang_version()
{
    return {1, 0};
}

std::tuple<std::string, std::string> api::device()
{
    return {"Nintendo", "GameCube (Flipper)"};
}

std::optional<std::string> api::device_driver()
{
    return std::string("libogc GX");
}

extensions_set api::extensions()
{
    return {};
}

void api::collect_info(comp_app::interfaces::AppInfo& appInfo)
{
    appInfo.add("graphics:api", "GX");
    appInfo.add("graphics:device", "Nintendo GameCube (Flipper)");
}

namespace detail {

static void glm_to_mtx(Matf4 const& m, Mtx out)
{
    auto const* g = reinterpret_cast<f32 const*>(&m);
    for(int r = 0; r < 3; r++)
        for(int c = 0; c < 4; c++)
            out[r][c] = g[c * 4 + r];
}

void gexxo_draw(draw_command const& cmd)
{
    auto vao = cmd.vertices;
    if(!vao || vao->m_attributes.empty())
        return;

    auto const& pos = vao->m_attributes[0];
    if(pos.buffer.id >= vao->m_vertex_buffers.size())
        return;
    auto buf = vao->m_vertex_buffers[pos.buffer.id];
    if(!buf || buf->m_data.empty())
        return;

    u32 const count = cmd.data.arrays.count;
    if(count == 0)
        return;

    auto const stride =
        pos.value.stride ? pos.value.stride : sizeof(f32) * pos.value.count;
    auto const base = pos.value.offset;

    auto const& xforms        = cmd.instance_transforms;
    bool const  has_transform = !xforms.empty();

    Mtx44 proj;
    if(has_transform)
    {
        // TODO: Make perspective params configurable
        guPerspective(proj, 90.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        GX_LoadProjectionMtx(proj, GX_PERSPECTIVE);
    } else
    {
        guOrtho(proj, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
        GX_LoadProjectionMtx(proj, GX_ORTHOGRAPHIC);
        Mtx mv;
        guMtxIdentity(mv);
        guMtxTransApply(mv, mv, 0.0f, 0.0f, -5.0f);
        GX_LoadPosMtxImm(mv, GX_PNMTX0);
    }

    GX_SetNumChans(1);
    GX_SetChanCtrl(
        GX_COLOR0A0,
        GX_DISABLE,
        GX_SRC_REG,
        GX_SRC_VTX,
        0,
        GX_DF_NONE,
        GX_AF_NONE);
    GX_SetNumTexGens(0);
    GX_SetNumTevStages(1);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(
        GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    static const u8 palette[4][3] = {
        {0x40, 0xE0, 0x40}, /* green */
        {0xE0, 0x40, 0x40}, /* red   */
        {0x40, 0x80, 0xE0}, /* blue  */
        {0xE0, 0xE0, 0x40}, /* yellow*/
    };

    u32 const instances =
        (cmd.call.instanced && has_transform)
            ? static_cast<u32>(xforms.size())
            : 1;

    for(u32 inst = 0; inst < instances; inst++)
    {
        if(has_transform)
        {
            Mtx mv;
            glm_to_mtx(xforms[inst], mv); /* xforms are modelview already */
            GX_LoadPosMtxImm(mv, GX_PNMTX0);
        }

        auto const& col = palette[inst % 4];

        GX_Begin(GX_TRIANGLES, GX_VTXFMT0, static_cast<u16>(count));
        for(u32 i = 0; i < count; i++)
        {
            auto const* p = reinterpret_cast<f32 const*>(
                buf->m_data.data() + base + i * stride);
            GX_Position3f32(p[0], p[1], 0.0f);
            GX_Color4u8(col[0], col[1], col[2], 0xFF);
        }
        GX_End();
    }
}

} // namespace detail

} // namespace gexxo
