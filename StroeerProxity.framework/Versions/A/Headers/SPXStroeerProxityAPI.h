//
//  SPXStroeerProxityAPI.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreLocation/CoreLocation.h>
#import <UIKit/UIKit.h>

@class SPXBeacon;
@class SPXLocation;
@class SPXError;

@protocol SPXStroeerProxityAPIDelegate;

NS_ASSUME_NONNULL_BEGIN

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
     *  @p SPXStroeerProxityAPI validates the current region of the device.
     */
    SPXStateRegionCheck,

    /**
     *  Setup of the @p SPXStroeerProxityAPI was successful but the data isn't up to date. In this state scanning is possible.
     */
    SPXStateOffline,
    
    /**
     *  Setup of the @p SPXStroeerProxityAPI was successful and the data is up to date . In this state scanning is possible.
     */
    SPXStateOnline
};

#pragma mark - SPXStroeerProxityAPI

/**
 * Main class of the SDK with handles the authentication and scanning for nearby beacons.
 */
@interface SPXStroeerProxityAPI : NSObject

/**
 *  A key to verify your app to use this SDK.
 */
@property (nonatomic) NSString *apiKey;

/**
 * Delegate that informs about important changes in the SDK.
 */
@property (nonatomic) id<SPXStroeerProxityAPIDelegate> delegate;

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
 */
- (void)startScan;

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
 *  To force updates it's necessary that the @p state is @p SPXStateOnline.
 *
 * @param successBlock  Will be called if the content update was successful.
 * @param errorBlock    Will be called if the content update wasn't successful.
 *
 */
- (void)updateContentFromServerWithSuccessBlock:(nullable void (^)())successBlock
                                     errorBlock:(nullable void (^)(SPXError *error))errorBlock;


#pragma mark - Analytics


/**
 *  With this property the creation of new analytics events can be prevented.
 *  Any unsent events will also be deleted. As default analytics is enabled.
 */
@property (nonatomic, getter=isAnalyticsEnabeld) BOOL analyticsEnabled;


#pragma mark - Outdoor Positioning


/**
 *  Set to @p YES to enable the sliding window approach for positioning.
 *
 *  @note The default value is @p NO.
 */
@property (nonatomic, getter=isSlidingWindowEnabled) BOOL slidingWindowEnabled;

/**
 *  If you also want to retrieve outdoor position updates (GPS) set this property to @p YES.
 *  To receive location updates you have to implement this method of the @p SPXStroeerProxityAPIDelegate:
 * @code
        - (void)locationUpdated:(nullable SPXLocation *)location;
 * @endcode
 *
 *  @note The @p state of SDK must be either @p SPXStateOffline or @p SPXStateOnline.
 */
@property (nonatomic, getter=isOutdoorPositioningEnabled) BOOL outdoorPositioningEnabled;

/**
 *  Specify the desired outoor location accuracy. 
 *  Default value: @p kCLLocationAccuracyBest
 */
@property (nonatomic) CLLocationAccuracy outdoorPositioningAccuracy;


#pragma mark - Logging

/**
 * Enable or disables the logging to a log file (NSDocumentDirectory/spx.log). Default = NO
 */
@property (nonatomic, getter=isFileLoggingEnabled) BOOL fileLoggingEnabled;

/**
 * Returns the full path and name to the logfile.
 */
@property (nonatomic, readonly) NSString *logFile;

/**
 * Deletes the create logfile.
 */
- (void)deleteLogFile;

@end


#pragma mark - SPXStroeerProxityAPIDelegate


/**
 * Protocol that informs about important changes in the SDK.
 */
@protocol SPXStroeerProxityAPIDelegate <NSObject>

@optional

/**
 * Informs the delegate about an occured error. In case of a critical error (e.g. invalid Api-Key)
 * the SDK will stop scanning automatically.
 *
 * @param spxAPi The current instance of the api.
 * @param error The occured error.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didFailWithError:(SPXError*)error;

/**
 *  This method informs about state changes of the SDK.
 *
 *  @param oldState The old state of the SDK.
 *  @param newState The new state of the SDK.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi sdkStateChangedFromState:(SPXState)oldState toState:(SPXState)newState;

/**
 * This method informs about state changes of the bluetooth hardware. 
 * To check if bluetooth is either enabled or not the @p bluetoothEnabled property can be used.
 * This method will only be called if the target isn't the iphone simulator (@p TARGET_IPHONE_SIMULATOR not defined).
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState;

/**
 *  This method will be called after the authorization status has changed to @p kCLAuthorizationStatusDenied or @p kCLAuthorizationStatusRestricted.
 *  This happens when the user doesn't allow the usage of location services. The SDK will stop scanning.
 */
- (void)stroeerProxityAPIUsageOfLocationServicesDenied:(SPXStroeerProxityAPI*)spxAPi;

/**
 *  Whenever a a new outdoor location was calculated this method will be triggered.
 *  A outdoor location will only be sent if @p outdoorPositioningEnabled is @p YES.
 *
 *  @param location     The new location.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi locationUpdated:(nullable SPXLocation *)location;

/**
 *  Informs the delegate which beacons were ranged during the last scan interval.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didScanBeacons:(nullable NSMutableArray<SPXBeacon*>*)scannedBeacons;

/**
 *  Will be called if a bluloc zone analytics event was successfully sent to the backend.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon;

/**
 *  Will be called after the latest data where downloaded from the server.
 *
 *  @param numberOfBeacons The number of beacons downloaded from the server.
 */
- (void)stroeerProxityAPIContentUpdated:(SPXStroeerProxityAPI*)spxAPi numberOfBeacons:(NSInteger)numberOfBeacons;

@end

NS_ASSUME_NONNULL_END
