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

@end
