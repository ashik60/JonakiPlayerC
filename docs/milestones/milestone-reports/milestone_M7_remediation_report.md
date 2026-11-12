# Milestone M7: Defect Remediation & Verification Report

> **Project:** JonakiPlayer  
> **Milestone:** M7 — Advanced Features & Preferences Dialog Hierarchy  
> **Date:** 2026-11-12  
> **Status:** REMEDIATED & EMPIRICALLY VERIFIED  
> **Baseline Suite:** 13/13 CTest Targets Passing (100%)

---

## 1. Executive Summary

Following the independent technical audit of Milestone M7, all Priority 0 and Priority 1 findings were systematically remediated, validated with new regression test suites, and integrated cleanly into the codebase.

| Defect / Finding | Priority | Remediation Applied | Empirical Test Evidence | Status |
|---|---|---|---|---|
| **Frame Capture Thread-Safety** | **Priority 0** | Removed unmanaged `std::thread().detach()`. Replaced with asynchronous libmpv command replies (`mpv_command_async`), tracked operations via `s_activeCapturesCount` & `s_activeCv`, RAII `TempFileGuard`, and lifecycle coordination during `MainWindow` close and `MpvPlaybackEngine::shutdown()`. | `testMultipleRapidCaptures`, `testDestroyEngineWithPendingCaptures`, `testShutdownCancellationSafety` in `FrameCaptureTest`. 13/13 tests pass. | **Verified** |
| **A-B Repeat Native State Cleanup & Wrapping** | **Priority 1** | Native mpv loop properties `ab-loop-a` and `ab-loop-b` are explicitly reset to `"no"` during `loadMedia()`, `stop()`, and `shutdown()`. Native toggle properly sets/clears bounds. | `testActualPlaybackWrappingBtoA`, `testSeekingWhileLoopActive`, `testChangingMediaClearsNativeLoopProperties` in `ABRepeatTest`. Playback wrapping from 1.0s to 0.5s verified on real media. 14/14 tests pass. | **Verified** |
| **Open URL Checkbox Behavior (Process Subtitles)** | **Priority 1** | Wired `dlg.isProcessSubtitlesEnabled()` in `MainWindow::openUrlDialog()` to `SubtitleManager::setNetworkSubtitleProcessingEnabled()` and `IPlaybackEngine::setSubtitleVisibility()`. | `testProcessSubtitlesCheckboxWiring` in `OpenUrlDialogTest`. 19/19 tests pass. | **Verified** |
| **Store Only Entered URLs Behavior** | **Priority 1** | Documented semantic: When `StoreEnteredUrlsOnly=true`, only manual entries from Open URL dialog are saved; playlist/drag-drop URLs are ignored. When `false`, all played stream URLs are appended to `RecentUrls`. Added hook in `PlaylistController::playIndex()`. | `testStoreOnlyEnteredUrlsManualEntry` in `OpenUrlDialogTest` and `testStoreOnlyEnteredUrlsPlaylistBehavior` in `PlaylistTest`. 7/7 tests pass. | **Verified** |
| **URL Redaction Robustness** | **Priority 1** | Expanded `UrlValidator::sanitizeForLogging` with case-insensitive, percent-decoded parameter handling. Added AWS (`x-amz-*`), Akamai (`hdnts`, `hdnea`), CloudFront (`policy`, `key-pair-id`), Google Cloud (`googleaccessid`, `x-goog-*`), and Azure SAS (`sig`, `se`, `sp`, `sv`, `sr`) tokens while preserving innocent parameters (`user=john`, `v=123`). | `testUrlValidatorCaseInsensitiveAndPercentEncodedCdnParameters` in `OpenUrlDialogTest`. 19/19 tests pass. | **Verified** |
| **HTTPS Media Playback Verification** | **Verification Gate** | Implemented deterministic local HTTPS fixture (`scripts/test_https_server.py`) with generated CA (`ca.crt`), server certificate (`server.crt`), and untrusted self-signed certificate (`untrusted.crt`). Added `setTlsCaFile()` in `IPlaybackEngine` to enforce strict certificate verification via libmpv `--tls-ca-file` and `--tls-verify=yes`. | `testHttpsWithTrustedCertificate` (plays smoothly) and `testHttpsWithUntrustedCertificateRejected` (rejected by TLS handshake) in `NetworkStreamingTest`. 11/11 tests pass. | **Verified** |
| **Hardware Decoding Profiles (H.264 vs AV1)** | **Audit Clarification** | Separated active decoder reporting in UI and documentation. Hardware decoding is verified on H.264 (`d3d11va`), while AV1 is explicitly identified as software fallback (`libdav1d`) due to host GPU capabilities. | `HwdecProfileTest` (5.62s execution). Decoder status explicitly differentiated. | **Verified (H.264: Hardware / AV1: Software Fallback)** |

