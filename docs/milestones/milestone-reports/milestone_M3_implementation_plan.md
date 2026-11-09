# Milestone M3 Implementation Plan: Skin Engine & UI Reproduction

> **Milestone:** M3 — Main Interface Reconstruction  
> **Status:** SPEC & ARCHITECTURE LOCKED  
> **Target:** 100% faithful PotPlayer default skin rendering & frameless window  
> **Framework:** GSD v1.6.0  

---

## 1. Objectives & Non-Negotiables

1. **Exact 723 x 399 Geometry:** Default startup client geometry is exactly **723 x 399 pixels**, with a 31px custom title bar and 62px bottom control area (22px seekbar + 40px buttons bar).
2. **Clean-Room Spritesheet Atlas:** Autonomously generate clean-room vector/raster textures at the exact coordinates extracted during M1 reconnaissance (`resources/skins/default/Main.png`), avoiding bundling proprietary Daum/Kakao binaries or assets.
3. **Clean-Room XML Parser:** Implement a resilient XML parser capable of interpreting PotPlayer skin directives (`<Static>`, `<Button>`, `<Slider>`, `<Label>`, `<Layout>`) and macro guards (`#if`, `#else`, `#endif`).
4. **Decoupled Architecture:** Keep skin rendering, window frame management, and playback engine completely isolated via clear interfaces.
5. **Aero Snap & Frameless Window:** Frameless window on Windows supporting standard resize hit-testing (`WM_NCHITTEST`), aero snap, drag-to-move, maximize/restore, and minimize.
6. **Context Menu:** Dynamic Qt context menu populated faithfully from `menu_hierarchy.json`.

---

## 2. Work Breakdown & Waves

### Wave 1: Skin Parser & Clean-Room Atlas Generation
- [ ] Task 1.1: Develop procedural atlas generator generating pixel-faithful clean-room icons at exact coordinates.
- [ ] Task 1.2: Implement `SkinParser` supporting XML directives and conditional blocks.
- [ ] Task 1.3: Unit test `SkinParser` validating directive resolution and bounding boxes.

### Wave 2: Frameless Window & Aero Snap (Windows / Linux)
- [ ] Task 2.1: Implement `FramelessWindowHelper` (Win32 `WM_NCHITTEST` / `WM_NCCALCSIZE` / Qt native events).
- [ ] Task 2.2: Implement custom title bar with menu button, title label, and system buttons (min/max/restore/close/pin).
- [ ] Task 2.3: Enforce minimum size (300x160) and default size (723x399).

### Wave 3: Skinned Controls & Bottom Playback Bar
- [ ] Task 3.1: Implement `SkinnedButton` using sliced sprites (Normal, Hover, Pressed, Disabled).
- [ ] Task 3.2: Implement `SkinnedSlider` with sliced background, buffered progress, active fill, and thumb knob.
- [ ] Task 3.3: Assemble 62px bottom control bar (22px seekbar row + 40px button row).

### Wave 4: Context Menu & Playback Wiring
- [ ] Task 4.1: Load `menu_hierarchy.json` into hierarchical Qt `QMenu` dispatched to `ActionManager`.
- [ ] Task 4.2: Wire all skinned buttons, seekbar, and volume slider to `IPlaybackEngine`.

### Wave 5: Verification & Parity Audit
- [ ] Task 5.1: Run full automated regression suite (`ctest`).
- [ ] Task 5.2: Launch application, capture screenshot at 723x399, and document visual delta against reference screenshot.
