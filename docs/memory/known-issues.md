# Known Issues & Technical Limitations

This document records active technical limitations, known defects, and pending fixes across the **JonakiPlayer** project.

---

## Active Issues

### ISS-001: MSYS2 Default Pacman Mirror DNS Resolution in Asia
- **Status:** Resolved
- **Impact:** `pacman -Sy` failed on default dynamic mirror (`mirror.msys2.org`) redirecting to `mirrors.ustc.edu.cn`.
- **Resolution:** Updated `C:\msys64\etc\pacman.d\mirrorlist.mingw` and `mirrorlist.msys` to prioritize direct mirrors (`repo.msys2.org`, `ftp2.osuosl.org`, `mirror.yandex.ru`).

### ISS-002: PowerShell Restricted Script Execution Policy for npm.ps1
- **Status:** Resolved
- **Impact:** Running `npm` directly in PowerShell 5.1 produced `PSSecurityException` due to script execution policy.
- **Resolution:** Standardized on invoking `.cmd` wrappers (`npm.cmd`, `npx.cmd`, `rea.cmd`) and added Node/npm paths to the User environment.

### ISS-003: UTF-8 BOM in PotPlayer XML/TXT Skin Specifications
- **Status:** Mitigated
- **Impact:** Text files inside `Default.dsf` (e.g. `BottomVideo.txt`, `Common.txt`) are encoded as UTF-8 with Byte Order Mark (`\xef\xbb\xbf`).
- **Resolution:** Python and C++ parsers must specify `utf-8-sig` (or strip BOM before XML parsing) to avoid syntax errors.

### ISS-004: AV1 Hardware Decoding Hardware Limitation on Haswell Intel HD Graphics 4400
- **Status:** Documented / Working as Intended
- **Impact:** Requesting hardware decoding (`auto-safe`, `d3d11va`) on AV1 streams falls back to software `libdav1d`.
- **Resolution:** Intel Haswell microarchitecture does not support fixed-function AV1 hardware decoding. High-performance software decoding via `libdav1d` verified and documented.

### ISS-005: Frame Capture Thread-Safety & Asynchronous Operation Tracking
- **Status:** Resolved
- **Impact:** Raw detached thread accessing `IPlaybackEngine*` presented use-after-free risk during engine destruction.
- **Resolution:** Implemented `mpv_command_async`, active capture counter with CV synchronization, `cancelPendingCaptures()`, and RAII `TempFileGuard`. Regression tests in `FrameCaptureTest`.

### ISS-006: A-B Repeat State Reset in Native mpv Core
- **Status:** Resolved
- **Impact:** Stale loop bounds persisted across media load because native `ab-loop-a` and `ab-loop-b` were not cleared.
- **Resolution:** Native properties cleared to `"no"` on `loadMedia()`, `stop()`, and `shutdown()`. Verified real playback wrapping in `ABRepeatTest`.

---

## Platform-Specific Constraints

- **Windows:** Frameless window resizing requires custom `WM_NCHITTEST` handling for native aero-snap and border resizing.
- **Linux:** Requires Wayland fractional scaling consideration and PipeWire / PulseAudio runtime fallback.
