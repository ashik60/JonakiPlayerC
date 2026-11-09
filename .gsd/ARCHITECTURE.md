# ARCHITECTURE.md — System Architecture: JonakiPlayer

> **Version:** 1.0  
> **Status:** Baseline Architecture  
> **Date:** 2026-11-09

---

## 1. System Overview

JonakiPlayer uses a layered, event-driven modular architecture designed to separate multimedia playback, user interface presentation, skin styling, and operating system integration.

```
┌────────────────────────────────────────────────────────────────────────┐
│                          USER INTERACTION                              │
│       (Mouse Clicks, Keyboard Hotkeys, Drag & Drop, Context Menus)     │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                        JONAKIPLAYER UI LAYER                           │
│  ┌──────────────────────┐ ┌───────────────────┐ ┌────────────────────┐ │
│  │   SkinRenderer       │ │  MainWindowFrame  │ │  PlaylistDrawer    │ │
│  │ (.dsf XML & Sprites) │ │(Controls, Seekbar)│ │  (Queue, Reorder)  │ │
│  └──────────────────────┘ └───────────────────┘ └────────────────────┘ │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Qt Signals / Slots
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                     APPLICATION & SETTINGS LAYER                       │
│  ┌──────────────────────┐ ┌───────────────────┐ ┌────────────────────┐ │
│  │   SettingsManager    │ │  ShortcutManager  │ │   HistoryManager   │ │
│  │   (INI persistence)  │ │  (Key mapping)    │ │   (Playback state) │ │
│  └──────────────────────┘ └───────────────────┘ └────────────────────┘ │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                      PLAYBACK ABSTRACTION LAYER                        │
│                           (IPlaybackEngine)                            │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                        CORE MULTIMEDIA ENGINES                         │
│  ┌────────────────────────────────────────┐ ┌────────────────────────┐ │
│  │              libmpv Core               │ │      FFmpeg Tooling    │ │
│  │   (mpv_handle, render_context, GL/VK)  │ │  (Demuxing, Codec info)│ │
│  └────────────────────────────────────────┘ └────────────────────────┘ │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                       PLATFORM HARDWARE & OS                           │
│     Windows: Direct3D11 / WASAPI   │   Linux: Wayland/X11 / PipeWire   │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Core Modules

### 2.1 UI Layer (`src/ui/`)
- **`SkinRenderer`**: Parses PotPlayer `.dsf` archives (XML layouts like `BottomVideo.txt`, `AudioSkin.xml`, and PNG spritesheets) to render native controls with pixel parity.
- **`MainWindow`**: Custom frameless Qt window handling DWM window snapping, edge resizing, fullscreen toggling, and floating controls.
- **`ControlBar`**: Houses playback controls (Play, Pause, Stop, Seek, Volume, Fullscreen) matching PotPlayer layout.
- **`TimelineSlider`**: Precise seek slider with buffered track progress, hover time tooltip, and chapter ticks.
- **`PlaylistWidget`**: Dockable / slide-out playlist manager with folder recursion, drag-drop, and search.

### 2.2 Playback Layer (`src/playback/`)
- **`IPlaybackEngine`**: Pure abstract interface declaring media operations (`loadMedia`, `play`, `pause`, `seek`, `setSpeed`, `setVolume`, `selectTrack`).
- **`MpvPlaybackEngine`**: Concrete implementation utilizing `libmpv`'s C API and OpenGL/Vulkan render context.
- **`VideoWidget`**: QOpenGLWidget / QWidget hosting the `mpv_render_context` surface.

### 2.3 Settings & State (`src/settings/`)
- **`SettingsManager`**: Cross-platform configuration serializer reading/writing INI files (`JonakiPlayer.ini`) identical in key naming to PotPlayer's registry/INI format.
- **`ShortcutManager`**: Translates Qt key events to PotPlayer command IDs (`ID_PLAY_PAUSE`, `ID_PROCESS_PREV`, `CMD_MEDIA_FAST_FORWARD`).

### 2.4 Platform Layer (`src/platform/`)
- **`src/platform/windows/`**: Custom `WM_NCHITTEST` handling, Direct3D swapchain helpers, WASAPI audio output settings.
- **`src/platform/linux/`**: Wayland fractional scaling helpers, X11 atoms, PipeWire / PulseAudio bindings.

---

## 3. Data Flow

1. **User Action:** User drags an MKV file onto `MainWindow`.
2. **Drop Event:** `MainWindow::dropEvent` catches URLs and forwards them to `PlaylistManager`.
3. **Queue & Selection:** `PlaylistManager` adds items and triggers `IPlaybackEngine::loadMedia(path)`.
4. **Engine Initialization:** `MpvPlaybackEngine` calls `mpv_command_async("loadfile", path)` and retrieves stream metadata (video codecs, audio tracks, subtitle tracks).
5. **UI Update:** Playback engine emits `durationChanged`, `trackListChanged`, and `stateChanged(Playing)`.
6. **Skin & Control Sync:** `ControlBar` updates Play/Pause button sprite, timeline adjusts range, title bar updates with media title and resolution.
