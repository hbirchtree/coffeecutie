//
//  AppDelegate.m
//  Test01
//
//  Created by Håvard Bjerke on 02/09/2017.
//  Copyright © 2017 Håvard Bjerke. All rights reserved.
//

#import "AppDelegate.h"

#import <CoreMotion/CoreMotion.h>

// This is where all the foreign stuff comes from
#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/coffee.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/types/application_main.h>
#include <coffee/foreign/foreign.h>
#include <peripherals/stl/types.h>
#include <peripherals/libc/signals.h>

// For accessibility from other parts of the Objective-C code
// We will be using this to attach a GLKViewController
// Stored in extern_storage.cpp
extern void* uikit_appdelegate;
extern void* uikit_window;

CMMotionManager* app_motionManager = NULL;

// Event handling, sending stuff to Coffee
void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);

void HandleForeignSignals(int event);
void HandleForeignSignalsNA(int event, void* ptr1, void* ptr2, void* ptr3);

void DispatchGeneralEvent(uint32_t type, void* data)
{
    struct CfGeneralEvent ev = {
        .type = static_cast<CfGeneralEventType>(type),
        .pad = 0,
    };
    
    CoffeeEventHandleNACall(CoffeeHandle_GeneralEvent, &ev, data, NULL);
}

namespace Coffee {

extern int MainSetup(MainWithArgs mainfun, int argc, char** argv, u32 flags);
extern int MainSetup(MainNoArgs mainfun, int argc, char** argv, u32 flags);

}

namespace libc {
namespace signal {

extern stl_types::Vector<exit_handler> global_exit_handlers;

}
} // namespace libc

using comp_app::LifecycleEvent;

static inline void DispatchAppEvent(comp_app::AppEvent::Type type, void* event)
{
    using bus_type = comp_app::EventBus<comp_app::AppEvent>;
    
    comp_app::AppEvent baseEvent;
    baseEvent.type = type;
    comp_app::createContainer().service<bus_type>()->process(baseEvent, event);
}

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    uikit_appdelegate = self;
    
    CoffeeForeignSignalHandle = HandleForeignSignals;
    CoffeeForeignSignalHandleNA = HandleForeignSignalsNA;
    
    NSLog(@"View controller");
    
    Coffee::SetPrintingVerbosity(10);
    // Call the Coffee entrypoint, it will set up a bunch of things
    if(Coffee::main_functions.is_no_args)
        Coffee::MainSetup(Coffee::main_functions.no_args);
    else
        Coffee::MainSetup(Coffee::main_functions.with_args, 0, nullptr);
    Coffee::SetPrintingVerbosity(10);
    
    self.hasInitialized = FALSE;
    self.window = [[UIWindow alloc] init];
    self.window.backgroundColor = [UIColor yellowColor];

    NSLog(@"AppDelegate::init");
    
    return YES;
}


- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    NSLog(@"AppDelegate::applicationDidReceiveMemoryWarning");
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::LowMemory;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);
}

- (void)applicationWillResignActive:(UIApplication *)application {
    NSLog(@"AppDelegate::applicationWillResignActive");

    if(!self.hasInitialized)
        return;
    
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
    
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::WillEnterBackground;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);
    
    NSLog(@"AppDelegate running cleanup");
    CoffeeEventHandleCall(CoffeeHandle_Cleanup);
    self.hasInitialized = FALSE;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    NSLog(@"AppDelegate::applicationDidEnterBackground");
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::Background;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);

    auto rev_handlers = libc::signal::global_exit_handlers;
    std::reverse(rev_handlers.begin(), rev_handlers.end());
    for(auto const& hnd : rev_handlers)
        hnd();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Do data stuff here
    NSLog(@"AppDelegate::applicationWillEnterForeground");
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::WillEnterForeground;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive.
    // If the application was previously in the background, optionally refresh the user interface.
    NSLog(@"AppDelegate::applicationDidBecomeActive");
    if(!self.hasInitialized)
    {
        NSLog(@"AppDelegate running setup");

        self.window.backgroundColor = [UIColor blackColor];
        uikit_window = self.window;
        [self.window makeKeyAndVisible];

        CoffeeEventHandleCall(CoffeeHandle_Setup);
        self.hasInitialized = TRUE;
    }
    
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::Foreground;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);
    
    CGSize screenRes = [[UIScreen mainScreen] nativeBounds].size;
    
    using namespace Coffee::Display;
    
    Event baseEvent;
    baseEvent.type = Event::Resize;
    ResizeEvent resize;
    resize.w = screenRes.width;
    resize.h = screenRes.height;
    
    comp_app::createContainer()
        .service<comp_app::EventBus<Event>>()->process(baseEvent, &resize);
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    NSLog(@"AppDelegate::applicationWillTerminate");
    LifecycleEvent event;
    event.lifecycle_type = LifecycleEvent::Terminate;
    DispatchAppEvent(comp_app::AppEvent::LifecycleEvent, &event);
}

