# STACK.md — Technology Stack: JonakiPlayer

> **Last Updated:** 2026-11-09

---

## 1. Programming Languages & Compilers

- **Language:** C++20 standard (`-std=c++20` / `/std:c++20`)
- **Compilers:**
  - GCC 16.x (MinGW-w64 UCRT64 on Windows, native GCC on Linux)
  - Clang / LLVM (>= 17.0)
  - MSVC 2022 (v143)
- **Scripting & Automation:** Python 3.14 (asset processing, test automation), Node.js LTS v26.13 (REA framework).

---

## 2. Core Libraries & Frameworks

| Library | Version | Purpose | License |
|---|---|---|---|
| **Qt 6 (Base, Tools)** | 6.12+ | Core GUI, Event loop, Widgets, Svg, Network | LGPL-3.0 / GPL-3.0 |
| **libmpv** | 0.42+ | Multimedia playback engine, hardware decoding, video rendering | LGPL-2.1+ / GPL-2.0+ |
| **FFmpeg** | 7.x / 8.x | Media container parsing, stream info, audio/video transcoding | LGPL-2.1+ / GPL-2.0+ |
| **libass** | 0.17+ | Advanced SubStation Alpha (ASS/SSA) subtitle rendering | ISC |
| **REA (rea-agents)** | 6.8.0 | Reverse engineering, asset inspection, protocol validation | MIT / Open Source |

---

## 3. Build & Development Tools

- **Build Generator:** CMake >= 3.24
- **Build System:** Ninja 1.13+
- **Version Control:** Git 2.56+
- **Orchestration:** GSD v1.6.0 (`toonight/get-shit-done-for-antigravity`)
- **Package Manager (Windows):** MSYS2 UCRT64 / winget
- **Testing:** CTest, Qt Test (`QTest`), GoogleTest (unit testing)
