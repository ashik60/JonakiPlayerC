# Project Memory — Long-Term Architectural Knowledge

> **Last Updated:** 2026-11-09  
> **Repository:** JonakiPlayer  
> **Status:** Milestone M0 (Project Initialization)

---

## 1. Core Vision & Requirements

- JonakiPlayer is a high-fidelity, autonomous, cross-platform reimplementation of **Daum/Kakao PotPlayer** for Windows 10/11 and Linux.
- The project reproduces PotPlayer's complete desktop user experience (visual aesthetics, frameless windowing, skin mechanics, complex menus, hotkeys, playlist drawer, multi-stream subtitle/audio switching, hardware decoding).
- Clean-room engineering: No proprietary code or assets copied from PotPlayer. All logic and UI implementations are original and evidence-driven.

---

## 2. Frameworks & Environment

- **REA Framework:**
  - Package: `rea-agents` (version 6.8.0).
  - CLI: `rea.cmd` installed globally via npm.
  - MCP Server: Registered in `C:\Users\User\.gemini\config\mcp_config.json`.
  - Tools: 229 MCP reverse-engineering tools and inspection capabilities.
- **GSD (Get Shit Done) Framework:**
  - Version: 1.6.0 (`toonight/get-shit-done-for-antigravity`).
  - Workflows: 27 workflows in `.agent/workflows/`.
  - Subagents: 5 native subagents (`gsd-planner`, `gsd-executor`, `gsd-verifier`, `gsd-researcher`, `gsd-debugger`).
  - Validation: 100% pass via `scripts/validate-all.ps1`.
- **Runtime Environment:**
  - Host OS: Windows 10 Pro 64-bit (Build 19045).
  - CPU: Intel Core i5-4200U @ 1.60GHz (2 cores, 4 threads).
  - Node.js: v26.13.0 LTS.
  - npm: 11.20.0.
  - Python: 3.14.8.
  - FFmpeg: `C:\FFmpeg\bin` (ffmpeg, ffplay, ffprobe available).
  - MSYS2: Installing at `C:\msys64` for MinGW-w64 GCC / Qt6 / libmpv toolchain.

---

## 3. Reverse-Engineering Findings (PotPlayer Reference)

- **Installation Path:** `C:\Program Files\DAUM\PotPlayer`
- **Main Executables & DLLs:**
  - `PotPlayerMini64.exe` (main frontend launcher).
  - `PotPlayer64.dll` (main application core & UI).
  - `ffcodec64.dll` (FFmpeg-based codec library).
  - `MediaDB64.dll` (media metadata & playlist database).
  - `ATextOut64.dll` (advanced subtitle text rendering engine).
- **Skin Architecture:**
  - Format: `.dsf` files (located in `Skins/Default.dsf`, `WindowFrame.dsf`).
  - Deconstruction: Standard ZIP archive with XML layout files (`AudioSkin.xml`, `BottomVideo.txt`, `ChildSkin.xml`) and spritesheets (`Main.png`, `Control.png`, `PlayList.png`).
  - Encoding: Text files are UTF-8 with BOM (`utf-8-sig`).
  - Layout Format: XML tags `<Static>`, `<Button>`, `<Slider>`, `<Label>`, `<Layout>` with explicit sprite offsets (`ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`), alignment, tooltips, and command IDs (`CMD_MEDIA_PLAY`, `CMD_MEDIA_PAUSE`, `ID_PROCESS_PREV`).
- **Command IDs & String Catalog:**
  - String dictionary in `Language/English.ini` (325 KB) mapping menu IDs, dialog IDs, settings keys, and action names.
  - Menu definition in `Menus/SimpleMenu.xml` detailing command hierarchies.

---

## 4. Key Architectural Decisions (ADRs)

1. **ADR-001: Clean-Room Skin Engine**
   - Implement an open-source Qt 6 `SkinRenderer` that parses the XML structure and renders sliced spritesheets onto Qt widgets. This gives 100% visual fidelity to PotPlayer without copying closed-source binaries.
2. **ADR-002: Modular Playback Engine (`libmpv`)**
   - Decouple playback logic from the UI via `IPlaybackEngine`. `libmpv` provides robust cross-platform decoding (hardware acceleration via D3D11VA/DXVA2 on Windows, VAAPI/NVDEC on Linux) and high-quality video rendering.
3. **ADR-003: Platform Abstraction Layer**
   - Encapsulate native window management (e.g. DWM blur, borderless snapping, WASAPI audio on Windows vs. X11/Wayland and PipeWire on Linux) in `src/platform/`.

---

## 5. Working Solutions & Verification Results

- Verified Node.js LTS silent install via `winget`.
- Fixed npm execution policy in PowerShell by targeting `.cmd` wrappers and updating User PATH.
- Configured REA MCP registration cleanly with Antigravity.
- Verified GSD v1.6.0 with `scripts/validate-all.ps1`.
- Verified `.dsf` archive structure using Python `zipfile`.
