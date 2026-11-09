# SPEC.md — Project Specification: JonakiPlayer

> **Status**: `FINALIZED`
>
> ⚠️ **Planning Lock**: No code may be written until this spec is marked `FINALIZED`. (Planning lock released; spec finalized).

---

## Vision
JonakiPlayer is an autonomous, high-fidelity, cross-platform clean-room reimplementation of Daum/Kakao PotPlayer targeting Windows 10/11 and Linux desktop environments. It delivers the complete desktop user experience of PotPlayer — including its visual appearance, frameless skinnable window structure, rich context menus, keyboard and mouse shortcuts, advanced subtitle styling, multi-stream audio/video processing, hardware decoding, and playlist management — without copying proprietary source code or assets.

---

## Goals
1. **Visual and Behavioral Parity** — Reproduce the recognizable PotPlayer interface (skins, timeline seekbar, volume controls, playlist drawer, frameless border, menu tree).
2. **Clean-Room Implementation** — Implement all models, controllers, skin rendering engines, and configuration serializers independently using evidence collected via REA.
3. **Cross-Platform Consistency** — First-class execution on Windows 10/11 and Linux (Ubuntu/Debian/Fedora on Wayland/X11) with platform-specific optimizations behind clean abstractions.
4. **High-Performance Multimedia Core** — Leverage C++20, Qt 6, libmpv, and FFmpeg for low idle CPU/RAM usage, seamless hardware acceleration (D3D11VA, DXVA2, VAAPI, NVDEC), and low-latency seeking.
5. **Deep Extensibility & Parity Scope** — Support full subtitle engines (ASS/SSA, SRT, styling, sync), audio processing (equalizer, normalization, delay), video processing (aspect ratio, deinterlacing, color correction), and playlist management.

---

## Non-Goals (Out of Scope)
- Copying or distributing proprietary binary files or copyrighted code from Daum/Kakao PotPlayer.
- Bundling adware, promotional banners, or closed-source tracking components found in commercial PotPlayer releases.
- Writing redundant low-level video/audio codecs already maturely handled by FFmpeg and libmpv.
- Mobile platforms (Android/iOS) or web browser player targets.

---

## Constraints
- **Language & Libraries:** C++20 standard, Qt 6 (Widgets/Core/Gui/Network), libmpv, FFmpeg.
- **Build System:** CMake (>= 3.24) and Ninja.
- **Testing:** Automated unit tests, integration tests, and visual regression comparisons.
- **Licensing:** GPL-3.0 / LGPL-3.0 clean-room compliance.

---

## Success Criteria
- [x] Development environment verified, Node.js LTS installed, REA registered as MCP server.
- [x] GSD v1.6.0 installed, workflows/skills/subagents validated.
- [x] Reference PotPlayer assets extracted and reverse engineered (`Default.dsf`, `SimpleMenu.xml`, `English.ini`).
- [ ] Working cross-platform Qt 6 + libmpv player window capable of playing 4K H.264/HEVC/AV1 media.
- [ ] Custom clean-room `SkinRenderer` rendering PotPlayer `.dsf` skins accurately.
- [ ] Context menus and keyboard shortcuts achieving 100% parity with PotPlayer default mappings.
- [ ] Subtitle engine rendering ASS/SSA and SRT with synchronization adjustments.
- [ ] Audio/video processing filters (10-band EQ, deinterlacing, aspect ratios) verified against test videos.
- [ ] Automated test suite passing on both Windows and Linux CI environments.

---

## Technical Requirements

| Requirement | Priority | Notes |
|---|---|---|
| Hardware-accelerated video decoding | Must-have | D3D11VA/DXVA2 on Windows, VAAPI/NVDEC on Linux |
| PotPlayer skin layout parsing (`.dsf` / XML) | Must-have | Slices texture atlas onto Qt widgets |
| Keyboard shortcut dispatcher | Must-have | Matches PotPlayer default keybindings |
| Multi-track audio and subtitle switching | Must-have | Dynamic switching during playback |
| Playlist panel with drag-drop and reordering | Must-have | Dockable drawer panel |
| Equalizer and audio normalization | Should-have | 10-band EQ with presets |
| Video color correction and deinterlacing | Should-have | Real-time contrast, brightness, YADIF |
| Frame capture tool | Should-have | Instant frame grab to PNG |
| Streaming URL playback | Should-have | HLS, DASH, YouTube |
