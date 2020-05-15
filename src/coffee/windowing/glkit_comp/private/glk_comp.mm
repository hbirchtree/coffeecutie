#include <coffee/glkit/glkit_comp.h>

#include <coffee/foreign/foreign.h>

#import <UIKit/UIKit.h>

extern void* uikit_appdelegate;
extern void* uikit_window;

UIResponder<UIApplicationDelegate>* appDelegate()
{
    return (UIResponder<UIApplicationDelegate>*)uikit_appdelegate;
}

namespace glkit {

comp_app::size_2d_t Windowing::size() const
{
    auto size = [[UIScreen mainScreen] nativeBounds].size;

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000
    UIInterfaceOrientation currOri =
        appDelegate().window.windowScene.interfaceOrientation;
#else
    UIInterfaceOrientation currOri = [[UIApplication sharedApplication] statusBarOrientation];
#endif
    // So we have to transpose it when running in landscape mode
    // We cannot use UIDevice's orientation, because it is undefined
    //  at load-time
    if(UIInterfaceOrientationIsLandscape(currOri))
        std::swap(size.width, size.height);

    return comp_app::size_2d_t(size.width, size.height);
}

comp_app::detail::WindowState Windowing::state() const
{
    return comp_app::detail::WindowState::FullScreen;
}

void Windowing::setState(comp_app::detail::WindowState state)
{
}

}
