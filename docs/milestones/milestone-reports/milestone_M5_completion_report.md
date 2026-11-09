# Milestone M5 Completion Report: Subtitle Engine & OSD System

> **Date:** 2026-11-09  
> **Status:** COMPLETED & EMPIRICALLY VERIFIED  
> **Target:** Milestone M5 — Subtitle Engine & OSD System  
> **Frameworks:** GSD v1.6.0 & REA v6.8.0  

---

## 1. Executive Summary

Milestone M5 delivers comprehensive subtitle processing, automatic subtitle discovery, real-time synchronization offsets, multi-format parsing (`.srt`, `.ass`/`.ssa`, `.vtt`), and on-screen display (OSD) status presentation for **JonakiPlayer**.

By leveraging `libmpv`'s integrated `libass` renderer within our hardware OpenGL pipeline, high-performance subtitle rasterization with precise typography, outlines, shadows, and styling is achieved with zero custom rendering overhead or tearing.

---

## 2. Implementation Overview

### 2.1 Subtitle Management Subsystem (`SubtitleManager`)
- **Location:** `src/subtitles/SubtitleManager.h`, `src/subtitles/SubtitleManager.cpp`
- **Automatic Matching:** Scans the parent directory of active media files for adjacent subtitle files matching the base filename (e.g., `movie.mp4` matches `movie.srt`, `movie.en.srt`, `movie.ass`, `movie.vtt`).
- **Visibility Control:** Toggles subtitle rendering on and off via `mpv_set_property_async("sub-visibility")`.
- **Synchronization Adjustment:** Dynamically offsets subtitle timing in +/-0.5s increments (`<` and `>`), with instant reset to default (`/`), dispatched to `sub-delay`.

### 2.2 On-Screen Display (OSD) Engine
- **Location:** `src/playback/MpvPlaybackEngine.cpp`
- Configured default OSD presentation: `osd-font=Tahoma`, `osd-font-size=24`, `osd-color=#FFFFFFFF`, `osd-border-color=#FF000000`, `osd-border-size=1.65`.
- Implemented `showOsdMessage(text, durationMs)` displaying notifications on the video canvas for subtitle delay changes, track selections, volume modifications, and playback state transitions.

### 2.3 Context Menu & Keyboard Shortcuts
- **Location:** `src/ui/ContextMenuBuilder.cpp`, `src/ui/MainWindow.cpp`
- **Context Menu:** Added "Subtitles" submenu with "Load Subtitle... (Alt+O)", "Show/Hide Subtitles (Alt+H)", "Subtitle Sync: Slower (-0.5s)", "Subtitle Sync: Faster (+0.5s)", and "Subtitle Sync: Reset".
- **Keyboard Shortcuts:**
  - `<` / `,`: Subtitle delay -0.5s (Slower)
  - `>` / `.`: Subtitle delay +0.5s (Faster)
  - `/`: Subtitle delay reset (0.0s)
  - `Alt+H`: Toggle subtitle visibility
  - `Alt+O`: Open subtitle dialog

---

## 3. Empirical Test & Verification Results

### 3.1 Automated Test Suite (`ctest`)
All 6 test targets passed 100%:
```
Test project F:/Projects/JonakiPlayer/build
    Start 1: SettingsTest
1/6 Test #1: SettingsTest .....................   Passed    0.07 sec
    Start 2: PlaybackEngineMockTest
2/6 Test #2: PlaybackEngineMockTest ...........   Passed    0.11 sec
    Start 3: MpvPlaybackIntegrationTest
3/6 Test #3: MpvPlaybackIntegrationTest .......   Passed    2.86 sec
    Start 4: SkinParserTest
4/6 Test #4: SkinParserTest ...................   Passed    0.06 sec
    Start 5: PlaylistTest
5/6 Test #5: PlaylistTest .....................   Passed    0.30 sec
    Start 6: SubtitleTest
6/6 Test #6: SubtitleTest .....................   Passed    1.26 sec

100% tests passed out of 6 (Total time: 4.67 sec)
```

### 3.2 Subtitle Format & Unicode Validation
- Tested external `.srt`, `.ass`, and `.vtt` loading without errors.
- Tested Unicode subtitles containing Japanese Kanji/Kana and Korean Hangul (`test_sub_unicode.srt`).
- Tested automatic adjacent subtitle discovery in isolated temporary directories.

### 3.3 Runtime Visual Verification
- Launched application playing `test_video_aac.mp4` with `test_sub.srt`.
- Captured verification screenshot: `docs/milestones/evidence/m5_jonaki_subtitles_osd.png`.

---

## 4. Deliverables & Git Record

| Component | Path | Description |
|---|---|---|
| Subtitle Manager | `src/subtitles/SubtitleManager.h/.cpp` | Subtitle auto-discovery and delay logic |
| Subtitle Unit Tests | `tests/unit/test_subtitles.cpp` | Comprehensive subtitle automated tests |
| Test Subtitles | `tests/test_data/test_sub.*` | Synthetic SRT, ASS, VTT, and Unicode assets |
| Subtitle Context Menu | `src/ui/ContextMenuBuilder.cpp` | Subtitle actions & shortcuts |
| Milestone Report | `docs/milestones/milestone-reports/milestone_M5_completion_report.md` | Verification documentation |

**Milestone M5 is officially complete and verified. Ready to proceed to Milestone M6 (Audio & Video Systems: 10-Band Equalizer, Deinterlacing, Aspect Ratio Scaling).**
