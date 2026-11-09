# PotPlayer Feature Parity Matrix

> **Application:** JonakiPlayer vs. Daum PotPlayer 64-bit  
> **Status:** M4 Playlist & Interaction System Completed & Verified  
> **Legend:**  
> - 🟢 **Full Parity:** Fully implemented and verified with tests.  
> - 🟡 **Partial Parity:** Implemented with minor gaps or in progress.  
> - ⚪ **Planned:** On roadmap, evidence gathered, pending implementation phase.  
> - 🔴 **Deferred / Unsupported:** Out of scope (e.g. proprietary Kakao broadcasts).  

---

## Comprehensive Feature Inventory & Parity Register

| ID | Category | Feature Name | Priority | Confidence | Evidence Status | Planned Milestone | Notes |
|---|---|---|---|---|---|---|---|
| **P-01** | Playback | Multi-format local playback (MKV, MP4, AVI, TS, WebM) | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟢 | libmpv / FFmpeg integration verified with MP4/MP3 tests |
| **P-02** | Playback | Frame stepping forward (`F`) and reverse (`D`) | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟢 | mpv `frame-step` / `frame-back-step` implemented in MpvPlaybackEngine |
| **P-03** | Playback | Playback speed control (0.2x–3.0x, 0.1x step, pitch scale) | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟢 | mpv `speed` with `audio-pitch-correction` verified in CTest |
| **P-04** | Playback | A-B Repeat loop (`[`, `]`, `\`) with timeline markers | Should-have | High | [DIRECTLY-OBSERVED] | M7 | Points render on seekbar |
| **P-05** | Playback | Resume from last position & history store | Must-have | High | [DIRECTLY-OBSERVED] | M4 🟢 | SQLite `MediaInfo.sdb` schema, tested in CTest |
| **P-06** | Playback | Hardware acceleration (D3D11VA, DXVA2, VAAPI, NVDEC) | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟡 | Configured via `hwdec=auto-safe`, introspection via `getHwdecActive()` |
| **UI-01** | Window | Frameless skinnable main window (723 x 399 default) | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟢 | Win32 aero-snap + Qt frameless |
| **UI-02** | Window | Custom title bar (31px) with menu & system buttons | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟢 | Clean-room TitleBarWidget matching TopCommon.txt |
| **UI-03** | Window | Timeline seekbar with buffer range & chapter markers | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟢 | BottomControlBar with seek position tracking |
| **UI-04** | Window | Volume slider (70x10px) with mute button toggle | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟢 | Integrated in seekbar row with mute toggle |
| **UI-05** | Window | Auto-hide controls in fullscreen mode | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟡 | Fullscreen toggle implemented, idle timer in M8 |
| **UI-06** | Window | Preferences dialog (751 x 564) with tree navigation | Must-have | High | [DIRECTLY-OBSERVED] | M7 | Matches 184 dialog hierarchy |
| **UI-07** | Menu | Complete context menu (886 entries from `[MenuString]`) | Must-have | High | [DIRECTLY-OBSERVED] | M3 🟢 | Loaded from `menu_hierarchy.json` |
| **PL-01** | Playlist | Collapsible / dockable playlist drawer (`F8`) | Must-have | High | [DIRECTLY-OBSERVED] | M4 🟢 | 280px drawer expanding window from 723 to 1003 px |
| **PL-02** | Playlist | Daum playlist format (`.dpl`) parser and serializer | Must-have | High | [DIRECTLY-OBSERVED] | M4 🟢 | Round-trip compatible, verified against reference DPLs |
| **PL-03** | Playlist | Drag-and-drop file, folder, and URL handling | Must-have | High | [DIRECTLY-OBSERVED] | M4 🟢 | Recursive directory scanning & queueing |
| **PL-04** | Playlist | Queue repeat (One/All/Stop) and shuffle playback | Must-have | High | [DIRECTLY-OBSERVED] | M4 🟢 | Controlled via PlaylistController |
| **SUB-01** | Subtitles | Embedded subtitle track auto-detection & switching | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟡 | Track enumeration in engine, full UI in M5 |
| **SUB-02** | Subtitles | External subtitle matching (same-basename auto-load) | Must-have | High | [DIRECTLY-OBSERVED] | M5 | Controlled by `OpenWithSameName=2` |
| **SUB-03** | Subtitles | Real-time subtitle delay adjustment (`<` and `>`) | Must-have | High | [DIRECTLY-OBSERVED] | M5 | 0.5s offset increment |
| **SUB-04** | Subtitles | Subtitle style string parsing (`CaptionStyle`) | Should-have | High | [DIRECTLY-OBSERVED] | M5 | Font, outline, shadow, alignment |
| **AUD-01** | Audio | 10-Band graphic equalizer (31Hz to 16kHz) | Should-have | High | [DIRECTLY-OBSERVED] | M6 | mpv `af=equalizer` |
| **AUD-02** | Audio | Volume normalization filter | Should-have | High | [DIRECTLY-OBSERVED] | M6 | mpv `af=dynaudnorm` |
| **AUD-03** | Audio | Real-time audio delay adjustment (`Shift+<`, `Shift+>`) | Must-have | High | [DIRECTLY-OBSERVED] | M6 | 0.05s offset increment |
| **VID-01** | Video | Aspect ratio modes (Default, 16:9, 4:3, Fill, Stretch) | Must-have | High | [DIRECTLY-OBSERVED] | M2 🟡 | Engine switching verified, context menu mapped |
| **VID-02** | Video | Hardware & software deinterlacing | Should-have | High | [DIRECTLY-OBSERVED] | M6 | mpv `deinterlace=yes` |
| **VID-03** | Video | Frame capture to PNG/JPG (`Ctrl+E`) | Should-have | High | [DIRECTLY-OBSERVED] | M7 | mpv `screenshot` command |
| **ADV-01** | Streaming | HTTP/HTTPS/HLS network stream playback | Should-have | High | [DIRECTLY-OBSERVED] | M7 | Stream loading via URL |
| **ADV-02** | Streaming | YouTube URL playback via yt-dlp integration | Nice-to-have | Medium | [INFERRED] | M7 | yt-dlp hook in libmpv |
| **OUT-01** | Cloud | Daum/Kakao live broadcast streaming & chat room | Out-of-scope | Low | [UNRESOLVED] | Deferred | Commercial proprietary service |
| **OUT-02** | Ads | Commercial advertising popups & banners | Out-of-scope | None | [DIRECTLY-OBSERVED] | Excluded | Adware non-goal (SPEC.md) |
