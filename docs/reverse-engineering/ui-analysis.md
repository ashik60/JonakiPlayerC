# PotPlayer User Interface & Skin Layout Analysis

> **Milestone:** M1 — PotPlayer Reconnaissance & Evidence Mapping  
> **Status:** Completed & Empirically Verified  
> **Target:** Daum PotPlayer 64-bit Default Skin (`Default.dsf`)  

---

## 1. Main Window Geometry & Frame Architecture

### 1.1 Measured Window Dimensions [DIRECTLY-OBSERVED]
Through direct runtime measurement via Win32 `GetWindowRect` and `PrintWindow`:
- **Default Window Size (Idle/Startup):** Exactly **723 x 399 pixels**.
- **Window Class Name:** `PotPlayer64`.
- **Title Bar:** Custom frameless title bar embedded within client area, height: **31 pixels** (`TopCommon.txt`).
- **Bottom Control Area:** Total height: **62 pixels** (split into two horizontal bars: Seekbar area **22px** + Buttons bar **40px**).
- **Video Viewport (Idle):** Client width: **723px**, Client height: **306px** (399 total - 31 top - 62 bottom).
- **Minimum Enforced Size:** Width: **300px**, Height: **160px**.
- **Preferences Dialog Geometry:** **751 x 564 pixels** (captured in [`evidence/potplayer_preferences_dialog.png`](file:///f:/Projects/JonakiPlayer/docs/reverse-engineering/evidence/potplayer_preferences_dialog.png)).

---

## 2. Default Skin Structure & Directive Semantics

The skin archive `Skins/Default.dsf` is a ZIP container containing 46 assets. Layout definitions are structured as declarative XML text files (`BottomVideo.txt`, `TopCommon.txt`, `Common.txt`).

### 2.1 XML Directive Syntax & Semantic Rules [DIRECTLY-OBSERVED]

| XML Tag | Attributes | Semantic Behavior |
|---|---|---|
| `<Static>` | `Align`, `Height`, `LeftMargin`, `RightMargin`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`, `IsTitleBar` | Container slice. Slices background bitmap using 3-horizontal or 9-patch border stretch. |
| `<Button>` | `Align`, `Width`, `Height`, `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`, `CmdID`, `Tooltip`, `AutoVisible`, `DownFlag` | Push button or toggle. Spritesheet contains 4 vertical or horizontal state frames (Normal, Hover, Pressed, Disabled). |
| `<Slider>` | `Align`, `CmdID`, `Width`, `Height`, `ThumbWidth`, `ThumbHeight`, `ImageLeft`, `ImageTop`, `BackImageLeft`, `BackImageTop`, `ThumbImageLeft`, `ThumbImageTop` | Interactive seekbar or volume knob. Renders rail background, filled progress rail, and draggable thumb sprite. |
| `<Label>` | `Align`, `CmdID`, `ColorX`, `ColorY`, `Width`, `FontName`, `FontSize`, `Text`, `AutoSize` | Text label displaying dynamic state (Current time, Total time, Codec name) or static text. Color coordinates index a color lookup bitmap. |
| `<Layout>` | `Align`, `Width`, `Height` | Spacing spacer widget to enforce padding between adjacent controls. |
| `<PlayPauseWidget>` | `Image`, `PlayImageLeft`, `PauseImageLeft`, `PlayImageTop`, `PauseImageTop` | Touch-screen overlay widget centered over video for large touch play/pause action. |

### 2.2 Slicing & Spritesheet Coordinates in `Main.png` [DIRECTLY-OBSERVED]

1. **Top Title Bar (`TopCommon.txt`):**
   - Background slice: `ImageLeft='3'`, `ImageTop='490'`, `ImageWidth='347'`, `ImageHeight='31'`, `Height='31'`.
   - Main Menu Button: `ImageLeft='0'`, `ImageTop='43'`, `Width='89'`, `Height='30'` (`CMD_MAIN_MENU`).
   - Window Control Buttons (Right aligned, `Height='13'`, `Width='14'`):
     - Minimize (`CMD_MIN_WINDOW`): `ImageLeft='0'`, `ImageTop='20'`.
     - Maximize (`CMD_MAX_WINDOW`): `ImageLeft='65'`, `ImageTop='20'`.
     - Restore (`CMD_RESTORE_WINDOW`): `ImageLeft='130'`, `ImageTop='20'`.
     - Close (`ID_APP_EXIT`): `ImageLeft='260'`, `ImageTop='20'`.
     - Always On Top (`ID_ETC_NEXTONTOP`): `ImageLeft='485'`, `ImageTop='68'`.

2. **Bottom Playback Controls (`BottomVideo.txt`):**
   - Play Button (`CMD_MEDIA_PLAY`): `ImageLeft='0'`, `ImageTop='259'`, `Width='41'`, `Height='40'`.
   - Pause Button (`CMD_MEDIA_PAUSE`): `ImageLeft='164'`, `ImageTop='259'`, `Width='41'`, `Height='40'`.
   - Stop Button (`CMD_MEDIA_CLOSE`): `ImageLeft='0'`, `ImageTop='300'`, `Width='41'`, `Height='40'`.
   - Previous Track (`ID_PROCESS_PREV`): `ImageLeft='164'`, `ImageTop='300'`, `Width='41'`, `Height='40'`.
   - Next Track (`ID_PROCESS_NEXT`): `ImageLeft='328'`, `ImageTop='300'`, `Width='41'`, `Height='40'`.
   - Open File (`ID_BASE_OPEN`): `ImageLeft='0'`, `ImageTop='341'`, `Width='41'`, `Height='40'`.

3. **Time Indicator Labels:**
   - Current Time (`CMD_CURRENTTIME_MESSAGE`): `Width='50'`, Font: `Tahoma`, Size: `11pt`.
   - Total Time (`CMD_TOTALTIME_MESSAGE`): `Width='50'`, Font: `Tahoma`, Size: `11pt`.

4. **Timeline Seekbar (`CMD_TIMEBAR_CTRL`):**
   - Dimensions: `Align='Client'`, `ImageHeight='10'`, `ThumbWidth='11'`, `ThumbHeight='10'`.
   - Background Rail: `ImageLeft='0'`, `ImageTop='179'`.
   - Buffered Range Bar: `BackImageLeft='0'`, `BackImageTop='193'`.
   - Progress Fill Bar: `AltImageLeft='0'`, `AltImageTop='207'`.
   - Drag Thumb Knob: `ThumbImageLeft='1'`, `ThumbImageTop='239'`.
   - A-B Repeat Markers: Point A (`148, 241`), Point B (`155, 241`).

5. **Volume Control Bar (`CMD_VOLUME_CTRL` & `ID_AUDIO_MUTE`):**
   - Mute Toggle Button: `ImageLeft='175'`, `ImageTop='167'`, `Width='13'`, `Height='10'`.
   - Volume Rail Slider: `ImageLeft='175'`, `ImageTop='194'`, `Width='70'`, `Height='10'`, Thumb: `ImageLeft='85'`, `ImageTop='239'`.

6. **Right Side Drawers & Feature Toggles:**
   - Control Box Toggle (`ID_CONTROL_BOX_WINDOW`): `ImageLeft='0'`, `ImageTop='455'`, `Width='23'`, `Height='40'`.
   - Playlist Drawer Toggle (`ID_PLAYLIST_WINDOW`): `ImageLeft='202'`, `ImageTop='455'`, `Width='23'`, `Height='40'`.

---

## 3. UI Element Classification: Skinned vs. Independent Implementation

| Element | Skin Bitmap Asset | Independent Logic Required | Implementation Strategy |
|---|---|---|---|
| Title Bar Buttons | `Main.png` sprites | Window minimize/maximize/snap logic | Paint pixmap slices onto custom Qt buttons; hook into `QMainWindow` state. |
| Play/Pause/Stop Buttons | `Main.png` sprites | State-dependent frame selection | `SkinRenderer` slices pixmap; signals drive `IPlaybackEngine`. |
| Seekbar / Timeline | `Main.png` sprites | Buffer fraction, chapter markers, time math | Custom `QSlider` subclass with custom paint event using sliced textures. |
| Volume Slider | `Main.png` sprites | Linear-to-logarithmic conversion, mute toggle | Custom horizontal slider with muted sprite override. |
| Video Viewport | N/A (Transparent client area) | Hardware-accelerated surface rendering | `QOpenGLWidget` or swapchain hosting `libmpv` render context. |
| Context Menu | N/A (Native GDI menu in PotPlayer) | Recursive action hierarchy, hotkey dispatch | Clean-room Qt `QMenu` built from `menu_hierarchy.json`. |
| Preferences Dialog | N/A (Standard Win32 Tab/Dialog controls) | Model/View settings serialization | Qt `QDialog` with `QTreeWidget` navigation and stacked settings pages. |
| Subtitle Overlay | N/A (Rendered into video surface) | Text styling, collision detection, positioning | `libass` rendered directly into `libmpv` video pipeline. |
