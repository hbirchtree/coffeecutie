#pragma once

#import <GLKit/GLKit.h>
#import <GLKit/GLKView.h>
#import <GLKit/GLKViewController.h>
#import <UIKit/UIKit.h>

@protocol EGLAppDelegate <UIApplicationDelegate,
                          GLKViewDelegate,
                          GLKViewControllerDelegate>
@end
