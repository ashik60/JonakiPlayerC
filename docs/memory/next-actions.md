# Next Actions Register

This file maintains the immediate, ordered implementation tasks for active sessions.

---

## Active Milestone: M7 — Advanced Features & Preferences Dialog Hierarchy

### Immediate Next Tasks
1. [x] **Real-World Media Final Evidence Audit:** Audit `docs/testing/real_media_test_report.md` and `tests/integration/test_real_media_playback.cpp` to explicitly distinguish 7 pipeline stages, smoke-test scope, filter vs. acoustic quality separation, and qualified memory stability.
2. [x] **M7 Reconnaissance & Preferences Hierarchy Design (Wave 7.1):**
   - Review PotPlayer Preferences Dialog layout (751 × 564 px measured baseline, 24 sub-pages across 6 categories in REV-018).
   - Implement `SettingsTransaction` staging buffer with commit, discard, reset to defaults, and INI export.
   - Implement tree-based Preferences window with left category tree and right settings pane (`src/ui/PreferencesDialog.h/.cpp`) and 6 modular category page groups.
   - Wire `F5` shortcut and Context Menu item.
   - Verified 100% pass on `PreferencesDialogTest`.
3. [x] **Frame Capture Subsystem & Clipboard Integration (Wave 7.2):**
   - Implement platform-independent `CaptureMode` enum and `captureFrame()` in `IPlaybackEngine` and `MpvPlaybackEngine` via `screenshot-to-file`.
   - Implement `FrameCaptureHelper` with collision-safe filenames (`Jonaki_YYYYMMDD_HHMMSS_mmm.png`), directory management, asynchronous capture, and clipboard image copying.
   - Wire `Ctrl+E`, `Ctrl+Alt+E`, `Ctrl+C`, `Ctrl+Alt+C`, and Video Capture context menu.
   - Verified 100% pass on `FrameCaptureTest`.
4. [x] **Network Streaming URL Playback (Wave 7.3):**
   - Implement `UrlValidator` supporting HTTP, HTTPS, RTMP, and RTMPS schemes, HLS (.m3u8) detection, parameter preservation, and credential sanitization.
   - Implement `OpenUrlDialog` (`Ctrl+U`) matching PotPlayer Dialog 310 layout with history combobox, auto-clipboard paste toggle, and persistence.
   - Wire `Ctrl+U` shortcut and File -> Open URL menu item, appending directly to playlist.
   - Implement automated local HTTP test server harness and deterministic HLS test stream fixtures.
   - Implement `tests/unit/test_open_url_dialog.cpp` and `tests/integration/test_network_streaming.cpp`.
   - Verified 100% pass across all 11 CTest targets.
5. [ ] **Hardware Decoding Configuration & A-B Repeat Loop (Wave 7.4):**
   - Provide hardware decoding profile selection (D3D11VA, DXVA2, copy-back vs native, auto, off).
   - Implement dynamic runtime `hwdec` switching and active status feedback.
   - Implement A-B repeat (`[`, `]`, `\`) with seekbar tick rendering.
6. [ ] **M7 Automated Integration Tests & Parity Verification:**
   - Verify full 12-target CTest suite passes.
   - Capture runtime screenshots of Preferences dialog, frame capture output, and open URL dialog.
   - Publish `docs/milestones/milestone-reports/milestone_M7_completion_report.md`.
