# Milestone M4 Completion Report: Playlist & Interaction System

> **Date:** 2026-11-09  
> **Status:** COMPLETED & EMPIRICALLY VERIFIED  
> **Target:** Milestone M4 — Playlist, Interaction & Persistence Engine  
> **Frameworks:** GSD v1.6.0 & REA v6.8.0  

---

## 1. Executive Summary

Milestone M4 delivers complete playlist management, non-destructive Daum `.dpl` file parsing and serialization, per-file playback resume position tracking via SQLite (`MediaInfo.sdb`), dockable right-side drawer UI (`F8`), recursive directory queueing, and tight synchronization with the `IPlaybackEngine` subsystem.

All implementations strictly adhere to clean-room engineering principles and maintain 100% test passing across the entire JonakiPlayer test suite.

---

## 2. Implementation Overview

### 2.1 Daum `.dpl` File Parser & Serializer (`DplPlaylistParser`)
- **Location:** `src/playlist/DplPlaylistParser.h`, `src/playlist/DplPlaylistParser.cpp`
- Parses `.dpl` header lines: `DAUMPLAYLIST`, `playname=...`, `playtime=...`, `topindex=...`, `saveplaypos=...`, `basefolder=...`.
- Parses index-keyed property entries: `<id>*file*...`, `<id>*title*...`, `<id>*duration2*...`, `<id>*start*...`.
- Non-destructive round-trip preservation: Unknown or future properties (`<id>*played*1`, `<id>*sub*...`) are preserved in `extraFields` and re-emitted on serialization.
- Verified against official PotPlayer `.dpl` samples (`PotPlayerMini64.dpl`).

### 2.2 Resume Position Store (`ResumeStore`)
- **Location:** `src/playlist/ResumeStore.h`, `src/playlist/ResumeStore.cpp`
- Implements SQLite persistence database at `%APPDATA%/JonakiPlayer/MediaInfo.sdb` matching reference PotPlayer schema (`MediaInfo` table).
- Percent-encodes file path primary keys (e.g., `F%3a%5cFolder%5cFile.mkv`) matching observed PotPlayer behavior.
- Thread-safe query and insertion methods for storing `time` (ms), `dur` (ms), and clearing records upon full playback completion.

### 2.3 Playlist Model & Controller
- **Location:** `src/playlist/PlaylistModel.h/.cpp`, `src/playlist/PlaylistController.h/.cpp`
- `PlaylistModel`: Subclasses `QAbstractListModel` with drag/drop and item reordering support (`moveItem`, `removeItem`, `clear`).
- `PlaylistController`: Orchestrates sequential, repeat-one, repeat-all, and shuffle playback orders; auto-restores playback resume offsets upon media load; scans folders recursively for supported media types (`mp4`, `mkv`, `avi`, `webm`, `flv`, `ts`, `mp3`, `flac`, `wav`, `aac`, `ogg`, `opus`).

### 2.4 Dockable Playlist Drawer UI (`PlaylistDrawer`)
- **Location:** `src/ui/PlaylistDrawer.h`, `src/ui/PlaylistDrawer.cpp`
- 280px sliding drawer dockable to the right side of `MainWindow`.
- Controlled via `F8` shortcut, bottom control bar button (☰), and title bar close button (✕).
- Dynamically resizes the main window from **723 x 399** to **1003 x 399** upon expanding, preserving the exact 723px video viewport without shrinking the video canvas.

---

## 3. Empirical Test & Verification Results

### 3.1 Automated Test Suite (`ctest`)
```
Test project F:/Projects/JonakiPlayer/build
    Start 1: SettingsTest
1/5 Test #1: SettingsTest .....................   Passed    0.05 sec
    Start 2: PlaybackEngineMockTest
2/5 Test #2: PlaybackEngineMockTest ...........   Passed    0.04 sec
    Start 3: MpvPlaybackIntegrationTest
3/5 Test #3: MpvPlaybackIntegrationTest .......   Passed    2.73 sec
    Start 4: SkinParserTest
4/5 Test #4: SkinParserTest ...................   Passed    0.05 sec
    Start 5: PlaylistTest
5/5 Test #5: PlaylistTest .....................   Passed    0.27 sec

100% tests passed out of 5 (Total test time: 3.15 sec)
```

### 3.2 Runtime Verification
- Launched `JonakiPlayer.exe`, loaded video, opened playlist via `F8`.
- Measured runtime expanded geometry: **1003 x 399 pixels** (723px main viewport + 280px drawer).
- Captured verification screenshot: `docs/milestones/evidence/m4_jonaki_with_playlist.png`.
- Compared with reference screenshot: `docs/reverse-engineering/evidence/potplayer_with_playlist.png`.

---

## 4. Deliverables & Git Record

| File / Component | Purpose |
|---|---|
| `src/playlist/DplPlaylistParser.h/.cpp` | Daum `.dpl` playlist format parser and serializer |
| `src/playlist/ResumeStore.h/.cpp` | Clean-room SQLite resume position manager |
| `src/playlist/PlaylistModel.h/.cpp` | Qt AbstractListModel for playlist queue |
| `src/playlist/PlaylistController.h/.cpp` | Navigation, repeat/shuffle logic, resume sync |
| `src/ui/PlaylistDrawer.h/.cpp` | 280px right-side collapsible playlist drawer |
| `tests/unit/test_playlist.cpp` | Unit tests for parser, round-trip, resume store |
| `docs/milestones/evidence/m4_jonaki_with_playlist.png` | Empirical verification screenshot |

**Milestone M4 is officially complete and verified. Ready to proceed to Milestone M5 (Subtitle Engine & OSD System).**
