---
updated: 2026-11-09T09:55:00+06:00
---

# Project State

## Current Position

**Milestone:** M4 - Playlist & Interaction System
**Phase:** Phase 4 - Playlist & Persistence
**Status:** completed
**Plan:** M4 Playlist & Interaction System (Completed & Verified)

## Last Action

- Audited M3 skin engine directives, recording unsupported PotPlayer directives as unresolved in `docs/reverse-engineering/skin-compatibility-audit.md`.
- Decoupled parity classification into Geometric, Visual, and Behavioral tiers in Feature Parity Matrix.
- Implemented `DplPlaylistParser`: Round-trip compatible Daum PotPlayer `.dpl` parser and serializer preserving unknown/future metadata fields.
- Implemented `ResumeStore`: Clean-room SQLite persistence engine matching PotPlayer's `MediaInfo.sdb` schema and percent-encoded keys.
- Implemented `PlaylistModel` and `PlaylistController`: Queue management, sequential / repeat / shuffle orders, folder recursive scanning, auto-resume position restoration upon playback start.
- Implemented `PlaylistDrawer`: 280px dockable / collapsible sliding drawer, expanding window from 723px to 1003px while preserving video viewport, toggled via `F8`.
- Added unit and round-trip tests (`PlaylistTest`) and maintained 100% CTest pass rate across 5 suites.
- Verified runtime execution with playlist open, capturing `docs/milestones/evidence/m4_jonaki_with_playlist.png`.

## Next Steps

1. Review M4 completion report and prepare for Milestone M5 (Subtitle Engine & OSD System).
2. Begin M5: Embedded subtitle track selection, external subtitle matching (`.srt`, `.ass`, `.vtt` with same basename), real-time subtitle delay adjustment (`<` and `>`), and custom OSD styling.

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

## Blockers

*None. M4 playlist and interaction system is fully operational and verified.*

## Concerns

*None.*
