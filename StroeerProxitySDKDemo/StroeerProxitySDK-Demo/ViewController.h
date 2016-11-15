//
//  ViewController.h
//
//

#import <UIKit/UIKit.h>
#import <StroeerProxity/StroeerProxity.h>

@interface ViewController : UIViewController<UIAlertViewDelegate, SPXStroeerProxityAPIDelegate>

- (void)updateUI;

@end

