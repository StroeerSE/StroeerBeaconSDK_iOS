# Stroeer iOS SPX-SDK Changelog

This file lists all notable changes to the Stroeer iOS SPX-SDK.

## Table of versions
<!-- TOC depthFrom:2 depthTo:6 withLinks:1 updateOnSave:1 -->

* [2.1.0](#210)
* [2.0.9](#209)
* [2.0.3](#203)
* [2.0.0](#200)
* [1.10.0](#1100)

<!-- /TOC -->


# 2.1.0
released Jul 26, 2017
### Changed
  The SDK is now compatible of handling any number of beacons in its proximity simultaneously.

### Fixed
- Fixed crashes which occurred under some circumstances when stopping the scan.
- Fixed an issue which caused analytics events to be sent with an incorrect leave time.
- Fixed an issue which caused analytics events to be sent multiple times.


# 2.0.9
released Apr 7, 2017
### Changed
The SDK pauses the scan when bluetooth is turned off. During this time, any gathered analytics events are stored. As soon as bluetooth is turned on again, the SDK resumes the scan with those stored events.

The SDK waits until bluetooth is turned on (if it was off) to start a scan. If bluetooth is not available or unsupported, the scan won't start at all.


# 2.0.3
released Feb 28, 2017
### Changed
Advertising tracking is now enabled by default. All events will be sent with the phones IDFA if it is allowed on the phone, or unless set to NO with `setAdvertisingTrackingEnabled`.



# 2.0.0
released Feb 1, 2017
### Added
- _CryptoV2:_

  The iOS SPX-SDK is able to scan and decrypt beacons using the encryption v2. To recognize these beacons they use a fixed UUID c2340cb0-d412-11e6-bf26-cec0c932ce01. Beacons with encryption v2 change their major and minor values from day to day, so the SDK needs to fetch the required decryption keys from the server. Upon starting the scan, the SDK will download these decryption keys. If needed, the SDK automatically downloads more valid keys.

- _Analytics:_

  Now, you can specify a custom advertising identifier for the analytics your app will send. This allows you to distinguish your events from others. Use `setCustomAdvertisingId` to set your custom ad identifier.
  You can also enable / disable to send the advertising identifier from iOS (IDFA) which will be added to each analytics event. These won't be sent by default and you need to call the method `setAdvertisingTrackingEnabled` to enable this. This method returns YES if ad tracking is enabled on the phone and the variable `advertisingTrackingEnabled` is successfully set to YES.

### Fixed
- Fixed an issue which made it unable to scan a specific beacon region.
- Fixed an issue which could stop the scan unexpectedly when no internet connection is available (as the SDK can not verify the API key).


# 1.10.0
released Nov 15, 2017
### Added
- _Compatibility:_

  The iOS SPX-SDK is running under iOS 8.0 and later. The SDK can be integrated into apps running under iOS 7 but then beacon scanning is not possible.

- _Scanning:_

  To scan for a beacon the app integrating the SPX-SDK needs location permissions and bluetooth. The SDK provides functions to start (`startScan`) and to stop (`stopScan`) beacon scanning. Upon starting the scan for the first time, the app requires an internet connection once to verify the API-key. The SDK detects only beacons with the following UUIDs:
    - 37ecc5b5-dc9f-4d5c-b1ee-7f09146572a2
    - 88084780-6528-11e6-bdf4-0800200c9a66

  If the beacon scanning is started the SDK scans for beacons no matter if the app integrating the SDK is in foreground or background. Furthermore, beacon scanning will still be performed when the app was closed. Beacon scanning also does not need a permanent internet connection.

  The SDK uses region monitoring and uses a special approach to reduce battery drain on the smartphone.

  Furthermore, the SDK can scan for beacons which shuffle the major and minor values on a random basis (Crypto V1 procedure). The SDK is able to decrypt the shuffled values and provide the original major and minor values.

- _Analytics:_

  The SDK creates analytics events no matter if the app using the SDK is in foreground or background. An event contains among others the following information:
  - the point in time when a new beacon region was entered,
  - the point in time when a beacon region was left,
  - the stay time in a beacon region.

  All created analytics events are persisted on the device and afterwards are sent to an IoT-hub if the device has an internet connection. It is possible to turn on or off the creation of analytics events.

- _Logfile:_

  The iOS SPX-SDK can write a logfile containing all actions the SDK has performed, e.g.
  - the point in time when beacon scanning was started,
  - the point in time when beacon scanning was stopped,
  - the point in time when a beacon region was entered,
  - the point in time when a beacon region was left,
  - the point in time when an analytics event was sent to the IoT-hub
  - all errors that occured.

  Writing such a logfile on the device can be enabled or disabled using `setFileLoggingEnabled`. A created logfile can be deleted using `deleteLogFile`.
