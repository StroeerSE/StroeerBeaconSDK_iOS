//
//  SPXStroeerProxityAPI.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreLocation/CoreLocation.h>
#import <UIKit/UIKit.h>

@class SPXBeacon;
@class SPXCustomAnalyticsType;
@class SPXDeviceAction;
@class SPXLocation;

@protocol SPXStroeerProxityAPIDelegate;

NS_ASSUME_NONNULL_BEGIN

/**
 * Possible errors that can occure in the SDK
 */
typedef NS_ENUM(NSInteger, SPXError)
{
    /**
     * An unknown error occured.
     */
    SPXErrorUnknown = 0,
    
    /**
     * The specified app key or sphere isn't valid.
     */
    SPXErrorInvalidAppKeyOrSphere,
    
    /**
     * The specified app key is deactivated.
     */
    SPXErrorInactiveAppKey,
    
    /**
     * The app key and sphere couldn't be validated or the operation couldn't be finished due to network errors.
     */
    SPXErrorNetworkConnection,
    
    /**
     * This error is thrown during the setup if the device doesn't support ranging for beacons.
     */
    SPXErrorBeaconRangingNotSupported,
};

/**
 * Possible states in which the controller can be. Depending on the state scanning for beacons is possible or not.
 */
typedef NS_ENUM(NSInteger, SPXState)
{
    /**
     *  @p SPXStroeerProxityAPI hasn't been setup yet. In this state scanning isn't possible.
     */
    SPXStateNone = 0,
    
    /**
     *  Setup of the @p SPXStroeerProxityAPI was successful but the data isn't up to date. In this state scanning is possible.
     */
    SPXStateOffline,
    
    /**
     *  Setup of the @p SPXStroeerProxityAPI was successful and the data is up to date . In this state scanning is possible.
     */
    SPXStateOnline
};

/**
 * Enum to specify the download strategy.
 */
typedef NS_ENUM(NSInteger, SPXDownloadStrategy)
{
    /**
     *  Download all content (beacons, actions, ...).
     */
    SPXDownloadStrategyAll = 0,
    
    /**
     * Download only content (beacons, actions, ...) which is in your vicinity.
     */
    SPXDownloadStrategyAroundMe
};


#pragma mark - SPXStroeerProxityAPI


/**
 * Main class of the SDK with handles the authentication and scanning for nearby beacons.
 */
@interface SPXStroeerProxityAPI : NSObject

/**
 * Delegate that informs about triggered actions.
 */
@property (nonatomic) id<SPXStroeerProxityAPIDelegate> delegate;

/**
    @brief Defines how the content should be downloaded.
    You can choose between two strategies: <i>SPXDownloadStrategyAll</i> and <i>SPXDownloadStrategyAroundMe</i>.
 
    @discussion The <b>SPXDownloadStrategyAll</b> strategy is the default approach which will download all relevant data from the server.
    This strategy is recommended if you have only a small amount of beacons and actions.
 
    The <b>SPXDownloadStrategyAroundMe</b> strategy should be used if you have a lot of beacons and actions.
    With this approach every beacon and all associated actions will be
    downloaded in a certain radius (1 km) around the current user location.
    Region Monitoring is used to determine the location of the user.
 
    @remark For proper function of the <b>SPXDownloadStrategyAroundMe</b> strategy you have to
 
    - set a position for every beacon.
 
    - enable the background capability "Location updates" for your app.
 
    - add the <i>NSLocationAlwaysUsageDescription</i> key to the <i>info.plist</i> file with a suitable value (iOS 8+).
 
    - add the <i>NSLocationUsageDescription</i> key to the <i>info.plist</i> file with a suitable value (iOS 7).
 
    <b>Default value</b>: <i>SPXDownloadStrategyAll</i>.
 
    @remark Has to be called before on of the @c - @c(void)setupWithAppKey:andSphere:... methods.
 */
@property (nonatomic) SPXDownloadStrategy downloadStrategy;

/**
 *  Returns the current state of the @p SPXStroeerProxityAPI. The state will be updated during the setup process or in the @p updateContentFromServer method. Default value is @p SPXStateNone
 */
