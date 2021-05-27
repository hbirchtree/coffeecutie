#include <glscreenshot/screenshot.h>

#include <coffee/core/CProfiling>
#include <glw/glw.h>
#include <platforms/types/vector_types.h>

namespace glscreenshot {

using namespace gl::groups;

comp_app::size_2d_t ScreenshotProvider::size() const
{
    stl_types::Array<GLint, 4> tmp;
    gl::v33::get_integerv<gl::Version<3, 3>>(get_prop::viewport, tmp);
    return {tmp[2], tmp[3]};
}

semantic::mem_chunk<typing::pixels::rgb_t> ScreenshotProvider::pixels() const
{
    using namespace ::typing::vector_types;

    Coffee::DProfContext _("glscreenshot::ScreenshotProvider::pixels");

    auto size_ = size();
    auto out =
        semantic::mem_chunk<typing::pixels::rgb_t>::withSize(size_.area());
    gl::v33::read_pixels<gl::Version<3, 3>>(
        Veci2{0, 0},
        Veci2{size_.w, size_.h},
        pixel_format::rgb,
        pixel_type::unsigned_byte,
        out.as<std::byte>().view);
    //    glwrap::baseline<glwrap::v::baseline>::ReadPixels(
    //        {0, 0},
    //        size_,
    //        typing::PixCmp::RGB,
    //        typing::pixels::BitFmt::UByte,
    //        out.data);
    return out;
}

} // namespace glscreenshot
