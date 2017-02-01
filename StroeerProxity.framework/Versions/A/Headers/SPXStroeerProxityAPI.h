//
//  SPXStroeerProxityAPI.h
//
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
 * Possible states in which the controller can be.
 */
typedef NS_ENUM(NSInteger, SPXState)
{
    /**
     *  The SDK does not scan for nearby beacons.
     */
    SPXStateNone = 0,
    
    /**
     * In this state the SDK checks if bluetooth turned on.
     */
    SPXStateBluetoothCheck,
    
    /**
     * In this state the specified API Key will be verified.
     */
    SPXStateAPIKeyValidation,

    /**
     * The SDK scans for nearby beacons.
     */
    SPXStateScanning
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
 *  Returns the current state of the @p SPXStroeerProxityAPI.
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
 * Determines wheter the device is supported by the SDK or not.
 *
 *  @return @p Yes, if the device is supported and can be used for scanning. 
 */
+ (BOOL)isCurrentDeviceSupported;

/**
 * This method starts scanning for nearby beacons until @p stopScan was called.
 *
 * @b Background-Scanning
 *
 * To enable scanning in background it is necessary to enable the background capability "Location updates" for your app.
 * For iOS 8 you have also to add the @p NSLocationAlwaysUsageDescription key to the @p info.plist file with a suitable value.
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


#pragma mark - Data Management


/**
 * This method removes all cached data from the device.
 * Furthermore it resets the connection to the server and stops the scanning process if started yet.
 * The @p state will be updated to @p SPXStateNone.
 */
- (void)clearCachedData;


#pragma mark - Analytics


/**
 * With this property the creation of new analytics events can be prevented.
 * Any unsent events will also be deleted. As default analytics is enabled.
 */
@property (nonatomic, getter=isAnalyticsEnabled) BOOL analyticsEnabled;

/**
 * If set, the custom advertising identifier will be added to each analytics event.
 * This can be used to identify the user across different apps.
 */
@property (nonatomic, nullable) NSString *customAdvertisingId;

/**
 * If set, the The Advertising Identifier (IDFA) from iOS will be added to each analytics event.
 * Default value is NO.
 *
 * During the Submitting the App you have to answer questions about the IDFA. Indicate whether your app uses the Advertising Identifier, and if so, in what way. 
 * If you checked No but Apple determine your app does use IDFA, your app will be put into the Invalid Binary status, and you will be notified by email.
 * Similarly, if you checked Yes but your app uses IDFA in ways that don’t match the statements you checked, your app will be rejected by App Review and put into the Rejected status.
 * In either case, when you resubmit the build, you will be presented with the IDFA questions again and can provide the appropriate answers.
 *
 * See The Advertising Identifier (IDFA) for more details about this step:
 *
 *   https://developer.apple.com/library/content/documentation/LanguagesUtilities/Conceptual/iTunesConnect_Guide/Chapters/SubmittingTheApp.html#//apple_ref/doc/uid/TP40011225-CH33-SW8
 */
@property (nonatomic, getter=isAdvertisingTrackingEnabled) BOOL advertisingTrackingEnabled;

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


#pragma mark - Helper


/**
 * Converts the given SPXState to a String.
 */
+ (NSString*)stringFromSPXState:(SPXState)state;

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
 *  Informs the delegate that a beacon was entered.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didEnterBeacon:(SPXBeacon*)beacon;

/**
 *  Informs the delegate that a beacon was left.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didExitBeacon:(SPXBeacon*)beacon;

/**
 *  Will be called if a zone event was successfully sent to the backend.
 */
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon;

/**
 * An error occured while downloading the decryption keys from the backend. Crypto V2 Beacons can not be used.
 */
- (void)loadingBeaconDecryptionKeysFailedForStroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi;

/**
 * The decryption keys were downloaded successful from the backend. Crypto V2 Beacons can be used.
 */
- (void)loadingBeaconDecryptionKeysSucceededForStroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi;

@end

NS_ASSUME_NONNULL_END
