#include <glscreenshot/screenshot.h>

#include <coffee/core/CProfiling>
#include <glw/glw.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>

namespace glscreenshot {

using namespace gl::group;

using glw =
    std::conditional<
        gl::core::enabled,
        gl::core::v33,
#if GLEAM_MAX_VERSION_ES >= 0x300
        gl::es::v30
#else
        gl::es::v20
#endif
>::type;

comp_app::size_2d_t ScreenshotProvider::size() const
{
    stl_types::Array<GLint, 4> tmp{};
    glw::get_integerv(get_prop::viewport, semantic::Span<GLint>(tmp));
    return {tmp[2], tmp[3]};
}

semantic::mem_chunk<typing::pixels::rgb_t> ScreenshotProvider::pixels() const
{
    using namespace ::typing::vector_types;

    Coffee::DProfContext _("glscreenshot::ScreenshotProvider::pixels");

    auto size_ = size();
    auto out =
        semantic::mem_chunk<typing::pixels::rgb_t>::withSize(size_.area());
    i32 currentBinding = 0;
    glw::get_integerv(
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
        gl::group::get_prop::draw_framebuffer_binding,
#else
        static_cast<gl::group::get_prop>(GL_FRAMEBUFFER_BINDING),
#endif
        semantic::SpanOne(currentBinding));
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
    glw::bind_framebuffer(gl::group::framebuffer_target::draw_framebuffer, 0);
    glw::read_buffer(gl::group::read_buffer_mode::back);
#else
    glw::bind_framebuffer(gl::group::framebuffer_target::framebuffer, 0);
#endif
    glw::read_pixels(
        Veci2{0, 0},
        typing::geometry::size_2d<i32>{size_.w, size_.h},
        pixel_format::rgb,
        pixel_type::unsigned_byte,
        out.as<std::byte>().view);
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
    glw::bind_framebuffer(
        gl::group::framebuffer_target::draw_framebuffer, currentBinding);
#else
    glw::bind_framebuffer(
        gl::group::framebuffer_target::framebuffer, currentBinding);
#endif
    return out;
}

} // namespace glscreenshot
