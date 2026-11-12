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

## 3. Independent Audit Findings & Remediation Register

In accordance with clean-room engineering principles and the independent M7 re-audit, all 7 findings were systematically remediated, validated with new integration tests, and documented in `milestone_M7_remediation_report.md`:

1. **HTTPS Streaming & Certificate Validation:**
   - *Audit Finding:* Static TLS capability was verified via linked libraries, but live HTTPS media streaming with certificate validation had not been executed in automated integration tests.
   - *Remediation & Status:* **Verified.** Implemented local TLS test server (`scripts/test_https_server.py`) and configured strict certificate verification via libmpv `tls-ca-file` and `tls-verify=yes`. Validated trusted CA playback and untrusted certificate rejection in `NetworkStreamingTest`.
2. **Process Subtitles Checkbox:**
   - *Audit Finding:* `OpenUrlDialog::isProcessSubtitlesEnabled()` was not wired into `MainWindow::openUrlDialog()` or `SubtitleManager`.
   - *Remediation & Status:* **Verified.** Wired checkbox value directly to `SubtitleManager::setNetworkSubtitleProcessingEnabled()` and `IPlaybackEngine::setSubtitleVisibility()`. Validated in `OpenUrlDialogTest`.
3. **Store Only Entered URLs:**
   - *Audit Finding:* Setting was not honored for playlist files (`.dpl`) or drag-and-drop URLs.
   - *Remediation & Status:* **Verified.** Implemented and documented setting semantics: manual entry always records into `RecentUrls`; playlists and drag-and-drop record into `RecentUrls` only when `StoreEnteredUrlsOnly=false`. Validated in `OpenUrlDialogTest` and `PlaylistTest`.
4. **URL Parameter Redaction:**
   - *Audit Finding:* Provider-specific parameters (e.g. AWS `X-Amz-Credential`, Akamai `hdnts`) were omitted from the redaction table.
   - *Remediation & Status:* **Verified.** Expanded redaction to handle AWS, Akamai, CloudFront, Google Cloud Storage, and Azure SAS parameters with case-insensitive and percent-encoded handling while preserving innocent parameters. Validated in `OpenUrlDialogTest`.
5. **HWDEC Active Decoder Separation:**
   - *Audit Finding:* Blanket hardware acceleration statement conflated H.264 and AV1.
   - *Remediation & Status:* **Verified.** On the test host (Intel Core i5-4200U, Haswell HD Graphics 4400), H.264 is verified as hardware accelerated (`d3d11va`), while AV1 is explicitly identified as software fallback (`libdav1d`) due to host silicon capabilities. Validated in `HwdecProfileTest`.
6. **A-B Repeat Playback Wrapping & State Reset:**
   - *Audit Finding:* Playback wrapping across Point B back to Point A was not verified with runtime time advancement, and `loadMedia()` failed to reset underlying mpv `ab-loop` properties.
   - *Remediation & Status:* **Verified.** Native mpv properties `ab-loop-a` and `ab-loop-b` are reset to `"no"` on `loadMedia()`, `stop()`, and `shutdown()`. Verified actual playback wrapping from 1.0s to 0.5s on real media in `ABRepeatTest`.
7. **Frame Capture Thread Safety:**
   - *Audit Finding:* `FrameCaptureHelper::captureToFile()` spawned a detached `std::thread` holding a raw `IPlaybackEngine*` pointer.
   - *Remediation & Status:* **Verified.** Removed detached threads; replaced with `mpv_command_async`, active operation registry with condition variable, RAII temporary file guard, and cancellation during `MainWindow` close and `shutdown()`. Validated rapid captures, engine destruction during capture, and shutdown cancellation in `FrameCaptureTest`.

---

## 4. Clean-Room & Security Compliance

1. **Strict Clean-Room:** No proprietary PotPlayer code was copied. All UI layouts and behaviors are clean-room implementations informed by REA structural observations.
2. **Decoupled Architecture:** `IPlaybackEngine` remains 100% free of Qt types (`QImage`, `QPixmap`, `QUrl`, `QString`). All GUI and clipboard integration resides in UI-layer adapters.
3. **Reference Integrity:** Read-only test media in `E:\Video Song` was strictly read and never modified, renamed, or deleted. Reference spritesheets in `resources/reference/` are strictly excluded from distributed build artifacts.
