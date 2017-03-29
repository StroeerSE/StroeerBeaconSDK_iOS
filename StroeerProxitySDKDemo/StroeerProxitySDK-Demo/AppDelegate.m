//
//  AppDelegate.m
//
//

#import "AppDelegate.h"
#import "ViewController.h"

/**
 * Implementation of the AppDelegate Class
 */
@implementation AppDelegate

/**
 * Restart any tasks that were paused (or not yet started) while the application was inactive.
 * If the application was previously in the background, optionally refresh the user interface.
 */
- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // get the root view controller of the app
    ViewController *viewController = (ViewController *)self.window.rootViewController;

    // update the user interface
    [viewController updateUI];
}

@end
