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

- Completed rigorous technical audit of real-world media playback verification across 85 files in `E:\Video Song`.
- Restructured `docs/testing/real_media_test_report.md` to explicitly define and evaluate the 7-stage playback pipeline (Demuxing -> State Playing -> PTS Progression -> Video Decoding -> OpenGL FBO Render -> Audio Decoding -> Audio Sink Delivery).
- Explicitly documented the 85 tests as playback smoke tests (0.5s–2.0s initialization, decode start, time progression over 66.5s runtime), separating them from full 12.54 hour playback.
- Clarified boundaries between automated headless testing (packet decoding, filter compilation) vs. interactive GUI playback (OpenGL FBO rasterization and WASAPI audio delivery).
- Formally separated filter graph compilation validity from subjective acoustic quality.
- Qualified memory stability observations (zero crashes, leaks, or zombie threads across 85 consecutive sessions vs. formal heap leak instrumentation).
- Verified `test_real_media_playback.cpp` compiles and all 7 core CTest targets pass 100%.
- Verified `E:\Video Song` remains completely untouched.

## Next Steps

1. Begin Milestone M7 implementation:
   - Implement Preferences Tree Dialog (`src/ui/PreferencesDialog.h/.cpp`) matching PotPlayer baseline (751x564 px).
   - Implement Frame Capture tool (`Ctrl+E` instant frame save to PNG with/without subtitle overlays).
   - Implement Open URL Dialog (`Ctrl+U`) supporting direct HTTP, HTTPS, and HLS `.m3u8` streams.
   - Implement Hardware Decoding Selection UI and persistence (D3D11VA, DXVA2, copy-back, auto, off).
2. Add automated unit/integration tests for M7 features and verify 100% CTest pass.
3. Capture runtime evidence screenshot and generate M7 completion report.

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
