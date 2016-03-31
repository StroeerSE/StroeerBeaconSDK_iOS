//
//  BLIndoorLocation.h
//
//  Copyright (c) 2015 match2blue GmbH. All rights reserved.
//

#import "BLLocation.h"

NS_ASSUME_NONNULL_BEGIN

__attribute__((deprecated(("Use BLUnknownLocationAccuracy instead"))))
static CLLocationAccuracy const BLUnknownIndoorLocationAccuracy = -1;

@interface BLIndoorLocation : BLLocation <NSCopying, NSCoding>

/**
 *  The floor of the location.
 */
@property (nonatomic) NSInteger floor;

/**
 *  Method to create a new indoor location.
 *
 *  @param lat   Latitude of the location
 *  @param lng   Longitude of the location
 *  @param floor Floor of the location
 *
 *  @return A new @p BLIndoorLocation object initialzed with the given values and a accuracy of @p BLUnknownLocationAccuracy.
 */
+ (BLIndoorLocation*)locationWithLatitude:(CLLocationDegrees)lat longitude:(CLLocationDegrees)lng floor:(NSInteger)floor;

/**
 *  Method to create a new indoor location.
 *
 *  @param lat      Latitude of the location
 *  @param lng      Longitude of the location
 *  @param floor    Floor of the location
 *  @param accuracy The estimated accuracy of the location.
 *
 *  @return A new @p BLIndoorLocation object initialzed with the given values.
 */
+ (BLIndoorLocation*)locationWithLatitude:(CLLocationDegrees)lat longitude:(CLLocationDegrees)lng floor:(NSInteger)floor accuracy:(CLLocationAccuracy)accuracy;

/**
 *  Calculates the distance between two indoor locations.
 *
 *  @param location another location
 *
 *  @return Returns the distance between two indoor locations.
 */
- (CLLocationDistance)distanceFromIndoorLocation:(BLIndoorLocation*)location __attribute__((deprecated(("Use distanceFromLocation: method instead"))));

/**
 *  Calculates the distance between a indoor location and a normal GPS location.
 *
 *  @param location A GPS location.
 *
 *  @return Returns the distance between two locations.
 */
- (CLLocationDistance)distanceFromGPSLocation:(CLLocation*)location __attribute__((deprecated(("Use distanceFromCLLocation: method instead"))));


@end

NS_ASSUME_NONNULL_END