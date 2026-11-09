# Next Actions Register

This file maintains the immediate, ordered implementation tasks for active sessions.

---

## Active Milestone: M7 — Advanced Features & Preferences Dialog Hierarchy

### Immediate Next Tasks
1. [x] **Real-World Media Final Evidence Audit:** Audit `docs/testing/real_media_test_report.md` and `tests/integration/test_real_media_playback.cpp` to explicitly distinguish 7 pipeline stages, smoke-test scope, filter vs. acoustic quality separation, and qualified memory stability.
2. [ ] **M7 Reconnaissance & Preferences Hierarchy Design:**
   - Review PotPlayer Preferences Dialog layout (751 × 564 px measured baseline, 184 dialog IDs in REV-007).
   - Implement tree-based Preferences window with left category tree and right settings pane (`src/ui/PreferencesDialog.h/.cpp`).
   - Wire persistent settings from `Settings` manager.
3. [ ] **Frame Capture Subsystem:**
   - Implement `Ctrl+E` instant frame capture to PNG in configured directory.
   - Support capture with/without subtitle overlays.
4. [ ] **Network Streaming URL Playback:**
   - Support opening direct HTTP/HTTPS media URLs, HLS playlists (`.m3u8`), and optional yt-dlp integration hook.
   - Implement Open URL dialog (`Ctrl+U`).
5. [ ] **Hardware Decoding Configuration UI:**
   - Provide hardware decoding profile selection (D3D11VA, DXVA2, copy-back vs. native, auto, off).
   - Display active hwdec in OSD/status.
6. [ ] **M7 Automated Integration Tests & Parity Verification:**
   - Add unit/integration tests for Preferences tree model, URL stream loading, frame capture, and hwdec profile switching.
   - Verify all 7 core CTest targets and real media playback smoke tests pass.
   - Capture runtime screenshot of Preferences dialog and frame capture output.
   - Publish `docs/milestones/milestone-reports/milestone_M7_completion_report.md`.
