//
//  AppDelegate.h
//  Test01
//
//  Created by Håvard Bjerke on 02/09/2017.
//  Copyright © 2017 Håvard Bjerke. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <objective_coffee/protocols/eglappdelegate.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate,
                         #if defined(COFFEE_APP_USE_GLKIT)
                         GLKViewDelegate, GLKViewControllerDelegate, EGLAppDelegate,
                         #endif
                         UIGestureRecognizerDelegate>

@property (strong, nonatomic) UIViewController *defaultController;
@property (strong, nonatomic) UIWindow *window;
@property BOOL hasInitialized;

- (void)handleTap: (UITapGestureRecognizer*) recog;

- (void)handleSwipe: (UISwipeGestureRecognizer*) recog;

- (void)handleSwipe: (UISwipeGestureRecognizer*) recog;

@end
