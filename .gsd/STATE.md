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
  - HTTPS: Distinguished static TLS protocol linkage from demonstrated live streaming with certificate validation (reclassified to Partially Verified).
  - Process Subtitles: Documented disconnect of `isProcessSubtitlesEnabled()` from `MainWindow::openUrlDialog()` and subtitle pipeline (reclassified to Partially Verified / Disconnected UI Feature).
  - Store Only Entered URLs: Documented missing hooks for playlist `.dpl` and drag-and-drop URLs (reclassified to Partially Verified / Incomplete Behavior).
  - URL Redaction: Verified uppercase/percent-encoding handling and documented omission of cloud/CDN parameters (reclassified to Partially Verified).
  - HWDEC: Empirically separated H.264 (hardware accelerated `d3d11va`) from AV1 (software fallback `libdav1d` due to host Haswell GPU limitations).
  - A-B Repeat: Documented lack of runtime wrap tests and missing mpv property reset on `loadMedia()`.
  - Frame Capture: Documented thread-safety defect in `FrameCaptureHelper::captureToFile()` with unmanaged detached worker thread.
- Produced comprehensive `docs/milestones/milestone-reports/milestone_M7_audit_correction_report.md` and updated `milestone_M7_completion_report.md` (eliminated zero defect claim).
- Reverse-engineered PotPlayer reference skin mechanics (`Default.dsf`, `WindowFrame.dsf`):
  - 4-frame horizontal button stride (`[Normal, Hover, Pressed, Disabled]`).
  - Dynamic palette sampling (`ColorX`/`ColorY` into `Main.png`).
  - `Alignment='5'` Win32 GDI centering flag -> `Qt::AlignCenter`.
  - Responsive visibility rules (`VisParentMinWidth`, `VisMinLeft`).
  - Scoped atlas overrides (`<ImageReplace>`).
  - Composite scrollbars (`ScrollBar.bmp`, `ScrollBarPL.bmp`).
  - `PlayPauseWidget` animation timing (150ms in, 350ms hold, 250ms out).
- Generated machine-readable directive inventory: `docs/reverse-engineering/skin_directive_inventory.json` (1,714 records across all 21 reference XML/TXT layout files) and companion `docs/reverse-engineering/skin_directive_inventory.md`.
- Updated `docs/reverse-engineering/skin-compatibility-audit.md`.
- Finalized M8 Architecture: `docs/architecture/m8_skin_and_theme_architecture.md` (Common IR `SkinThemeModel`, single `SkinRenderer`, Zip Slip & Bomb guards, XXE prevention, malformed theme containment).
- Authored `docs/milestones/milestone-reports/milestone_M8_implementation_plan.md` across Waves 8.1–8.4 with detailed empirical acceptance criteria.
- **Wave 8.1 implementation is held locked pending user review.**

## Next Steps

1. Await user review and approval of the M7 Audit Correction and M8 Architecture / Implementation Plan.
2. Upon approval, proceed to Wave 8.1: Parser Expansion & Palette Sampling.

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
