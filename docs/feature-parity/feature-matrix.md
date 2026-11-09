# PotPlayer Feature Parity Matrix

> **Application:** JonakiPlayer vs. Daum PotPlayer 64-bit  
> **Status:** Baseline Established (M0)  
> **Legend:**  
> - 🟢 **Full Parity:** Fully implemented and verified with tests.  
> - 🟡 **Partial Parity:** Implemented with minor gaps or in progress.  
> - ⚪ **Planned:** On roadmap, not yet implemented.  
> - 🔴 **Deferred / Unsupported:** Out of scope or unsupported on platform.  

---

## 1. Playback Engine

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Video Playback (MP4, MKV, AVI, MOV, TS) | Yes | ⚪ Planned (M2) | Via `libmpv` / FFmpeg |
| Audio Playback (MP3, FLAC, AAC, OGG, OPUS) | Yes | ⚪ Planned (M2) | Via `libmpv` |
| Hardware Acceleration (DXVA2 / D3D11VA) | Yes | ⚪ Planned (M6) | Native Windows decoders |
| Hardware Acceleration (VAAPI / NVDEC) | N/A (Linux) | ⚪ Planned (M6) | Native Linux decoders |
| Frame Stepping (Next / Prev Frame) | Yes (Hotkeys: `D`, `F`) | ⚪ Planned (M2) | Native `mpv` frame-step |
| Playback Speed Control (0.2x – 3.0x) | Yes (Hotkeys: `C`, `X`, `Z`) | ⚪ Planned (M2) | With pitch scaletempo |
| A-B Repeat Loop | Yes (Hotkeys: `[`, `]`, `\`) | ⚪ Planned (M7) | Precise time loops |
| Resume from Last Position | Yes | ⚪ Planned (M4) | Stored in settings |
| Playback History | Yes | ⚪ Planned (M4) | SQLite or INI store |

---

## 2. User Interface & Window Management

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Frameless Custom Skinned Window | Yes | ⚪ Planned (M3) | Native aero snap & resize |
| Skin Engine (`.dsf` parser & renderer) | Yes | ⚪ Planned (M3) | Custom Qt skin engine |
| Seek Slider with Chapter Marks | Yes | ⚪ Planned (M3) | Custom QSlider subclass |
| Time Indicator (Elapsed / Total) | Yes | ⚪ Planned (M3) | Formatted `hh:mm:ss` |
| Volume Slider & Mute Toggle | Yes | ⚪ Planned (M3) | Linear / logarithmic scale |
| Autohide Controls in Fullscreen | Yes | ⚪ Planned (M3) | Mouse movement timer |
| Fullscreen & Windowed Modes | Yes (Hotkey: `Enter`) | ⚪ Planned (M3) | Qt fullscreen state |
| Comprehensive Context Menu | Yes | ⚪ Planned (M3) | Follows `SimpleMenu.xml` |

---

## 3. Playlist Management

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Dockable / Collapsible Playlist Panel | Yes (Hotkey: `F8`) | ⚪ Planned (M4) | Right side drawer |
| Drag-and-Drop Media / Folders | Yes | ⚪ Planned (M4) | Qt drag-drop events |
| Multi-Select, Delete, Clear | Yes | ⚪ Planned (M4) | QTreeWidget / QListView |
| Shuffle & Repeat Modes | Yes | ⚪ Planned (M4) | Sequential / Random queue |
| Save / Load Playlist Files (M3U, PLS) | Yes | ⚪ Planned (M4) | Standard parsers |

---

## 4. Subtitle System

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Embedded Subtitles (SRT, ASS, PGS) | Yes | ⚪ Planned (M5) | Handled via `libmpv` |
| External Subtitle Auto-Load | Yes | ⚪ Planned (M5) | Same-basename matching |
| Subtitle Synchronization (`<`, `>`) | Yes (0.5s step) | ⚪ Planned (M5) | Adjust subtitle delay |
| Subtitle Font / Size / Color Customization | Yes | ⚪ Planned (M5) | libass property overrides |
| Second Subtitle Stream Display | Yes | ⚪ Planned (M5) | Dual subtitle rendering |

---

## 5. Audio Processing

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Audio Stream Selection | Yes | ⚪ Planned (M6) | Track selector |
| Audio Output Device Selection | Yes | ⚪ Planned (M6) | WASAPI / PipeWire |
| 10-Band Graphic Equalizer | Yes | ⚪ Planned (M6) | mpv `af=equalizer` |
| Volume Normalization | Yes | ⚪ Planned (M6) | mpv `dynaudnorm` |
| Audio Delay Adjustment (`Shift + <`, `Shift + >`) | Yes | ⚪ Planned (M6) | Dynamic offset |

---

## 6. Video Processing

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Aspect Ratio Presets | Yes | ⚪ Planned (M6) | 16:9, 4:3, original, fill |
| Color Controls (Brightness, Contrast, etc.) | Yes | ⚪ Planned (M6) | mpv video eq controls |
| Deinterlacing (YADIF, etc.) | Yes | ⚪ Planned (M6) | Hardware / software deint |
| Video Rotation / Flip | Yes | ⚪ Planned (M6) | 90°, 180°, 270°, H/V flip |
| Frame Capture (PNG, JPG) | Yes (Hotkey: `Ctrl+E`) | ⚪ Planned (M7) | mpv `screenshot` command |

---

## 7. Keyboard & Mouse Customization

| Feature | Reference (PotPlayer) | JonakiPlayer Status | Notes |
|---|---|---|---|
| Full PotPlayer Default Keymap | Yes | ⚪ Planned (M4) | Space=Pause, Left/Right=Seek |
| Configurable Hotkey Manager | Yes | ⚪ Planned (M4) | User shortcut editor |
| Mouse Wheel Volume / Seek | Yes | ⚪ Planned (M4) | Configurable wheel actions |
