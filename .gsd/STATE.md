---
updated: 2026-11-11T11:05:00+06:00
---

# Project State

## Current Position

**Milestone:** M7 - Advanced Features & Preferences Dialog Hierarchy
**Phase:** Phase 7 - Advanced Features & Preferences
**Status:** in_progress
**Plan:** M7 Advanced Features & Preferences Hierarchy

## Last Action

- Completed and verified Wave 7.3: Open URL & Network Streaming (`01d1f9e`).
- Implemented `UrlValidator` (`src/network/UrlValidator.h/.cpp`) supporting HTTP, HTTPS, RTMP, and RTMPS schemes, HLS (.m3u8) detection, credential sanitization for diagnostics, and parameter/fragment preservation.
- Implemented `OpenUrlDialog` (`src/ui/OpenUrlDialog.h/.cpp`) reconstructing PotPlayer's Dialog 310 layout:
  - Header prompt ("Input the URL of the stream or file to be played.") and subtext example.
  - Editable URL ComboBox with history dropdown.
  - "Auto paste URLs from clipboard", "Process subtitles", and "Store only entered URLs" options.
  - "Clear all", "Delete selected", "Open" (Default / Enter), and "Cancel" (Escape) buttons.
  - Non-intrusive clipboard auto-paste (only when auto-paste enabled and valid URL on clipboard; never overwrites user-entered text).
  - Most-recent-first history persistence through `SettingsManager` with deduplication and 20-item cap.
- Integrated `Ctrl+U` shortcut in `MainWindow` and wired "Open URL...\tCtrl+U" in context menu, directly appending opened stream URLs to the playlist and starting playback.
- Generated deterministic local HLS VOD fixture with `.m3u8` playlist and `.ts` transport stream segments in `tests/test_data/hls/`.
- Implemented unit test suite `tests/unit/test_open_url_dialog.cpp` (`OpenUrlDialogTest`) covering URL scheme validation, invalid formatting, credential masking, clipboard auto-paste, manual input preservation, and history persistence/deduplication/clearing (14 passing cases).
- Implemented integration test suite `tests/integration/test_network_streaming.cpp` (`NetworkStreamingTest`) featuring an embedded `LocalHttpServer` covering direct HTTP MP4 playback, HLS VOD streaming, HTTP 302 redirects, HTTP 404 error transitions, invalid M3U8 handling, missing HLS segment recovery, and clean engine shutdown during buffering.
- Verified 100% pass across all 11 CTest targets in 32.89s.

## Next Steps

1. Begin Wave 7.4: Hardware-Decoding Profiles & A-B Repeat Loop:
   - Investigate and verify supported hardware decoding profiles in bundled libmpv/MSYS2 UCRT64 environment (e.g. `d3d11va`, `d3d11va-copy`, `auto-safe`, `no`).
   - Implement dynamic runtime `hwdec` switching through `MpvPlaybackEngine` and observe active decoder via `hwdec-current`.
   - Implement A-B repeat mechanism (`ab-loop-a`, `ab-loop-b`, `ab-loop-clear`) with keyboard shortcuts `[` (set A), `]` (set B), `\` (clear).
   - Add A-B repeat seekbar tick markers in `BottomControlBar`.
   - Implement `tests/integration/test_hwdec_ab_loop.cpp` and register into CTest.

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
