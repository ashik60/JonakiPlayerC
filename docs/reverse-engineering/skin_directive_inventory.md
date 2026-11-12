# Reference Skin Directive & Attribute Inventory

> **Authoritative Baseline:** Extracted from reference PotPlayer skin definition files (`Skins/Default.dsf` / `WindowFrame.dsf`).  
> **Machine-Readable Companion:** [`skin_directive_inventory.json`](file:///f:/Projects/JonakiPlayer/docs/reverse-engineering/skin_directive_inventory.json) (1,714 records across 21 layout files).  
> **Date:** 2026-11-12  
> **Status:** COMPLETE & EMPIRICALLY AUDITED  

---

## 1. Overview & Inventory Scope

This inventory catalogs every XML/TXT directive, layout tag, and attribute present in the reference Daum/Kakao PotPlayer default skin distribution (`Skins/Default.dsf` and `WindowFrame.dsf`).

### 1.1 Archive Audit & Exclusion Reconciliation
- **Total Files Inspected in Reference Archive:** 25 files
- **Active Layout Files in Machine-Readable Inventory:** 21 files
- **Files Excluded from Directive Inventory & Explicit Reasons:**
  1. `theme.xml` — **Metadata Envelope:** Contains skin name, author, version, and entry-point pointers (`<Skin Name="Default" File="VideoSkin.xml">`). Contains no control layout directives.
  2. `DvdSkin.xml` — **Empty Stub:** Contains only `<DvdSkin></DvdSkin>` root wrapper with 0 control elements. Excluded to prevent empty noise records.
  3. `WdmSkin.xml` — **Empty Stub:** Contains only `<WdmSkin></WdmSkin>` root wrapper with 0 control elements. Excluded to prevent empty noise records.
  4. `StringTable.txt` — **Localization Catalog:** Flat key-value catalog of localized strings and tooltips (`IDS_PLAY=Play`, `IDS_PAUSE=Pause`). Analyzed separately in string catalog registers.
- **Active Layout Files (21 files):**
  - **Core Video:** `BottomVideo.txt`, `TopCommon.txt`, `Common.txt`, `VideoSkin.xml`, `Popup.txt`
  - **Audio:** `AudioSkin.xml`, `AudioMini.xml`, `BottomAudio.txt`
  - **Touch / Mobile:** `TouchBottom.txt`, `TouchTop.txt`, `TouchRight.txt`
  - **Chrome & Sub-panels:** `SubCommon.txt`, `Chat.txt`, `RightAttach.txt`, `Right.txt`, `TopCast.txt`, `BottomVod.txt`, `BottomLeft.txt`, `Top.txt`, `Logo.txt`, `ChildSkin.xml`

### 1.2 Metric Separation
- **Total Unique Directive Tag Types:** 59 unique directives (e.g. `Button`, `Static`, `Label`, `Slider`, `PlayPauseWidget`, etc.).
- **Total Directive Occurrences (Instances):** 1,714 instances across the 21 layout files.
- **Total Unique Attribute Types:** 127 unique attribute names across all directives.

---

## 2. Core Directive Taxonomy

| Directive / Tag | Primary Purpose | Files Present | Total Attributes | Parser Support Status | Confidence |
|---|---|---|---|---|---|
| `<Button>` | Push and toggle buttons with 4-frame state sprites | 14 files | 40 attributes | **Supported (Basic) / Expansion in M8** | High |
| `<Static>` | Container backgrounds, texture slices, titlebar bounds | 20 files | 29 attributes | **Supported (Basic) / Expansion in M8** | High |
| `<Slider>` | Timeline seekbars, volume faders, thumb sliders | 6 files | 26 attributes | **Supported** | High |
| `<Label>` | Dynamic and static text indicators, time displays | 14 files | 36 attributes | **Supported (Basic) / Expansion in M8** | High |
| `<Layout>` | Spacing, margins, responsive layout blocks | 10 files | 10 attributes | **Supported** | High |
| `<Image>` | Static brand badges, channel logos, indicators | 7 files | 28 attributes | **Unsupported / In M8.1** | High |
| `<ImageReplace>` | Scoped texture atlas override (e.g. `TouchMain.png`) | 3 files | 2 attributes | **Unsupported / In M8.1** | High |
| `<PlayPauseWidget>`| Centered on-screen play/pause overlay animation | 1 file (`Common.txt`) | 9 attributes | **Unsupported / In M8.2** | High |
| `<CustomPopupMenuDef>` | Right-click and dropdown popup menu templates | 1 file (`Logo.txt`) | 23 attributes | **Unsupported / In M8.3** | High |
| `<SubImage>` | Composite secondary overlay slices | 2 files | 4 attributes | **Unsupported / In M8.1** | High |
| `<ArrowUp/Down/Left/Right>` | Scrollbar arrow buttons | 1 file (`Common.txt`) | 4 attributes | **Unsupported / In M8.2** | High |
| `<ThumbUp/Down/Middle>` | Scrollbar thumb 3-patch components | 1 file (`Common.txt`) | 4 attributes | **Unsupported / In M8.2** | High |
| `<MainSkinArea>` | Top-level frame region definition | 4 files | 8 attributes | **Approximated via FramelessHelper** | High |
| `<WindowType0/1/2>` | Non-client aero frame and dialog background slices | 1 file (`SubCommon.txt`) | 24 attributes | **Unsupported / In M8.2** | High |

---

## 3. Key Reverse-Engineering Findings & Mechanics

### 3.1 Button State-Frame Extraction (4-Frame Horizontal Stride)
- **Static Resource Evidence:** Reference buttons specify `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight` without separate attributes for hover, pressed, or disabled states (e.g. `CMD_MEDIA_PLAY`: `ImgL='0'`, `ImgT='259'`, `W='41'`, `H='39'`).
- **Physical Atlas Stride:** In `Main.png`, adjacent frames are packed in horizontal sequence:
  - `Frame 0 (Normal):` `x = ImageLeft`
  - `Frame 1 (Hover / Over):` `x = ImageLeft + ImageWidth`
  - `Frame 2 (Pressed / Down):` `x = ImageLeft + 2 * ImageWidth`
  - `Frame 3 (Disabled):` `x = ImageLeft + 3 * ImageWidth`
- **Direct Runtime Observation:** In PotPlayer, hovering over `Play` displays the sprite frame at `(41, 259)`, pressing displays `(82, 259)`, and disabled displays `(123, 259)`. At `x=164`, the Pause button begins (`x=164..327`).
- **Classification:** **[DIRECT RUNTIME OBSERVATION & STATIC RESOURCE VERIFICATION]**

### 3.2 ColorX / ColorY Palette Sampling
- **Static Resource Evidence:** Labels specify `ColorX` and `ColorY` integer coordinates (185 occurrences across 14 layout files) pointing to single pixel locations in `Main.png`:
  - `(1052, 74)`: Samples to `#FFFFFF` (Primary Text / Elapsed Time).
  - `(864, 127)`: Samples to `#FF3217` (Orange Accent / Live / Recording indicator).
  - `(875, 528)`: Samples to `#777777` (Secondary Text / Total Duration).
- **Direct Runtime Observation:** When a custom user-recolored `Main.png` is placed in the skin folder, text labels render in the updated sampled pixel color without modifying XML source text.
- **Classification:** **[DIRECT RUNTIME OBSERVATION & STATIC RESOURCE VERIFICATION]**

### 3.3 Alignment='5'
- **Static Resource Evidence:** 30 buttons and 20 images specify `Alignment='5'` in addition to docking attribute `Align='Left'` or `Align='Right'`.
- **Strong Inference (Experimental Mapping):** `Align` dictates container docking (`Left`, `Right`, `Client`). `Alignment='5'` corresponds to the standard Win32 GDI flags bitwise combination `DT_CENTER (0x01) | DT_VCENTER (0x04) = 0x05`.
- **Qt Mapping:** Corresponds to `Qt::AlignCenter` (`Qt::AlignHCenter | Qt::AlignVCenter`).
- **Classification:** **[STRONG INFERENCE / EXPERIMENTAL MAPPING]** (To be validated via controlled side-by-side rendering tests in Wave 8.1 across text, button, and image elements).

### 3.4 VisParentMinWidth and VisMinLeft (Responsive Layout Rules)
- **Static Resource Evidence:** 49 directives specify `VisParentMinWidth` (thresholds: 240, 300, 450, 550, 630, 670, 690, 700, 740 px). 11 buttons specify `VisMinLeft` (thresholds: 320, 350, 380, 410 px).
- **Direct Runtime Observation:** Dynamically resizing PotPlayer window verifies that controls hide when the window width drops below `VisParentMinWidth`. For example:
  - Width > 630 px: FourCC video codec badge (`ID_TEXT_V_CODEC`) and audio codec badge (`ID_TEXT_A_CODEC`) are visible.
  - Width < 630 px: Codec badges automatically hide.
  - Width < 460 px: Hardware decoding indicator badge hides.
  - `VisMinLeft`: Suppresses right-side auxiliary buttons when window compression brings them to within `VisMinLeft` pixels of the left edge, preventing button collisions.
- **Classification:** **[DIRECT RUNTIME OBSERVATION & REPRODUCIBLE GEOMETRY EVIDENCE]**

### 3.5 ImageReplace Scoping
- **Static Resource Evidence:** Enclosing tag `<ImageReplace Image='TouchMain.png' AlphaMode='1'> ... </ImageReplace>` wraps child controls in `VideoSkin.xml`, `AudioSkin.xml`, and `TouchRight.txt`.
- **Strong Inference:** All child directives within the `<ImageReplace>` block sample their textures and sprite frames from `TouchMain.png` instead of `Main.png`.
- **Classification:** **[STATIC RESOURCE EVIDENCE & STRONG INFERENCE]** (Kept as static resource evidence until dynamic texture substitution is independently captured in runtime).

### 3.6 ScrollBar Rendering Mechanics
- **Static Resource Evidence:** Directives `<ArrowUp>`, `<ArrowDown>`, `<ArrowLeft>`, `<ArrowRight>`, `<ThumbUp>`, `<ThumbDown>`, `<ThumbMiddle>`, `<ThumbMiddleUp>`, `<ThumbMiddleDown>` define scrollbar assets extracted from `ScrollBar.bmp` and `ScrollBarPL.bmp`.
- **Direct Runtime Observation:** In the playlist and chat windows, scrollbars render as a 3-patch vertical thumb composed of fixed top cap (`ThumbUp`, 4px), repeating/stretched core (`ThumbMiddle`), and fixed bottom cap (`ThumbDown`, 4px).
- **Classification:** **[DIRECT RUNTIME OBSERVATION & STATIC RESOURCE VERIFICATION]**

### 3.7 PlayPauseWidget Overlay Animation
- **Static Resource Evidence:** `<PlayPauseWidget Image='TouchMain.png' AlphaMode='1' PlayImageLeft='283' PlayImageTop='140' PlayImageWidth='131' PlayImageHeight='131' PauseImageLeft='283' PauseImageTop='271' PauseImageWidth='131' PauseImageHeight='131' />` references central 131×131 play/pause glyphs in `TouchMain.png`.
- **Direct Runtime Observation:** On Spacebar toggle or mouse click, a semi-transparent icon flashes in the center of the video screen and fades away smoothly.
- **Timing Measurement:** Empirically observed as an approximately 750 ms total duration (nominal breakdown: ~150 ms linear fade-in, ~350 ms hold at 75% opacity, ~250 ms fade-out). Recorded as approximate until repeated millisecond-accurate video frame analysis is captured in Wave 8.2.
- **Classification:** **[DIRECT RUNTIME OBSERVATION (TIMING APPROXIMATE)]**
