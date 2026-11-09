---
updated: 2026-11-09T10:05:00+06:00
---

# Project State

## Current Position

**Milestone:** M5 - Subtitle Engine & OSD System
**Phase:** Phase 5 - Subtitles & OSD
**Status:** completed
**Plan:** M5 Subtitle Engine & OSD System (Completed & Verified)

## Last Action

- Performed preliminary audit of M4 playlist persistence, non-destructive field retention, and URI keying for Windows/UNC/URLs.
- Implemented `SubtitleManager`: Directory scanning for adjacent subtitles matching media basename, format recognition (`.srt`, `.ass`, `.vtt`, `.sub`), delay adjustments (+/-0.5s), and visibility toggling.
- Enhanced `IPlaybackEngine` and `MpvPlaybackEngine` with `setSubtitleDelay()`, `setSubtitleVisibility()`, and `showOsdMessage()` using libmpv's built-in OSD and `libass` rendering pipeline.
- Added subtitle context menu actions and PotPlayer keyboard shortcuts (`<`, `>`, `/`, `Alt+H`, `Alt+O`).
- Created synthetic test assets for SRT, ASS, VTT, and multilingual CJK/Korean subtitles.
- Implemented automated test target `SubtitleTest`, passing 100% alongside all previous 5 test targets (6/6 targets passing in CTest).
- Verified runtime execution, capturing screenshot at `docs/milestones/evidence/m5_jonaki_subtitles_osd.png`.
- Published `docs/milestones/milestone-reports/milestone_M5_completion_report.md` and updated Feature Parity Matrix.

## Next Steps

1. Review M5 completion report and prepare for Milestone M6 (Audio & Video Systems).
2. Begin M6: 10-Band graphic equalizer (31Hz–16kHz), dynamic audio normalization filter (`dynaudnorm`), hardware/software deinterlacing, audio delay adjustment (`Shift+<` / `Shift+>`), and video aspect ratio scaling modes.

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

## Blockers

*None. Subtitle and OSD systems are fully operational and verified.*

## Concerns

*None.*