#if defined(COFFEE_APP_USE_GLKIT)
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if(self.hasInitialized)
        CoffeeEventHandleCall(CoffeeHandle_Loop);
}

- (void)glkViewControllerUpdate:(GLKViewController *)controller
{
    // NOOP
}
#endif

@end

// The simple foreign signal handler
void HandleForeignSignals(int)
{
}

// Foreign signal handler
void HandleForeignSignalsNA(int event, void* ptr1, void* ptr2, void* ptr3)
{
    AppDelegate* appdelegate_typed = (AppDelegate*)uikit_appdelegate;

    switch(event)
    {
        case CoffeeForeign_DisplayMessage:
        {
            NSString* title = [NSString
                        stringWithUTF8String: (const char*)ptr1];
            NSString* message = [NSString
                        stringWithUTF8String: (const char*)ptr2];
        
            UIAlertController* c = [UIAlertController
                        alertControllerWithTitle: title
                        message: message
                        preferredStyle:UIAlertControllerStyleAlert];
            
            void (^complete_handler)(void) = nil;
            
            bool no_actions = true;
            
            if(ptr3)
            {
                struct CfMessageDisplay* messageActions = (struct CfMessageDisplay*)ptr3;
                
                // We have to explicitly get the ptr here
                // Otherwise, it is dereferenced too late, when CfMessageDisplay
                //  does not exist
                void(*complete_c_fun)(void*) = messageActions->completion;
                void* user_ptr = messageActions->user_ptr;
                
                if(complete_c_fun)
                {
                    complete_handler = ^{
                        complete_c_fun(user_ptr);
                    };
                }
                
                if(messageActions->actions)
                {
                    struct CfMessageAction* actionPtr = messageActions->actions;
                    // We can only check actionPtr->text, because it's a 1D array
                    while (actionPtr->text)
                    {
                        NSString* actionTitle = [NSString
                                            stringWithUTF8String: actionPtr->text];
                        
                        void(*action_handler)(void*) = actionPtr->action;
                        
                        void(^action_obj_handler)(UIAlertAction*) = nil;
                        
                        if(action_handler)
                            action_obj_handler = ^(UIAlertAction* action){
                                            action_handler(user_ptr);
                                        };
                        
                        [c addAction: [UIAlertAction actionWithTitle: actionTitle
                                        style:UIAlertActionStyleDefault
                                        handler: action_obj_handler]];
                        
                        
                        no_actions = false;
                        actionPtr++;
                    }
                }
            }
            
            // If no action is created by CfMessageDisplay, create default one
            if(no_actions)
                [c addAction: [UIAlertAction actionWithTitle:@"OK"
                                style:UIAlertActionStyleDefault
                                handler:nil]];
            
            // And then present it to the user
            [appdelegate_typed.window.rootViewController
                        presentViewController:c
                        animated:YES
                        completion:complete_handler];
            
            break;
        }
        case CoffeeForeign_GetWinSize:
        {
            int* winSize = (int*)ptr1;
            // nativeBounds returns the portrait resolution...
            CGSize size = [[UIScreen mainScreen] nativeBounds].size;
            
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000
            UIInterfaceOrientation currOri =
                appdelegate_typed.window.windowScene.interfaceOrientation;
#else
            UIInterfaceOrientation currOri = [[UIApplication sharedApplication] statusBarOrientation];
#endif
            // So we have to transpose it when running in landscape mode
            // We cannot use UIDevice's orientation, because it is undefined
            //  at load-time
            if(UIInterfaceOrientationIsLandscape(currOri))
            {
                CGFloat width = size.width;
                size.width = size.height;
                size.height = width;
            }
            
            // ... and then we just fill the C++-side buffers.
            winSize[0] = size.width;
            winSize[1] = size.height;
            break;
        }
        case CoffeeForeign_GetSafeMargins:
        {
        #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
            UIEdgeInsets a = appdelegate_typed.window.safeAreaInsets;
        #else
            UIEdgeInsets a = appdelegate_typed.window.layoutMargins;
        #endif
            
            float* margins = (float*)ptr1;
            if(margins)
            {
                margins[0] = a.left;
                margins[1] = a.top;
                margins[2] = a.right;
                margins[3] = a.bottom;
            }
            
            break;
        }
        case CoffeeForeign_ActivateMotion:
        {
            // Make sensor initialization explicit, for power consumption
        
            printf("Request to activate motion device");
            if(!app_motionManager)
                app_motionManager = [CMMotionManager alloc];
            
            uint32_t flags = *((uint32_t*)ptr1);
            
            if(flags & CfSensor_Accel)
                [app_motionManager startAccelerometerUpdates];
            if(flags & CfSensor_Gyro)
                [app_motionManager startGyroUpdates];
            if(flags & CfSensor_Magnetometer)
                [app_motionManager startMagnetometerUpdates];
            
            break;
        }
        default:
        printf("Unhandled signal: %i\n", event);
        break;
    }
}
