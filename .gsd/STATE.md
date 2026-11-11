---
updated: 2026-11-11T09:20:00+06:00
---

# Project State

## Current Position

**Milestone:** M7 - Advanced Features & Preferences Dialog Hierarchy
**Phase:** Phase 7 - Advanced Features & Preferences
**Status:** in_progress
**Plan:** M7 Advanced Features & Preferences Hierarchy

## Last Action

- Completed and verified Wave 7.2: Frame Capture Subsystem & Snapshot Management (`cd93e48`).
- Extended `IPlaybackEngine.h` with `enum class CaptureMode { Video, Subtitles, Window }` and `virtual bool captureFrame(const std::string& outputPath, CaptureMode mode) = 0`.
- Implemented `captureFrame()` in `MpvPlaybackEngine` using libmpv's `screenshot-to-file` with proper mode flags (`video`, `subtitles`, `window`) and UTF-8 path handling.
- Implemented `FrameCaptureHelper` in the UI layer (`src/ui/FrameCaptureHelper.h/.cpp`):
  - Configurable capture folder with fallback to `Pictures/JonakiPlayer`.
  - Collision-safe unique timestamped filenames (`Jonaki_YYYYMMDD_HHMMSS_mmm.png`).
  - Directory verification and automatic creation.
  - Asynchronous non-blocking file capture with OSD notification feedback.
  - Safe clipboard image capture using temporary file, `QImage` loading, and `QGuiApplication::clipboard()->setImage()`.
  - Folder reveal via `QDesktopServices::openUrl()`.
- Added shortcuts and menus:
  - `Ctrl+E`: Capture source frame (video).
  - `Ctrl+Alt+E`: Capture screen frame (subtitles).
  - `Ctrl+C`: Copy source frame to clipboard (with fallback to standard text copy when text widget is focused).
  - `Ctrl+Alt+C`: Copy screen frame to clipboard.
  - Video Capture submenu in context menu with all actions.
- Implemented comprehensive integration test suite `tests/integration/test_frame_capture.cpp` registered as `FrameCaptureTest`.
- Verified 100% pass across all 9 CTest targets in 30.64s.

## Next Steps

1. Begin Wave 7.3: Open URL & Network Streaming:
   - Implement `UrlValidator` supporting HTTP, HTTPS, HLS (`.m3u8`), DASH (`.mpd`), and RTMP.
   - Implement `OpenUrlDialog` with URL history combobox, auto-clipboard paste toggle, and Dialog geometry matching PotPlayer reference.
   - Implement URL history persistence in `SettingsManager`.
   - Wire `Ctrl+U` shortcut and File -> Open URL menu item.
   - Create local HTTP test server harness for automated HLS stream playback test.
   - Implement `tests/integration/test_network_streaming.cpp` and register into CTest.

## Active Decisions

| Decision | Choice | Made | Affects |
|----------|--------|------|---------|
| TDR-001 Architecture Stack | C++20 + Qt6 + CMake + libmpv + FFmpeg | 2026-11-09 | All phases |
| TDR-002 Skin Engine | Clean-room Qt XML skin parser & spritesheet renderer | 2026-11-09 | M3, M4, M8 |
| TDR-003 Playback Decoupling | Abstract `IPlaybackEngine` interface | 2026-11-09 | M2, M5, M6 |
| TDR-004 Windows Toolchain | MSYS2 UCRT64 toolchain | 2026-11-09 | Build & test workflows |
| TDR-005 Video Surface Architecture | `QOpenGLWidget` + `mpv_render_context` (OpenGL FBO) | 2026-11-09 | M2, M3, M6 |
| TDR-006 Frameless Window Integration | Win32 `WM_NCHITTEST` + Qt frameless widget hierarchy | 2026-11-09 | M3, M4 |
| TDR-007 Playlist & Resume Schema | Daum `.dpl` parser + SQLite `MediaInfo.sdb` | 2026-11-09 | M4, M7 |
| TDR-008 Subtitle & OSD Pipeline | `libmpv` native `libass` + OSD overlay via `show-text` | 2026-11-09 | M5, M6 |
| TDR-009 Filter Chain Management | Unified `applyAudioFilters()` managing EQ and dynaudnorm | 2026-11-09 | M6, M7 |

## Blockers

*None. Audio & Video systems are fully operational and verified.*

## Concerns

*None.*
