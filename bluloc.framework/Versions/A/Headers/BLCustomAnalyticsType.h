//
//  BLCustomAnalyticsType.h
//
//  Copyright © 2015 match2blue. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *  This class represents a custom analytics type created in the bluloc dashboard.
 *  Use instances of this class to send custom analytics events to the database.
 */
@interface BLCustomAnalyticsType : NSObject

/**
 *  Name of the custom analytics type.
 */
@property (nonatomic, readonly) NSString *name;

/**
 *  Type-ID of the custom analytics type.
 */
@property (nonatomic, readonly) NSString *type;


@end
