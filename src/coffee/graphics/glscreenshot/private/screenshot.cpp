#include <glscreenshot/screenshot.h>

#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/levels/all_levels.h>

namespace glscreenshot {

comp_app::size_2d_t ScreenshotProvider::size() const
{
    stl_types::Array<GLint, 4> tmp;
    glwrap::baseline<glwrap::v::baseline>::IntegerGetv(GL_VIEWPORT, tmp.data());
    return {tmp[2], tmp[3]};
}

semantic::mem_chunk<typing::pixels::rgb_t> ScreenshotProvider::pixels() const
{
    Coffee::DProfContext _("glscreenshot::ScreenshotProvider::pixels");

    auto size_ = size();
    auto out =
        semantic::mem_chunk<typing::pixels::rgb_t>::withSize(size_.area());
    glwrap::baseline<glwrap::v::baseline>::ReadPixels(
        {0, 0},
        size_,
        typing::PixCmp::RGB,
        typing::pixels::BitFmt::UByte,
        out.data);
    return out;
}

} // namespace glscreenshot
