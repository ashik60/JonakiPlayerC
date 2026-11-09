# Technical Decisions Register (TDR / ADR)

This document tracks architectural and technical decisions made during the development of **JonakiPlayer**, along with context, alternatives considered, and rationale.

---

## TDR-001: Project Architecture and Technology Selection

- **Status:** Accepted
- **Date:** 2026-11-09
- **Context:** JonakiPlayer requires high-performance multimedia playback, cross-platform UI fidelity matching Daum PotPlayer, and maintainable native desktop code on Windows 10/11 and Linux.
- **Alternatives Considered:**
  1. *Electron + HTML5/WebAssembly*: Rejected due to high memory consumption, high idle CPU, lack of low-level Direct3D/WASAPI audio pipeline control, and sluggish window resize.
  2. *C# / .NET MAUI / WPF*: Rejected due to poor Linux parity, heavy runtime dependency, and complex native multimedia filter interop.
  3. *C++20 + Qt 6 + libmpv + FFmpeg*: **Selected**. Native performance, low idle footprint, rock-solid video/audio rendering pipelines, rich widget toolkit capable of custom painting/skinning, and identical source code compiling on Windows and Linux.
- **Decision:** Adopt C++20, Qt 6 (Widgets), CMake, Ninja, libmpv, and FFmpeg as the core engineering stack.

---

## TDR-002: Skin Engine Architecture (Clean-Room PotPlayer Parity)

- **Status:** Accepted
- **Date:** 2026-11-09
- **Context:** PotPlayer's primary visual identity comes from its custom skinnable frameless interface (`.dsf` format).
- **Alternatives Considered:**
  1. *Hardcoding Qt QSS/StyleSheet to look roughly like PotPlayer*: Rejected as it fails pixel-level fidelity, breaks custom DPI scaling, and deviates from PotPlayer's dynamic element behavior.
  2. *Dynamic Clean-Room XML Skin Parser*: **Selected**. Reverse engineering of `Default.dsf` confirmed it is a standard ZIP archive containing XML layout specifications (`BottomVideo.txt`, `AudioSkin.xml`, `ChildSkin.xml`) and texture atlas bitmaps (`Main.png`, `Control.png`). A dedicated `SkinRenderer` parses these layouts and paints UI controls using exact sprite offsets.
- **Decision:** Implement an independent `SkinParser` and `SkinRenderer` in C++/Qt to render PotPlayer `.dsf` skins cleanly.

---

## TDR-003: Playback Engine Decoupling

- **Status:** Accepted
- **Date:** 2026-11-09
- **Context:** The application needs a robust playback subsystem that can handle all video/audio formats, hardware decoding, stream playback, and subtitle synchronization without tightly coupling the UI to mpv specifics.
- **Decision:** Encapsulate playback behind the `IPlaybackEngine` abstract C++ interface. Implement `MpvPlaybackEngine` using `libmpv`'s C API and render context. This allows future pluggable backends (e.g., custom DirectShow/MediaFoundation or GStreamer) if required.

---

## TDR-004: Windows Development Toolchain via MSYS2 UCRT64

- **Status:** Accepted
- **Date:** 2026-11-09
- **Context:** Need a unified, reproducible C++20 and Qt6/libmpv toolchain on Windows that matches Linux packaging paradigms.
- **Decision:** Standardize on MSYS2 UCRT64 (`mingw-w64-ucrt-x86_64`) for local builds and CI, providing precompiled GCC 14, Qt 6.x, CMake, Ninja, libmpv, and FFmpeg, while maintaining standard MSVC CMake build compatibility.

---

## TDR-005: Video Surface Architecture via Qt OpenGL Framebuffer (`mpv_render_context`)

- **Status:** Accepted
- **Date:** 2026-11-09
- **Context:** Modern multimedia players require zero-copy or direct hardware-accelerated video rendering integrated with Qt's widget composition tree.
- **Alternatives Considered:**
  1. *Direct HWND window embedding (`wid` property)*: Causes flickering during resize, breaks cross-platform Wayland composition, and prevents overlaying semi-transparent Qt HUD/controls directly over the video surface without complex airspace workarounds.
  2. *Software rendering to QImage / QPixmap*: Excessive CPU overhead for high-bitrate 4K/60fps video.
  3. *OpenGL FBO rendering via `mpv_render_context`*: **Selected**. `VideoOpenGLWidget` subclasses `QOpenGLWidget`. During `paintGL()`, `mpv_render_context_render()` renders directly to the widget's default OpenGL framebuffer (`defaultFramebufferObject()`), with an update callback invoking `QMetaObject::invokeMethod` to trigger repaints on the main Qt GUI thread.
- **Decision:** Standardize on `QOpenGLWidget` + `mpv_render_context` with OpenGL API parameters (`MPV_RENDER_PARAM_OPENGL_FBO`) for cross-platform zero-airspace video presentation.
