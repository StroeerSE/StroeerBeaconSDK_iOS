//
//  SPXError.h
//
//

#import <Foundation/Foundation.h>


/**
 * Possible errors that can occur in the SDK
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
    SPXErrorCodeNetworkConnection __attribute__ ((deprecated)),
    
    /**
     * This error is thrown if the device doesn't fully support ranging or monitoring for beacons.
     */
    SPXErrorCodeDeviceNotSupported,
    
    /**
     * This error is thrown if the bluetooth is not available.
     */
    SPXErrorCodeBluetoothNotAvailable,

    /**
     * This error is thrown if bluetooth was deactivated on the device during scanning
     */
     SPXErrorCodeBluetoothDeactivated,
    
    /**
     * This error is thrown if the user has denied the usage of the location services.
     */
    SPXErrorCodeLocationUsageDenied __attribute__ ((deprecated))
};

@interface SPXError : NSObject

@property (nonatomic, readonly) SPXErrorCode code;
@property (nonatomic, readonly) NSString *message;

- (id)initWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

+ (SPXError*)errorWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

@end
