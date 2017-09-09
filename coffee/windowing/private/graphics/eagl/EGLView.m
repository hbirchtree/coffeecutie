#import <Foundation/Foundation.h>
#import "EGLView.h"

extern void* appdelegate_ptr;

EGLView* current_view = NULL;

extern int(*apple_entry_point)(int, char**);
extern int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv);

@implementation EGLView {
    AppDelegate* mAppDelegate;
    EAGLContext* mContext;
    GLKView* mView;
}

+ (EGLView*) createView
{
    if(current_view)
    {
        [current_view dealloc];
    }

    current_view = [[EGLView alloc] init];
    return current_view;
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
    GLKView* view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    
    view.context = self->mContext;
    view.delegate = self->mAppDelegate;
    
    self->mView = view;
    
    GLKViewController* vc = nil;
    
    vc = [[GLKViewController alloc]
          initWithNibName: nil
          bundle:nil];
    
    vc.view = view;
    vc.preferredFramesPerSecond = 60;
    
    vc.delegate = self->mAppDelegate;
    
    self->mAppDelegate.window.rootViewController = vc;

    return true;
}

- (void) dealloc
{
    GLKViewController* vc = (GLKViewController*)self->mAppDelegate.window.rootViewController;
    
    vc.view = nil;
    
    current_view = nil;
    
    self->mAppDelegate.window.rootViewController = [[UIViewController alloc] init];
    
    [super dealloc];
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
