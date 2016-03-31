//
//  BLBlulocController.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreLocation/CoreLocation.h>
#import <UIKit/UIKit.h>

@class BLBeacon;
@class BLCustomAnalyticsType;
@class BLDeviceAction;
@class BLIndoorLocation;
@class BLLocation;

@protocol BLBlulocControllerDelegate;

NS_ASSUME_NONNULL_BEGIN

/**
 * Possible errors that can occure in the SDK
 */
typedef NS_ENUM(NSInteger, BLError)
{
    /**
     * An unknown error occured.
     */
    BLErrorUnknown = 0,
    
    /**
     * The specified app key or sphere isn't valid.
     */
    BLErrorInvalidAppKeyOrSphere,
    
    /**
     * The specified app key is deactivated.
     */
    BLErrorInactiveAppKey,
    
    /**
     * The app key and sphere couldn't be validated or the operation couldn't be finished due to network errors.
     */
    BLErrorNetworkConnection,
    
    /**
     * This error is thrown during the setup if the device doesn't support ranging for beacons.
     */
    BLErrorBeaconRangingNotSupported,
};

/**
 * Possible errors that can occure after a indoor navigation was started
 */
typedef NS_ENUM(NSInteger, BLNavigationError)
{
    /**
     * This error is thrown when the indoor navigation algorithm wasn't able to find a route from the given start to destination coordinates.
     */
    BLNavigationErrorFailedNoPath = 0,
    
    /**
     * This error is thrown if the @p state of the SDK isn't @p BLStateOffline or @p BLStateOnline.
     */
    BLNavigationErrorFailedInvalidState,
    
    /**
     * This error is thrown when the navigation data isn't stored on the bluloc server for the current client.
     */
    BLNavigationErrorFailedNoData,
    
    /**
     * An unspecified error occured.
     */
    BLNavigationErrorOther
};

/**
 * Possible states in which the controller can be. Depending on the state scanning for blulocs is possible or not.
 */
typedef NS_ENUM(NSInteger, BLState)
{
    /**
     *  @p BLBlulocController hasn't been setup yet. In this state scanning isn't possible.
     */
    BLStateNone = 0,
    
    /**
     *  Setup of the @p BLBlulocController was successful but the data isn't up to date. In this state scanning is possible.
     */
    BLStateOffline,
    
    /**
     *  Setup of the @p BLBlulocController was successful and the data is up to date . In this state scanning is possible.
     */
    BLStateOnline
};

/**
 * Enum to specify the download strategy.
 */
typedef NS_ENUM(NSInteger, BLDownloadStrategy)
{
    /**
     *  Download all content (beacons, actions, ...).
     */
    BLDownloadStrategyAll = 0,
    
    /**
     * Download only content (beacons, actions, ...) which is in your vicinity.
     */
    BLDownloadStrategyAroundMe
};


#pragma mark - BLBlulocController


/**
 * Main class of the bluloc SDK with handles the authentication and scanning for nearby blulocs.
 */
@interface BLBlulocController : NSObject

/**
 * Delegate that informs about triggered actions.
 */
@property (nonatomic) id<BLBlulocControllerDelegate> delegate;

/**
 *  Define the strategy which shall be used to download your content (beacons, actions, ...).
 *  Default value: @p BLDownloadStrategyAll.
 *
 *  @note Has to be called before @p setupWithAppKey... method.
 */
@property (nonatomic) BLDownloadStrategy downloadStrategy;

/**
 *  Returns the current state of the @p BLBlulocController. The state will be updated during the setup process or in the @p updateContentFromServer method. Default value is @p BLStateNone
 */
@property (nonatomic, readonly) BLState state;

/**
 *  Contains the current state of the bluetooth hardware.
 *  This property will only be updated if the target isn't the iphone simulator (@p TARGET_IPHONE_SIMULATOR not defined).
 */
@property (nonatomic, readonly) CBCentralManagerState bluetoothState;

/**
 *  The user token is a 128-bit UUID generated during the first successful SDK setup.
 *  With this token it's possible to restore user specific sdk information (e.g. capping) after a fresh install of the app on the same device or another.
 *
 *  Once generated the user token will be re-used until the cache was cleared, you specified another user token or the app was deleted.
 *
 *  @note Simultaneous use of the same user token on different devices may lead to information loss.
 */
