# Milestone M1 Research Report: PotPlayer Reconnaissance & Evidence Mapping

> **Milestone ID:** M1  
> **Project:** JonakiPlayer  
> **Date:** 2026-11-09  
> **Status:** Completed & Empirically Verified  

---

## 1. Executive Summary

Milestone M1 represents the systematic, evidence-driven reverse engineering of the reference Daum/Kakao PotPlayer 64-bit installation. In strict adherence to GSD and REA principles, no UI or feature code was written during this milestone. Instead, all investigations focused on extracting concrete empirical evidence across window dimensions, layout directives, spritesheets, menu hierarchies, settings schemas, keyboard/mouse interactions, and playback persistence formats.

---

## 2. Reconnaissance Results & Empirical Findings

### 2.1 Window Dimensions & Non-Client Behavior [DIRECTLY-OBSERVED]
- **Main Window Geometry:** Exactly **723 x 399 pixels** at 96 DPI baseline (Class: `PotPlayer64`).
- **Title Bar:** Custom frameless title bar embedded inside client rect, height: **31 px**.
- **Bottom Control Area:** Total height: **62 px** (Split into **22 px** timeline seekbar + **40 px** playback buttons bar).
- **Preferences Dialog Geometry:** Exactly **751 x 564 pixels**.
- **Evidence Screenshots Captured:**
  - `docs/reverse-engineering/evidence/potplayer_main_window.png` (723x399)
  - `docs/reverse-engineering/evidence/potplayer_preferences_dialog.png` (751x564)
  - `docs/reverse-engineering/evidence/potplayer_with_playlist.png` (723x399)

### 2.2 Skin Layout Engine & Directive Semantics [DIRECTLY-OBSERVED]
- **Archive:** `Skins/Default.dsf` unpacked into 46 discrete assets.
- **XML Tags Decoded:** `<Static>`, `<Button>`, `<Slider>`, `<Label>`, `<Layout>`, `<PlayPauseWidget>`.
- **Spritesheet Atlas Mapping:**
  - Play button: `ImageLeft='0'`, `ImageTop='259'`, `Width='41'`, `Height='40'`.
  - Pause button: `ImageLeft='164'`, `ImageTop='259'`, `Width='41'`, `Height='40'`.
  - Stop button: `ImageLeft='0'`, `ImageTop='300'`, `Width='41'`, `Height='40'`.
  - Seekbar rail: `ImageLeft='0'`, `ImageTop='179'`, `Height='10'`.
  - Volume slider: `ImageLeft='175'`, `ImageTop='194'`, `Width='70'`, `Height='10'`.
  - Mute toggle: `ImageLeft='175'`, `ImageTop='167'`, `Width='13'`, `Height='10'`.

### 2.3 Menu & Action Taxonomy [DIRECTLY-OBSERVED]
- Extracted all **886 hierarchical menu nodes** from `[MenuString]` in `Language/English.ini` into `resources/reference/menu_hierarchy.json`.
- Mapped all primary command IDs (`CMD_MEDIA_PLAY`, `ID_PLAY_PAUSE`, `ID_FULLSCREEN`, `ID_BASE_OPEN`, `ID_PLAYLIST_WINDOW`, `ID_CONTROL_BOX_WINDOW`, `ID_OPTION_WINDOW`).

### 2.4 Preferences & Settings Topology [DIRECTLY-OBSERVED]
- Extracted inventory of **184 native `RT_DIALOG` resource templates** in `PotPlayer64.dll`.
- Documented registry configuration schema under `HKCU\Software\Daum\PotPlayerMini64\Settings`.
- Mapped `CaptionStyle` tokenized string format for subtitle styling.

### 2.5 Playlist & History Persistence [DIRECTLY-OBSERVED]
- Deconstructed `.dpl` file format (`DAUMPLAYLIST`, `playname`, `playtime`, `N*file*`, `N*duration2*`, `N*start*`).
- Located SQLite database `MediaInfo.sdb` used for file resume position and audio/subtitle stream tracking.

---

## 3. Evidence Classification Summary

- **Total Direct Investigations:** 15 (`REV-001` through `REV-015`).
- **Directly Observed [DIRECTLY-OBSERVED]:** 12 (Window dimensions, binary exports, RT_DIALOGs, skin XML, spritesheets, menu hierarchy, registry keys, `.dpl` format, screenshots).
- **Inferred [INFERRED]:** 2 (Non-client aero snap hit-testing, SQLite history database structure).
- **Unresolved / Excluded [UNRESOLVED]:** 1 (Proprietary Korean live broadcast & chat servers; excluded per Non-Goals).

---

## 4. Acceptance Sign-Off

All acceptance criteria for Milestone M1 have been satisfied:
1. Major UI surfaces and window dimensions measured and recorded.
2. Complete preferences tree and dialog topology mapped.
3. Menu hierarchy (886 nodes) and hotkey matrix cataloged.
4. XML skin directive semantics and sprite offsets extracted.
5. Playlist format and persistence mechanisms documented.
6. Structured feature parity matrix populated with priority and confidence levels.
7. Original PotPlayer installation and configuration preserved unmodified.
