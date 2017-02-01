## Ströer Proxity SDK

<!-- TOC depthFrom:3 depthTo:3 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Installation with CocoaPods](#installation-with-cocoapods)
- [Setup your App for Beacon Scanning](#setup-your-app-for-beacon-scanning)
- [Setup the Ströer Proxity SDK](#setup-the-ströer-proxity-sdk)
- [Advertising Identifier](#advertising-identifier)
- [Error Handling](#error-handling)
- [SDK State](#sdk-state)
- [Bluetooth hardware state](#bluetooth-hardware-state)
- [Location authorisation state](#location-authorisation-state)
- [Debugging](#debugging)

<!-- /TOC -->


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

target 'YourTargetName' do
  pod 'stroeer-proxity', '~> 2.0'
end
```

Then, run the following command:

```bash
$ pod install
```
___

### Setup your App for Beacon Scanning
Scanning nearby beacons is realised with a mix of Beacon Region Monitoring and Beacon Ranging by using the CoreLocation API.
Therefore it's necessary to enable the background capability "*Location updates*" for your app by assigning `location` to the `UIBackgroundModes` key to your `info.plist`.
Furthermore you have to add the `NSLocationAlwaysUsageDescription` key to your `info.plist` file with a suitable value.

```xml
<key>NSLocationAlwaysUsageDescription</key>
<string>Scan for blulocs</string>
<key>UIBackgroundModes</key>
<array>
	<string>location</string>
</array>
```

Also ensure that you comply to the latest App Store Review Guidelines:
> 2.5.4 Multitasking apps may only use background services for their intended purposes: VoIP, audio playback, location, task completion, local notifications, etc. If your app uses location background mode, include a reminder that doing so may dramatically decrease battery life.

> 5.1.5 Location Services: Use Location services in your app only when it is directly relevant to the features and services provided by the app. [...] If your app uses background location services, be sure to explain the purpose in your app; refer to the Human Interface Guidelines for best practices on doing so.

#### iOS 7.0
Scanning for beacons is no longer supported on iOS 7. It is still possible to integrate the SDK in a iOS 7 project but start scanning will throw an error.

___

### Setup the Ströer Proxity SDK

To implement the SDK you just have to add two to three lines of code to your Application Delegate, for example inside the ``applicationWillEnterForeground:`` method.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setDelegate:self];  // Step 1 (optional)
[[SPXStroeerProxityAPI sharedInstance] setApiKey:@""];     // Step 2
[[SPXStroeerProxityAPI sharedInstance] startScan];         // Step 3
```

#### Step 1 - Set the Delegate (optional)
In order to receive notifications from the SDK you have to implement the `SPXStroeerProxityAPIDelegate` protocol. Furthermore you have to set the delegate for the `SPXStroeerProxityAPI` class.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setDelegate:self];
```

#### Step 2 - Authorize your App
The next step is to authorize your app against the backend. Just set the Api-Key you received from Ströer.

```objective-c
[[SPXStroeerProxityAPI sharedInstance] setApiKey:@""];
```

#### Step 3 - Start Scanning
The last step is to start scanning for nearby beacons:

```objective-c
[[SPXStroeerProxityAPI sharedInstance] startScan];
```

In the first stage the entered API-Key will be validated. Is the API-Key valid the location usage dialog will appear to the user.
As soon the user has confirmed the dialog the SDK scans for nearby beacons and the SDK state has changed to scanning.

#### Step 4 - Stop Scanning (optional)
When you want to stop scanning, you simply call:
```objective-c
[[SPXStroeerProxityAPI sharedInstance] stopScan];
```
___

### Advertising Identifier

The Ströer Proxity SDK provides two ways to identify a user across different apps in order to show targeted advertisements.
```objective-c
/**
 * If set, the custom advertising identifier will be added to each analytics event.
 * This can be used to identify the user across different apps.
 */
@property (nonatomic, nullable) NSString *customAdvertisingId;

/**
 * If set, the The Advertising Identifier (IDFA) from iOS will be added to each analytics event.
 * Default value is NO.
 */
@property (nonatomic, getter=isAdvertisingTrackingEnabled) BOOL advertisingTrackingEnabled;
```
Use the `customAdvertisingId` property to specify your own advertising identifier.

Be aware of the Apple Guidelines regarding the usage of the Advertising Identifier (IDFA):
> During the Submitting the App you have to answer questions about the IDFA. Indicate whether your app uses the Advertising Identifier, and if so, in what way.
If you checked No but Apple determine your app does use IDFA, your app will be put into the Invalid Binary status, and you will be notified by email.
Similarly, if you checked Yes but your app uses IDFA in ways that don’t match the statements you checked, your app will be rejected by App Review and put into the Rejected status.
In either case, when you resubmit the build, you will be presented with the IDFA questions again and can provide the appropriate answers.

> See The Advertising Identifier (IDFA) for more details about this step:
https://developer.apple.com/library/content/documentation/LanguagesUtilities/Conceptual/iTunesConnect_Guide/Chapters/SubmittingTheApp.html#//apple_ref/doc/uid/TP40011225-CH33-SW8

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
 * An unknown error occurred.
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
 * This error is thrown if the device doesn't fully support ranging or monitoring for beacons.
 */
SPXErrorCodeDeviceNotSupported,

/**
 * This error is thrown if the bluetooth is not available.
 */
SPXErrorCodeBluetoothNotAvailable,

/**
 * This error is thrown if the user has denied the usage of the location services.
 */
SPXErrorCodeLocationUsageDenied

```

___

### SDK State
There are three possible states in which the SDK can be:

#### SPXStateNone
This is the default state before you have done anything. In this state scanning isn't active and no delegate method will be called. To switch to another state you have to call the `startScan` method.

#### SPXStateBluetoothCheck
In this state the SDK checks if bluetooth turned on.

#### SPXStateAPIKeyValidation
In this state the specified API Key will be verified.

#### SPXStateScanning
This state means that scanning is active.

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

#### Entered and left Beacons
This delegate method informs you about which beacon was entered and left.
```objective-c
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didEnterBeacon:(SPXBeacon*)beacon;
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didExitBeacon:(SPXBeacon*)beacon;
```

#### Send Analytics Events
Each time an analytics event was sent to the server you get informed by this callback.
```objective-c
- (void)stroeerProxityAPI:(SPXStroeerProxityAPI*)spxAPi didSendAnalyticsEventForBeacon:(nullable SPXBeacon*)beacon;
```
