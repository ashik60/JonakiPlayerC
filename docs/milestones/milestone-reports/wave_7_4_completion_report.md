# Wave 7.4 Completion Report: Hardware Decoding Profiles & A-B Repeat Loop

**Milestone:** M7 — Advanced Features & Preferences Dialog Hierarchy  
**Wave:** 7.4 — Hardware Decoding Profiles & A-B Repeat Loop  
**Status:** COMPLETED & VERIFIED  
**Date:** 2026-11-11  

---

## 1. Executive Summary

Wave 7.4 delivers high-performance hardware-accelerated video decoding management and seamless A-B repeat looping in JonakiPlayer.
All functionality is cleanly abstracted through `IPlaybackEngine`, implemented asynchronously in `MpvPlaybackEngine`, and exposed via the GUI in `PreferencesDialog` (`Video -> Renderer & HWDEC`), `BottomControlBar` (timeline markers), and `MainWindow` (hotkeys `[`, `]`, `\`, `Shift+[`, `Shift+]`).

All 13 automated test targets in CTest (including 10 unit/integration tests in `HwdecProfileTest` and 9 unit tests in `ABRepeatTest`) pass 100% with zero regressions.

---

## 2. Implemented Architecture & Interfaces

### 2.1 Hardware Decoding Profiles (`IPlaybackEngine` & `MpvPlaybackEngine`)
- **Engine Methods:**
  - `virtual void setHwdecProfile(const std::string& profile) = 0;`
  - `virtual std::string getRequestedHwdecProfile() const = 0;`
  - `virtual std::string getActiveHwdecDecoder() const = 0;`
  - `virtual std::string getVideoCodec() const = 0;`
- **Supported Profiles:**
  - `auto-safe` (default, recommended)
  - `d3d11va` / `d3d11va-copy` (Direct3D 11 acceleration)
  - `dxva2` / `dxva2-copy` (DirectX Video Acceleration 2.0)
  - `d3d12va` / `d3d12va-copy` (Direct3D 12 acceleration)
  - `no` (Software decoding fallback)
- **Runtime Switching:** Supports dynamic hot-switching during playback (`mpv_set_property_string(m_mpv, "hwdec", ...)`). If an unsupported or invalid profile is supplied, mpv gracefully logs a warning and falls back to safe software decoding without playback termination or crash.
- **Active Inspection:** Monitors property changes on `hwdec-current` and demuxer tracks to report exact active decoder hardware contexts and video codecs (e.g., `h264`, `av1`, `hevc`).

### 2.2 A-B Repeat Loop Subsystem
- **Engine Methods:**
  - `virtual void setLoopPointA(double seconds) = 0;`
  - `virtual void setLoopPointB(double seconds) = 0;`
  - `virtual void clearLoopPointA() = 0;`
  - `virtual void clearLoopPointB() = 0;`
  - `virtual void clearLoop() = 0;`
  - `virtual double getLoopPointA() const = 0;`
  - `virtual double getLoopPointB() const = 0;`
  - `virtual bool isLoopActive() const = 0;`
  - `virtual void toggleLoop() = 0;`
- **mpv Native Mapping:** Backed by mpv properties `ab-loop-a` and `ab-loop-b`.
- **Keyboard Shortcuts (PotPlayer Reference):**
  - `[`: Set Point A at current playback position.
  - `]`: Set Point B at current playback position (automatically enables loop if B > A).
  - `\`: Toggle A-B Repeat loop on/off.
  - `Shift+[`: Clear Point A.
  - `Shift+]`: Clear Point B.
- **Timeline Visual Markers (`BottomControlBar`):**
  - Point A marker: Cyan vertical tick mark at normalized position on the seekbar.
  - Point B marker: Red/orange vertical tick mark at normalized position on the seekbar.
  - Auto-updates dynamically as points are assigned, toggled, or cleared.
  - Cleared automatically on loading new media.

---

## 3. Verification & Test Evidence

### 3.1 Automated Test Execution (CTest)
```
Test project F:/Projects/JonakiPlayer/build
      Start  1: SettingsTest
 1/13 Test  #1: SettingsTest .....................   Passed    0.07 sec
      Start  2: PlaybackEngineMockTest
 2/13 Test  #2: PlaybackEngineMockTest ...........   Passed    0.06 sec
      Start  3: MpvPlaybackIntegrationTest
 3/13 Test  #3: MpvPlaybackIntegrationTest .......   Passed    2.48 sec
      Start  4: SkinParserTest
 4/13 Test  #4: SkinParserTest ...................   Passed    0.08 sec
      Start  5: PlaylistTest
 5/13 Test  #5: PlaylistTest .....................   Passed    0.30 sec
      Start  6: SubtitleTest
 6/13 Test  #6: SubtitleTest .....................   Passed    6.16 sec
      Start  7: AudioVideoFilterTest
 7/13 Test  #7: AudioVideoFilterTest .............   Passed    2.11 sec
      Start  8: PreferencesDialogTest
 8/13 Test  #8: PreferencesDialogTest ............   Passed    0.99 sec
      Start  9: FrameCaptureTest
 9/13 Test  #9: FrameCaptureTest .................   Passed   13.27 sec
      Start 10: OpenUrlDialogTest
10/13 Test #10: OpenUrlDialogTest ................   Passed    0.51 sec
      Start 11: NetworkStreamingTest
11/13 Test #11: NetworkStreamingTest .............   Passed    5.74 sec
      Start 12: HwdecProfileTest
12/13 Test #12: HwdecProfileTest .................   Passed   11.63 sec
      Start 13: ABRepeatTest
13/13 Test #13: ABRepeatTest .....................   Passed    0.91 sec

100% tests passed out of 13
Total Test time (real) =  44.34 sec
```

### 3.2 Real Media Verification (`E:\Video Song`, Read-Only)
- Verified with `E:\Video Song\A.R. Rahman - Barso Re Best Video_Guru_Aishwarya Rai_Shreya Ghoshal_Uday Mazumdar(720P_HD).mp4` (H.264 / AAC, 1280x550 @ 25fps):
  - Hardware decoder engaged: `d3d11va` / `d3d11[nv12]`.
  - A-B loop between `1.0s` and `3.0s` loops continuously without audio glitches or decoder stall.
  - Zero modifications to reference directory.
