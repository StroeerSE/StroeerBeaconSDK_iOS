//
//  SPXError.h
//
//

#import <Foundation/Foundation.h>


/**
 * Possible errors that can occure in the SDK
 */
typedef NS_ENUM(NSInteger, SPXErrorCode)
{
    /**
     * An unknown error occurred.
     */
    SPXErrorCodeUnknown = 0,
    
    /**
     * The specified api key is invalid.
     */
    SPXErrorCodeInvalidApiKey,
    
    /**
     * The api key couldn't be validated or the operation couldn't be finished due to network errors.
     */
    SPXErrorCodeNetworkConnection,
    
    /**
     * This error is thrown if the device doesn't fully support ranging or monitoring for beacons.
     */
    SPXErrorCodeDeviceNotSupported,
    
    /**
     * This error is thrown if the bluetooth is not available.
     */
    SPXErrorCodeBluetoothNotAvailable,
    
    /**
     * This error is thrown if the user has denied the usage of the location services.
     */
    SPXErrorCodeLocationUsageDenied,
    
    /**
     * This error is thrown if the device doesn't support ranging for beacons.
     */
    SPXErrorCodeBeaconRangingNotSupported __deprecated_msg("Use SPXErrorCodeDeviceNotSupported instead.") = SPXErrorCodeDeviceNotSupported,
    
    /**
     * This error is thrown if the device isn't located in one of the supported countries.
     */
    SPXErrorCodeCurrentLocationNotSupported __deprecated_msg("There is no region check anymore.")
};

@interface SPXError : NSObject

@property (nonatomic, readonly) SPXErrorCode code;
@property (nonatomic, readonly) NSString *message;

- (id)initWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

+ (SPXError*)errorWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

@end