@property (nonatomic, readonly) NSString *userToken;

/**
 *  Returns the singleton instance of the @p BLBlulocController class. Use this method instead of @p init or @p new.
 *
 *  @return A @p BLBlulocController singleton instance.
 */
+ (instancetype)sharedInstance;

/**
 *  Returns the current SDK-Version
 *
 *  @return Current version of the SDK.
 */
+ (NSString *)sdkVersion;


#pragma mark - SDK Setup

/**
 * Method to setup the @p BLBlulocController with one proximityUUID. It is necessary to call this method before starting the scanning process.
 * The method returns immediately because the given @p appKey and @p sphere will be checked asynchronously.
 * 
 * @note This method will be removed soon. Please use this method instead:
 *
 * @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @note The @p successBlock will be called if
 * - the setup was successful and data could read from the server. The @p state will be updated to @p BLStateOnline.
 * - internet connection isn't available but the setup process and content update was successful previously. The @p state will be updated to @p BLStateOffline. After the internet connection is back the @p BLBlulocController will try updating the data automatically. If this succeeds the @p state will be updated to @p BLStateOnline.
 *
 * @note The @p errorBlock will be called if
 * - an unexpected error occurred during the setup or content loading (e.g. invalid app key or sphere). The @p state will be updated to @p BLStateNone.
 * - there wasn't a successful setup yet and a internet connection isn't available. The @p state will be updated to @p BLStateNone.
 *
 * @param appKey            A key to verify your app to use this framework.
 * @param sphere            A specific value defined for the current client.
 * @param proximityUUID     The UUID of your bluloc beacons.
 * @param successBlock      Will be called if the @p state was updated to @p BLStateOffline or @p BLStateOnline.
 * @param errorBlock        Will be called if the @p state was updated to @p BLStateNone.
 */
- (void)setupWithAppKey:(NSString*)appKey
              andSphere:(NSString*)sphere
          proximityUUID:(NSString*)proximityUUID
           successBlock:(nullable void (^)(NSString *userToken))successBlock
             errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock __attribute__((deprecated(("Use the - (void)setupWithAppKey:andSphere:proximityUUIDs:successBlock:errorBlock: method"))));




/**
 * Method to setup the @p BLBlulocController with multiple proximityUUIDs. It is necessary to call this method before starting the scanning process.
 * The method returns immediately because the given @p appKey and @p sphere will be checked asynchronously.
 *
 * On the first successful call this method will generate a new user token. After this the generated user token will be reused.
 *
 * @note The @p successBlock will be called if
 * - the setup was successful and data could read from the server. The @p state will be updated to @p BLStateOnline.
 * - internet connection isn't available but the setup process and content update was successful previously. 
 *      The @p state will be updated to @p BLStateOffline. After the internet connection is back the @p BLBlulocController will try updating the data automatically. 
 *      If this succeeds the @p state will be updated to @p BLStateOnline.
 *
 * @note The @p errorBlock will be called if
 * - an unexpected error occurred during the setup or content loading (e.g. invalid app key or sphere). The @p state will be updated to @p BLStateNone.
 * - there wasn't a successful setup yet and a internet connection isn't available. The @p state will be updated to @p BLStateNone.
 *
 * @param appKey            A key to verify your app to use this framework.
 * @param sphere            A specific value defined for the current client.
 * @param proximityUUIDs    The UUIDs of your bluloc beacons.
 * @param successBlock      Will be called if the @p state was updated to @p BLStateOffline or @p BLStateOnline.
 * @param errorBlock        Will be called if the @p state was updated to @p BLStateNone.
 */
- (void)setupWithAppKey:(NSString*)appKey
              andSphere:(NSString*)sphere
         proximityUUIDs:(NSArray*)proximityUUIDs
           successBlock:(nullable void (^)(NSString *userToken))successBlock
             errorBlock:(nullable void (^)(BLError error, NSString *errorMessage))errorBlock;

