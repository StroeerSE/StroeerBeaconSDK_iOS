//
//  ViewController.m
//
//  Copyright (c) 2015 bluloc GmbH. All rights reserved.
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
@property (nonatomic) IBOutlet UIButton *updateButton;

- (IBAction)start:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)update:(id)sender;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Set the delegate to receive relevant informations
    [[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    [self updateUI];
}

- (IBAction)start:(id)sender
{
    [self updateUIToState:UIStateScanStarted];

    // Configure the SDK to your needs
    [[SPXStroeerProxityAPI sharedInstance] setApiKey:@"TODO: set you api key"];

    // If you are supporting devices below iOS 8 you can adjust the shown notifications for the background mode
    [[SPXStroeerProxityAPI sharedInstance] setBackgroundScanningNotificationBody:@"Open the app to enable background mode"];
    [[SPXStroeerProxityAPI sharedInstance] setBackgroundScanningNotificationAction:@"Open"];
    [[SPXStroeerProxityAPI sharedInstance] setBackgroundScanningInactiveInterval:600];
    [[SPXStroeerProxityAPI sharedInstance] setBackgroundScanningNotificationSoundName:nil];

    // For Debugging Purposes you can turn file logging on
    [[SPXStroeerProxityAPI sharedInstance] setFileLoggingEnabled:YES];

    // To delete an existing log file just call deleteLogFile
    [[SPXStroeerProxityAPI sharedInstance] deleteLogFile];

    // To view the logifle you can use an internal viewer like UIDocumentInteractionController
    // or you download the file via Xcode
    NSLog(@"Logfile saved at: %@", [[SPXStroeerProxityAPI sharedInstance] logFile]);

    [[SPXStroeerProxityAPI sharedInstance] startScan];
}

- (IBAction)update:(id)sender
{
    [self updateUIToState:UIStateDisabled];

    [[SPXStroeerProxityAPI sharedInstance] updateContentFromServerWithSuccessBlock:^{

        NSLog(@"Content update was successful.");

        [self updateUI];

    } errorBlock:^(SPXError *error) {

        NSLog(@"Content update failed: %@", [error message]);

        [self updateUI];
    }];
}

- (IBAction)stop:(id)sender
{
    [self updateUIToState:UIStateScanStopped];
    
    // stop scanning for nearby blulocs
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
            [_updateButton setEnabled:NO];
            [_startButton setEnabled:YES];
            break;
        case UIStateScanStarted:
            [_stopButton setEnabled:YES];
            [_updateButton setEnabled:YES];
            [_startButton setEnabled:NO];
            break;
        case UIStateDisabled:
            [_startButton setEnabled:NO];
            [_updateButton setEnabled:NO];
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
    switch ([error code])
    {
        case SPXErrorCodeUnknown:
            NSLog(@"An unknown error occured: %@", [error message]);
            break;
        case SPXErrorCodeNetworkConnection:
            NSLog(@"A problem with the network connection occured: %@", [error message]);
            break;
        case SPXErrorCodeBeaconRangingNotSupported:
            NSLog(@"The current device doesn't support ragnig for blulocs: %@", [error message]);
            break;
        case SPXErrorCodeInvalidApiKey:
            NSLog(@"The specified api key is not valid.");
            break;
        case SPXErrorCodeLocationUsageDenied:
            NSLog(@"The usage of location services is not allowed.");
            break;
        case SPXErrorCodeCurrentLocationNotSupported:
            NSLog(@"The current location is not supported.");
            break;
    }

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
    NSLog(@"SDK state has changed from %@ to %@.", [self stringFromSPXState:oldState], [self stringFromSPXState:newState]);
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
    // The user doesn't allow the app to use location services. Without this permission we can't scan for nearby blulocs.
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Hint"
                                                        message:@"To use all features of this app you have to authorize this app to use location services."
                                                       delegate:self
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:[self canOpenSettings]?@"Settings":nil, nil];
    [alertView show];
}

/**
 *  Whenever a a new outdoor location was calculated this method will be triggered.
 *  A outdoor location will only be sent if @p outdoorPositioningEnabled is @p YES.
 *
 *  @param location     The new location.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi locationUpdated:(nullable SPXLocation *)location
{
    // We received a new location which we can show on a Map for example.
    NSLog(@"#### new location: %@", location);
}

/**
 *  Informs the delegate which beacons were ranged during the last scan interval.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didScanBeacons:(nullable NSMutableArray<SPXBeacon*>*)scannedBeacons
{
    NSLog(@"Scanned %ld beacons: %@", (long)[scannedBeacons count], scannedBeacons);
}

/**
 *  Will be called if a bluloc zone analytics event was successfully sent to the backend.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon
{
    // an analytics event was generated for the given beacon
    NSLog(@"Analytics event was send for beacon %@", beacon);
}

/**
 *  Will be called after the latest data where downloaded from the server.
 *
 *  @param numberOfBeacons The number of beacons downloaded from the server.
 */
- (void)stroeerProxityAPIContentUpdated:(SPXStroeerProxityAPI*)spxAPi numberOfBeacons:(NSInteger)numberOfBeacons
{
    NSLog(@"Currently available beacons: %ld", (long)numberOfBeacons);
}


#pragma mark - Helper methods

- (NSString *)stringFromSPXState:(SPXState)state
{
    switch (state) {
        case SPXStateNone:
            return @"SPXStateNone";
        case SPXStateOffline:
            return @"SPXStateOffline";
        case SPXStateOnline:
            return @"SPXStateOnline";
        case SPXStateRegionCheck:
            return @"SPXStateRegionCheck";
    }
    return nil;
}

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

- (BOOL)canOpenSettings
{
    return (&UIApplicationOpenSettingsURLString != NULL);
}

- (void)openSettings
{
    NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
    [[UIApplication sharedApplication] openURL:url];
}

@end