@property (nonatomic, readonly) SPXState state;

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
 *  Returns the singleton instance of the @p SPXStroeerProxityAPI class. Use this method instead of @p init or @p new.
 *
 *  @return A @p SPXStroeerProxityAPI singleton instance.
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
 * Method to setup the @p SPXStroeerProxityAPI with multiple proximityUUIDs. It is necessary to call this method before starting the scanning process.
 * The method returns immediately because the given @p appKey and @p sphere will be checked asynchronously.
 *
 * On the first successful call this method will generate a new user token. After this the generated user token will be reused.
 *
 * @note The @p successBlock will be called if
 * - the setup was successful and data could read from the server. The @p state will be updated to @p SPXStateOnline.
 * - internet connection isn't available but the setup process and content update was successful previously. 
 *      The @p state will be updated to @p SPXStateOffline. After the internet connection is back the @p SPXStroeerProxityAPI will try updating the data automatically. 
 *      If this succeeds the @p state will be updated to @p SPXStateOnline.
 *
 * @note The @p errorBlock will be called if
 * - an unexpected error occurred during the setup or content loading (e.g. invalid app key or sphere). The @p state will be updated to @p SPXStateNone.
 * - there wasn't a successful setup yet and a internet connection isn't available. The @p state will be updated to @p SPXStateNone.
 *
 * @param appKey            A key to verify your app to use this framework.
 * @param sphere            A specific value defined for the current client.
 * @param proximityUUIDs    The UUIDs of your beacons.
 * @param successBlock      Will be called if the @p state was updated to @p SPXStateOffline or @p SPXStateOnline.
 * @param errorBlock        Will be called if the @p state was updated to @p SPXStateNone.
 */
- (void)setupWithAppKey:(NSString*)appKey
              andSphere:(NSString*)sphere
         proximityUUIDs:(NSArray*)proximityUUIDs
           successBlock:(nullable void (^)(NSString *userToken))successBlock
             errorBlock:(nullable void (^)(SPXError error, NSString *errorMessage))errorBlock;

/**
 * Method to setup the @p SPXStroeerProxityAPI with multiple proximityUUIDs. It is necessary to call this method before starting the scanning process.
 * The method returns immediately because the given @p appKey and @p sphere will be checked asynchronously.
 *
 * This method won't generate a new user token. The SDK will try to validate the specified user token. If this token is invalid the setup fails.
 *
 * @note The @p successBlock will be called if
 * - the setup was successful and data could read from the server. The @p state will be updated to @p SPXStateOnline.
 * - internet connection isn't available but the setup process and content update was successful previously. 
 *      The @p state will be updated to @p SPXStateOffline. After the internet connection is back the @p SPXStroeerProxityAPI will try updating the data automatically. 
 *      If this succeeds the @p state will be updated to @p SPXStateOnline.
 *
 * @note The @p errorBlock will be called if
 * - an unexpected error occurred during the setup or content loading (e.g. invalid app key or sphere). The @p state will be updated to @p SPXStateNone.
 * - there wasn't a successful setup yet and a internet connection isn't available. The @p state will be updated to @p SPXStateNone.
 *
 * @param appKey A key to verify your app to use this framework.
 * @param sphere A specific value defined for the current client.
 * @param proximityUUIDs The UUIDs of your beacons.
 * @param userToken With this user token it's possible to login an existing device user on another device or the same device after the app was re-installed.
 * @param successBlock Will be called if the @p state was updated to @p SPXStateOffline or @p SPXStateOnline.
 * @param errorBlock Will be called if the @p state was updated to @p SPXStateNone.
 */
- (void)setupWithAppKey:(NSString*)appKey
              andSphere:(NSString*)sphere
         proximityUUIDs:(NSArray*)proximityUUIDs
              userToken:(nullable NSString*)userToken
           successBlock:(nullable void (^)(NSString *userToken))successBlock
             errorBlock:(nullable void (^)(SPXError error, NSString *errorMessage))errorBlock;


#pragma mark - Scanning


/**
 *  Defines the time in seconds that is used to collect beacon data before analysing them.
 *  Default value are 5 seconds and the minimum scan period amounts to two seconds.
 *  The custom scan period has to be set before start scanning with the @p startScan method.
 */
@property (nonatomic) CGFloat scanPeriod;

/**
 * This method downloads the latest data asynchon from the server and starts scanning for nearby beacons until @p stopScan was called.
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
 * To scan nearby beacons it's necessary that the @p state is @p SPXStateOffline or @p SPXStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(SPXError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @return @p NO if the @p state is @p SPXStateNone.
 */
- (BOOL)startScan;

/**
 * Stops the current scanning process.
 */
- (void)stopScan;

/**
 * Indicates whether the SDK is currently scanning for nearby beacons.
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
 *  The @p state will be updated to @p SPXStateNone.
 */
- (void)clearCachedData;

/**
 *  This method resets all properties to their default values.
 *  Furthermore it resets the connection to the server and stops the scanning process if started yet.
 *  The @p state will be updated to @p SPXStateNone.
 */
- (void)resetToDefaultValues;

