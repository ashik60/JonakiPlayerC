---
updated: 2026-11-11T12:20:00+06:00
---

# Project State

## Current Position

**Milestone:** M7 - Advanced Features & Preferences Dialog Hierarchy
**Phase:** Phase 7 - Advanced Features & Preferences
**Status:** completed
**Plan:** M7 Advanced Features & Preferences Hierarchy

## Last Action

- Completed and verified Wave 7.4: Hardware Decoding Profiles & A-B Repeat Loop.
- Extended `IPlaybackEngine` and implemented dynamic hardware-decoding profile switching (`auto-safe`, `d3d11va`, `d3d11va-copy`, `dxva2`, `dxva2-copy`, `d3d12va`, `d3d12va-copy`, `no`) in `MpvPlaybackEngine` without playback crash or interruption.
- Implemented active decoder and video codec inspection with fallback to track properties.
- Implemented native A-B repeat looping (`[` set A, `]` set B, `\` toggle, `Shift+[` clear A, `Shift+]` clear B).
- Added timeline marker rendering in `BottomControlBar` (cyan for Point A, red/orange for Point B).
- Wired `PreferencesDialog` hardware acceleration profile selection to the active playback engine.
- Implemented `tests/integration/test_hwdec_profiles.cpp` (10 passing cases) and `tests/unit/test_ab_repeat.cpp` (9 passing cases).
- Verified 100% pass across all 13 CTest targets.
- Verified real-world media playback and hardware acceleration on `E:\Video Song`.
- Generated `docs/milestones/milestone-reports/wave_7_4_completion_report.md` and `docs/milestones/milestone-reports/milestone_M7_completion_report.md`.

## Next Steps

1. Present Milestone M7 Completion Report for user review.
2. Upon approval, transition to Milestone M8: Skin Engine Fidelity & Complete Theming.

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
