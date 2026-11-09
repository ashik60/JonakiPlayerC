# Milestone M5 Implementation Plan: Subtitle Engine & OSD System

> **Milestone:** M5 — Subtitle Engine & OSD System  
> **Status:** SPEC & ARCHITECTURE LOCKED  
> **Target:** Full subtitle lifecycle (SRT, ASS, VTT), track switching, auto-matching, real-time sync, and PotPlayer-faithful OSD  
> **Framework:** GSD v1.6.0  

---

## 1. Objectives & Non-Negotiables

1. **Embedded & External Subtitle Support:** Seamless loading and decoding of embedded tracks and external subtitle files (`.srt`, `.ass`/`.ssa`, `.vtt`, `.sub`, `.smi`).
2. **Zero Redundant Decoding:** Leverage `libmpv`'s integrated `libass` and ffmpeg demuxer for hardware-blended subtitle rasterization and timing, eliminating custom subtitle parser lag.
3. **Automatic Subtitle Matching:** When playing `video.mp4`, automatically detect and load adjacent subtitle files sharing the same basename (`video.srt`, `video.en.srt`, `video.ass`, `video.vtt`).
4. **Real-Time Synchronization:** Hotkey adjustments: `<` (decrease delay by 0.5s), `>` (increase delay by 0.5s), `/` (reset delay to 0.0s), with real-time OSD confirmation.
5. **PotPlayer-Faithful OSD:** On-screen display notifications rendered directly on the video viewport for playback status, volume changes, seek position, track changes, and delay offsets.
6. **Decoupled Architecture:** Subtitle track discovery and preferences encapsulated in `SubtitleManager` rather than bloated inside `MainWindow`.

---

## 2. Work Breakdown & Waves

### Wave 1: Subtitle Architecture & IPlaybackEngine Extensions
- [ ] Task 1.1: Extend `IPlaybackEngine` and `MpvPlaybackEngine` with full subtitle controls:
  - `loadExternalSubtitle(path)`
  - `setSubtitleDelay(seconds)`
  - `getSubtitleDelay()`
  - `setSubtitleVisibility(bool)`
  - `isSubtitleVisible()`
  - `showOsdMessage(text, durationMs)`
- [ ] Task 1.2: Implement `SubtitleManager` handling auto-discovery of adjacent subtitle files, subtitle styling preferences, and track enumeration.

### Wave 2: Subtitle Context Menu & Keyboard Shortcuts
- [ ] Task 2.1: Implement Subtitle submenu in `ContextMenuBuilder` (Select track, Add subtitle, Toggle visibility, Subtitle sync +/-0.5s).
- [ ] Task 2.2: Map PotPlayer keyboard shortcuts:
  - `Alt+O`: Open subtitle file dialog
  - `Alt+H`: Toggle subtitle display
  - `<` / `>`: Subtitle sync -0.5s / +0.5s
  - `/`: Subtitle sync reset
  - `Alt+PageUp` / `Alt+PageDown`: Subtitle font size +/-

### Wave 3: Automated Regression & Subtitle Test Suite
- [ ] Task 3.1: Implement `tests/unit/test_subtitles.cpp`:
  - Validate external subtitle loading (`test_sub.srt`, `test_sub.ass`, `test_sub.vtt`).
  - Validate Unicode filenames and CJK/Hangul subtitle content.
  - Validate track enumeration and dynamic track selection.
  - Validate delay adjustment and boundary clamping.
- [ ] Task 3.2: Verify 100% CTest pass rate across all 6 test suites.

### Wave 4: Runtime Verification & Visual Comparison
- [ ] Task 4.1: Launch `JonakiPlayer.exe` with `test_video_aac.mp4` and `test_sub.srt`.
- [ ] Task 4.2: Capture verification screenshot showing active subtitle and OSD text.
- [ ] Task 4.3: Document visual and behavioral parity in `milestone_M5_completion_report.md`.
