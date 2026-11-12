---
updated: 2026-11-12T08:36:00+06:00
---

# Project State

## Current Position

**Milestone:** M7 / M8 Transition (M7 Audit Correction & M8 Architecture Finalization)
**Phase:** Phase 8 - Skin Engine Fidelity & Complete Theming
**Status:** planning
**Plan:** Milestone M8 Implementation Plan (Pre-Implementation Lock Active)

## Last Action

- Completed independent M7 audit correction and empirical re-examination across source code, tests, and runtime artifacts:
  - HTTPS: Distinguished static TLS protocol linkage from demonstrated live streaming with certificate validation.
  - Process Subtitles: Documented and remediated disconnect of `isProcessSubtitlesEnabled()` from `MainWindow::openUrlDialog()` and subtitle pipeline.
  - Store Only Entered URLs: Documented and implemented setting semantics for manual vs playlist/drag-and-drop URLs.
  - URL Redaction: Expanded table to handle AWS, Akamai, CloudFront, GCS, and Azure SAS keys with case-insensitive and percent-encoded handling.
  - HWDEC: Empirically separated H.264 (hardware accelerated `d3d11va`) from AV1 (software fallback `libdav1d` due to host Haswell GPU limitations).
  - A-B Repeat: Reset native mpv properties on media load/stop/shutdown; verified actual B to A playback wrapping on real media.
  - Frame Capture: Remediated thread-safety defect by eliminating detached threads and implementing `mpv_command_async`, active operation tracking, and RAII cleanup guard.
- Executed and passed all 13 CTest test targets (100% pass rate in 54.02s).
- Produced comprehensive `docs/milestones/milestone-reports/milestone_M7_remediation_report.md`, updated `milestone_M7_audit_correction_report.md` and `milestone_M7_completion_report.md`.
- Refined M8 directive inventory: documented 25 inspected vs 21 active layout files (explained stubs, metadata, and string table exclusions), separated 59 unique directives from 1,714 occurrence instances, and explicitly categorized evidence confidence (direct runtime observation, static resource evidence, strong inference, approximate timing).
- **Wave 8.1 implementation remains held locked pending user review.**

## Next Steps

1. Present M7 remediation results, refined inventory, and finalized M8 architecture to the user.
2. Await user confirmation before unlocking Wave 8.1 implementation.

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
| TDR-010 Common Skin & Theme IR | Unified `SkinThemeModel` IR + single `SkinRenderer` pipeline | 2026-11-12 | M8 |
| TDR-011 Clean-Room Reference Isolation | Exclude reference spritesheets/assets from distribution packaging | 2026-11-12 | M8, M10 |

## Blockers

*None. Pre-implementation lock active.*

## Concerns

*None.*
