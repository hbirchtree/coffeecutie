#include <platforms/osx/sysinfo.h>

#import <Foundation/Foundation.h>

#if defined(COFFEE_IOS)
#import <UIKit/UIKit.h>
#elif defined(COFFEE_MACOS)
#import <AppKit/AppKit.h>
#endif

namespace platform::info {

libc_types::u32 proc::apple::core_count(libc_types::u32, libc_types::u32)
{
    return [NSProcessInfo processInfo].activeProcessorCount;
}

DeviceType device::apple::variant()
{
#if defined(COFFEE_IOS)
    return DeviceType::DevicePhone;
#else
    return DeviceType::DeviceDesktop;
#endif
}

libc_types::f32 display::apple::dpi()
{
#if defined(COFFEE_MACOS)
    NSArray<NSScreen *> *screens = [NSScreen screens];
    for(size_t i=0; i<screens.count; i++)
    {
        return screens[i].backingScaleFactor;
    }
    return 96.f;
#elif defined(COFFEE_IOS)
    auto screens = [UIScreen screens];
    if(screens.count == 0)
        return 1.f;
    return screens.firstObject.nativeScale;
#endif
}

}
