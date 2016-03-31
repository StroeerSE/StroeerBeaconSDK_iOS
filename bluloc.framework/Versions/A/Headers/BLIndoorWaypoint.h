//
//  BLIndoorWaypoint.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import "BLIndoorLocation.h"

@interface BLIndoorWaypoint : BLIndoorLocation<NSCopying, NSCoding>

/**
 *  The shortest distance to the next (connected) waypoint.
 */
@property (nonatomic) double distanceToNextWP;

/**
 *  Helper method to create a new waypoint with data.
 *
 *  @param lat   Latitude of the waypoint
 *  @param lng   Longitude of the waypoint
 *  @param floor Floor of the waypoint
 *
 *  @return A new @p BLIndoorWaypoint object initialzed with the given values.
 */
+ (BLIndoorWaypoint*)waypointWithLatitude:(CLLocationDegrees)lat andLongitude:(CLLocationDegrees)lng andFloor:(NSInteger)floor;

/**
 * Calculates and returns the distance (in meters) from the receiver's location to the location of the specified waypoint.
 *
 * @param waypoint The other waypoint.
 *
 * @return The distance (in meters) between the two waypoints.
 */
- (CLLocationDistance)distanceToWaypoint:(BLIndoorWaypoint *)waypoint;


@end
