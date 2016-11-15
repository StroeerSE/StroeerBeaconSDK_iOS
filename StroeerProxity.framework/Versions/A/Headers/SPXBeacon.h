//
//  SPXBeacon.h
//
//

#import <Foundation/Foundation.h>


/**
 * Class that represents a beacon.
 */
@interface SPXBeacon : NSObject <NSCoding, NSCopying>

/**
 * The UUID of the beacon.
 */
@property (nonatomic, readonly, nonnull) NSString* uuid;

/**
 * The major value of the beacon.
 */
@property (nonatomic, readonly) NSInteger major;

/**
 * The minor value of the beacon.
 */
@property (nonatomic, readonly) NSInteger minor;

/**
 * The beaconId identifies a beacon in the following format 'uuid:major:minor'
 */
- (nonnull NSString*)beaconId;

@end
