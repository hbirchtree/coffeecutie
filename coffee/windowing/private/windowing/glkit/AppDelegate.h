//
//  AppDelegate.h
//  Test01
//
//  Created by Håvard Bjerke on 02/09/2017.
//  Copyright © 2017 Håvard Bjerke. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <EGLView.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate, GLKViewDelegate, GLKViewControllerDelegate, UIGestureRecognizerDelegate, EGLAppDelegate>

@property (strong, nonatomic) UIWindow *window;

- (void)handleTap: (UITapGestureRecognizer*) recog;

- (void)handleSwipe: (UISwipeGestureRecognizer*) recog;

- (void)handleSwipe: (UISwipeGestureRecognizer*) recog;

@end
