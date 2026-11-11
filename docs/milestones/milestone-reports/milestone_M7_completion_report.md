# Milestone M7 Completion Report: Advanced Features & Preferences Dialog Hierarchy

**Project:** JonakiPlayer  
**Milestone:** M7 — Advanced Features & Preferences Dialog Hierarchy  
**Baseline Targets Passed:** 13 of 13 CTest targets (100% pass rate)  
**Status:** COMPLETED & AUDITED  
**Date:** 2026-11-11  

---

## 1. Milestone Overview

Milestone M7 expands JonakiPlayer from core playback into a full-featured media player matching Daum/Kakao PotPlayer's preferences organization, frame capture capabilities, network stream ingestion, and hardware acceleration controls.

M7 was executed across four distinct waves:
1. **Wave 7.1 — Preferences Dialog Hierarchy & Transactional Settings:** Full two-level 6-category navigation tree (22 leaf pages), Dialog 324 layout, transactional staging (`SettingsTransaction`), High-DPI handling, and Reset/Export/Apply/OK/Cancel semantics.
2. **Wave 7.2 — Frame Capture Subsystem & Snapshot Management:** Platform-independent `IPlaybackEngine::captureFrame`, three capture modes (`Video`, `Subtitles`, `Window`), auto-naming `Jonaki_YYYYMMDD_HHMMSS_mmm.png`, collision avoidance, non-blocking disk writes, clipboard copy, and shortcuts (`Ctrl+E`, `Ctrl+C`).
3. **Wave 7.3 — Open URL & Network Streaming:** Dialog 310 reconstruction, URL validation (`UrlValidator`), sensitive credential/token query parameter redaction in logs, MRU history persistence, clipboard auto-paste, and HTTP/HTTPS/HLS streaming.
4. **Wave 7.4 — Hardware Decoding Profiles & A-B Repeat Loop:** Dynamic hardware profile switching (`auto-safe`, `d3d11va`, `dxva2`, `d3d12va`, `no`), safe software fallback, decoder state inspection, and A-B repeat looping (`[`, `]`, `\`, `Shift+[`, `Shift+]`) with visual timeline markers.

---

## 2. Milestone Test Target Progression

| Wave | Primary Deliverables | CTest Targets | Pass Rate |
|---|---|---|---|
| Baseline (M6) | Core playback, subtitles, filters, playlist | 9 | 100% |
| Wave 7.1 | Preferences dialog, settings staging buffer | 10 (`+PreferencesDialogTest`) | 100% |
| Wave 7.2 | Frame capture, screenshot helper | 11 (`+FrameCaptureTest`) | 100% |
| Wave 7.3 | Open URL dialog, network streaming | 11 (updated `OpenUrlDialogTest`, `NetworkStreamingTest`) | 100% |
| Wave 7.4 | Hardware decoding profiles, A-B repeat loop | 13 (`+HwdecProfileTest`, `+ABRepeatTest`) | 100% |

All 13 test targets pass cleanly:
- `SettingsTest`
- `PlaybackEngineMockTest`
- `MpvPlaybackIntegrationTest`
- `SkinParserTest`
- `PlaylistTest`
- `SubtitleTest`
- `AudioVideoFilterTest`
- `PreferencesDialogTest`
- `FrameCaptureTest`
- `OpenUrlDialogTest`
- `NetworkStreamingTest`
- `HwdecProfileTest`
- `ABRepeatTest`

---

## 3. Clean-Room & Architectural Compliance

1. **Strict Clean-Room:** No proprietary PotPlayer code was copied. All UI layouts and behaviors are clean-room implementations informed by REA structural observations.
2. **Decoupled Architecture:** `IPlaybackEngine` remains 100% free of Qt types (`QImage`, `QPixmap`, `QUrl`, `QString`). All GUI and clipboard integration resides in UI-layer adapters.
3. **Reference Integrity:** Read-only test media in `E:\Video Song` was strictly read and never modified, renamed, or deleted.
