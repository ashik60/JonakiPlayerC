# Skin Parser & Directive Completeness Audit (Pre-M4)

> **Date:** 2026-11-09  
> **Target:** PotPlayer Skin Directive Compatibility Register  
> **Status:** AUDITED & CATEGORIZED  

---

## 1. Supported Skin Directives (Implemented in JonakiPlayer M3)

| Directive / Feature | Supported Attributes | Implementation Status | Notes |
|---|---|---|---|
| `<Static>` | `Align`, `Height`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`, `IsTitleBar` | **Supported** | Slices background textures or manages container layouts. |
| `<Button>` | `CmdID`, `Tooltip`, `Align`, `Width`, `Height`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight` | **Supported** | Push/toggle buttons with multi-state sprite frames. |
| `<Slider>` | `CmdID`, `Align`, `Width`, `Height`, `ThumbWidth`, `ThumbHeight`, `ImageLeft`, `ImageTop`, `ThumbImageLeft`, `ThumbImageTop` | **Supported** | Seekbar and volume slider rails and thumb knobs. |
| `<Label>` | `CmdID`, `Align`, `Text`, `FontName`, `FontSize` | **Supported** | Static and dynamic text indicators (e.g. current/total time). |
| `<Layout>` | `Align`, `Width` | **Supported** | Horizontal and vertical padding spacers. |
| Macro Preprocessor | `#if<Var==Val>`, `#else`, `#endif` | **Supported** | Conditional layout compilation based on code page and skin mode. |

---

## 2. Unsupported / Unresolved Skin Directives & Behaviors [RECORDED AS UNRESOLVED]

The following skin directives and attributes exist in reference PotPlayer `.dsf` skins but are **NOT** currently implemented in JonakiPlayer. They are recorded here as unresolved and scheduled for future skin engine expansion:

| Directive / Attribute | Context in Reference Skins | Current Handling | Target Milestone |
|---|---|---|---|
| `<PlayPauseWidget>` | Centered touch screen play/pause overlay in `TouchMain.png` | **Ignored / Unresolved** | M8 (Touch & Gestures) |
| `ColorX`, `ColorY` | Label color indexing into a bitmap palette (`Main.png`) | **Approximated via QColor / CSS** | M8 |
| `Alignment='5'` | Win32 GDI control center-vertical alignment flag | **Approximated via Qt Layouts** | M8 |
| `DownFlag='1'` | Sticky toggle button state mapping | **Approximated via QAbstractButton::isCheckable** | M4/M8 |
| `UserMenuName` | Context popup menu binding on button click (e.g. `CastPopupMenu`) | **Unresolved** | M7 |
| `VisParentMinWidth` | Dynamic responsive element hiding when window is below min width | **Unresolved** | M8 |
| `AltImageLeft/Top` | Alternate spritesheet frame for active/playing state | **Unresolved (handled in C++ state)** | M8 |
| `RepeatAImageLeft/Top` | A-B repeat loop start/end marker sprites on seekbar | **Unresolved** | M7 |
| `BookmarkImageLeft/Top`| Seekbar bookmark point flags | **Unresolved** | M7 |
| 9-patch border stretch | Skin background stretching across both X and Y axes | **Approximated via 3-slice horizontal** | M8 |

---

## 3. Parity Classification Matrix (Geometry vs. Visual vs. Behavioral)

To ensure strict engineering integrity, feature parity is decoupled into three distinct tiers:

1. **Geometric Parity:** Exact layout dimensions, widget bounds, hit-test borders, and pixel heights.
   - Main Window: **723 x 399 px** (100% Full Parity 🟢)
   - Title Bar: **31 px** (100% Full Parity 🟢)
   - Bottom Control Bar: **62 px** (22px seekbar + 40px buttons) (100% Full Parity 🟢)
   - Minimum Window Bounds: **300 x 160 px** (100% Full Parity 🟢)

2. **Visual Parity:** Iconography, color palettes, typography, and texture rendering.
   - Palette & Gradients: **Near Parity 🟡** (Independently rendered charcoal and amber theme; exact reference texture assets excluded to comply with clean-room non-redistribution requirements).
   - Dynamic Layout Spacing: **Partial Parity 🟡** (Controlled via clean-room Qt layouts matching observed coordinates).

3. **Behavioral Parity:** Playback synchronization, shortcuts, event loops, and window dragging.
   - Frameless Aero Snap & Dragging: **Full Parity 🟢** (Win32 `WM_NCHITTEST`).
   - Playback Controls & Keybindings: **Full Parity 🟢** (Space, F, D, Left/Right, Up/Down, Mute).
   - Playlist Operations (`.dpl`, recursive queue, resume store): **In Progress for M4 🟡**.
