//
//  ViewController.m
//
//  Copyright (c) 2015 bluloc GmbH. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@property (nonatomic) IBOutlet UIButton *startButton;
@property (nonatomic) IBOutlet UIButton *stopButton;
@property (nonatomic) IBOutlet UIButton *updateButton;

/**
 *  Contains all actions which aren't shown with an UIAlertView to the user yet
 */
@property (nonatomic) NSMutableArray *nonProcessedActions;

/**
 *  Contains all actions which are currently shown as UIAlertView to the user.
 */
@property (nonatomic) NSMutableArray *waitingForConfirmationActions;


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
    
    [_updateButton setEnabled:NO];
    [_stopButton setEnabled:NO];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    // On iOS8 we have to ask the user for permission to receive local notifications
    if ([UIApplication instancesRespondToSelector:@selector(registerUserNotificationSettings:)])
    {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert | UIUserNotificationTypeBadge | UIUserNotificationTypeSound categories:nil]];
    }
    
    [self updateButtonState];
}

- (void)updateButtonState
{
    if ([[SPXStroeerProxityAPI sharedInstance] isScanning])
    {
        [_startButton setEnabled:NO];
        [_updateButton setEnabled:YES];
        [_stopButton setEnabled:YES];
    }
    else
    {
        [_startButton setEnabled:YES];
        [_updateButton setEnabled:NO];
        [_stopButton setEnabled:NO];
    }
}

- (IBAction)start:(id)sender
{
    [_startButton setEnabled:NO];
    
    /*
     
     Define how the content should be downloaded
     -------------------------------------------
     
     You can choose between two strategies: SPXDownloadStrategyAll and SPXDownloadStrategyAroundMe.
     
     The SPXDownloadStrategyAll strategy is the default approach which will download all relevant data from the server.
     This strategy is recommended if you have only a small amount of blulocs and actions.

     The SPXDownloadStrategyAroundMe strategy should be used if you have a lot of blulocs and actions.
     With this approach every bluloc and all associated actions will be
     downloaded in a certain radius (1 km) around the current user location. 
     Region Monitoring is used to determine the location of the user.
     
     For proper function you have to
     - set a position for every bluloc.
     - enable the background capability "Location updates" for your app.
     - add the NSLocationAlwaysUsageDescription key to the info.plist file with a suitable value (iOS 8+).
     - add the NSLocationUsageDescription key to the info.plist file with a suitable value (iOS 7).
    
    */
    [[SPXStroeerProxityAPI sharedInstance] setDownloadStrategy:SPXDownloadStrategyAroundMe];
    
    //
    // Before we can start scanning we have to validate our credentials. Additionally all relevant data will be downloaded.
    //
    [[SPXStroeerProxityAPI sharedInstance] setupWithAppKey:@"TODO: Your app key"
                                               andSphere:@"TODO: Your sphere name"
                                          proximityUUIDs:@[@"TODO: The proximity UUIDs of your blulocs"]
                                               userToken:@"TODO: A user token if you want to login a specific user"
                                            successBlock:^(NSString *userToken) {
                          
                              // Now we can start scanning for nearby blulocs
                              [[SPXStroeerProxityAPI sharedInstance] startScan];
                              [[SPXStroeerProxityAPI sharedInstance] setOutdoorPositioningEnabled:YES];
                                                
                              [_updateButton setEnabled:YES];
                              [_stopButton setEnabled:YES];
                              
                          } errorBlock:^(SPXError error, NSString *errorMessage) {
                              
                              // Something went wrong. Now we can check the error type and message for further informations.
                              switch (error)
                              {
                                  case SPXErrorUnknown:
                                      NSLog(@"An unknown error occured: %@", errorMessage);
                                      break;
                                  case SPXErrorInvalidAppKeyOrSphere:
                                      NSLog(@"The provided app key or sphere is invalid: %@", errorMessage);
                                      break;
                                  case SPXErrorNetworkConnection:
                                      NSLog(@"A problem with the network connection occured: %@", errorMessage);
                                      break;
                                  case SPXErrorBeaconRangingNotSupported:
                                      NSLog(@"The current device doesn't support ragnig for blulocs: %@", errorMessage);
                                      break;
                                  case SPXErrorInactiveAppKey:
                                      NSLog(@"The specified app key is not activated.");
                                      break;
                              }
                              
                              [_startButton setEnabled:YES];
                          }];
}

