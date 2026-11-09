---
updated: 2026-11-09T09:41:00+06:00
---

# Project State

## Current Position

**Milestone:** M3 - Main Interface Reconstruction & Skin Engine
**Phase:** Phase 3 - Skin Engine & UI Reproduction
**Status:** completed
**Plan:** M3 Skin Engine & UI Reproduction (Completed & Verified)

## Last Action

- Performed focused technical audit of M2 playback foundation: unhooked mpv render callbacks, enforced strict shutdown ordering, added hardware decoding introspection (`getHwdecActive()`), and expanded integration regression suite with 3 tests (`testRepeatedLifecycle`, `testRapidStateChanges`, `testHardwareDecodingDetection`).
- Generated clean-room texture atlas (`resources/skins/default/Main.png`) via procedural generator (`scripts/generate_clean_atlas.py`), avoiding bundling proprietary Daum/Kakao assets.
- Implemented `SkinParser` supporting XML directives and conditional macros (`#if`, `#else`, `#endif`), verified with `SkinParserTest`.
- Implemented `FramelessWindowHelper` supporting native Win32 `WM_NCHITTEST` resize handles and aero snap.
- Reconstructed `TitleBarWidget` (31px custom title bar with "Jonaki ▼" menu, title, pin, min, max, close).
- Reconstructed `BottomControlBar` (62px total: 22px seekbar row with volume/mute + 40px buttons row with time indicators).
- Implemented `ContextMenuBuilder` faithfully mirroring PotPlayer context menu hierarchy.
- Reconstructed `MainWindow` at exact 723x399 px geometry, integrating video surface and controls.
- Verified 100% CTest pass rate across all 4 test suites (Settings, Mock, Integration, SkinParser).
- Verified runtime execution at 723x399 px and captured verification screenshot: `docs/milestones/evidence/m3_jonaki_main_window.png`.

## Next Steps

1. Review M3 completion report and prepare for Milestone M4 (Playlist & Interaction System).
2. Begin M4: Dockable playlist drawer (`F8`), Daum playlist format (`.dpl`) parser/serializer, drag-and-drop recursive queueing, repeat/shuffle modes, and resume position history (`MediaInfo.sdb`).

## Active Decisions

| Decision | Choice | Made | Affects |
|----------|--------|------|---------|
| TDR-001 Architecture Stack | C++20 + Qt6 + CMake + libmpv + FFmpeg | 2026-11-09 | All phases |
| TDR-002 Skin Engine | Clean-room Qt XML skin parser & spritesheet renderer | 2026-11-09 | M3, M4, M8 |
| TDR-003 Playback Decoupling | Abstract `IPlaybackEngine` interface | 2026-11-09 | M2, M5, M6 |
| TDR-004 Windows Toolchain | MSYS2 UCRT64 toolchain | 2026-11-09 | Build & test workflows |
| TDR-005 Video Surface Architecture | `QOpenGLWidget` + `mpv_render_context` (OpenGL FBO) | 2026-11-09 | M2, M3, M6 |
| TDR-006 Frameless Window Integration | Win32 `WM_NCHITTEST` + Qt frameless widget hierarchy | 2026-11-09 | M3, M4 |

## Blockers

*None. M3 UI reconstruction is fully operational and verified.*

## Concerns

*None.*
