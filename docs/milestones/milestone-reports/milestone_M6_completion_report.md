# Milestone M6 Completion Report: Audio & Video Systems

> **Date:** 2026-11-09  
> **Status:** COMPLETED & EMPIRICALLY VERIFIED  
> **Target:** Milestone M6 — Audio & Video Systems  
> **Frameworks:** GSD v1.6.0 & REA v6.8.0  

---

## 1. Executive Summary

Milestone M6 delivers the core audio and video processing systems for **JonakiPlayer**:
1. **10-Band Graphic Equalizer:** Centered around 31Hz, 62Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz, and 16kHz with 1-octave Q-factor bandwidth, independent gain control clamped to [-12.0dB, +12.0dB], reset, and toggle (`Shift+E`).
2. **Dynamic Audio Normalization (`dynaudnorm`):** Real-time auto volume control (`Shift+N`) utilizing libavfilter's `dynaudnorm` with peak normalization (`p=0.95`, `m=10`), with unified filter graph management preventing chain corruption.
3. **Hardware & Software Deinterlacing:** Modes `off`, `auto`, and `on` (`Ctrl+I`), verified with synthetic interlaced H.264 video assets (`interlaced_frame=1`).
4. **Audio Delay Adjustment:** Synchronized delay adjustment in +/-0.05s steps (`Shift+<` / `Shift+>`), with instant default reset (`Shift+/`) and non-blocking OSD confirmation.
5. **Video Aspect Ratio & Scaling:** Independent display aspect ratio override (`Ctrl+F6`: Default, 16:9, 4:3, 16:10, 1:1, 1.85:1, 2.35:1, custom).
6. **M5 Verification Pass:** Bengali complex text shaping (যুক্তাক্ষর, কারচিহ্ন) empirically tested and verified alongside deterministic adjacent candidate subtitle ordering.

---

## 2. Implementation Details

### 2.1 Audio Equalizer & Normalization Pipeline
- **Files:** `src/playback/IPlaybackEngine.h`, `src/playback/MpvPlaybackEngine.h`, `src/playback/MpvPlaybackEngine.cpp`
- **Filter Chain Architecture:** `applyAudioFilters()` combines the 10-band equalizer filter graph (`lavfi=[equalizer=f=...:width_type=o:w=1:g=...]`) and dynamic audio normalizer (`lavfi=[dynaudnorm=f=500:g=31:p=0.95:m=10]`) into a clean, comma-separated mpv `af` filter graph without clearing or corrupting either filter's state.
- **Clipping Protection:** Gain parameters for all 10 bands are clamped strictly within [-12.0, +12.0] dB.

### 2.2 Deinterlacing Subsystem
- **Files:** `src/playback/MpvPlaybackEngine.cpp`
- **Modes Supported:** `"off"` (`no`), `"auto"` (`auto`), `"on"` (`yes`).
- **Pipeline Integration:** Leverages mpv's hardware deinterlacing when hardware decoding is active (`d3d11va-copy`) or falls back to software `bwdif`/`yadif`.
- **Interlaced Asset Testing:** Verified using `tests/test_data/test_interlaced.mp4` generated via FFmpeg `tinterlace=mode=interleave_top`.

### 2.3 Audio Delay Adjustment
- **Files:** `src/playback/MpvPlaybackEngine.cpp`, `src/ui/MainWindow.cpp`
- **Hotkeys:** `Shift+<` / `Shift+,` (-0.05s), `Shift+>` / `Shift+.` (+0.05s), `Shift+/` (Reset to 0.00s).
- **Bounds:** Clamped safely within [-10.0, +10.0] seconds.
- **OSD:** Visual feedback formatted to two decimal places (e.g. `Audio Sync: +0.05 s`).

### 2.4 Video Aspect Ratio Modes
- **Files:** `src/playback/MpvPlaybackEngine.cpp`, `src/ui/ContextMenuBuilder.cpp`, `src/ui/MainWindow.cpp`
- **Cycle Shortcut:** `Ctrl+F6` cycles through Default -> 16:9 -> 4:3 -> 16:10 -> 1:1 -> Default.
- **Context Menu:** Full submenu under Video > Aspect Ratio offering Default (Source), 16:9, 4:3, 16:10, 1:1, 1.85:1, and 2.35:1.

---

## 3. Empirical Verification Results

### 3.1 Automated Test Suite (`ctest`)
7 out of 7 test targets passed 100%:
```
Test project F:/Projects/JonakiPlayer/build
    Start 1: SettingsTest
1/7 Test #1: SettingsTest .....................   Passed    0.06 sec
    Start 2: PlaybackEngineMockTest
2/7 Test #2: PlaybackEngineMockTest ...........   Passed    0.04 sec
    Start 3: MpvPlaybackIntegrationTest
3/7 Test #3: MpvPlaybackIntegrationTest .......   Passed    3.02 sec
    Start 4: SkinParserTest
4/7 Test #4: SkinParserTest ...................   Passed    0.06 sec
    Start 5: PlaylistTest
5/7 Test #5: PlaylistTest .....................   Passed    0.28 sec
    Start 6: SubtitleTest
6/7 Test #6: SubtitleTest .....................   Passed    6.41 sec
    Start 7: AudioVideoFilterTest
7/7 Test #7: AudioVideoFilterTest .............   Passed    1.85 sec

100% tests passed out of 7

Total Test time (real) =  11.74 sec
```

### 3.2 Specific Audio/Video Test Cases (`AudioVideoFilterTest`)
- `testEqualizer10Bands`: Verified gain setting across all 10 bands (31Hz to 16kHz), gain clamping to [-12dB, +12dB], enable/disable toggle, and full reset to 0dB.
- `testDynamicAudioNormalizationCoexistence`: Verified enabling normalizer alongside active equalizer preserves equalizer gains without corruption, disabling equalizer leaves normalizer active, and disabling both clears the filter graph cleanly.
- `testDeinterlacingModes`: Verified mode switching across off, auto, and on on real interlaced video.
- `testAudioDelayAdjustment`: Verified +/-0.05s increments, boundary clamping, and reset.
- `testAspectRatioScalingModes`: Verified standard and custom ratio settings.
- `testSettingsPersistence`: Verified persistence of all audio/video configuration in INI storage.

### 3.3 Visual & Empirical Evidence
- Screenshot captured: `docs/milestones/evidence/m6_jonaki_audio_video_systems.png` (723x399 px baseline).
- Interlaced test media: `tests/test_data/test_interlaced.mp4`.
- Bengali subtitle test media: `tests/test_data/test_sub_bengali.srt`.

---

## 4. Next Milestone Readiness

JonakiPlayer has achieved full operational status for Audio & Video Systems. Milestone M6 is complete.
The project is ready to proceed to **Milestone M7: Advanced Features & Preferences Dialog Hierarchy**.
