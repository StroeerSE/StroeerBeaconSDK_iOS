//
//  ViewController.m
//
//

#import "ViewController.h"

/**
 * The state of the SDK represented in the user interface.
 */
typedef NS_ENUM(NSInteger, UIState)
{
    /**
     * The scan is stopped.
     */
    UIStateScanStopped = 0,

    /**
     * The scan is started.
     */
    UIStateScanStarted,

    /**
     * Scan is not possible (for example when bluetooth is disabled).
     */
    UIStateDisabled
};

/**
 * Private Interface definition of the ViewController
 */
@interface ViewController ()

/**
 * Button to start the Scanning process.
 */
@property (nonatomic) IBOutlet UIButton *startButton;

/**
 * Button to stop the Scanning process.
 */
@property (nonatomic) IBOutlet UIButton *stopButton;

/**
 * This method will be executed if the user has tapped on the start button.
 * @param sender The startButton.
 */
- (IBAction)start:(id)sender;

/**
 * This method will be executed if the user has tapped on the stop button.
 * @param sender The stopButton.
 */
- (IBAction)stop:(id)sender;

@end

/**
 * Implementation of the ViewController class
 */
@implementation ViewController

/**
 * Notifies the view controller that its view is about to be added to a view hierarchy.
 * @param animated If YES, the view is being added to the window using an animation.
 */
- (void)viewWillAppear:(BOOL)animated
{
    // Call super implementation
    [super viewWillAppear:animated];

    // Update user interface
    [self updateUI];
}

/**
 * This method will be executed if the user has tapped on the start button.
 * @param sender The startButton.
 */
