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
@property (nonatomic, readonly, nonnull) NSString* dmc;

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

@end
