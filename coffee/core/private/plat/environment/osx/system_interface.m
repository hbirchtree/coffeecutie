//
//  system_interface.m
//  Coffee
//
//  Created by Håvard Bjerke on 31/12/2017.
//
//

#import <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#import <UIKit/UIKit.h>
#elif defined(TARGET_OS_MAC)
#import <AppKit/AppKit.h>
#endif

#include <stdint.h>

extern void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis);

void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis)
{
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
    if(num_dpis)
        *num_dpis = 0;
#elif defined(TARGET_OS_MAC)
    if(num_dpis)
        *num_dpis = [NSScreen screens].count;
    
    if(dpis)
    {
        NSArray<NSScreen *> *screens = [NSScreen screens];
        for(size_t i=0; i<screens.count; i++)
            dpis[i] = screens[i].backingScaleFactor;
    }
#endif
}