- (IBAction)start:(id)sender
{
    // update ui state
    [self updateUIToState:UIStateScanStarted];

    // Set the delegate to receive relevant information
    [[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
    
    // Specify your API Key
    [[SPXStroeerProxityAPI sharedInstance] setApiKey:@"TODO: set you api key"];

    // (optional) For Debugging Purposes you can turn file logging on
    [[SPXStroeerProxityAPI sharedInstance] setFileLoggingEnabled:YES];

    // (optional) To delete an existing log file just call deleteLogFile
    [[SPXStroeerProxityAPI sharedInstance] deleteLogFile];

    // To view the logfile you can use an internal viewer like UIDocumentInteractionController
    // or you download the file via Xcode
    NSLog(@"Logfile saved at: %@", [[SPXStroeerProxityAPI sharedInstance] logFile]);

    // (optional) Add the Advertisement Identifier (IDFA) from iOS to each analytics event
    [[SPXStroeerProxityAPI sharedInstance] setAdvertisingTrackingEnabled:YES];
    
    // (optional) Set also a custom Advertisement Identifier (Could be the user-id)
    [[SPXStroeerProxityAPI sharedInstance] setCustomAdvertisingId:@"TODO: Some user ID"];

    // start scan
    [[SPXStroeerProxityAPI sharedInstance] startScan];
}

/**
 * This method will be executed if the user has tapped on the stop button.
 * @param sender The stopButton.
 */
- (IBAction)stop:(id)sender
{
    // update ui state
    [self updateUIToState:UIStateScanStopped];
    
    // stop scanning for nearby beacons
    [[SPXStroeerProxityAPI sharedInstance] stopScan];
}

/**
 * Synchronise the ui state with the sdk state.
 */
- (void)updateUI
{
    // Check whether the SDK is scanning or not
    if ([[SPXStroeerProxityAPI sharedInstance] isScanning])
    {
        // SDK is scanning, update state to UIStateScanStarted
        [self updateUIToState:UIStateScanStarted];
    }
    else
    {
        // SDK is not scanning, update state to UIStateScanStopped
        [self updateUIToState:UIStateScanStopped];
    }
}

/**
 * Update the UI according to the given UI state.
 * @param state the new state for the UI
 */
- (void)updateUIToState:(UIState)state
{
    // Evaluate the given UI state
    switch (state)
    {
        // state UIStateScanStopped
        case UIStateScanStopped:
            // disable the stop button
            [_stopButton setEnabled:NO];
            // enable the start button
            [_startButton setEnabled:YES];
            break;
        // state UIStateScanStarted
        case UIStateScanStarted:
            // enable the stop button
            [_stopButton setEnabled:YES];
            // disable the start button
            [_startButton setEnabled:NO];
            break;
        // state UIStateDisabled
        case UIStateDisabled:
            // disable the stop button
            [_stopButton setEnabled:NO];
            // disable the start button
            [_startButton setEnabled:NO];
            break;
    }
}

#pragma mark - UIAlertView Delegate

/**
 * Alert View Delegate method which will be called if the user has tapped on a alert view button.
 * @param alertView The AlertView instance.
 * @param buttonIndex The pressed button index.
 */
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    // Check if the user has tapped on the settings button
    if ([[alertView buttonTitleAtIndex:buttonIndex] isEqualToString:@"Settings"])
    {
        // open the system settings of iOS
        [self openSettings];
    }
}

#pragma mark - SPXStroeerProxityAPIDelegate

/**
 * Informs the delegate about an occurred error. In case of a critical error (e.g. invalid Api-Key)
 * the SDK will stop scanning automatically.
 *
 * @param spxAPi The current instance of the api.
 * @param error The occurred error.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didFailWithError:(SPXError*)error
{
    // Something went wrong. Now we can check the error type and message for further information.
    NSLog(@"An error occurred: %@", [error message]);
    
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

    // update the UI
    [self updateUI];
}

/**
 * This method informs about state changes of the bluetooth hardware.
 * To check if bluetooth is either enabled or not the @p bluetoothEnabled property can be used.
 * This method will only be called if the target isn't the iphone simulator (@p TARGET_IPHONE_SIMULATOR not defined).
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi bluetoothStateChangedFromState:(CBManagerState)oldState toState:(CBManagerState)newState
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
    // show the alert view
    [alertView show];
}

/**
 * The proximity of a beacon was entered.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didEnterBeacon:(SPXBeacon*)beacon;
{
    // Print log message
    NSLog(@"Beacon entered: %@", beacon);
}

/**
 * The proximity of a beacon was left.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didExitBeacon:(SPXBeacon*)beacon
{
    // Print log message
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

/**
 * Helper method to convert the CBManagerState to a NSString
 * @param state CBManagerState state
 * @return String representation of state
 */
- (NSString*)stringFromCentralManagerState:(CBManagerState)state
{
    // Evaluate the given CBManagerState state
    switch (state)
    {
        // state CBManagerStatePoweredOff
        case CBManagerStatePoweredOff:
        {
            // return the string representation of this state
            return @"CBManagerStatePoweredOff";
        }
        // state CBManagerStatePoweredOn
        case CBManagerStatePoweredOn:
        {
            // return the string representation of this state
            return @"CBManagerStatePoweredOn";
        }
        // state CBManagerStateResetting
        case CBManagerStateResetting:
        {
            // return the string representation of this state
            return @"CBManagerStateResetting";
        }
        // state CBManagerStateUnauthorized
        case CBManagerStateUnauthorized:
        {
            // return the string representation of this state
            return @"CBManagerStateUnauthorized";
        }
        // state CBManagerStateUnknown
        case CBManagerStateUnknown:
        {
            // return the string representation of this state
            return @"CBManagerStateUnknown";
        }
        // state CBManagerStateUnsupported
        case CBManagerStateUnsupported:
        {
            // return the string representation of this state
            return @"CBManagerStateUnsupported";
        }
    }

    // state was not found. return nil value
    return nil;
}

/**
 * Open the system settings
 */
- (void)openSettings
{
    // generate a url object from UIApplicationOpenSettingsURLString
    NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];

    // open the URL
    [[UIApplication sharedApplication] openURL:url];
}

@end
