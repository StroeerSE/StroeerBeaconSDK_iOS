//
//  BLBeacon.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import <Foundation/Foundation.h>


/**
 * Class that represents a bluloc.
 */
@interface BLBeacon : NSObject <NSCoding, NSCopying>

/**
 * The name / identifier of the bluloc.
 */
@property (nonatomic, readonly, nonnull) NSString* name;

/**
 * The UUID of the bluloc.
 */
@property (nonatomic, readonly, nonnull) NSString* uuid;

/**
 * The major value of the bluloc.
 */
@property (nonatomic, readonly) NSInteger major;

/**
 * The minor value of the bluloc.
 */
@property (nonatomic, readonly) NSInteger minor;



/**
 *  The latitiude of the bluloc.
 */
@property (nonatomic, readonly) double latitude;

/**
 *  The longitude of the bluloc.
 */
@property (nonatomic, readonly) double longitude;

/**
 *  The floor of the bluloc.
 */
@property (nonatomic, readonly) NSInteger floor;



/**
 * The bluloc to be used for indoor navigation.
 */
@property (nonatomic, readonly) BOOL useForIndoorNavigation;



/**
 * Helper method which returns the current content of the instance as dictionary.
 *
 * @return Returns a dictionary whose keys matches the property names.
 */
- (nonnull NSDictionary*)toDictionary;

@end
