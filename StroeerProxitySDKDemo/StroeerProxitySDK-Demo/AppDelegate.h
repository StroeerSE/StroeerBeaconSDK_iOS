//
//  AppDelegate.h
//
//

#import <UIKit/UIKit.h>

/**
 * The app delegate works alongside the app object to ensure your app interacts properly with the system and with other apps.
 * Specifically, the methods of the app delegate give you a chance to respond to important changes.
 * For example, you use the methods of the app delegate to respond to state transitions,
 * such as when your app moves from foreground to background execution, and to respond to incoming notifications.
 * In many cases, the methods of the app delegate are the only way to receive these important notifications.
 *
 * see also https://developer.apple.com/reference/uikit/uiapplicationdelegate
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

/**
 * The main window of the app.
 */
@property (strong, nonatomic) UIWindow *window;


@end

