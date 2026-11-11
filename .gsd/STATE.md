---
updated: 2026-11-09T11:00:00+06:00
---

# Project State

## Current Position

**Milestone:** M7 - Advanced Features & Preferences Dialog Hierarchy
**Phase:** Phase 7 - Advanced Features & Preferences
**Status:** in_progress
**Plan:** M7 Advanced Features & Preferences Hierarchy

## Last Action

- Completed and verified Wave 7.1: Preferences Dialog Hierarchy & Transactional Settings Architecture.
- Implemented `SettingsTransaction` staging buffer (`src/settings/SettingsTransaction.h/.cpp`) with dirty tracking, commit, discard, reset to defaults, and INI export.
- Implemented `PreferencesPageBase` abstract class and 6 modular category pages (`GeneralPreferencesPage`, `PlaybackPreferencesPage`, `VideoPreferencesPage`, `AudioPreferencesPage`, `SubtitlePreferencesPage`, `FilterControlPreferencesPage`) covering all 22 leaf sub-pages from the verified PotPlayer dialog topology.
- Implemented `PreferencesDialog` (`src/ui/PreferencesDialog.h/.cpp`) matching the 751x564 px baseline geometry at 96 DPI with automatic High-DPI scaling, left category navigation tree, stacked pages, and bottom Dialog 324 action buttons (Reset, Export Settings, OK, Cancel, Apply).
- Wired `F5` hotkey and Context Menu ("Preferences...\tF5") to launch `PreferencesDialog`.
- Implemented `tests/unit/test_preferences_dialog.cpp` with 7 test cases covering transaction staging, discard, defaults reset, INI export, dialog geometry, tree navigation across all 22 leaf pages, Apply/Cancel transactional semantics, and page reset.
- Verified 100% pass across all 8 CTest targets in 16.03s.
- Committed atomically in `ce35ac9`.

## Next Steps

1. Begin Wave 7.2: Frame Capture Subsystem & Snapshot Management:
   - Extend `IPlaybackEngine.h` with `captureFrame(const std::string& outputPath, CaptureMode mode)`.
   - Implement `screenshot-to-file` in `MpvPlaybackEngine` supporting `video`, `subtitles`, and `window` modes.
   - Implement `FrameCaptureHelper` in UI layer for directory configuration, timestamping (`Jonaki_YYYYMMDD_HHMMSS_mmm.png`), overwrite prevention, and temporary file clipboard copying (`Ctrl+C` / `Ctrl+Alt+C`).
   - Wire `Ctrl+E`, `Ctrl+Alt+E`, `Ctrl+C`, `Ctrl+Alt+C` in `MainWindow` and context menu.
   - Implement `tests/integration/test_frame_capture.cpp` and register into CTest.

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
