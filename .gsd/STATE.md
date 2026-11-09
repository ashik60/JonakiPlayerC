---
updated: 2026-11-09T09:20:00+06:00
---

# Project State

## Current Position

**Milestone:** M2 - Architecture & Playback Foundation
**Phase:** Phase 2 - Core Engine & Pipeline
**Status:** completed
**Plan:** M2 Architecture & Playback Foundation (Completed & Verified)

## Last Action

- Implemented pure C++ `IPlaybackEngine` decoupled interface with complete state transitions, error callbacks, and track lists.
- Implemented `MpvPlaybackEngine` backed by `libmpv` asynchronous event worker thread (`mpv_wait_event`), handling property observation (`time-pos`, `duration`, `pause`, `volume`, `speed`, `mute`, `track-list`) and hardware acceleration defaults (`hwdec=auto-safe`).
- Integrated `VideoOpenGLWidget` subclassing `QOpenGLWidget` + `mpv_render_context` with OpenGL FBO rendering and GUI thread updates.
- Decoupled command management via `ActionManager` and integrated responsive UI in `MainWindow` (seekbar, volume slider, time labels, keyboard shortcuts).
- Implemented automated tests:
  - `SettingsTest`: Configuration loading/saving/defaults.
  - `PlaybackEngineMockTest`: Deterministic state transitions, error handling, seek bounds.
  - `MpvPlaybackIntegrationTest`: Real media playback verification with `test_video_aac.mp4` and `test_audio.mp3`.
- Verified 100% CTest pass rate (3/3 tests passed).
- Verified runtime execution and clean termination, capturing `docs/milestones/evidence/m2_playback_runtime.png`.
- Documented TDR-005 (Video Surface Architecture via `mpv_render_context`) and updated Feature Parity Matrix.

## Next Steps

1. Review M2 completion report and prepare for Milestone M3 (Skin Engine & UI Reproduction).
2. Begin M3: Custom frameless window (723x399 default geometry), aero-snap Win32 integration, independent XML skin parsing, and context menu hierarchy mapping.

## Active Decisions

| Decision | Choice | Made | Affects |
|----------|--------|------|---------|
| TDR-001 Architecture Stack | C++20 + Qt6 + CMake + libmpv + FFmpeg | 2026-11-09 | All phases |
| TDR-002 Skin Engine | Clean-room Qt XML skin parser & spritesheet renderer | 2026-11-09 | M3, M4, M8 |
| TDR-003 Playback Decoupling | Abstract `IPlaybackEngine` interface | 2026-11-09 | M2, M5, M6 |
| TDR-004 Windows Toolchain | MSYS2 UCRT64 toolchain | 2026-11-09 | Build & test workflows |
| TDR-005 Video Surface Architecture | `QOpenGLWidget` + `mpv_render_context` (OpenGL FBO) | 2026-11-09 | M2, M3, M6 |

## Blockers

*None. Playback foundation is fully operational and verified.*

## Concerns

*None.*