- (IBAction)update:(id)sender
{
    [_startButton setEnabled:NO];
    [_updateButton setEnabled:NO];
    [_stopButton setEnabled:NO];
    
    if (![[SPXStroeerProxityAPI sharedInstance] updateContentFromServerWithSuccessBlock:^{

        NSLog(@"Content update was successfully.");
        
        [_startButton setEnabled:YES];
        [_updateButton setEnabled:YES];
        [_stopButton setEnabled:YES];
        
    } errorBlock:^(SPXError error, NSString *errorMessage) {
        
        NSLog(@"Content update failed: %@", errorMessage);
        
        [_startButton setEnabled:YES];
        [_updateButton setEnabled:YES];
        [_stopButton setEnabled:YES];
    }])
    {
        NSLog(@"Content update failed because the validation of the credentials wasn't successfully.");
        
        [_startButton setEnabled:YES];
        [_updateButton setEnabled:YES];
        [_stopButton setEnabled:YES];
    }
}

- (IBAction)stop:(id)sender
{
    [_stopButton setEnabled:NO];
    [_updateButton setEnabled:NO];
    [_startButton setEnabled:YES];
    
    // stop scanning for nearby blulocs
    [[SPXStroeerProxityAPI sharedInstance] stopScan];
}

#pragma mark - SPXStroeerProxityAPIDelegate

- (void)actionReceived:(SPXDeviceAction *)action
{
    NSLog(@"######### actionReceived: %@", action);
    
    /*
     When the received action has a message entry, we present an alert view or local notification with its attached information.
     */
    if ([[action data] objectForKey:@"message"])
    {
        if (!_nonProcessedActions)
        {
            _nonProcessedActions = [NSMutableArray new];
        }
        [_nonProcessedActions addObject:action];
        
        UIApplicationState state = [[UIApplication sharedApplication] applicationState];
        
        if (state != UIApplicationStateActive)
        {
            /**
             *  App is not active we have to show a local notification to inform the user about the received action
             */
            
            NSString *pushMessage = [[action data] objectForKey:@"message"];
            UILocalNotification* localNotification = [UILocalNotification new];
            [localNotification setAlertBody:pushMessage];
            [localNotification setAlertAction:@"Show in App"];
            [localNotification setSoundName:UILocalNotificationDefaultSoundName];
            [localNotification setTimeZone:[NSTimeZone defaultTimeZone]];
            [localNotification setUserInfo:nil];
            
            [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
        }
        else
        {
            /**
             *  App is active we can show directly an alert view to the user
             */
            [self checkForToConfirmingActions];
        }
    }
}

- (void)locationUpdated:(SPXLocation *)location
{
    // We received a new location which we can show on a Map for example.
    NSLog(@"#### new outdoor location: %@", location);
}

- (void)bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState
{
    // The status of bluetooth (BT) has changed. If the user has turned off BT we can show him a notification that this is a bad idea.
    
    NSLog(@"Bluetooth state changed from %@ to %@", [self stringFromCentralManagerState:oldState], [self stringFromCentralManagerState:newState]);
}

- (void)usageOfLocationServicesIsDenied
{
    // The user doesn't allow the app to use location services. Without this permission we can't scan for nearby blulocs.
    
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Hint"
                                                    message:@"To use all features of this app you have to authorize this app to use location services."
                                                   delegate:self
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:[self canOpenSettings]?@"Settings":nil, nil];
    [alertView show];
}

#pragma mark - Action Handling

