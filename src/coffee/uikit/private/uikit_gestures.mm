#include <coffee/uikit/uikit_comp.h>

#import <UIKit/UIKit.h>

#include <coffee/uikit/gestures.h>

#include <coffee/core/CDebug>

extern void* uikit_appdelegate;
extern void* uikit_window;

@interface GestureDelegate : UIResponder<UIGestureRecognizerDelegate>

- (void)handleTap:(UITapGestureRecognizer *)recog;
- (void)handlePan:(UIPanGestureRecognizer *)recog;
- (void)handlePinch: (UIPinchGestureRecognizer*) recog;
- (void)handleSwipe:(UISwipeGestureRecognizer *)recog;
- (void)handleRotation: (UIRotationGestureRecognizer*) recog;

@property (retain) UIWindow* window;

@end

@implementation GestureDelegate

- (void)handleTap:(UITapGestureRecognizer *)recog
{
    Coffee::cDebug("Hello tap");
    CGPoint point = [recog locationInView: self.window.rootViewController.view];
}

- (void)handlePan:(UIPanGestureRecognizer *)recog
{
    Coffee::cDebug("Hello pan");
    UIView* targetView = self.window.rootViewController.view;
    
    CGPoint point = [recog locationInView: targetView];
    
    CGPoint translation = [recog translationInView: targetView];
    CGPoint velocity = [recog velocityInView: targetView];
}

- (void)handlePinch: (UIPinchGestureRecognizer*) recog
{
    Coffee::cDebug("Hello pinch");
    CGPoint point = [recog locationInView: self.window.rootViewController.view];
}

- (void)handleSwipe:(UISwipeGestureRecognizer *)recog
{
    Coffee::cDebug("Hello swipe");
    CGPoint point = [recog locationInView: self.window.rootViewController.view];
}

- (void)handleRotation: (UIRotationGestureRecognizer*) recog
{
    Coffee::cDebug("Hello rotate");
    CGPoint point = [recog locationInView: self.window.rootViewController.view];
}

- (GestureDelegate*)init
{
    self = [super init];
    
    UITapGestureRecognizer* tapRecog = [[UITapGestureRecognizer alloc]
                                        initWithTarget:self
                                        action:@selector(handleTap:)];

    UIPinchGestureRecognizer* pinchRecog = [[UIPinchGestureRecognizer alloc]
                                        initWithTarget:self
                                        action:@selector(handlePinch:)];

    UISwipeGestureRecognizer* swipeRecog = [[UISwipeGestureRecognizer alloc]
                                        initWithTarget:self
                                        action:@selector(handleSwipe:)];

    UIPanGestureRecognizer* panRecog = [[UIPanGestureRecognizer alloc]
                                            initWithTarget:self
                                            action:@selector(handlePan:)];

    UIRotationGestureRecognizer* rotRecog = [[UIRotationGestureRecognizer alloc]
                                            initWithTarget:self
                                            action:@selector(handleRotation:)];

    self.window = (__bridge UIWindow*)uikit_window;
    
    UIView* view = self.window.rootViewController.view;

    [view addGestureRecognizer: tapRecog];
    [view addGestureRecognizer: pinchRecog];
    [view addGestureRecognizer: swipeRecog];
    [view addGestureRecognizer: panRecog];
    [view addGestureRecognizer: rotRecog];
    
    return self;
}

@end

namespace uikit {

void GestureInput::load(
        comp_app::AppLoadableService::entity_container &e,
        comp_app::app_error &ec)
{
    m_container = &e;

    GestureDelegate* gestures = [[GestureDelegate alloc] init];
}

void GestureInput::start_frame(ContainerProxy&, time_point const&)
{
    auto ibus =
            m_container->service<comp_app::EventBus<Coffee::Input::CIEvent>>();
    
    /* TODO: Receive input events and distribute on ibus */
}

}
