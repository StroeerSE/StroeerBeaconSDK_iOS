## Ströer Proxity SDK

### Installation with CocoaPods

[CocoaPods](http://cocoapods.org) is a dependency manager for Objective-C, which simplifies the process of using 3rd-party libraries in projects. You can install it with the following command:

```bash
$ gem install cocoapods
```

> CocoaPods 0.39.0+ is required

To integrate the Ströer Proxity SDK into your Xcode project, specify it in your `Podfile`:

```ruby
source 'https://github.com/StroeerSE/StroeerBeaconSDK_iOS_Pods.git'
source 'https://github.com/CocoaPods/Specs.git'

platform :ios, "7.1"

pod 'stroeer-proxity', '~> 1.6'
```

Then, run the following command:

```bash
$ pod install
```

### Setup your App for background Scanning
With region monitoring and background location usage scanning for beacons is also possible while your app is running in background. If you call `startScan` a `CLBeaconRegion` with the UUID of your blulocs will be monitored. After the user or rather the device enters this region different things could happen dependent on the status of the app and the iOS version.

**Please note** that it's necessary to enable the background capability "Location updates" for your app and for iOS 8 you also have to add the `NSLocationAlwaysUsageDescription` key to your `info.plist` file with a suitable value. Otherwise scanning in background won't be possible.
Ensure also that you comply to the latest App Store Review Guidelines:
> 4.5 Apps using background location services must provide a reason that clarifies the purpose of the use, using mechanisms described in the Human Interface Guideline

#### iOS 7.1
If the app is running in foreground location updates will be turned on to enable background scanning. Otherwise a `UILocalNotification` will be presented. The primary function of the last one is to encourage the user to open the app because on iOS 7.1 it isn't possible to turn on location updates from the background. If he do so location updates will be turned on as if the app was running in foreground while he enters the region. The `UILocalNotification` can be changed with these properties of the `SPXStroeerProxityAPI` class:

```objective-c
@property (nonatomic) NSString *backgroundScanningNotificationBody;
@property (nonatomic) NSString *backgroundScanningNotificationSoundName;
@property (nonatomic) NSString *backgroundScanningNotificationAction;
```

If the device leaves the region background scanning will be deactivated after a specific amount of time. The default time interval is ten minutes and can be adjusted with this property of the `SPXStroeerProxityAPI` class:

```objective-c
@property (nonatomic) NSTimeInterval backgroundScanningInactiveInterval;
```

#### iOS 8+
Since iOS 8 it's possible to activate location updates not only if the app is running in foreground but rather also from the background. Therefore no `UILocalNotification` will be shown if the user enters a region. Background location updates will just be turned on. If the device leaves the region background location updates will be turned off immediately and not after a specific amount of time.

### Setup the Ströer Proxity SDK

#### Set the Delegate (optional)
In order to receive notifications from the SDK you have to implement the `SPXStroeerProxityAPIDelegate` protocol. Furthermore you have to set the delegate for the `SPXStroeerProxityAPI` class.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
```

#### Change the download strategy
Due to the fact that not all beacon information shall be downloaded from the backend but just those which are near the device you have to set the download strategy to `SPXDownloadStrategyAroundMe`.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setDownloadStrategy:SPXDownloadStrategyAroundMe];
```

#### Authorize your App
The next step is to authorize your app against the backend and download all relevant data onto the device. This process is running asynchronously on another thread.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setupWithAppKey:@"1c2cea768b5c703fe01458ce67526b3e1cbd3cf5"
                                             andSphere:@"Stroeer"
                                        proximityUUIDs:@[@"37ecc5b5-dc9f-4d5c-b1ee-7f09146572a2"]
                                          successBlock:^(NSString *userToken) {

                              // Now we can start scanning for nearby beacons                              

                          } errorBlock:^(SPXError error, NSString *errorMessage) {

                              // Something went wrong. You can check the error type and message for further information.
                              switch (error)
                              {
                                  case SPXErrorUnknown:
                                      NSLog(@"An unknown error occurred: %@", errorMessage);
                                      break;
                                  case SPXErrorInvalidAppKeyOrSphere:
                                      NSLog(@"The provided app key or sphere name is invalid: %@", errorMessage);
                                      break;
                                  case SPXErrorNetworkConnection:
                                      NSLog(@"A problem with the network connection occurred: %@", errorMessage);
                                      break;
                                  case SPXErrorBeaconRangingNotSupported:
                                      NSLog(@"The current device doesn't support ranging for beacons: %@", errorMessage);
                                      break;
                                  case SPXErrorInactiveAppKey:
                                      NSLog(@"The specified app key is not activated.");
                                      break;
                              }
                          }];
```

To identify the current user you will receive a User-Token on the first setup call. The User-Token won't change unless the user deletes the app, you call the clearCache method or setup the SDK with a specific token.

#### Start Scanning
The last step is to start scanning for nearby beacons:

```objective-c
[[SPXStroeerProxityAPI sharedInstance] startScan];
```

The SDK now scans for beacons near you and its scanning property will be set to true. This way you can find out whether the SDK is currently scanning or not. Since the SDK is scanning for nearby beacons, you might get notifications from it fairly soon.

#### Stop Scanning

When you're done with scanning, you simply call:
[[BLBlulocController sharedInstance] stopScan];

___

### SDK State
There are three possible states in which the SDK can be: `BLStateNone`, `BLStateOffline` and `BLStateOnline`. Dependent on the state scanning for near blulocs is possible or not.

#### BLStateNone
This is the default state before you have done anything. In this state scanning isn't possible and no delegate method will be called. To switch to another state you have to successfully call the `setupWithAppKey:andSphere:proximityUUID:successBlock:errorBlock:` method.

#### BLStateOffline
The offline state means that you've successfully called the setup method previously. Scanning is now possible but maybe you don't have the latest data from the server. To get into the online state you have to call the `setupWithAppKey:andSphere:proximityUUID:successBlock:errorBlock:` method until it was successful. Please note that the `setupWithAppKey...` method is also successful if the state becomes `BLStateOffline`.

#### BLStateOnline
This state means that you have successfully authenticated against the server, the latest data was downloaded and scanning is possible.

___

### Scan Period
It's possible to define a period of time which is used to collect raw beacon data before they will be analysed:

```objective-c
@property (nonatomic) CGFloat scanPeriod;
```

The default value is five seconds. In this time the SDK will collect all scanned beacon informations in your vicinity. After this time span you get informed if there were any results.

___

### Update data
At each time one of the setup methods is called the SDK will update the local data with the latest data on the server. If you want to update the data manually you can use this method:

```objective-c
- (BOOL)updateContentFromServerWithSuccessBlock:(void (^)())successBlock
                                     errorBlock:(void (^)(BLError error, NSString *errorMessage))errorBlock;
```

___

### Bluetooth hardware state

The current state of the bluetooth hardware is stored in the `bluetoothState` property of the `SPXStroeerProxityAPI` class and the `SPXStroeerProxityAPIDelegate` protocol informs you about changes of it:

```objective-c
@property (nonatomic, readonly) CBCentralManagerState bluetoothState;

- (void)bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState;
```

If you implement this method, you could for example notify the user about the bluetooth state using a popup or whatever method you prefer.

___

### Location authorisation state
If the user of your app has denied the usage of location services you get informed about this change with this `SPXStroeerProxityAPIDelegate` protocol method:

```objective-c
- (void)usageOfLocationServicesIsDenied;
```

Scanning for blulocs will stop immediately after the user denies the usage of location services.