/**
 * Method to setup the @p BLBlulocController with multiple proximityUUIDs. It is necessary to call this method before starting the scanning process.
 * The method returns immediately because the given @p appKey and @p sphere will be checked asynchronously.
 *
 * This method won't generate a new user token. The SDK will try to validate the specified user token. If this token is invalid the setup fails.
 *
 * @note The @p successBlock will be called if
 * - the setup was successful and data could read from the server. The @p state will be updated to @p BLStateOnline.
 * - internet connection isn't available but the setup process and content update was successful previously. 
 *      The @p state will be updated to @p BLStateOffline. After the internet connection is back the @p BLBlulocController will try updating the data automatically. 
 *      If this succeeds the @p state will be updated to @p BLStateOnline.
 *
 * @note The @p errorBlock will be called if
 * - an unexpected error occurred during the setup or content loading (e.g. invalid app key or sphere). The @p state will be updated to @p BLStateNone.
 * - there wasn't a successful setup yet and a internet connection isn't available. The @p state will be updated to @p BLStateNone.
 *
 * @param appKey A key to verify your app to use this framework.
 * @param sphere A specific value defined for the current client.
 * @param proximityUUIDs The UUIDs of your bluloc beacons.
 * @param userToken With this user token it's possible to login an existing device user on another device or the same device after the app was re-installed.
 * @param successBlock Will be called if the @p state was updated to @p BLStateOffline or @p BLStateOnline.
 * @param errorBlock Will be called if the @p state was updated to @p BLStateNone.
 */
- (void)setupWithAppKey:(NSString*)appKey
              andSphere:(NSString*)sphere
         proximityUUIDs:(NSArray*)proximityUUIDs
              userToken:(nullable NSString*)userToken
           successBlock:(nullable void (^)(NSString *userToken))successBlock
             errorBlock:(nullable void (^)(BLError error, NSString *errorMessage))errorBlock;


#pragma mark - Scanning


/**
 *  Defines the time in seconds that is used to collect beacon data before analysing them.
 *  Default value are 5 seconds and the minimum scan period amounts to two seconds.
 *  The custom scan period has to be set before start scanning with the @p startScan method.
 */
@property (nonatomic) CGFloat scanPeriod;

/**
 * This method downloads the latest data asynchon from the server and starts scanning for nearby blulocs until @p stopScan was called.
 * To setup a custom scan period the property @p scanPeriod can be used.
 *
 * @b Background-Scanning
 *
 * To enable scanning in background it is necessary to enable the background capability "Location updates" for your app.
 * For iOS 8 you have also to add the @p NSLocationAlwaysUsageDescription key to the @p info.plist file with a suitable value.
 *
 * If the app is running in background, scanning is active and the device comes in the vicinity of one of your beacons a @p UILocalNotification will be shown on devices running iOS 7.
 * This @p UILocalNotification is used to encourage the user to open the app and background scanning can be activated.
 * For devices running iOS 8 or higher background scanning will just activated without such a @p UILocalNotification.
 *
 * @b Hint:
 *
 * To scan nearby blulocs it's necessary that the @p state is @p BLStateOffline or @p BLStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @return @p NO if the @p state is @p BLStateNone.
 */
- (BOOL)startScan;

/**
 * Stops the current scanning process.
 */
- (void)stopScan;

/**
 * Indicates whether the SDK is currently scanning for nearby blulocs.
 */
- (BOOL)isScanning;


#pragma mark - Background Scanning


/**
 *  If the app is running in background, scanning is active and the device comes in the vicinity of one of your beacons a @p UILocalNotification will be shown on devices running iOS 7.
 *  This @p UILocalNotification is used to encourage the user to open the app and background scanning can be activated.
 *  For devices running iOS 8 or higher backround scanning will just activated without such a @p UILocalNotification.
 *
 *  With this property the value of the @p UILocalNotification alert body can be adjusted.
 *  Default value is "Open the app to enable background mode".
 *
 *  @note This property isn't used on devices running iOS 8 or higher.
 */
@property (nonatomic, nullable) NSString *backgroundScanningNotificationBody;

/**
 *  If the app is running in background, scanning is active and the device comes in the vicinity of one of your beacons a @p UILocalNotification will be shown on devices running iOS 7.
 *  This @p UILocalNotification is used to encourage the user to open the app and background scanning can be activated.
 *  For devices running iOS 8 or higher backround scanning will just activated without such a @p UILocalNotification.
 *
 *  With this property the sound of the @p UILocalNotification can be adjusted.
 *  If not set the default system sound will be used.
 *
 *  @note This property isn't used on devices running iOS 8 or higher.
 *
 *  @see The @p soundName property of the @p UILocalNotification class for further information.
 */
