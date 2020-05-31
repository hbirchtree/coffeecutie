#import <Foundation/Foundation.h>

#include <peripherals/identify/system.h>
#include <platforms/osx/sysinfo.h>

#if defined(COFFEE_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

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

}
}
