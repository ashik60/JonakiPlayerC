# Milestone M7 Completion & Audit Report: Advanced Features & Preferences Hierarchy

**Project:** JonakiPlayer  
**Milestone:** M7 — Advanced Features & Preferences Dialog Hierarchy  
**Baseline Targets Passed:** 13 of 13 CTest targets (100% test pass rate with MSYS2 environment)  
**Status:** COMPLETED WITH QUALIFIED DEFECT REGISTER & AUDIT CORRECTIONS  
**Date:** 2026-11-11 (Audited & Reconciled: 2026-11-12)  
**Detailed Audit Report:** [`milestone_M7_audit_correction_report.md`](file:///f:/Projects/JonakiPlayer/docs/milestones/milestone-reports/milestone_M7_audit_correction_report.md)  

---

## 1. Milestone Overview

Milestone M7 expands JonakiPlayer from basic playback into a full-featured media player matching Daum/Kakao PotPlayer's preferences organization, frame capture capabilities, network stream ingestion, and hardware acceleration controls.

M7 was executed across four distinct waves:
1. **Wave 7.1 — Preferences Dialog Hierarchy & Transactional Settings:** Full two-level 6-category navigation tree (22 leaf pages), Dialog 324 layout, transactional staging (`SettingsTransaction`), High-DPI handling, and Reset/Export/Apply/OK/Cancel semantics.
2. **Wave 7.2 — Frame Capture Subsystem & Snapshot Management:** Platform-independent `IPlaybackEngine::captureFrame`, three capture modes (`Video`, `Subtitles`, `Window`), auto-naming `Jonaki_YYYYMMDD_HHMMSS_mmm.png`, collision avoidance, non-blocking disk writes, clipboard copy, and shortcuts (`Ctrl+E`, `Ctrl+C`).
3. **Wave 7.3 — Open URL & Network Streaming:** Dialog 310 reconstruction, URL validation (`UrlValidator`), sensitive credential/token query parameter redaction in logs, MRU history persistence, clipboard auto-paste, and HTTP/HTTPS/HLS streaming.
4. **Wave 7.4 — Hardware Decoding Profiles & A-B Repeat Loop:** Dynamic hardware profile switching (`auto-safe`, `d3d11va`, `dxva2`, `d3d12va`, `no`), safe software fallback, decoder state inspection, and A-B repeat looping (`[`, `]`, `\`, `Shift+[`, `Shift+]`) with visual timeline markers.

---

## 2. Milestone Test Target Progression & Baseline

| Wave | Primary Deliverables | CTest Targets | Pass Rate |
|---|---|---|---|
| Baseline (M6) | Core playback, subtitles, filters, playlist | 9 | 100% |
| Wave 7.1 | Preferences dialog, settings staging buffer | 10 (`+PreferencesDialogTest`) | 100% |
| Wave 7.2 | Frame capture, screenshot helper | 11 (`+FrameCaptureTest`) | 100% |
| Wave 7.3 | Open URL dialog, network streaming | 11 (updated `OpenUrlDialogTest`, `NetworkStreamingTest`) | 100% |
| Wave 7.4 | Hardware decoding profiles, A-B repeat loop | 13 (`+HwdecProfileTest`, `+ABRepeatTest`) | 100% |

All 13 test targets pass cleanly when executed with the MSYS2 UCRT64 toolchain in PATH:
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

## 3. Independent Audit Findings & Known Defect Register

In accordance with clean-room engineering principles and the independent M7 re-audit, the following specific defects and partial implementations are recorded:

1. **HTTPS Streaming:** Static TLS capability is verified via linked libraries, but demonstrated live HTTPS media streaming with certificate validation was not executed in automated integration tests (plain HTTP `LocalHttpServer` only). *(Partially Verified)*
2. **Process Subtitles Checkbox:** `OpenUrlDialog::isProcessSubtitlesEnabled()` is not wired into `MainWindow::openUrlDialog()` or `SubtitleManager`. Unchecking the box has no effect on subtitle loading. *(Partially Verified / Disconnected UI Feature)*
3. **Store Only Entered URLs:** Setting is not honored for playlist files (`.dpl`) or drag-and-drop URLs. Played stream URLs are never added to Recent URLs when the setting is false. *(Partially Verified / Incomplete Behavior)*
4. **URL Parameter Redaction:** Provider-specific parameters (e.g. AWS `X-Amz-Credential`, Akamai `hdnts`) are omitted from the redaction table and remain unredacted in log outputs. *(Partially Verified)*
5. **HWDEC Active Decoder Separation:** On the test host (Intel Core i5-4200U, Haswell HD Graphics 4400), H.264 is verified as hardware accelerated (`d3d11va`), while AV1 is decoded via software fallback (`libdav1d`). Hardware acceleration for AV1 is not supported by host silicon. *(Partially Verified)*
6. **A-B Repeat Playback Wrapping:** Playback wrapping across boundary Point B back to Point A was not verified with runtime time advancement, and `loadMedia()` fails to reset underlying mpv `ab-loop` properties. *(Partially Verified)*
7. **Frame Capture Thread Safety:** `FrameCaptureHelper::captureToFile()` spawns a detached `std::thread` holding an unmanaged raw `IPlaybackEngine*` pointer, creating a use-after-free risk during engine destruction or application shutdown. *(Partially Verified / Thread-Safety Defect)*

---

## 4. Clean-Room & Security Compliance

1. **Strict Clean-Room:** No proprietary PotPlayer code was copied. All UI layouts and behaviors are clean-room implementations informed by REA structural observations.
2. **Decoupled Architecture:** `IPlaybackEngine` remains 100% free of Qt types (`QImage`, `QPixmap`, `QUrl`, `QString`). All GUI and clipboard integration resides in UI-layer adapters.
3. **Reference Integrity:** Read-only test media in `E:\Video Song` was strictly read and never modified, renamed, or deleted. Reference spritesheets in `resources/reference/` are strictly excluded from distributed build artifacts.
