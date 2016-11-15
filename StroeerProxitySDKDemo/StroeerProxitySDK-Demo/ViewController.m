//
//  ViewController.m
//
//

#import "ViewController.h"

typedef NS_ENUM(NSInteger, UIState)
{
    UIStateScanStopped = 0,
    UIStateScanStarted,
    UIStateDisabled
};

@interface ViewController ()

@property (nonatomic) IBOutlet UIButton *startButton;
@property (nonatomic) IBOutlet UIButton *stopButton;

- (IBAction)start:(id)sender;
- (IBAction)stop:(id)sender;

@end

@implementation ViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    [self updateUI];
}

- (IBAction)start:(id)sender
{
    [self updateUIToState:UIStateScanStarted];

    // Set the delegate to receive relevant informations
    [[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
    
    // Specify your API Key
    [[SPXStroeerProxityAPI sharedInstance] setApiKey:@"TODO: set you api key"];

    // For Debugging Purposes you can turn file logging on
    [[SPXStroeerProxityAPI sharedInstance] setFileLoggingEnabled:YES];

    // To delete an existing log file just call deleteLogFile
    [[SPXStroeerProxityAPI sharedInstance] deleteLogFile];

    // To view the logifle you can use an internal viewer like UIDocumentInteractionController
    // or you download the file via Xcode
    NSLog(@"Logfile saved at: %@", [[SPXStroeerProxityAPI sharedInstance] logFile]);

    [[SPXStroeerProxityAPI sharedInstance] startScan];
}

- (IBAction)stop:(id)sender
{
    [self updateUIToState:UIStateScanStopped];
    
    // stop scanning for nearby beacons
    [[SPXStroeerProxityAPI sharedInstance] stopScan];
}

- (void)updateUI
{
    if ([[SPXStroeerProxityAPI sharedInstance] isScanning])
    {
        [self updateUIToState:UIStateScanStarted];
    }
    else
    {
        [self updateUIToState:UIStateScanStopped];
    }
}

- (void)updateUIToState:(UIState)state
{
    switch (state)
    {
        case UIStateScanStopped:
            [_stopButton setEnabled:NO];
            [_startButton setEnabled:YES];
            break;
        case UIStateScanStarted:
            [_stopButton setEnabled:YES];
            [_startButton setEnabled:NO];
            break;
        case UIStateDisabled:
            [_startButton setEnabled:NO];
            [_stopButton setEnabled:NO];
            break;
    }
}

#pragma mark - UIAlertView Delegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if ([[alertView buttonTitleAtIndex:buttonIndex] isEqualToString:@"Settings"])
    {
        [self openSettings];
    }
}

#pragma mark - SPXStroeerProxityAPIDelegate

- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didFailWithError:(SPXError*)error
{
    // Something went wrong. Now we can check the error type and message for further information.
    NSLog(@"An error occured: %@", [error message]);
    
    // update the UI in case the SDK has stopped scanning due to an error.
    [self updateUI];
}

/**
 *  This method informs about state changes of the SDK.
 *
 *  @param oldState The old state of the SDK.
 *  @param newState The new state of the SDK.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi sdkStateChangedFromState:(SPXState)oldState toState:(SPXState)newState
{
    // The SDK state has changed
    NSLog(@"SDK state has changed from %@ to %@.", [SPXStroeerProxityAPI stringFromSPXState:oldState], [SPXStroeerProxityAPI stringFromSPXState:newState]);
    [self updateUI];
}

/**
 * This method informs about state changes of the bluetooth hardware.
 * To check if bluetooth is either enabled or not the @p bluetoothEnabled property can be used.
 * This method will only be called if the target isn't the iphone simulator (@p TARGET_IPHONE_SIMULATOR not defined).
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState
{
    // The status of bluetooth (BT) has changed. If the user has turned off BT we can show him a notification that this is a bad idea.
    NSLog(@"Bluetooth state has changed from %@ to %@.", [self stringFromCentralManagerState:oldState], [self stringFromCentralManagerState:newState]);
}

/**
 *  This method will be called after the authorization status has changed to @p kCLAuthorizationStatusDenied or @p kCLAuthorizationStatusRestricted.
 *  This happens when the user doesn't allow the usage of location services. The SDK will stop scanning.
 */
- (void)stroeerProxityAPIUsageOfLocationServicesDenied:(SPXStroeerProxityAPI*)spxAPi
{
    // The user doesn't allow the app to use location services. Without this permission we can't scan for nearby beacons
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Hint"
                                                        message:@"To use all features of this app you have to authorize this app to use location services."
                                                       delegate:self
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:@"Settings", nil];
    [alertView show];
}

/**
 * The proximity of a beacon was entered.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didEnterBeacon:(SPXBeacon*)beacon;
{
    NSLog(@"Beacon entered: %@", beacon);
}

/**
 * The proximity of a beacon was left.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didExitBeacon:(SPXBeacon*)beacon
{
    NSLog(@"Beacon left: %@", beacon);
}

/**
 *  Will be called if a zone event was successfully sent to the backend.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon
{
    // an analytics event was generated for the given beacon
    NSLog(@"Analytics event was send for beacon %@", beacon);
}

#pragma mark - Helper methods

- (NSString*)stringFromCentralManagerState:(CBCentralManagerState)state
{
    switch (state)
    {
        case CBCentralManagerStatePoweredOff:
        {
            return @"CBCentralManagerStatePoweredOff";
        }
        case CBCentralManagerStatePoweredOn:
        {
            return @"CBCentralManagerStatePoweredOn";
        }
        case CBCentralManagerStateResetting:
        {
            return @"CBCentralManagerStateResetting";
        }
        case CBCentralManagerStateUnauthorized:
        {
            return @"CBCentralManagerStateUnauthorized";
        }
        case CBCentralManagerStateUnknown:
        {
            return @"CBCentralManagerStateUnknown";
        }
        case CBCentralManagerStateUnsupported:
        {
            return @"CBCentralManagerStateUnsupported";
        }
    }
    return nil;
}

- (void)openSettings
{
    NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
    [[UIApplication sharedApplication] openURL:url];
}

@end
