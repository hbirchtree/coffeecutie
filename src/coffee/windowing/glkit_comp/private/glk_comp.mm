#include <coffee/glkit/glkit_comp.h>

#import <UIKit/UIKit.h>

namespace glkit {
namespace {

comp_app::size_2d_t screen_pixels()
{
    /* Points follow the interface orientation, scale turns them into pixels */
    UIScreen* screen = UIScreen.mainScreen;
    CGSize    points = screen.bounds.size;
    return comp_app::size_2d_t{
        static_cast<libc_types::i32>(points.width * screen.scale),
        static_cast<libc_types::i32>(points.height * screen.scale)};
}

} // namespace

comp_app::size_2d_t Windowing::size() const
{
    return screen_pixels();
}

comp_app::window_flags_t Windowing::state() const
{
    return comp_app::window_flags_t::fullscreen;
}

void Windowing::setState(comp_app::window_flags_t)
{
}

comp_app::size_2d_t DisplayInfo::virtualSize() const
{
    return screen_pixels();
}

/* UIKit does not report the panel's physical dimensions */
comp_app::size_2d_t DisplayInfo::physicalSize(libc_types::u32) const
{
    return {};
}

libc_types::f32 DisplayInfo::dpi(libc_types::u32) const
{
    return static_cast<libc_types::f32>(UIScreen.mainScreen.scale);
}

libc_types::f32 DisplayInfo::diagonal(libc_types::u32) const
{
    return 0.f;
}

} // namespace glkit
