#include <peripherals/identify/system.h>

#import <Foundation/Foundation.h>

#if defined(COFFEE_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

#include <stdint.h>

extern void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis);

void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis)
{
#if defined(COFFEE_APPLE_MOBILE)
    if(num_dpis)
        *num_dpis = [UIScreen screens].count;
    
    if(dpis)
    {
        NSArray<UIScreen*> *screens = [UIScreen screens];
        for(size_t i=0;i<screens.count; i++)
            dpis[i] = screens[i].nativeScale;
    }
#else
    if(num_dpis)
        *num_dpis = [NSScreen screens].count;
    
    if(dpis)
    {
        NSArray<NSScreen *> *screens = [NSScreen screens];
        for(size_t i=0; i<screens.count; i++)
        {
            NSScreen* screen = screens[i];
            dpis[i] = screen.backingScaleFactor;
        }
    }
#endif
}