@property (nonatomic, nullable) NSString *backgroundScanningNotificationSoundName;

/**
 *  If the app is running in background, scanning is active and the device comes in the vicinity of one of your beacons a @p UILocalNotification will be shown on devices running iOS 7.
 *  This @p UILocalNotification is used to encourage the user to open the app and background scanning can be activated.
 *  For devices running iOS 8 or higher background scanning will just activated without such a @p UILocalNotification.
 *
 *  With this property the value of the @p UILocalNotification alert action can be adjusted.
 *  Default value is "Open".
 *
 *  @note This property isn't used on devices running iOS 8 or higher.
 */
@property (nonatomic, nullable) NSString *backgroundScanningNotificationAction;

/**
 *  If the app is running in background, scanning is active and the device comes in the vicinity of one of your beacons a @p UILocalNotification will be shown on devices running iOS 7.
 *  This @p UILocalNotification is used to encourage the user to open the app and background scanning can be activated.
 *  For devices running iOS 8 or higher background scanning will just activated without such a @p UILocalNotification.
 *
 *  Background scanning will be deactivated after this amount of time in seconds if no own beacon was scanned.
 *  Default value are 600 seconds (10 minutes).
 *
 *  @note This property isn't used on devices running iOS 8 or higher.
 */
@property (nonatomic) NSTimeInterval backgroundScanningInactiveInterval;

/**
 *  This method checks if the device supports beacon region monitoring and background scanning can be used.
 *
 *  @return @p Yes, if background scanning is supported.
 */
+ (BOOL)backgroundScanningSupportedByDevice;


#pragma mark - Data Management


/**
 *  This method removes all cached data from the device.
 *  Furthermore it resets the connection to the server and stops the scanning process if started yet. 
 *  The @p state will be updated to @p BLStateNone.
 */
- (void)clearCachedData;

/**
 *  This method resets all properties to their default values.
 *  Furthermore it resets the connection to the server and stops the scanning process if started yet.
 *  The @p state will be updated to @p BLStateNone.
 */
- (void)resetToDefaultValues;

/**
 *  Method to force updating the data from the server on the device.
 *
 *  @b Hint:
 *
 *  To force updates it's necessary that the @p state is @p BLStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @param successBlock  Will be called if the content update was successful.
 * @param errorBlock    Will be called if the content update wasn't successful.
 *
 * @return @p NO if the @p state isn't @p BLStateOnline.
 */
- (BOOL)updateContentFromServerWithSuccessBlock:(nullable void (^)())successBlock
                                     errorBlock:(nullable void (^)(BLError error, NSString *errorMessage))errorBlock;


#pragma mark - Analytics


/**
 *  With this property the creation of new analytics events can be prevented.
 *  Any unsent events will also be deleted. As default analytics is enabled.
 */
@property (nonatomic, getter=isAnalyticsEnabeld) BOOL analyticsEnabled;

/**
 * With this method the action will be confirmed on the server.
 * Call this after e.g. the user pressed on the push notification.
 *
 * @note This method will be removed soon. Please use this method instead:
 * @code
 - (void)sendConfirmAnalyticsEventWithAction:(BLDeviceAction*)action;
 * @endcode
 *
 * @param action    The action that has to be confirmed.
 */
- (void)analyticsConfirmAction:(BLDeviceAction*)action __attribute__((deprecated(("Use the sendConfirmAnalyticsEventWithAction: method instead."))));

/**
 * With this method the action will be confirmed on the server.
 * Call this method after e.g. the user pressed on the push notification.
 *
 * @param action    The action which shall be confirmed.
 *
 * @return If the action has a capping and the maximum number is reached, @p NO will be returned, otherwise @p YES.
 */
- (BOOL)sendConfirmAnalyticsEventWithAction:(BLDeviceAction*)action;


#pragma mark - Custom Analytics Events


/**
 *  Returns all in the dashboard created custom analytics event types. 
 *  Use one of these objects to call the sendCustomAnalyticsEventWithAction:andType: method.
 *
 *  @return A list of BLCustomAnalyticsType objects.
 */