/**
 *  Method to force updating the data from the server on the device.
 *
 *  @b Hint:
 *
 *  To force updates it's necessary that the @p state is @p SPXStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey
               andSphere:(NSString*)sphere
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock
              errorBlock:(void (^)(SPXError error, NSString *errorMessage))errorBlock;
 *  @endcode
 *
 * @param successBlock  Will be called if the content update was successful.
 * @param errorBlock    Will be called if the content update wasn't successful.
 *
 * @return @p NO if the @p state isn't @p SPXStateOnline.
 */
- (BOOL)updateContentFromServerWithSuccessBlock:(nullable void (^)())successBlock
                                     errorBlock:(nullable void (^)(SPXError error, NSString *errorMessage))errorBlock;


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
 - (void)sendConfirmAnalyticsEventWithAction:(SPXDeviceAction*)action;
 * @endcode
 *
 * @param action    The action that has to be confirmed.
 */
- (void)analyticsConfirmAction:(SPXDeviceAction*)action __attribute__((deprecated(("Use the sendConfirmAnalyticsEventWithAction: method instead."))));

/**
 * With this method the action will be confirmed on the server.
 * Call this method after e.g. the user pressed on the push notification.
 *
 * @param action    The action which shall be confirmed.
 *
 * @return If the action has a capping and the maximum number is reached, @p NO will be returned, otherwise @p YES.
 */
- (BOOL)sendConfirmAnalyticsEventWithAction:(SPXDeviceAction*)action;


#pragma mark - Custom Analytics Events


/**
 *  Returns all in the dashboard created custom analytics event types. 
 *  Use one of these objects to call the sendCustomAnalyticsEventWithAction:andType: method.
 *
 *  @return A list of SPXCustomAnalyticsType objects.
 */
- (NSArray<SPXCustomAnalyticsType*> *)customAnalyticsEventTypes;

/**
 * This method creates an analytics event on the server for a custom analytics type.
 * Call this method after e.g. the user pressed a like button.
 *
 * @param action    The action for which an event shall be created.
 * @param type      A custom event type which is defined in the dashboard.
 */
- (void)sendCustomAnalyticsEventWithAction:(SPXDeviceAction*)action andType:(SPXCustomAnalyticsType*)type;


#pragma mark - Free Analytics Events


/**
 *  This method allows you to send whatever type of analytics event with a custom amount of payload.
 *  Such events won't be appear in the dashboard. The payload must not be empty.
 *
 *  @param payload Customizable payload.
 */
- (void)sendFreeAnalyticsEventWithPayload:(NSDictionary*)payload;


/**
 *  Set to @p YES to enable the sliding window approach for positioning.
 *
 *  @note The default value is @p NO.
 */
@property (nonatomic, getter=isSlidingWindowEnabled) BOOL slidingWindowEnabled;
#pragma mark - Outdoor Positioning

/**
 *  If you also want to retrieve outdoor position updates (GPS) set this property to @p YES.
 *  To receive location updates you have to implement this method of the @p SPXStroeerProxityAPIDelegate:
 * @code
        - (void)locationUpdated:(nullable SPXLocation *)location;
 * @endcode
 *
 *  @note The @p state of SDK must be either @p SPXStateOffline or @p SPXStateOnline. The state can be updated with this method:
 *  @code
 - (void)setupWithAppKey:(NSString*)appKey 
               andSphere:(NSString*)sphere 
          proximityUUIDs:(NSArray*)proximityUUIDs
            successBlock:(void (^)())successBlock 
              errorBlock:(void (^)(SPXError error, NSString *errorMessage))errorBlock;
 *  @endcode
 */
@property (nonatomic, getter=isOutdoorPositioningEnabled) BOOL outdoorPositioningEnabled;

/**
 *  Specify the desired outoor location accuracy. 
 *  Default value: @p kCLLocationAccuracyBest
 */
@property (nonatomic) CLLocationAccuracy outdoorPositioningAccuracy;

@end


#pragma mark - SPXStroeerProxityAPIDelegate


/**
 * Protocol that informs about scanned beacons.
 */
@protocol SPXStroeerProxityAPIDelegate <NSObject>

@optional

/**
 * A nearby beacon was scanned and triggered the given action.
 *
 * @param action    The action that is linked with the scanned beacon.
 */
- (void)actionReceived:(SPXDeviceAction*)action;

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
 *  Whenever a a new outdoor location was calculated this method will be triggered.
 *  A outdoor location will only be sent if @p outdoorPositioningEnabled is @p YES.
 *
 *  @param location     The new location.
 */
- (void)locationUpdated:(nullable SPXLocation *)location;

/**
 *  Informs the delegate that beacons were ranged during the last scan interval.
 */
- (void)didRangeBeacons;

/**
 *  Will be called after the latest data where downloaded from the server.
 */
- (void)contentUpdated;

@end

NS_ASSUME_NONNULL_END
