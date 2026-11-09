# Milestone M1 Reconnaissance & Evidence Protocol

> **Milestone:** M1 — PotPlayer Reconnaissance & Evidence Mapping  
> **Protocol:** GSD DISCUSS → RESEARCH → PLAN → EXECUTE → VERIFY  
> **Status:** APPROVED & LOCKED FOR M1 EXECUTION  
> **Date:** 2026-11-09  

---

## 1. Milestone Purpose & Scope Boundary

**M1 is an evidence-gathering milestone, NOT an implementation milestone.**
Under no circumstances will production Qt widgets or playback feature code be written during M1. M1 is successful only when the observable architecture, window behaviors, layouts, menu trees, settings dialogs, shortcuts, and interaction patterns of Daum PotPlayer 64-bit have been reverse-engineered, captured, empirically cataloged, and verified into formal evidence registers.

---

## 2. Seven Mandatory Reconnaissance Targets

### Target 1: Main Window Geometry, States & Dimensions
- **Objective:** Measure and record exact pixel dimensions, min/max constraints, aspect ratios, and padding.
- **Artifacts & Data:**
  - Default startup window size (width x height, client rect vs window rect).
  - Frameless non-client geometry (title bar height, resize border thickness, grip hit-test areas).
  - Window state behaviors: Normal, Maximized, Fullscreen (`Enter`), and Compact mode (`Ctrl+Enter` / Preset).
  - Floating controls and auto-hide timing in fullscreen mode.
  - Multi-DPI scaling behavior (96 DPI baseline vs high-DPI scaling).

### Target 2: Default Skin Architecture & Directive Semantics
- **Objective:** Fully decode the XML layout engine in `Default.dsf`.
- **Artifacts & Data:**
  - Formal semantic mapping of all XML tags: `<Static>`, `<Button>`, `<Slider>`, `<Label>`, `<Layout>`, `<SubMenu>`.
  - Coordinate system mapping for spritesheets (`Main.png`, `Control.png`, `PlayList.png`, `TouchControl.png`):
    - Normal, Hover, Pressed, Disabled state slice calculations.
  - Timeline slider structure: background rail, download buffer bar, current progress thumb, chapter markers.
  - Volume slider structure: mute button state toggle, logarithmic/linear volume knob.
  - Tooltip bindings (`$CMD_*`, `$ID_*`) resolving to localized catalogs.
  - Identification of elements renderable from skin bitmaps vs those requiring independent native rendering (e.g. video rendering surface, font overlays, context menus).

### Target 3: Complete Menu, Context Menu & Submenu Hierarchy
- **Objective:** Reverse-engineer all context menus, menu bar options, and toolbar popups.
- **Artifacts & Data:**
  - Root popup menu hierarchy (886 nodes extracted from `[MenuString]`).
  - Menu command ID taxonomy: mapping `CMD_*` and `ID_*` commands.
  - Dynamic menu behaviors (Recent files, Open URL history, Audio stream checkmarks, Subtitle selection ticks).
  - Menu keyboard accelerator bindings.

### Target 4: Preferences / Settings Hierarchy & Dialog Topology
- **Objective:** Catalog the entire settings tree from `PotPlayer64.dll` and registry keys.
- **Artifacts & Data:**
  - Complete inventory of all 184 `RT_DIALOG` resource IDs in `PotPlayer64.dll`.
  - Tree structure of the Preferences dialog (`F5`):
    - General (Startup, Skin, Logo, Mouse, Keyboard, OSD, Association, Network)
    - Playback (Play, Time jump, Repeat, Resume, Speed)
    - Video (Renderer, Color, Aspect ratio, Deinterlacing, Pixel Shader, 3D/VR)
    - Audio (Output device, Channels, Resample, Normalizer, Equalizer)
    - Subtitles (Language, Font, Display position, Processing, Word search)
    - Filter Control (Built-in splitters/decoders, DirectShow, Media Foundation, FFmpeg)
    - Extension / Web (Captions, Live broadcasting, Open API)
  - Registry storage schema: `HKCU\Software\Daum\PotPlayerMini64\Settings` keys, types, defaults, and value ranges.

