# Milestone M2 Completion Report: Architecture & Playback Foundation

> **Date:** 2026-11-09  
> **Status:** COMPLETED & EMPIRICALLY VERIFIED  
> **Target:** Milestone M2 — Playback Engine & Video Pipeline Integration  
> **Frameworks:** GSD v1.6.0 & REA v6.8.0  

---

## 1. Executive Summary

Milestone M2 establishes the complete, production-grade playback foundation for **JonakiPlayer**. All playback operations, video rendering surfaces, settings management, and input mappings are implemented strictly in accordance with clean-room principles and tested against real media files.

No proprietary PotPlayer binary code or copyrighted assets were incorporated into the implementation. All multimedia operations are driven through `libmpv` (v0.42.0) and FFmpeg (v9.0.2), encapsulated cleanly behind an abstract interface.

---

## 2. Implementation Overview

### 2.1 Abstract Playback Architecture (`IPlaybackEngine`)
- **Location:** `src/playback/IPlaybackEngine.h`
- Defined a pure C++ standard interface free of Qt or libmpv types in its public API.
- Implements complete state enumeration: `Stopped`, `Playing`, `Paused`, `Buffering`, `Error`.
- Defines callback hooks for:
  - `onStateChanged(PlaybackState)`
  - `onTimeChanged(currentTime, duration)`
  - `onTrackListChanged(const std::vector<TrackInfo>&)`
  - `onError(PlaybackError, const std::string& message)`
- Full support for aspect ratios, frame stepping (`frameStepForward`, `frameStepBackward`), seek (absolute/relative), volume, mute, speed, and track selection.

### 2.2 libmpv Engine Integration (`MpvPlaybackEngine`)
- **Location:** `src/playback/MpvPlaybackEngine.h`, `src/playback/MpvPlaybackEngine.cpp`
- Dedicated asynchronous worker thread polling `mpv_wait_event()` with thread-safe property synchronization.
- Real-time property observation:
  - `time-pos`, `duration`
  - `pause`, `speed`, `volume`, `mute`
  - `track-list` (audio, video, subtitle)
- Hardware acceleration default configured via `hwdec=auto-safe`.
- Audio pitch correction enabled via `audio-pitch-correction=yes`.

### 2.3 Hardware Video Surface (`VideoOpenGLWidget`)
- **Location:** `src/video/VideoOpenGLWidget.h`, `src/video/VideoOpenGLWidget.cpp`
- Subclasses `QOpenGLWidget` to render video frames directly to OpenGL framebuffers (`defaultFramebufferObject()`).
- Initializes `mpv_render_context` with `MPV_RENDER_PARAM_API_TYPE` (`MPV_RENDER_API_TYPE_OPENGL`) and `get_proc_address` resolved via Qt's `QOpenGLContext`.
- Hooks `mpv_render_context_set_update_callback()` to trigger repaints on the main Qt GUI thread using `QMetaObject::invokeMethod()`.

### 2.4 Decoupled Action & UI Architecture
- **Location:** `src/input/ActionManager.h`, `src/input/ActionManager.cpp`, `src/ui/MainWindow.h`, `src/ui/MainWindow.cpp`
- Implemented `ActionManager` mapping numeric `PlayerAction` enums to Qt actions and key combinations.
- Responsive UI controls:
  - Custom timeline seekbar with real-time tracking and seek throttling.
  - Volume slider and mute toggle button.
  - Current time / duration indicator.
  - Standard PotPlayer keyboard shortcuts: `Space` (Play/Pause), `F` / `D` (Frame step), `Right` / `Left` (Seek +/-5s), `Up` / `Down` (Volume +/-5%).

---

## 3. Empirical Test & Verification Results

### 3.1 Test Suite Summary (`ctest`)
All 3 automated test suites passed 100% across multiple repeated runs:

```
Test project F:/Projects/JonakiPlayer/build
    Start 1: SettingsTest
1/3 Test #1: SettingsTest .....................   Passed    0.05 sec
    Start 2: PlaybackEngineMockTest
2/3 Test #2: PlaybackEngineMockTest ...........   Passed    0.04 sec
    Start 3: MpvPlaybackIntegrationTest
3/3 Test #3: MpvPlaybackIntegrationTest .......   Passed    1.04 sec

100% tests passed out of 3
Total Test time (real) = 1.14 sec
```

### 3.2 Integration Test Details (`tests/integration/test_mpv_integration.cpp`)
- **Media Assets Tested:**
  - `tests/test_data/test_video_aac.mp4`: H.264 640x360 @ 30 fps with AAC audio (5s).
  - `tests/test_data/test_audio.mp3`: 1000 Hz sine wave @ 44.1 kHz (3s).
- **Verified Capabilities:**
  - Real video loading, decoding, and playback state progression.
  - Asynchronous pause, seek, volume, and stop commands.
  - Audio speed adjustment and mute toggling.
  - Error reporting on non-existent files (`PlaybackError::InvalidMedia` / `FileNotFound`).

### 3.3 Application Runtime Verification
- Launched `build/JonakiPlayer.exe` playing `tests/test_data/test_video_aac.mp4`.
- Log output from `%APPDATA%/JonakiPlayer/JonakiPlayer.log`:
  ```
  [INFO] Starting JonakiPlayer...
  [INFO] libmpv engine initialized successfully with async event thread.
  [INFO] MainWindow UI and video surface created.
  [INFO] mpv OpenGL render context initialized successfully.
  ```
- Captured empirical runtime screenshot: `docs/milestones/evidence/m2_playback_runtime.png`.
- Verified clean shutdown via standard `WM_CLOSE` window messaging.

---

## 4. Deliverables & Git Record

| File / Component | Purpose |
|---|---|
| `src/playback/IPlaybackEngine.h` | Pure abstract playback interface |
| `src/playback/MpvPlaybackEngine.h/.cpp` | libmpv asynchronous engine |
| `src/video/VideoOpenGLWidget.h/.cpp` | QOpenGLWidget + mpv_render_context video surface |
| `src/input/ActionManager.h/.cpp` | Decoupled action and shortcut dispatcher |
| `src/ui/MainWindow.h/.cpp` | Main window integrating video widget, seekbar, controls |
| `tests/unit/MockPlaybackEngine.h` | Deterministic mock engine |
| `tests/unit/test_playback_mock.cpp` | State transition and timing tests |
| `tests/integration/test_mpv_integration.cpp` | Real libmpv media playback test suite |
| `tests/test_data/` | Synthetic H.264/AAC and MP3 test media |
| `docs/milestones/evidence/m2_playback_runtime.png` | Empirical runtime screenshot |

**Milestone M2 is officially complete and verified. Ready to proceed to Milestone M3 (Skin Engine & UI Reproduction).**