- (NSArray<BLCustomAnalyticsType*> *)customAnalyticsEventTypes;

/**
 * This method creates an analytics event on the server for a custom analytics type.
 * Call this method after e.g. the user pressed a like button.
 *
 * @param action    The action for which an event shall be created.
 * @param type      A custom event type which is defined in the dashboard.
 */
- (void)sendCustomAnalyticsEventWithAction:(BLDeviceAction*)action andType:(BLCustomAnalyticsType*)type;


#pragma mark - Free Analytics Events


/**
 *  This method allows you to send whatever type of analytics event with a custom amount of payload.
 *  Such events won't be appear in the dashboard. The payload must not be empty.
 *
 *  @param payload Customizable payload.
 */
- (void)sendFreeAnalyticsEventWithPayload:(NSDictionary*)payload;


#pragma mark - Navigation


/** 
 *  Calculates asynchron the shortest path from a given indoor position to a specified destination.
 *
 *  In order for the algorithm to find a route, the floors of the start and destination point have to match a floor that
 *  has navigation data stored on the bluloc server.
 *
 *  After the shortest route was calculated the successblock will be called on the main thread with an array containing @p BLIndoorWaypoint objects.
 *  The calculated route will also contain the start and destination coordinates.
 *
 *  If the algorithm wasn't able to find a suitable route or an error occurred the errorBlock will be called on the main thread with the appropriate error type and message.
 *
 * @b Hint:
 *
 *  To successfully calculate a route it's necessary that the @p state is @p BLStateOffline or @p BLStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @param startCoordinate           The geo-coordinates of the starting point.
 * @param startFloor                The floor of the starting point.
 * @param destinationCoordinate     The geo-coordinates of the destination.
 * @param destinationFloor          The floor of the destination.
 * @param successBlock              This block will be executed on the main thread if a route was found.
 * @param errorBlock                This block will be executed on the main thread if an error occurred.
 *
 */
- (void)navigationRouteFrom:(CLLocationCoordinate2D)startCoordinate
                    atFloor:(NSInteger)startFloor
                         to:(CLLocationCoordinate2D)destinationCoordinate
                    atFloor:(NSInteger)destinationFloor
               successBlock:(nullable void (^)(NSMutableArray *route))successBlock
                 errorBlock:(nullable void (^)(BLNavigationError error, NSString *errorMessage))errorBlock;

/**
 *  Calculates asynchron the shortest path from a given indoor position to a specified destination.
 *
 *  In order for the algorithm to find a route, the floors of the start and destination point have to match a floor that
 *  has navigation data stored on the bluloc server.
 *
 *  After the shortest route was calculated the successblock will be called on the main thread with an array containing @p BLIndoorWaypoint objects.
 *  The calculated route will also contain the start and destination coordinates.
 *
 *  If the algorithm wasn't able to find a suitable route or an error occurred the errorBlock will be called on the main thread with the appropriate error type and message.
 *
 * @b Hint:
 *
 *  To successfully calculate a route it's necessary that the @p state is @p BLStateOffline or @p BLStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey 
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 *
 * @param startLocation         The indoor location from which the navigation should start.
 * @param destinationLocation   The indoor location at which the navigation should end.
 * @param successBlock          This block will be executed on the main thread if a route was found.
 * @param errorBlock            This block will be executed on the main thread if an error occurred.
 *
 */
- (void)navigationRouteFrom:(BLIndoorLocation*)startLocation
                         to:(BLIndoorLocation*)destinationLocation
               successBlock:(nullable void (^)(NSMutableArray *route))successBlock
                 errorBlock:(nullable void (^)(BLNavigationError error, NSString *errorMessage))errorBlock;


#pragma mark - Positioning Configuration

/**
 *  If set to @p YES the location will jump only a maximum distance per second.
 *  This distance can be defined with the @p maximumMetersPerSecond parameter.
 *
 *  @note The default value is @p YES.
 */
@property (nonatomic, getter=isLimitToDistanceFilterEnabled) BOOL limitToDistanceFilterEnabled;

/**
 *  If set to a value greater than @p 0 and @p limitToDistanceFilterEnabled is @p YES 
 *  the location will jump only a maximum distance per second.
 *
 *  @note The default value is @p 1.7 which is the average speed of a person walking inside a building.
 */
