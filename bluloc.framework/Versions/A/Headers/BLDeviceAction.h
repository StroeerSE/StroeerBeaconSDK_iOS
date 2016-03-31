//
//  BLDeviceAction.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

@class BLBeacon;

#import "BLActionActivity.h"

typedef NS_ENUM(int, BLActionProximityTrigger) {
    BLActionProximityTriggerImmediate = 0,
    BLActionProximityTriggerNear,
    BLActionProximityTriggerFar
};

/**
 * This class represents an action that should show a local push notificaiton.
 */
@interface BLDeviceAction : NSObject<NSCopying, NSCoding>

/**
 * The name of the action.
 */
@property (nonatomic, readonly) NSString *name;

/**
 * The plain description of the action.
 */
@property (nonatomic, readonly) NSString *plainDescription;

/**
 * The data of the action.
 */
@property (nonatomic, readonly) NSDictionary *data;

/**
 * The type of the action.
 */
@property (nonatomic, readonly) NSString *type;

/**
 * The activity of the action.
 */
@property (nonatomic, readonly) BLActionActivity actionActivity;

/**
 * The linked bluloc that triggered this action.
 */
@property (nonatomic, readonly) BLBeacon *bluloc;

/**
 *  The proximity trigger for this action set in the dashboard.
 *  Can be either immediate, near or far.
 */
@property (nonatomic, readonly) BLActionProximityTrigger proximityTrigger;

/**
 * Helper method which returns the current content of the instance as dictionary.
 *
 * @return Returns a dictionary whose keys matches the property names.
 */
- (NSDictionary*)toDictionary;

@end
