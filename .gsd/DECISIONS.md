# DECISIONS.md — Architecture & Technical Decisions

---

## 2026-11-09

### DEC-001: Adopt C++20, Qt 6, libmpv, and FFmpeg
- **Context:** Need cross-platform, high-performance multimedia player replicating PotPlayer on Windows and Linux.
- **Decision:** Use C++20 for performance and modern language features, Qt 6 for pixel-level UI customization, libmpv for playback core, and FFmpeg for media handling.
- **Status:** Accepted

### DEC-002: Dynamic Skin Parsing for `.dsf` Format
- **Context:** PotPlayer interface styling relies on `.dsf` ZIP archives containing XML layouts and image atlases.
- **Decision:** Build a custom clean-room `SkinRenderer` in Qt to interpret layout XML and paint sprites without proprietary PotPlayer binary components.
- **Status:** Accepted

### DEC-003: Abstract `IPlaybackEngine` Layer
- **Context:** Protect UI and business logic from tight coupling to libmpv properties.
- **Decision:** Interface-based architecture isolating player state and commands behind `IPlaybackEngine`.
- **Status:** Accepted

### DEC-004: Standardize on MSYS2 UCRT64 on Windows
- **Context:** Need automated, reproducible C++20/Qt6/libmpv toolchain on Windows.
- **Decision:** Use MSYS2 UCRT64 providing native GCC 16, Qt 6.12, libmpv 0.42, and CMake.
- **Status:** Accepted
