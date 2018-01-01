//
//  system_interface.m
//  Coffee
//
//  Created by Håvard Bjerke on 31/12/2017.
//
//

#include <coffee/core/plat/plat_primary_identify.h>

#import <Foundation/Foundation.h>

#if defined(COFFEE_APPLE_MOBILE)
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
        *num_dpis = 0;
#else
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
