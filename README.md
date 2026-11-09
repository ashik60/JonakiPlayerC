# JonakiPlayer

> Autonomous Cross-Platform PotPlayer Reimplementation for Windows and Linux.

---

## Overview

**JonakiPlayer** is an open-source, clean-room desktop multimedia player architected to faithfully replicate the visual aesthetics, interaction model, workflow mechanics, and feature breadth of **Daum/Kakao PotPlayer** across modern operating systems (Windows 10/11 and Linux).

The project is developed using an evidence-driven engineering approach powered by:
- **REA** ([Reverse Engineer Anything](https://github.com/morluto/rea)) for reference behavior inspection and architectural validation.
- **GSD** ([Get Shit Done for Antigravity](https://github.com/toonight/get-shit-done-for-antigravity)) for rigorous specification, phase planning, atomic task execution, and verification.

---

## Core Principles

- **Visual & Behavioral Parity:** Reconstructs the recognizable PotPlayer interface (skins, frameless windowing, timeline, control bar, context menus, playlist drawers) without simplifying into generic flat player widgets.
- **Clean-Room Implementation:** No proprietary PotPlayer source code or binary assets are included. All layouts, models, engines, and parsers are independently created based on observable evidence.
- **High Performance:** Native C++20 core integrated with `libmpv` and `FFmpeg` for hardware-accelerated playback (DXVA2/D3D11VA/VAAPI/NVDEC), low memory footprint, and low latency.
- **Cross-Platform Parity:** First-class support for both Windows 10/11 (WASAPI, Direct3D) and Linux (PipeWire/PulseAudio, Wayland/X11).

---

## Architecture Stack

| Component | Technology |
|---|---|
| Language | C++20 standard |
| GUI Framework | Qt 6 (Widgets, Gui, Core, Network) |
| Playback Engine | libmpv (`mpv_create`, `mpv_render_context`) |
| Multimedia & Demuxing | FFmpeg (libavformat, libavcodec, libavutil, libswscale) |
| Build System | CMake (>= 3.24), Ninja |
| Subtitle Engine | libass / embedded SSA/ASS/SRT styling |
| Audio/Video Pipeline | Native hardware decoders, WASAPI, PipeWire, OpenGL/Vulkan |

---

## Project Structure

```text
JonakiPlayer/
├── src/
│   ├── application/     # App initialization, single-instance, lifecycle
│   ├── core/            # Common types, event bus, utilities, logging
│   ├── playback/        # Media playback engine abstraction over libmpv
│   ├── video/           # Video render widgets, filters, shaders
│   ├── audio/           # Audio configuration, equalizers, volume control
│   ├── subtitle/        # Subtitle synchronization, styling, parsers
│   ├── playlist/        # Playlist management, queuing, sorting, persistence
│   ├── filters/         # Audio/video processing filters
│   ├── capture/         # Screen/frame capture and recording
│   ├── streaming/       # Network stream handling (HLS, DASH, RTSP, YouTube)
│   ├── settings/        # Configuration management, INI serializer
│   ├── input/           # Hotkeys, mouse actions, shortcut mapping
│   ├── ui/              # Skin rendering engine, main window, dialogs, menus
│   └── platform/        # Native platform adapters (Windows / Linux)
├── tests/               # Unit, integration, regression, and UI tests
├── resources/           # Clean-room icons, themes, and translations
├── docs/                # Comprehensive architecture and reverse-engineering docs
└── CMakeLists.txt       # Root build configuration
```

---

## Building JonakiPlayer

### Windows (MSYS2 / MinGW-w64 or MSVC)

```powershell
# Configure CMake
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

### Linux (Debian / Ubuntu / Fedora / Arch)

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## Current Status

- **Milestone:** M0 (Project Initialization & Environment Setup) — In Progress.
- **REA Setup:** Verified and configured with Antigravity MCP integration.
- **GSD System:** Validated with all 27 workflows, 12 skills, 5 subagents, and canonical rules active.
- **Reference Installation:** Daum PotPlayer 64-bit located, skin archives and language specifications cataloged.

---

## License

GPL-3.0 / LGPL-3.0 Compatible. See `LICENSE` for details.
