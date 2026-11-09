# Milestone M2 Implementation Plan: Architecture & Playback Foundation

> **Milestone:** M2 — Architecture & Playback Foundation  
> **Status:** READY FOR REVIEW & PLANNING LOCK  
> **Dependencies:** Milestone M1 (Completed & Verified)  

---

## 1. Milestone Objective

Construct the clean-room multimedia playback pipeline, Qt 6 event loop, logging subsystem, settings persistence, and abstract playback engine integrated with `libmpv` and hardware decoding on Windows (D3D11VA/DXVA2) and Linux (VAAPI).

---

## 2. Work Breakdown Structure (Waves & Tasks)

### Wave 2.1: Core Infrastructure & Settings Persistence
- **Task 2.1.1: SettingsManager PotPlayer Schema Alignment**
  - Implement full registry/INI schema based on REV-005 (`PlaybackMode`, `RememberPosition`, `OnTopMode`, `LastSkinXmlName`).
  - Unit tests verifying serialization and deserialization of all setting types.
- **Task 2.1.2: EventBus & Logging Pipeline**
  - Thread-safe event bus for state, time, duration, track, and volume notifications.

### Wave 2.2: Playback Engine Core (`libmpv`)
- **Task 2.2.1: MpvPlaybackEngine Integration**
  - Concrete implementation of `IPlaybackEngine` using `mpv_handle` and `mpv_render_context`.
  - Implement media loading (`loadfile`), state machine (Stopped, Playing, Paused), exact and relative seeking, speed stepping (0.2x–3.0x with pitch correction), and volume control.
- **Task 2.2.2: Video Surface Rendering**
  - Create `VideoOpenGLWidget` subclassing `QOpenGLWidget` to render `mpv_render_context` directly with hardware acceleration.

### Wave 2.3: Basic Host Window & Integration Testing
- **Task 2.3.1: Playback Host Window**
  - Base `MainWindow` configured to initial measured dimensions (723 x 399 px).
  - Drag-and-drop file support forwarding media to `IPlaybackEngine`.
- **Task 2.3.2: Automated Playback Integration Tests**
  - Headless/unit tests loading real media files, verifying playback state transitions, seek verification, and speed adjustment.

---

## 3. Acceptance Criteria for M2

1. Application boots with native Qt 6 window sized to 723 x 399.
2. `libmpv` initializes cleanly with hardware decoding enabled.
3. Media file dropped onto window immediately plays audio/video smoothly.
4. Space bar pauses/resumes, Left/Right seeks, Up/Down adjusts volume.
5. All automated unit and playback integration tests pass.
