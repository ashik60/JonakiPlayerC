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

---

## Platform-Specific Constraints

- **Windows:** Frameless window resizing requires custom `WM_NCHITTEST` handling for native aero-snap and border resizing.
- **Linux:** Requires Wayland fractional scaling consideration and PipeWire / PulseAudio runtime fallback.
