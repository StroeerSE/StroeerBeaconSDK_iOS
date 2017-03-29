//
//  ViewController.h
//
//

#import <UIKit/UIKit.h>
#import <StroeerProxity/StroeerProxity.h>

/**
 * The Main ViewController of the Demo App. Implements the UIAlertViewDelegate and SPXStroeerProxityAPIDelegate protocols.
 */
@interface ViewController : UIViewController<UIAlertViewDelegate, SPXStroeerProxityAPIDelegate>

/**
 * Synchronise the ui state with the sdk state.
 */
- (void)updateUI;

@end