### Target 5: Keyboard Shortcuts & Mouse Interaction Matrix
- **Objective:** Exhaustive catalog of all default input behaviors.
- **Artifacts & Data:**
  - Playback navigation: Space (Play/Pause), Left/Right (Seek 5s), Ctrl+Left/Right (Seek 30s), Shift+Left/Right (Seek 1m), Ctrl+Alt+Left/Right (Seek 5m).
  - Speed adjustments: Z (Reset 1.0x), X (Slow 0.1x), C (Fast 0.1x).
  - Audio adjustments: Up/Down (Volume +/-), M (Mute), Shift+< / Shift+> (Audio sync +/-0.05s).
  - Subtitle controls: < / > (Subtitle sync +/-0.5s), Alt+Up/Down (Position), Alt+PageUp/PageDown (Size).
  - Frame stepping: D (Previous frame), F (Next frame).
  - Mouse interaction mappings: Single click (Play/Pause), Double click (Fullscreen), Wheel (Volume/Seek), Drag & Drop (Open/Enqueue).

### Target 6: Playlist, Playback Workflows & History Architecture
- **Objective:** Document playlist drawer mechanics and media database persistence.
- **Artifacts & Data:**
  - Playlist drawer dock / undock behavior (`F8`).
  - `.dpl` playlist file format specification (`DAUMPLAYLIST`, `playname`, `playtime`, `file`, `duration2`, `start`).
  - History tracking and resume position logic (`MediaInfo.sdb` SQLite/binary structure).
  - Queue management, sorting (by name, date, path, random), repeat modes.

### Target 7: Audio/Video Processing Pipelines & Hardware Acceleration
- **Objective:** Document video/audio processing chain and renderer selections.
- **Artifacts & Data:**
  - Video renderers supported (Built-in Direct3D 11 Video Renderer, Direct3D 9, OpenGL, MadVR).
  - Hardware decoders supported (D3D11VA, DXVA2 Copy-Back, Native, Intel QuickSync, NVIDIA NVDEC).
  - Aspect ratio calculation modes (Keep aspect ratio, Fit to window, Stretch, 16:9, 4:3, 2.35:1).
  - Audio equalizer frequency bands (10-band: 31Hz, 62Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz, 16kHz).

---

## 3. Evidence Classification Standard

Every finding in the M1 research report and registers must carry an explicit evidence tag:
- **[DIRECTLY-OBSERVED]**: Empirically confirmed via binary inspection, memory read, file capture, screenshot, or runtime test.
- **[INFERRED]**: Deduced logically from XML layouts, string tables, or standard Windows/Qt multimedia conventions.
- **[UNRESOLVED]**: Black-box behavior requiring further investigation or platform-specific isolation.

---

## 4. Deliverables for M1 Acceptance

1. `docs/reverse-engineering/application-analysis.md` — Binary architecture, DLL dependencies, process lifecycle.
2. `docs/reverse-engineering/ui-analysis.md` — Detailed window dimensions, states, skin layout engine specification.
3. `docs/reverse-engineering/configuration-analysis.md` — Complete preferences tree, dialog mapping, registry schema.
4. `docs/reverse-engineering/playback-analysis.md` — Playback workflows, playlist `.dpl` format, history persistence.
5. `docs/feature-parity/feature-matrix.md` — Fully expanded matrix with priority (Must/Should/Nice) and confidence level (High/Med/Low).
6. `docs/reverse-engineering/evidence-register.md` — Updated with investigation records REV-001 through REV-015+.
7. `docs/milestones/milestone-reports/milestone_M1_research_report.md` — Comprehensive M1 milestone completion report.
8. Implementation plan for Milestone M2 (Playback & Architecture Foundation).
