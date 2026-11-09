# PotPlayer Playback, Playlist & Media Workflows Analysis

> **Milestone:** M1 — PotPlayer Reconnaissance & Evidence Mapping  
> **Status:** Completed & Empirically Verified  
> **Target:** Daum PotPlayer 64-bit Playback Engine & File Persistence  

---

## 1. Playback Architecture & Hardware Acceleration

### 1.1 Video Decoder & Renderer Chain [DIRECTLY-OBSERVED]
From the binary resource strings and registry keys (`Dialog(325)`, `Dialog(326)`):
- **Splitters:** Built-in FFmpeg Source/Splitter (`ffcodec64.dll`) handles demuxing of MKV, MP4, AVI, TS, FLV, WebM.
- **Hardware Acceleration Modes:**
  - DXVA2 (DirectX Video Acceleration 2.0 Native & Copy-Back)
  - D3D11VA (Direct3D 11 Video Acceleration)
  - NVIDIA NVDEC / CUDA
  - Intel QuickSync Video (QSV)
- **Audio Output Chain:**
  - WASAPI Exclusive Mode (low latency, bit-exact streaming)
  - WASAPI Shared Mode (standard desktop audio mix)
  - DirectSound fallback

### 1.2 Playback Stepping & Speed Control [DIRECTLY-OBSERVED]
- **Speed Bounds:** Minimum **0.2x**, Maximum **3.0x**, Step **0.1x**.
- **Pitch Correction:** Scales tempo while maintaining original audio pitch via internal phase vocoder / WSOLA (clean-room: mpv `scaletempo2`).
- **Frame Stepping:** Exact single frame advance forward (`F`) and reverse (`D`).
- **A-B Repeat:**
  - `[`: Mark start point A.
  - `]`: Mark end point B.
  - `\`: Toggle loop or clear markers.
  - Sliced markers render directly on the seekbar at `RepeatAImageLeft='148'` and `RepeatBImageLeft='155'`.

---

## 2. Playlist File Format (`.dpl`) Specification [DIRECTLY-OBSERVED]

Reverse engineering of `C:\Users\User\AppData\Roaming\PotPlayerMini64\Playlist\PotPlayerMini64.dpl` reveals an INI-like text schema:

```text
DAUMPLAYLIST
playname=C:\path\to\media.mkv
playtime=53432
topindex=0
saveplaypos=0
1*file*C:\path\to\media.mkv
1*duration2*2434688
1*start*53432
2*file*C:\path\to\second_media.mp4
2*duration2*1800000
2*start*0
```

### 2.1 Schema Breakdown
- **Header:** Exact token `DAUMPLAYLIST` on line 1.
- `playname`: Absolute file path or URL of the active item currently playing or selected.
- `playtime`: Playback position of the active item in milliseconds.
- `topindex`: Scroll position index of the playlist view.
- `saveplaypos`: Boolean flag (1=save position per file, 0=do not save).
- `N*file*<path>`: 1-indexed item path or stream URL.
- `N*duration2*<ms>`: Item duration in milliseconds (`2434688` = ~40.5 minutes).
- `N*start*<ms>`: Saved resume position for item `N` in milliseconds.
- `N*title*<title>`: Optional custom display title override.

---

## 3. History & Bookmark Database Architecture

- Database located at `C:\Users\User\AppData\Roaming\PotPlayerMini64\MediaInfo.sdb` (53 KB binary SQLite/structured format).
- Tracks: File hash / size / path, last played timestamp, last seek offset, audio stream selected, external subtitle path selected.
- In clean-room JonakiPlayer, this will be modeled via an embedded SQLite database (`history.db`) or structured JSON.
