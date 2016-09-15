## Ströer Proxity SDK

### Installation with CocoaPods

[CocoaPods](http://cocoapods.org) is a dependency manager for Objective-C, which simplifies the process of using 3rd-party libraries in projects. You can install it with the following command:

```bash
$ gem install cocoapods
```

> CocoaPods 1.0 or higher is recommended

To integrate the Ströer Proxity SDK into your Xcode project, specify it in your `Podfile`:

```ruby
source 'https://github.com/StroeerSE/StroeerBeaconSDK_iOS_Pods.git'
source 'https://github.com/CocoaPods/Specs.git'

platform :ios, "7.0"

target 'TargetName' do
pod 'stroeer-proxity', '~> 1.8'
end
```

Then, run the following command:

```bash
$ pod install
```
___
### Setup your App for background Scanning
With region monitoring and background location usage scanning for beacons is also possible while your app is running in background. If you call `startScan` a `CLBeaconRegion` with the UUID of your beacons will be monitored. After the user or rather the device enters this region different things could happen depending on the status of the app and the iOS version.

**Please note** that it's necessary to enable the background capability "Location updates" for your app by assigning "location" to the `UIBackgroundModes` key to your `info.plist`.
For iOS 8 you also have to add the `NSLocationAlwaysUsageDescription` key to your `info.plist` file with a suitable value. Otherwise scanning in background won't be possible.
Also ensure that you comply to the latest App Store Review Guidelines:
> 2.5.4 Multitasking apps may only use background services for their intended purposes: VoIP, audio playback, location, task completion, local notifications, etc. If your app uses location background mode, include a reminder that doing so may dramatically decrease battery life.

> 5.1.5 Location Services: Use Location services in your app only when it is directly relevant to the features and services provided by the app. [...] If your app uses background location services, be sure to explain the purpose in your app; refer to the Human Interface Guidelines for best practices on doing so.

#### iOS 7.1
If the app is running in foreground location updates will be turned on to enable background scanning. Otherwise a `UILocalNotification` will be presented. The primary function of the last one is to encourage the user to open the app because on iOS 7.1 it isn't possible to turn on location updates from the background. If he does so location updates will be turned on as if the app was running in foreground while he enters the region. The `UILocalNotification` can be changed with these properties of the `SPXStroeerProxityAPI` class:

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
Since iOS 8 it's possible to activate location updates not only if the app is running in foreground but also from the background. Therefore no `UILocalNotification` will be shown if the user enters a region. Background location updates will just be turned on. If the device leaves the region background location updates will be turned off immediately and not after a specific amount of time.

___
### Setup the Ströer Proxity SDK

#### 1. Set the Delegate (optional)
In order to receive notifications from the SDK you have to implement the `SPXStroeerProxityAPIDelegate` protocol. Furthermore you have to set the delegate for the `SPXStroeerProxityAPI` class.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
```

#### 2. Authorize your App
The next step is to authorize your app against the backend. Just set the Api-Key you received from Ströer.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setApiKey:@""];    
```

#### 3. Start Scanning
The last step is to start scanning for nearby beacons:

```objective-c
[[SPXStroeerProxityAPI sharedInstance] startScan];
```

Now the SDK scans for beacons near you and its scanning property will be set to true. This way you can find out whether the SDK is currently scanning or not. Since the SDK is scanning for nearby beacons, you might get notifications from it fairly soon.

#### 4. Stop Scanning
When you're done with scanning, you simply call:
```objective-c
[[SPXStroeerProxityAPI sharedInstance] stopScan];
```
___

### Error Handling
The `SPXStroeerProxityAPIDelegate` protocol provides a callback that informs you about occurred errors.
```objective-c
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didFailWithError:(SPXError*)error;
```
Depending on the occurred error the SDK will stop automatically if scanning for nearby beacons isn't possible anymore.
The `SPXError` class provides an error message and an error code which can be one of the following:

```objective-c
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
```


___

### SDK State
There are three possible states in which the SDK can be: `SPXStateNone`, `SPXStateRegionCheck`, `SPXStateOffline` and `SPXStateOnline`. Dependening on the state scanning for near beacons is active or not.

#### SPXStateNone
This is the default state before you have done anything. In this state scanning isn't active and no delegate method will be called. To switch to another state you have to call the `startScan` method.

#### SPXStateRegionCheck
In this state the SDK is checking whether the current location of the user is supported by the SDK or not. In case of the latter the scanning process will stop immediately.

#### SPXStateOffline
The offline state means that scanning is active but the SDK was not able to download the latest data from the server yet.

#### SPXStateOnline
This state means that you have successfully authenticated against the server, the latest data was downloaded and scanning is active.

___

### Scan Period
It's possible to define a period of time which is used to collect raw beacon data before they will be analysed:

```objective-c
@property (nonatomic) CGFloat scanPeriod;
```

The default value is five seconds. In this time the SDK will collect all scanned beacon informations in your vicinity. After this time span you get informed if there were any results, e.g. a received action.

___

### Manual content update
The SDK will update the local data automatically if the user has moved a certain distance. If you want to update the data manually in between you can use this method:

```objective-c
- (void)updateContentFromServerWithSuccessBlock:(nullable void (^)())successBlock
                                     errorBlock:(nullable void (^)(SPXError *error))errorBlock;
```

___

### Bluetooth hardware state
The current state of the bluetooth hardware is stored in the `bluetoothState` property of the `SPXStroeerProxityAPI` class and the `SPXStroeerProxityAPIDelegate` protocol informs you about changes of it:

```objective-c
@property (nonatomic, readonly) CBCentralManagerState bluetoothState;

- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi bluetoothStateChangedFromState:(CBCentralManagerState)oldState toState:(CBCentralManagerState)newState;
```

If you implement this method, you could for example notify the user about the bluetooth state using a popup or whatever method you prefer.

___

### Location authorisation state
If the user of your app has denied the usage of location services you get informed about this change with this `SPXStroeerProxityAPIDelegate` protocol method:

```objective-c
- (void)stroeerProxityAPIUsageOfLocationServicesDenied:(SPXStroeerProxityAPI*)spxAPi;
```

Scanning for beacons will stop immediately after the user denies the usage of location services.

___

### Debugging

#### Logging
In order to debug the SDK you can turn on file logging by setting the `fileLoggingEnabled` property to `YES`.
The path to the log file is stored in the `logFile` property. If you want to delete the logfile you can call this method:
```objective-c
[[SPXStroeerProxityAPI sharedInstance] deleteLogFile];
```

#### Scanned Beacons
This delegate method informs you about the amount of scanned beacons during the last scan interval.
```objective-c
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didScanBeacons:(nullable NSMutableArray<SPXBeacon*>*)scannedBeacons;
```

#### Send Analytics Events
Each time an analytics event was sent to the server you get informed by this callback.
```objective-c
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon;
```

#### Number of available beacons
This delegate method tells you the amount of available beacons downloaded from the server for you current location.
```objective-c
- (void)stroeerProxityAPIContentUpdated:(SPXStroeerProxityAPI*)spxAPi numberOfBeacons:(NSInteger)numberOfBeacons;
```
