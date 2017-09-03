//
//  AppDelegate.m
//  Test01
//
//  Created by Håvard Bjerke on 02/09/2017.
//  Copyright © 2017 Håvard Bjerke. All rights reserved.
//

#import "AppDelegate.h"

// This is where all the foreign stuff comes from
#include <coffee/core/base/renderer/eventapplication_wrapper.h>

// For accessibility from other parts of the Objective-C code
// We will be using this to attach a GLKViewController
void* appdelegate_ptr = NULL;
AppDelegate* appdelegate_typed = NULL;

// Event handling, sending stuff to Coffee
void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);
void* coffee_event_handling_data;

// Receiving stuff
void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);

// General initialization
extern int(*apple_entry_point)(int, char**);
extern int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv);

void HandleForeignSignals(int event);
void HandleForeignSignalsNA(int event, void* ptr1, void* ptr2, void* ptr3);

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    appdelegate_ptr = self;
    appdelegate_typed = self;
    
    CoffeeForeignSignalHandle = HandleForeignSignals;
    CoffeeForeignSignalHandleNA = HandleForeignSignalsNA;
    
    self.window = [[UIWindow alloc] init];
    self.window.backgroundColor = [UIColor redColor];
    
    // Call the Coffee entrypoint, it will set up a bunch of things
//    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{

    deref_main_c(apple_entry_point, 0, NULL);
//    });
    
    [self.window makeKeyAndVisible];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
    
    CoffeeEventHandleCall(CoffeeHandle_Cleanup);
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    CoffeeEventHandleCall(CoffeeHandle_IsBackground);
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Do data stuff here
    CoffeeEventHandleCall(CoffeeHandle_TransForeground);
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    CoffeeEventHandleCall(CoffeeHandle_Setup);
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    CoffeeEventHandleCall(CoffeeHandle_IsTerminating);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    CoffeeEventHandleCall(CoffeeHandle_Loop);
}

- (void)glkViewControllerUpdate:(GLKViewController *)controller
{
    // NOOP
}

@end

void HandleForeignSignals(int event)
{
    switch(event)
    {
        case CoffeeForeign_DidFinish:
        printf("Oh no! We gonna die!\n");
        break;
        default:
        printf("Unhandled signal: %i\n", event);
        break;
    }
}
void HandleForeignSignalsNA(int event, void* ptr1, void* ptr2, void* ptr3)
{
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
            CGSize size = [[UIScreen mainScreen] bounds].size;
            winSize[0] = size.width;
            winSize[1] = size.height;
            break;
        }
        default:
        printf("Unhandled signal: %i\n", event);
        break;
    }
}

int main(int argc, char** argv)
{
    @autoreleasepool {
        UIApplicationMain(argc, argv, NULL, NSStringFromClass([AppDelegate class]));
    }
}

