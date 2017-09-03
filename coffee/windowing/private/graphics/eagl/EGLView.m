#import <Foundation/Foundation.h>
#import "EGLView.h"

extern void* appdelegate_ptr;



extern int(*apple_entry_point)(int, char**);
extern int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv);

@implementation EGLView

+ (EGLView*) createView
{
    return [[EGLView alloc] init];
}

- (bool) createContext:(uint32_t)contextVer
{
    self->mAppDelegate = (AppDelegate*)appdelegate_ptr;

    NSUInteger renderApi = kEAGLRenderingAPIOpenGLES2;
    
    if(contextVer == EGL_OPENGL_ES3_BIT)
        renderApi = kEAGLRenderingAPIOpenGLES3;

    self->mContext = [[EAGLContext alloc] initWithAPI:renderApi];
    
    return true;
}

- (bool) createView
{
    // Remove previous root viewcontroller
    if(self->mAppDelegate.window.rootViewController)
        [self->mAppDelegate.window.rootViewController release];
    
    GLKView* view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    view.context = self->mContext;
    view.delegate = self->mAppDelegate;
    
    GLKViewController* vc = [[GLKViewController alloc]
     initWithNibName: nil
     bundle:nil];
    
    vc.view = view;
    vc.preferredFramesPerSecond = 60;
    
    vc.delegate = self->mAppDelegate;
    
    self->mAppDelegate.window.rootViewController = vc;
    
    self->mView = view;
    return true;
}

- (AppDelegate*) getApp
{
    return self->mAppDelegate;
}

- (EAGLContext*) getContext
{
    return self->mContext;
}

- (GLKView*) getView
{
    return self->mView;
}

/*
self->mView = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context: self->mContext];
    
mView.delegate = self;
*/

@end
