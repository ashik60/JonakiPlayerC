# AGENTS.md — Permanent Engineering Instructions

This document governs all AI agents and engineers working on **JonakiPlayer**.
Read this file at the start of every session before performing any work.

---

## 1. Product Vision & Non-Negotiables

- **Project:** JonakiPlayer
- **Purpose:** An autonomous, faithful cross-platform clean-room reimplementation of **Daum/Kakao PotPlayer** for Windows 10/11 and Linux.
- **Fidelity over Redesign:** JonakiPlayer must reproduce the PotPlayer look, feel, layout, menu structures, shortcuts, and playback capabilities. Do not "modernize" or simplify the interface into a generic flat media player.
- **Clean-Room Engineering:** Evidence-driven reverse engineering via REA and direct observation. No proprietary binary code or copyrighted code assets from PotPlayer may be copied into the codebase. All logic, parsers, and UI implementations must be clean-room written.
- **Truth over Assumption:** Never claim feature parity or test completion without concrete empirical evidence.

---

## 2. Mandatory Frameworks

### 2.1 REA (Reverse Engineer Anything)
- Framework: [https://github.com/morluto/rea](https://github.com/morluto/rea)
- Used for: Investigating the reference PotPlayer installation (binaries, resources, skins, menus, string catalogs, behaviors).
- Evidence Register: All findings must be recorded in `docs/reverse-engineering/evidence-register.md` with an Investigation ID before or alongside implementation.

### 2.2 GSD (Get Shit Done for Antigravity)
- Framework: [https://github.com/toonight/get-shit-done-for-antigravity](https://github.com/toonight/get-shit-done-for-antigravity)
- Core protocol: **SPEC → PLAN → EXECUTE → VERIFY → COMMIT**
- Planning Lock: Do not begin substantial implementation until `.gsd/SPEC.md` has `Status: FINALIZED`.
- Atomic tasks: One task = one verified commit (`feat:`, `fix:`, `refactor:`, `test:`, `docs:`, `build:`, `perf:`).
- Independent verification: Verification must be empirically proven via test outputs, logs, or screenshots before marking a task done.

---

## 3. Technology Stack & Coding Standards

- **Language:** C++20 standard (`-std=c++20` or `/std:c++20`).
- **GUI Toolkit:** Qt 6 (Qt 6 Core, Gui, Widgets, Network, Svg).
- **Playback Core:** libmpv (`mpv_create`, `mpv_render_context`, hardware decoders).
- **Multimedia Processing & Extraction:** FFmpeg (`avformat`, `avcodec`, `avutil`, `swscale`, `swresample`).
- **Build System:** CMake (>= 3.24) with Ninja generator.
- **Cross-Platform Abstraction:** Clean interfaces for platform differences (e.g., Windows Direct3D/WASAPI vs. Linux Wayland/PipeWire) in `src/platform/`.
- **Formatting & Style:** Clang-format (Google/Qt hybrid), clear ownership (`std::unique_ptr`, `std::shared_ptr`), RAII everywhere, no memory leaks or raw owning pointers.

---

## 4. Mandatory Session Startup Protocol

At the start of **EVERY** new session, agents must:
1. Read `AGENTS.md` (this file).
2. Read `PROJECT_CONTEXT.md`.
3. Read `ROADMAP.md` and `.gsd/ROADMAP.md`.
4. Read `.gsd/STATE.md` and `.gsd/SPEC.md`.
5. Read `docs/memory/project-memory.md` and `docs/memory/next-actions.md`.
6. Inspect `git status` and recent git history (`git log -5 --oneline`).
7. Resume from the active milestone and wave without repeating already verified work.

---

## 5. Shell & Git Discipline

- One shell command per execution; do not chain with `&&` or `||` on Windows PowerShell.
- Verify every command output before proceeding.
- Commit atomically: verify tests first, then commit with clean conventional message.
- Update `PROJECT_CONTEXT.md`, `ROADMAP.md`, `.gsd/STATE.md`, and `docs/memory/` after every major milestone or wave.