---

## 2. Detailed Technical Remediation Records

### 2.1 Frame Capture Subsystem (Priority 0)
- **Problem:** `FrameCaptureHelper::captureToFile()` launched a detached `std::thread` holding a raw `IPlaybackEngine*`. If `MainWindow` or `MpvPlaybackEngine` was destroyed while a write was pending, this caused an access violation / use-after-free.
- **Solution:**
  1. Replaced detached threads with native libmpv asynchronous command dispatching: `mpv_command_async(m_mpv, reqId, cmd)`.
  2. Maintained an active capture registry protected by `std::mutex` and `std::condition_variable` in `FrameCaptureHelper`.
  3. Added `cancelPendingCaptures()` in `IPlaybackEngine` and `MpvPlaybackEngine` which invalidates all pending callbacks and resets reply tracking.
  4. Added `cancelPendingOperations()` and `waitForPendingOperations(int timeoutMs)` in `FrameCaptureHelper`, wired into `MainWindow::~MainWindow()` and `MainWindow::closeEvent()`.
  5. Implemented RAII `TempFileGuard` inside clipboard capture callback to ensure temporary files are deleted even if capture fails, is cancelled, or errors out.
- **Empirical Evidence:**
  - `FrameCaptureTest`: 13 passed in 22.00s.
  - Rapid dispatch of 5 simultaneous captures: all completed cleanly without contention.
  - Destruction of engine with active capture: engine was deleted immediately, zero use-after-free crashes or hangs detected.

### 2.2 A-B Repeat State Reset & Playback Wrapping (Priority 1)
- **Problem:** `loadMedia()` only reset internal variables (`m_loopA = -1.0`, `m_loopB = -1.0`) without clearing native mpv properties `ab-loop-a` and `ab-loop-b`. Stale loop boundaries persisted across media changes. No tests verified actual playback wrapping.
- **Solution:**
  1. Updated `MpvPlaybackEngine::loadMedia()`, `stop()`, and `shutdown()` to dispatch `mpv_set_property_string(m_mpv, "ab-loop-a", "no")` and `mpv_set_property_string(m_mpv, "ab-loop-b", "no")`.
  2. Updated `toggleLoop()` to toggle native mpv properties on and off properly.
  3. Added integration test `testActualPlaybackWrappingBtoA()` using `tests/test_data/test_video_aac.mp4`. Verified that native `ab-loop-a` and `ab-loop-b` are set to `0.5` and `1.2`, seeked to `1.0s`, and verified playback wrapped back to `~0.5s` automatically.
  4. Added `testChangingMediaClearsNativeLoopProperties()` verifying native properties return to `"no"` upon loading new media.
- **Empirical Evidence:**
  - `ABRepeatTest`: 14 passed in 3.89s.

### 2.3 Open URL Checkbox & History Semantics (Priority 1)
- **Problem:** "Process subtitles" was disconnected from the media pipeline. "Store only entered URLs" behavior was not connected to playlists and lacked specification.
- **Solution:**
  1. Connected `dlg.isProcessSubtitlesEnabled()` in `MainWindow::openUrlDialog()` to `SubtitleManager::setNetworkSubtitleProcessingEnabled()` and `m_engine->setSubtitleVisibility()`.
  2. Documented semantic behavior:
     - When `StoreEnteredUrlsOnly` is `true`: Only URLs manually typed into the Open URL dialog (Ctrl+U) are stored in `RecentUrls`. Stream URLs from `.dpl` playlists or drag-and-drop are excluded.
     - When `StoreEnteredUrlsOnly` is `false`: All played network streams (manual, playlist, drag-and-drop) are recorded into `RecentUrls`.
  3. Added hook in `PlaylistController::playIndex()` to check `StoreEnteredUrlsOnly` and update `RecentUrls` accordingly.
- **Empirical Evidence:**
  - `OpenUrlDialogTest`: 19 passed in 0.61s.
  - `PlaylistTest`: 7 passed in 0.26s.

