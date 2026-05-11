#pragma once

#import <GLKit/GLKView.h>
#import <GLKit/GLKViewController.h>
#import <GLKit/GLKit.h>
#import <UIKit/UIKit.h>

@protocol EGLAppDelegate<
    UIApplicationDelegate,
    GLKViewDelegate,
    GLKViewControllerDelegate>
@end
