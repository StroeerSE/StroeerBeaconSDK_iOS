//
//  SPXBeacon.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import <Foundation/Foundation.h>


/**
 * Class that represents a beacon.
 */
@interface SPXBeacon : NSObject <NSCoding, NSCopying>

/**
 * The name / identifier of the beacon.
 */
@property (nonatomic, readonly, nonnull) NSString* name;

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
 *  The latitiude of the beacon.
 */
@property (nonatomic, readonly) double latitude;

/**
 *  The longitude of the beacon.
 */
@property (nonatomic, readonly) double longitude;


/**
 * Helper method which returns the current content of the instance as dictionary.
 *
 * @return Returns a dictionary whose keys matches the property names.
 */
- (nonnull NSDictionary*)toDictionary;

@end
