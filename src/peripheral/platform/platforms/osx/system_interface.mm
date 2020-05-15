#import <Foundation/Foundation.h>

#include <coffee/core/platform_data.h>
#include <peripherals/identify/system.h>
#include <platforms/osx/sysinfo.h>

#if defined(COFFEE_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

extern void* uikit_appdelegate;

namespace platform {
namespace mac {

void get_display_dpi(stl_types::Vector<libc_types::f32>& dpis)
{
#if defined(COFFEE_APPLE_MOBILE)
    NSArray<UIScreen*> *screens = [UIScreen screens];
    for(size_t i=0;i<screens.count; i++)
        dpis.push_back(screens[i].nativeScale);
#else
    NSArray<NSScreen *> *screens = [NSScreen screens];
    for(size_t i=0; i<screens.count; i++)
    {
        NSScreen* screen = screens[i];
        dpis.push_back(screen.backingScaleFactor);
    }
#endif
}

void get_safe_insets(C_UNUSED(platform::info::SafeArea& insets))
{
#if defined(COFFEE_IOS)
    auto appdelegate_typed = (UIResponder<UIApplicationDelegate>*)uikit_appdelegate;
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    UIEdgeInsets a = appdelegate_typed.window.safeAreaInsets;
#else
    UIEdgeInsets a = appdelegate_typed.window.layoutMargins;
#endif

    insets.left   = a.left;
    insets.right  = a.right;
    insets.top    = a.top;
    insets.bottom = a.bottom;
#endif
}

}
}
