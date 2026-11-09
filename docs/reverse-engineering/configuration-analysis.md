# PotPlayer Configuration & Preferences Architecture Analysis

> **Milestone:** M1 — PotPlayer Reconnaissance & Evidence Mapping  
> **Status:** Completed & Empirically Verified  
> **Target:** Daum PotPlayer 64-bit Registry & Dialog Schema  

---

## 1. Preferences Hierarchy & Dialog Topology

Direct binary inspection of `PotPlayer64.dll` identified **184 native `RT_DIALOG` resource templates**. The main Preferences window (`F5` / `ID_OPTION_WINDOW`) is structured around a master navigation tree on the left and stacked child property pages on the right (measured at **751 x 564 pixels**).

```
Preferences (F5) [751x564]
├── General
│   ├── General Settings (Dialog 354)
│   ├── Startup (Dialog 355)
│   ├── OSD (Dialog 356)
│   ├── Logo (Dialog 357)
│   ├── Mouse (Dialog 358)
│   ├── Keyboard (Dialog 359)
│   ├── Association (Dialog 360)
│   └── Skin / Color Theme (Dialog 361)
├── Playback
│   ├── Playback Settings (Dialog 362)
│   ├── Time Jump (Dialog 363)
│   ├── Repeat / Auto-resume (Dialog 364)
│   └── Speed (Dialog 365)
├── Video
│   ├── Video Processing (Dialog 366)
│   ├── Video Renderer (Dialog 367)
│   ├── Color Management / HDR (Dialog 368)
│   ├── Pixel Shader (Dialog 369)
│   └── 3D / 360 Video (Dialog 370)
├── Audio
│   ├── Audio Output (Dialog 371)
│   ├── Volume Normalizer (Dialog 372)
│   ├── 10-Band Equalizer (Dialog 373)
│   └── Audio Processing / Delay (Dialog 374)
├── Subtitles
│   ├── Subtitle Language / Priority (Dialog 375)
│   ├── Font & Styling (Dialog 376)
│   ├── Placement & Margins (Dialog 377)
│   └── Subtitle Sync & Search (Dialog 378)
└── Filter Control
    ├── Source / Splitter Priority (Dialog 325)
    ├── Video Decoders (Built-in, FFmpeg, DXVA2, D3D11VA) (Dialog 326)
    ├── Audio Decoders (Dialog 327)
    └── Custom Filter Manager (Dialog 328)
```

---

## 2. Configuration Storage Schema [DIRECTLY-OBSERVED]

PotPlayer supports two persistence mechanisms:
1. **Windows Registry:** `HKCU\Software\Daum\PotPlayerMini64\Settings` (Default).
2. **INI File:** When the option `"Store settings into .ini file"` (`Dialog(354)/3083`) is enabled, all settings are serialized to `PotPlayerMini64.ini` alongside the executable or in AppData.

### 2.1 Key Setting Definitions Mapped in Registry & INI

| Key Name | Type | Reference Value | Purpose / Meaning in Playback |
|---|---|---|---|
| `PlaybackMode` | DWORD | `1` | Default repeat mode (0=Stop, 1=Play next, 2=Repeat one, 3=Repeat all) |
| `RememberPosition` | DWORD | `1` | Remember playback position and prompt to resume |
| `OnTopMode` | DWORD | `2` | Always on top (0=Never, 1=Always, 2=While playing) |
| `LastSkinXmlName` | String | `VideoSkin.xml` | Currently selected active skin layout template |
| `OpenWithSameName` | DWORD | `2` | Auto-load external audio/subtitles with same base filename |
| `VideoSaturaction` | DWORD | `66` | Default video saturation level (0-100 scale, baseline 66) |
| `MftDecoder` | DWORD | `1` | Media Foundation decoder enabled |
| `DmoDecoder` | DWORD | `1` | DirectX Media Object decoder enabled |
| `CaptionStyle` | String | Multi-field string | Semicolon-delimited ASS font name, size, outline, shadow, color RGBA |
| `PrevSpeed` | Binary | Byte array | Historical playback speed stepping table |
| `LastPlayListName` | String | `PotPlayerMini64.dpl` | Path or filename of the active persistent playlist |
| `CheckAutoUpdate` | DWORD | `0` | Disables automated update ping check |

---

## 3. Subtitle Style String Serialization [DIRECTLY-OBSERVED]

The `CaptionStyle` setting encodes subtitle styling in a structured token format:
`[FontName];[Size];[Bold];[Italic];[Underline];[PrimaryColor];[SecondaryColor];[OutlineColor];[ShadowColor];[OutlineWidth];[ShadowWidth];[Alignment];[MarginV]`
This maps directly to standard ASS (Advanced SubStation Alpha) style parameters, which JonakiPlayer will map to `libass` style objects.
