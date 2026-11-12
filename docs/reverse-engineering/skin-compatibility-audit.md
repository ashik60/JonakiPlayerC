# Skin Engine Directive Compatibility Matrix & Reverse-Engineering Audit

> **Date:** 2026-11-12  
> **Status:** AUDITED, RECONCILED & UPDATED FOR M8  
> **Target:** PotPlayer Skin Compatibility (`Default.dsf`, `WindowFrame.dsf`) & JonakiPlayer Common Theme IR  
> **Evidence Register:** Cross-referenced with REV-002, REV-011, REV-018, and `docs/reverse-engineering/skin_directive_inventory.json`  

---

## 1. Classification Methodology

To maintain absolute engineering integrity, every directive, attribute, and behavioral mechanic is categorized into one of four precise tiers:

1. **Direct Runtime Observation [DRO]:** Verified via direct live execution and measurement of PotPlayer and JonakiPlayer.
2. **Static Resource Evidence [SRE]:** Confirmed via structural, byte-level, or coordinate analysis of reference `.dsf` skins and PE resources.
3. **Strong Inference [SI]:** Inferred from standard Win32 GDI / DirectUI conventions and consistent layout behaviors.
4. **Unresolved Behavior [UB]:** Not yet fully reverse-engineered; documented with an explicit placeholder and target milestone.

---

## 2. Updated Skin Compatibility Matrix

| Category | Directive / Feature | Supported Attributes | Status | Evidence Tier | Notes & IR Mapping |
|---|---|---|---|---|---|
| **Layout** | `<Static>` | `Align`, `Height`, `Width`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`, `IsTitleBar`, `LeftMargin`, `RightMargin` | **Supported (Expanding in M8)** | SRE / DRO | Maps to `ControlDefinition` (Container / Slice). Slices 3-patch and 9-patch borders. |
| **Controls** | `<Button>` | `CmdID`, `Tooltip`, `Align`, `Alignment`, `Width`, `Height`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`, `DownFlag`, `VisParentMinWidth`, `VisMinLeft` | **Supported (Expanding in M8)** | SRE / DRO | 4-frame horizontal stride (`[Normal, Hover, Pressed, Disabled]`). `DownFlag='1'` maps to checkable state. |
| **Controls** | `<Slider>` | `CmdID`, `Align`, `Width`, `Height`, `ThumbWidth`, `ThumbHeight`, `ImageLeft`, `ImageTop`, `ThumbImageLeft`, `ThumbImageTop` | **Supported** | SRE / DRO | Maps to seekbar and volume rails + draggable thumbs. |
| **Typography** | `<Label>` | `CmdID`, `Align`, `Text`, `FontName`, `FontSize`, `ColorX`, `ColorY`, `VertAlignment` | **Supported (Expanding in M8)** | SRE / DRO | `ColorX`/`ColorY` samples pixel palette from `Main.png`. `VertAlignment='1'` centers text vertically. |
| **Geometry** | `<Layout>` | `Align`, `Width`, `Height`, `VisParentMinWidth` | **Supported** | SRE / DRO | Spacer and container layout manager. |
| **Atlas** | `<ImageReplace>` | `Image`, `AlphaMode` | **Scheduled for M8.1** | SRE | Scopes child directives to override active texture (e.g. `TouchMain.png`). |
| **Atlas** | `<Image>` | `CmdID`, `Align`, `Alignment`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight` | **Scheduled for M8.1** | SRE | Static graphic badge / channel icon blitting. |
| **Interactive**| `<PlayPauseWidget>` | `Image`, `AlphaMode`, `PlayImageLeft/Top/Width/Height`, `PauseImageLeft/Top/Width/Height` | **Scheduled for M8.2** | DRO / SI | Centered play/pause fade overlay. Timing: 150ms in, 350ms hold, 250ms out. |
| **Chrome** | `<ScrollBar>` (Composite) | `<ArrowUp/Down>`, `<ThumbUp/Down/Middle>` | **Scheduled for M8.2** | SRE | Custom 3-patch vertical scrollbar thumbs and buttons from `ScrollBar.bmp`. |
| **Chrome** | `<WindowType0/1/2>` | `TopLeft`, `TopRight`, `BottomLeft`, `BottomRight`, `Left`, `Right`, `Top`, `Bottom`, `Center` | **Scheduled for M8.2** | SRE / DRO | 9-patch window chrome border slicing from `Window.png`. |
| **Menus** | `<CustomPopupMenuDef>` | `CmdID`, `Title`, `FontName`, `FontSize`, `ItemHeight` | **Scheduled for M8.3** | SRE / DRO | Clean-room popup menu template styling. |
| **Macro** | `#if / #else / #endif` | `Var == Val` conditions | **Supported** | SRE | Preprocessor conditionally branches layout on language/mode. |
| **Korean VOD** | Korean Chat / VOD modules | Broadcast UI, Chat, Kakao TV | **Excluded (Non-Goal)** | SRE | Proprietary Korean broadcast services (Out of scope per SPEC.md). |

---

## 3. Discovered Behavioral Mechanics & Invariants

### 3.1 Four-Frame Horizontal Sprite Stride
In PotPlayer `.dsf` skins, buttons do not define distinct hover/pressed coordinates in XML. Instead, the sprite atlas packs 4 distinct visual states along a horizontal row:
- `Normal Frame:` `(ImageLeft + 0 * ImageWidth, ImageTop)`
- `Hover Frame:` `(ImageLeft + 1 * ImageWidth, ImageTop)`
- `Pressed Frame:` `(ImageLeft + 2 * ImageWidth, ImageTop)`
- `Disabled Frame:` `(ImageLeft + 3 * ImageWidth, ImageTop)`

*Evidence:* Confirmed across `CMD_MEDIA_PLAY`, `ID_PROCESS_PREV`, `ID_PROCESS_NEXT`, `ID_PLAYLIST_WINDOW` in `Main.png`. Total span of button slice in spritesheet is always `4 * ImageWidth`.

### 3.2 Dynamic Palette Sampling (`ColorX` / `ColorY`)
Label text colors are resolved dynamically by reading pixel colors from the active skin atlas:
- Primary Text (`ColorX=1052, ColorY=74`): `#FFFFFF`
- Accent / Recording (`ColorX=864, ColorY=127`): `#FF3217`
- Inactive / Total Time (`ColorX=875, ColorY=528`): `#777777`

*Evidence:* Verified via PIL pixel sampling on `Main.png`. Eliminates hardcoded CSS colors in parser.

### 3.3 Responsive Visibility Thresholds (`VisParentMinWidth`, `VisMinLeft`)
- `VisParentMinWidth`: Defines container width below which a control is hidden to prevent cluttering narrow player sizes.
- `VisMinLeft`: Defines the minimum X coordinate for right-aligned controls to prevent overlapping left-aligned transport controls.

*Evidence:* Verified in `BottomVideo.txt` lines 180-240.

### 3.4 Scoped Atlas Replacement (`<ImageReplace>`)
`<ImageReplace Image='TouchMain.png'>` alters the active texture reference for all nested children, allowing touch-optimized or alternate control panels without duplicating layout XML.

---

## 4. Architectural Target for Milestone M8

All supported and upcoming directives will compile into the unified **SkinThemeModel** Intermediate Representation (IR), decoupling skin sources (PotPlayer `.dsf` vs Native Jonaki `.json`) from the single Qt `SkinRenderer`.
