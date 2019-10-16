#include <coffee/uikit/uikit_comp.h>

#import <UIKit/UIKit.h>

#include <coffee/uikit/gestures.h>

#include <coffee/core/CDebug>

extern void* uikit_appdelegate;
extern void* uikit_window;

@interface GestureDelegate : UIResponder<UIGestureRecognizerDelegate>

+ (void)initRecognizers;
- (void)handleTap:(UITapGestureRecognizer *)recog;
- (void)handlePinch: (UIPinchGestureRecognizer*) recog;
- (void)handleSwipe:(UISwipeGestureRecognizer *)recog;
- (void)handleRotation: (UIRotationGestureRecognizer*) recog;

@end

@implementation GestureDelegate

- (void)handleTap:(UITapGestureRecognizer *)recog
{
    Coffee::DebugFun::cDebug("Hello tap");
}

- (void)handlePinch: (UIPinchGestureRecognizer*) recog
{

}

- (void)handleSwipe:(UISwipeGestureRecognizer *)recog
{
    Coffee::DebugFun::cDebug("Hello swipe");
}

- (void)handleRotation: (UIRotationGestureRecognizer*) recog
{

}

- (void)initRecognizers
{


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

    UIView* view = ((UIWindow*)uikit_window).rootViewController.view;

    [view addGestureRecognizer: tapRecog];
    [view addGestureRecognizer: pinchRecog];
    [view addGestureRecognizer: swipeRecog];
    [view addGestureRecognizer: panRecog];
    [view addGestureRecognizer: rotRecog];
}

@end

namespace uikit {

void GestureInput::load(
        comp_app::AppLoadableService::entity_container &e,
        comp_app::app_error &ec)
{
    m_container = &e;

    GestureDelegate* gestures = [GestureDelegate alloc];
    
    //[gestures initRecognizers];
}

void GestureInput::start_restricted(Proxy& c, time_point const&)
{
    auto ibus =
            m_container->service<comp_app::EventBus<Coffee::Input::CIEvent>>();
}

}
