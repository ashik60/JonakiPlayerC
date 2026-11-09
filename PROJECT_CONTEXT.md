# PROJECT_CONTEXT.md — Primary Project Context

> **Last Updated:** 2026-11-09  
> **Status:** Active Development — Milestone M0 (Project Initialization & Reconnaissance)

---

## 1. What is JonakiPlayer?

JonakiPlayer is an autonomous, clean-room, cross-platform reimplementation of **Daum/Kakao PotPlayer** targeting Windows 10/11 and Linux (X11 & Wayland). Its mission is to deliver the full PotPlayer desktop user experience — including its skin engine, frameless title bar, timeline controls, multi-track audio/subtitle switching, playlist sidebar, detailed preference tree, and keyboard/mouse configuration — powered by modern C++20, Qt 6, CMake, libmpv, and FFmpeg.

---

## 2. Current Development Stage & Milestones

- **Active Milestone:** **M0 — Project Initialization**
- **Next Milestone:** **M1 — PotPlayer Reconnaissance & Parity Matrix**
- **Overall Progress:** Environment verified, Node.js LTS installed, REA framework registered with Antigravity MCP, GSD v1.6.0 workflows, skills and subagents validated, initial project repository structure created, reference PotPlayer assets extracted and analyzed.

### Milestone Schedule Summary

| ID | Milestone Name | Status | Deliverables |
|---|---|---|---|
| **M0** | Project Initialization & Environment | **In Progress** | REA & GSD verified, repo initialized, memory docs set |
| **M1** | PotPlayer Reconnaissance & Evidence | Pending | Complete string, skin, and command catalog |
| **M2** | Architecture & Playback Foundation | Pending | Qt6 + libmpv core playback window and event loop |
| **M3** | Main Interface Reconstruction | Pending | PotPlayer skin renderer, frameless frame, timeline, controls |
| **M4** | Playlist & Interaction System | Pending | Playlist drawer, drag-drop, hotkeys, mouse gestures |
| **M5** | Subtitle Engine | Pending | SRT, ASS/SSA rendering, styling, delay adjustment |
| **M6** | Audio & Video Processing | Pending | Equalizer, deinterlacing, color correction, aspect ratio |
| **M7** | Advanced Features | Pending | Streaming URLs, capture, hardware decoding selection |
| **M8** | Visual & Functional Parity Audit | Pending | Screenshot comparison, parity matrix sign-off |
| **M9** | Stability & Optimization | Pending | Benchmarks, memory leak tests, stress playback |
| **M10** | Packaging & Release Engineering | Pending | Windows installer / portable zip, Linux AppImage/packages |

---

## 3. Technology Stack & Architecture

- **Core Language:** C++20 standard (`std::span`, `std::format`, concepts, ranges).
- **UI Framework:** Qt 6 (Widgets, Gui, Network, Svg).
- **Playback Backend:** `libmpv` (C API, render context, hardware acceleration).
- **Format Demuxing & Audio/Video Tooling:** FFmpeg (`avformat`, `avcodec`, `avutil`).
- **Reverse Engineering:** REA (`rea@6.8.0`, Antigravity MCP integration).
- **Execution & Orchestration:** GSD (`toonight/get-shit-done-for-antigravity`).

---

## 4. Key Technical Decisions (TDRs)

1. **Clean-Room Engineering:** All UI code, state management, and configuration engines are independently implemented. No copyrighted binaries or code from Daum/Kakao PotPlayer are incorporated into the repository.
2. **Skin Engine Strategy:** PotPlayer uses `.dsf` files which are standard zip archives containing XML definitions (`AudioSkin.xml`, `BottomVideo.txt`, `ChildSkin.xml`) and spritesheets (`Main.png`, `Control.png`). We will implement an open-source `SkinRenderer` in Qt that parses these XML layouts and paints buttons and slices accordingly.
3. **Decoupled Architecture:** The playback backend (`libmpv`) is isolated behind a clean C++ abstract interface (`IPlaybackEngine`), ensuring UI and settings are not hardcoded to mpv properties.
4. **Toolchain on Windows:** Utilizing MSYS2 MinGW-w64 toolchain (GCC 14 / Clang) providing native Qt6, libmpv, FFmpeg, and CMake packages, complemented by MSVC compatibility.

---

## 5. Active Work & Next Actions

1. Complete M0 initialization (finalize MSYS2 environment, build dependencies).
2. Commit M0 milestone artifacts to Git.
3. Advance to M1 (Extract and catalog full command IDs from PotPlayer's `English.ini` and `SimpleMenu.xml`).
4. Generate the formal `docs/feature-parity/feature-matrix.md`.

---

## 6. Critical Constraints & Guidelines

- **Planning Lock:** No substantial production code before `.gsd/SPEC.md` is `FINALIZED`.
- **Atomic Commits:** Every task must be verified with output or tests before committing.
- **Cross-Platform:** No Windows-specific API calls without abstraction (`src/platform/`).