- (void)checkForToConfirmingActions
{
    if ([[UIApplication sharedApplication] applicationState] == UIApplicationStateActive)
    {
        //
        // App is running in foreground. We can show the user an alert view for each action with the corresponding message to confirm the action or not.
        //
        
        if (!_waitingForConfirmationActions)
        {
            _waitingForConfirmationActions = [NSMutableArray new];
        }
        
        for (int i = 0; i < [_nonProcessedActions count]; i++)
        {
            SPXDeviceAction *action = _nonProcessedActions[i];
            
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"bluloc"
                                                            message:[[action data] objectForKey:@"message"]
                                                           delegate:self
                                                  cancelButtonTitle:@"Cancel"
                                                  otherButtonTitles:@"Confirm", nil];
            
            [_waitingForConfirmationActions addObject:action];
            
            // The tag is used to being able to respond to user interaction with the alert view.
            [alert setTag:[_waitingForConfirmationActions count]];
            [alert show];
        }
        [_nonProcessedActions removeAllObjects];
    }
}

#pragma mark - UIAlertView Delegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if ([[alertView buttonTitleAtIndex:buttonIndex] isEqualToString:@"Settings"])
    {
        [self openSettings];
    }
    else
    {
        NSInteger index = ([alertView tag] - 1);
        if (index < [_waitingForConfirmationActions count])
        {
            SPXDeviceAction *action = [_waitingForConfirmationActions objectAtIndex:index];
            [_waitingForConfirmationActions removeObjectAtIndex:index];
            
            if ([alertView cancelButtonIndex] != buttonIndex)
            {
                BOOL someCondition = YES;
                
                // You can either confirm an action ..
                if (someCondition)
                {
                    NSLog(@"###### alertView->sendConfirmAnalyticsEventWithAction: %@", action);
                    BOOL confirmed = [[SPXStroeerProxityAPI sharedInstance] sendConfirmAnalyticsEventWithAction:action];
                    
                    if (!confirmed)
                    {
                        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
                                                                        message:[NSString stringWithFormat:@"Maximum capping reached for action: '%@'!", [action name]]
                                                                       delegate:nil
                                                              cancelButtonTitle:@"OK"
                                                              otherButtonTitles:nil];
                        [alert show];
                    }
                }
                else
                {
                    // or you can use one of the types you have defined in the dashboard to send an analytics event
                    NSLog(@"###### alertView->sendCustomAnalyticsEventWithAction: %@", action);
                    
                    NSString *typeString = @"TODO: Type-ID created in the dashboard";
                    SPXCustomAnalyticsType *typeToBeSend = nil;
                    
                    for (SPXCustomAnalyticsType *type in [[SPXStroeerProxityAPI sharedInstance] customAnalyticsEventTypes])
                    {
                        if ([typeString isEqualToString:[type type]])
                        {
                            typeToBeSend = type;
                            break;
                        }
                    }
                    
                    if (typeToBeSend)
                    {
                        [[SPXStroeerProxityAPI sharedInstance] sendCustomAnalyticsEventWithAction:action andType:typeToBeSend];
                    }
                    else
                    {
                        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
                                                                        message:[NSString stringWithFormat:@"The analytics type '%@' doesn't exist!", typeString]
                                                                       delegate:nil
                                                              cancelButtonTitle:@"OK"
                                                              otherButtonTitles:nil];
                        [alert show];
                    }
                }
            }
        }
    }
}

#pragma mark - Helper methods

- (NSString*)stringFromCentralManagerState:(CBCentralManagerState)state
{
    NSString *string = nil;
    switch (state)
    {
        case CBCentralManagerStatePoweredOff:
        {
            string = @"CBCentralManagerStatePoweredOff";
            break;
        }
        case CBCentralManagerStatePoweredOn:
        {
            string = @"CBCentralManagerStatePoweredOn";
            break;
        }
        case CBCentralManagerStateResetting:
        {
            string = @"CBCentralManagerStateResetting";
            break;
        }
        case CBCentralManagerStateUnauthorized:
        {
            string = @"CBCentralManagerStateUnauthorized";
            break;
        }
        case CBCentralManagerStateUnknown:
        {
            string = @"CBCentralManagerStateUnknown";
            break;
        }
        case CBCentralManagerStateUnsupported:
        {
            string = @"CBCentralManagerStateUnsupported";
            break;
        }
        default:
            break;
    }
    return string;
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
