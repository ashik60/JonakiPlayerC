---
updated: 2026-11-09T10:30:00+06:00
---

# Project State

## Current Position

**Milestone:** M6 - Audio & Video Systems
**Phase:** Phase 6 - Audio & Video Processing
**Status:** completed
**Plan:** M6 Audio & Video Systems (Completed & Verified)

## Last Action

- Performed focused M5 verification pass: tested Bengali subtitle rendering (যুক্তাক্ষর, কারচিহ্ন) and deterministic adjacent candidate selection priority.
- Implemented 10-band graphic equalizer (31, 62, 125, 250, 500, 1k, 2k, 4k, 8k, 16k Hz) with independent gain adjustment clamped to [-12dB, +12dB], reset, and toggle (`Shift+E`).
- Implemented dynamic audio normalization using FFmpeg `dynaudnorm` (`f=500:g=31:p=0.95:m=10`), with unified filter graph management preventing chain corruption when toggled alongside the equalizer (`Shift+N`).
- Implemented deinterlacing (`off`, `auto`, `on`) with hotkey `Ctrl+I`, verified against synthetic interlaced test media (`test_interlaced.mp4`).
- Implemented audio delay synchronization in +/-0.05s steps (`Shift+<` / `Shift+>`), reset (`Shift+/`), and non-blocking OSD confirmation.
- Implemented display aspect ratio scaling (Default, 16:9, 4:3, 16:10, 1:1, custom) with hotkey `Ctrl+F6` and context submenu.
- Added comprehensive unit/integration test target `AudioVideoFilterTest`. All 7 test suites passing 100% in CTest.
- Captured runtime evidence screenshot: `docs/milestones/evidence/m6_jonaki_audio_video_systems.png`.
- Published `docs/milestones/milestone-reports/milestone_M6_completion_report.md` and updated Feature Parity Matrix.

## Next Steps

1. Review M6 completion report and prepare for Milestone M7: Advanced Features & Preferences Dialog Hierarchy.
2. Begin M7: Preferences tree dialog (751x564 px baseline matching PotPlayer), streaming URL playback (HTTP/HLS/YouTube yt-dlp hook), frame capture tool (`Ctrl+E` to PNG), and hardware decoding selection.

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
