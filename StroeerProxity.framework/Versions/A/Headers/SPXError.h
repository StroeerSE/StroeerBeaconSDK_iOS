//
//  SPXError.h
//  bluloc
//
//  Created by René Fischer on 08/07/16.
//  Copyright © 2016 match2blue. All rights reserved.
//

#import <Foundation/Foundation.h>


/**
 * Possible errors that can occure in the SDK
 */
typedef NS_ENUM(NSInteger, SPXErrorCode)
{
    /**
     * An unknown error occured.
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
     * This error is thrown if the device doesn't support ranging for beacons.
     */
    SPXErrorCodeBeaconRangingNotSupported,
    
    /**
     * This error is thrown if the user has denied the usage of the location services.
     */
    SPXErrorCodeLocationUsageDenied,
    
    /**
     * This error is thrown if the device isn't located in one of the supported countries.
     */
    SPXErrorCodeCurrentLocationNotSupported
};

@interface SPXError : NSObject

@property (nonatomic, readonly) SPXErrorCode code;
@property (nonatomic, readonly) NSString *message;

- (id)initWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

+ (SPXError*)errorWithCode:(SPXErrorCode)code andMessage:(NSString*)message;

@end
