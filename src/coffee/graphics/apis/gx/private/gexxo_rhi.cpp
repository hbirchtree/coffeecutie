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
    GXColor c = {
        to_u8(color[0]), to_u8(color[1]), to_u8(color[2]), to_u8(color[3])};
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
    return "ATI Flipper TEV";
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
    return std::string("GX");
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

} // namespace gexxo