### 2.4 URL Query Parameter Redaction (Priority 1)
- **Problem:** `sanitizeForLogging()` missed CDN and cloud provider parameters (`X-Amz-Credential`, `hdnts`, `Policy`, `Key-Pair-Id`, `GoogleAccessId`, `sig`), and substring matching on 2-letter tokens like `se` caused false positives (redacting `user=john`).
- **Solution:**
  1. Separated exact-match parameter set from substring pattern set.
  2. Short parameters (`se`, `sp`, `sv`, `sr`, `sig`) and cloud tokens (`hdnts`, `hdnea`, `policy`, `key-pair-id`, `googleaccessid`) require exact match.
  3. Parameters starting with `x-amz-` and `x-goog-` are automatically redacted.
  4. Fully decoded percent-encoded query keys (e.g. `Key%2DPair%2DId`, `X%2DAmz%2DCredential`) and processed keys case-insensitively.
  5. Preserved innocent parameters (`user=john`, `video_id=987`, `channel=sportsHD`, `format=mp4`, `title=EpicMovie`).
- **Empirical Evidence:**
  - `OpenUrlDialogTest::testUrlValidatorCaseInsensitiveAndPercentEncodedCdnParameters`: verified 5 provider URLs (AWS, Akamai, CloudFront, GCS, Azure SAS).

### 2.5 HTTPS Verification Fixture
- **Problem:** HTTPS was previously classified as Verified based solely on libmpv static linking, without an automated local test fixture demonstrating certificate validation.
- **Solution:**
  1. Created standalone Python TLS test server `scripts/test_https_server.py`.
  2. Generated deterministic test credentials:
     - `ca.crt` (JonakiTestCA)
     - `server.crt` & `server.key` (SAN: `IP:127.0.0.1`, `DNS:localhost`, signed by `JonakiTestCA`)
     - `untrusted.crt` & `untrusted.key` (UntrustedSelfSignedCA)
  3. Implemented `setTlsCaFile()` in `IPlaybackEngine` and `MpvPlaybackEngine` configuring mpv `tls-ca-file` and enabling `tls-verify=yes`.
  4. Created automated tests in `NetworkStreamingTest`:
     - `testHttpsWithTrustedCertificate`: Validated playback starts and time advances over HTTPS with trusted CA.
     - `testHttpsWithUntrustedCertificateRejected`: Validated that untrusted certificate is rejected during TLS handshake, preventing unauthorized streaming.
- **Empirical Evidence:**
  - `NetworkStreamingTest`: 11 passed in 9.44s.

---

## 3. CTest Regression Suite Results

```text
Test project F:/Projects/JonakiPlayer/build
      Start  1: SettingsTest
 1/13 Test  #1: SettingsTest .....................   Passed    0.06 sec
      Start  2: PlaybackEngineMockTest
 2/13 Test  #2: PlaybackEngineMockTest ...........   Passed    0.06 sec
      Start  3: MpvPlaybackIntegrationTest
 3/13 Test  #3: MpvPlaybackIntegrationTest .......   Passed    3.42 sec
      Start  4: SkinParserTest
 4/13 Test  #4: SkinParserTest ...................   Passed    0.07 sec
      Start  5: PlaylistTest
 5/13 Test  #5: PlaylistTest .....................   Passed    0.26 sec
      Start  6: SubtitleTest
 6/13 Test  #6: SubtitleTest .....................   Passed    6.42 sec
      Start  7: AudioVideoFilterTest
 7/13 Test  #7: AudioVideoFilterTest .............   Passed    1.37 sec
      Start  8: PreferencesDialogTest
 8/13 Test  #8: PreferencesDialogTest ............   Passed    0.77 sec
      Start  9: FrameCaptureTest
 9/13 Test  #9: FrameCaptureTest .................   Passed   22.00 sec
      Start 10: OpenUrlDialogTest
10/13 Test #10: OpenUrlDialogTest ................   Passed    0.61 sec
      Start 11: NetworkStreamingTest
11/13 Test #11: NetworkStreamingTest .............   Passed    9.44 sec
      Start 12: HwdecProfileTest
12/13 Test #12: HwdecProfileTest .................   Passed    5.62 sec
      Start 13: ABRepeatTest
13/13 Test #13: ABRepeatTest .....................   Passed    3.89 sec

100% tests passed out of 13
Total Test time (real) = 54.02 sec
```