@property (nonatomic) CGFloat maximumMetersPerSecond;

/**
 *  Set to @p YES if you want your locations to be snapped to the nearest navigation-path to smoothen the displayed results.
 *
 *  @note The default value is @p NO.
 *
 *  To use this feature you also have to upload a valid KML file with the navigation-path.
 */
@property (nonatomic, getter=isPathSnappingEnabled) BOOL pathSnappingEnabled;

/**
 *  If set to @p YES the @p CMMotionManager will be used to detect whether the user moves or not.
 *  If so, no new location will be calculated until the user rather the device is moving again.
 *  You can use the @p motionDetectorThreshold parameter to update the confidence threshold.
 *
 *  @note The default value is @p YES.
 *
 *  This might decrease performance and battery life, so only use if necessary!
 */
@property (nonatomic, getter=isMotionDetectorEnabled) BOOL motionDetectorEnabled;

/**
 *  Use this parameter to configure the confidence threshold for the movement detection.
 *  If set to @p 0 everything is detected as walking.
 *
 *  @note The default value is @p 0.025
 */
@property (nonatomic) CGFloat motionDetectorThreshold;

/**
 *  Set to @p YES to enable the sliding window approach for positioning.
 *
 *  @note The default value is @p NO.
 */
@property (nonatomic, getter=isSlidingWindowEnabled) BOOL slidingWindowEnabled;

#pragma mark - Outdoor Positioning

/**
 *  If you also want to retrieve outdoor position updates (GPS) set this property to @p YES.
 *  To receive location updates you have to implement this method of the @p BLBlulocControllerDelegate:
 * @code
        - (void)locationUpdated:(nullable BLLocation *)location;
 * @endcode
 *
 *  @note The @p state of SDK must be either @p BLStateOffline or @p BLStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey 
               andSphere:(NSString*)sphere 
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock 
              errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
 *  @endcode
 */
@property (nonatomic, getter=isOutdoorPositioningEnabled) BOOL outdoorPositioningEnabled;

/**
 *  Specify the desired outoor location accuracy. 
 *  Default value: @p kCLLocationAccuracyBest
 */
@property (nonatomic) CLLocationAccuracy outdoorPositioningAccuracy;

@end


#pragma mark - BLBlulocControllerDelegate


/**
 * Protocol that informs about scanned blulocs.
 */
@protocol BLBlulocControllerDelegate <NSObject>

@optional

/**
 * A nearby bluloc was scanned and triggered the given action.
 *
 * @param action    The action that is linked with the scanned bluloc.
 */
- (void)actionReceived:(BLDeviceAction*)action;

/**
 * This method informs about state changes of the bluetooth hardware. 
 * To check if bluetooth is either enabled or not the @p bluetoothEnabled property can be used.
 * This method will only be called if the target isn't the iphone simulator (@p TARGET_IPHONE_SIMULATOR not defined).
 */
- (void)bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState;

/**
 *  This method will be called after the authorization status has changed to @p kCLAuthorizationStatusDenied or @p kCLAuthorizationStatusRestricted.
 *  This happens when the user doesn't allow the usage of location services. The SDK will stop scanning.
 */
- (void)usageOfLocationServicesIsDenied;

/**
 *  When a new indoor location is detected, this method is triggered.
 *
 *  @param location     The new location.
 */
- (void)indoorLocationUpdated:(nullable BLIndoorLocation *)location  __attribute__((deprecated(("Use the locationUpdated: method instead"))));

/**
 *  Whenever a a new location (indoor or outdoor) was calculated this method will be triggered.
 *  A outdoor location will only be sent if @p outdoorPositioningEnabled is @p YES and no indoor location was calculated during the last three scan intervals.
 *
 *  @param location     The new location.
 */
- (void)locationUpdated:(nullable BLLocation *)location;

/**
 *  If no position could be calculated or is not pushed to the delegate for some reason,
 *  the delegate will be informed that beacons have been ranged and the device is still inside the "indoor-positioning-zone"
 */
- (void)didRangeBeacons;

/**
 *  Will be called after the latest data where dowloaded from the server.
 */
- (void)contentUpdated;

@end

NS_ASSUME_NONNULL_END
