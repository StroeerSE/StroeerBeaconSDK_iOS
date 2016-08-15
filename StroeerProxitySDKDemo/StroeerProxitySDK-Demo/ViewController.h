//
//  ViewController.h
//  blulocSDK-Demo
//
//  Copyright (c) 2015 bluloc GmbH. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <StroeerProxity/StroeerProxity.h>

@interface ViewController : UIViewController<UIAlertViewDelegate, SPXStroeerProxityAPIDelegate>

- (void)updateUI;

@end

