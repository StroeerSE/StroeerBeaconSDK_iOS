# Stroeer iOS SPX-SDK Changelog

This file lists all notable changes to the Stroeer iOS SPX-SDK.

## Table of versions
<!-- TOC depthFrom:2 depthTo:6 withLinks:1 updateOnSave:1 -->

* [Unreleased](#unreleased)
* [2.1.0](#210)
* [2.0.9](#209)
* [2.0.3](#203)
* [2.0.0](#200)
* [1.10.0](#1100)

<!-- /TOC -->
## Unreleased


## 2.1.0
released Jul 26, 2017
### Changed
- The SDK is now compatible of handling any number of beacons in its proximity simultaneously.

### Fixed
- Fixed crashes which occured under some circumstances when stopping the scan.
- Fixed an issue which caused analytic events to be sent with an incorrect leave time.
- Fixed an issue which caused analytics to be sent multiple times.


## 2.0.9
released Apr 7, 2017
### Changed
- The SDK pauses the scan when bluetooth is turned off. During this time, any gathered analytic events are stored. Upon turning bluetooth back on, the SDK resumes the scan with those stored events.
- The SDK waits until bluetooth is turned on (if it was off) to start a scan. If bluetooth is not available or unsupported, the scan won't start at all.


## 2.0.3
released Feb 28, 2017
### Changed
- advertising tracking is now enabled by default. All events will be sent with the phones IDFA if it is allowed on the phone, or unless set to NO with setAdvertisingTrackingEnabled.
- setAdvertisingTrackingEnabled returns YES if ad tracking is enabled on the phone and the advertisingTrackingEnabled is successfully set to YES.


## 2.0.0
released Feb 1, 2017
### Added
- Crypto V2:
  - scan and decrypt Crypto v2 bluloc beacons
  - hard-coded CryptoV2 UUID: c2340cb0-d412-11e6-bf26-cec0c932ce01
  - download last three and next 15 decryption keys from Backend
  - re-download decryption keys if only 10 or less valid keys are available on the sdk
  - decrypt major/minor values
  - handle time drift of beacon
- Analytics
  - specify a custom Advertising ID which will be added to each analytics event
  - enable / disable advertising identifier feature from iOS (IDFA) which will be added to each analytics event, defaults to not send

### Fixed
- Fixed an issue which made it unable to scan a specific beacon region.
- Fixed an issue which could stop the scan unexpectedly when internet is off (as the SDK can not verify the API key).


## 1.10.0
released Nov 15, 2017
### Added
- Compatibility
  - iOS 8.0+
  - iOS 7: SDK can be integrated into apps, but scanning is not possible
- Scanning
  - start / stop
  - requires location permission
  - only hardcoded UUIDs
    - 37ecc5b5-dc9f-4d5c-b1ee-7f09146572a2 (not encrypted beacons)
    - 88084780-6528-11e6-bdf4-0800200c9a66 (Crypto V1)
  - while in foreground/background, online/offline
  - improved battery life with beacon ranging
  - Beacon Region Monitoring
- Crypto V1
  - scan beacons
  - decrypt major/minor values
- Analytics
  - send to Azure if online
  - gather events
    - while in foreground/background, online/offline
    - will be persisted until sent
  - beacon leave event contains (among others) enter, leave and stay times
  - turn on/off gathering events
- API-Key
  - configurable by developer
  - will be checked before starting a scan
- State Restoration
  - SDK will keep to scan and send events when the app containing it is closed
- Logfile
  - enable/disable writing messages to a logfile
  - delete created logfile
