# Next Actions Register

This file maintains the immediate, ordered implementation tasks for active sessions.

---

## Active Milestone: M8 — Skin Engine Fidelity & Complete Theming

### Planning & Reconnaissance (Completed)
1. [x] **M7 Independent Audit Correction (Part A):**
   - Re-examined source code, tests, and runtime evidence across 7 areas.
   - Identified and documented defects in Subtitle checkbox wiring, playlist URL history recording, missing AV1 HWDEC empirical separation, missing A-B wrap tests, and FrameCapture detached thread safety.
   - Published `docs/milestones/milestone-reports/milestone_M7_audit_correction_report.md` and updated `milestone_M7_completion_report.md`.
2. [x] **Reference Skin Reverse-Engineering Validation (Part B):**
   - Conducted controlled tests on PotPlayer skins (`Default.dsf`, `WindowFrame.dsf`).
   - Discovered and verified 4-frame button stride, `ColorX`/`ColorY` palette sampling, `Alignment='5'` Win32 centering, `VisParentMinWidth`/`VisMinLeft` responsive hiding, `ImageReplace` scoping, and composite scrollbars.
   - Generated machine-readable inventory `docs/reverse-engineering/skin_directive_inventory.json` (1,714 records across 21 files) and summary `docs/reverse-engineering/skin_directive_inventory.md`.
   - Updated `docs/reverse-engineering/skin-compatibility-audit.md`.
3. [x] **Common Theme Intermediate Representation Architecture (Part C & D):**
   - Designed platform-neutral `SkinThemeModel` IR unifying PotPlayer DSF/XML and Native JSON.
   - Enforced single Qt `SkinRenderer` pipeline.
   - Established security specifications: Zip Slip protection, Zip Bomb limits, XXE-safe XML parsing, and malformed theme default fallback.
   - Audited reference assets; enforced strict exclusion from distribution packaging.
   - Published `docs/architecture/m8_skin_and_theme_architecture.md`.
4. [x] **Visual Regression Architecture & Plan Refinement (Part E & F):**
   - Distinguishes logical Qt bounds from physical screenshot pixels across 100%, 125%, 150% DPI.
   - Defined multi-resolution geometry assertions and tolerant component-level SSIM visual comparison.
   - Published `docs/milestones/milestone-reports/milestone_M8_implementation_plan.md` across Waves 8.1–8.4 with empirical acceptance criteria.

---

### Implementation Waves (Pre-Implementation Lock Active)

**Do not begin implementation until user review and approval.**

1. [ ] **Wave 8.1 — Parser Expansion & Palette Sampling:**
   - Secure DSF archive extractor (`DsfArchiveExtractor`) with Zip Slip and bomb guards.
   - XXE-safe XML parser expansion (`<ImageReplace>`, `<Image>`, `<SubImage>`, `Alignment='5'`, `DownFlag='1'`).
   - Dynamic palette sampler (`ColorX`/`ColorY` lookup into `Main.png`).
   - Responsive visibility rule evaluator (`VisParentMinWidth`, `VisMinLeft`).
2. [ ] **Wave 8.2 — Nine-Patch Rendering & Multi-State Sprites:**
   - 4-frame button state stride extraction (`[Normal, Hover, Pressed, Disabled]`).
   - 3-patch horizontal/vertical border slicing.
   - 9-patch window chrome slicing (`WindowType0/1/2`, `Window.png`).
   - ScrollBar composite component rendering (`ScrollBar.bmp`).
   - `PlayPauseWidget` centered animation overlay.
3. [ ] **Wave 8.3 — Native Theme Architecture & Common IR:**
   - Formalize `SkinThemeModel` IR data structures.
   - Implement `PotPlayerSkinCompiler` and `NativeThemeLoader` (JSON schema).
   - Unify painting under single `SkinRenderer`.
   - Malformed theme containment and default fallback.
4. [ ] **Wave 8.4 — Visual Regression & High-DPI Validation:**
   - High-DPI scaling validation (DPR 1.0, 1.25, 1.50).
   - Multi-resolution geometry test suite (723×399, 800×450, 1024×576, 1366×768, Fullscreen).
   - Component-level SSIM visual regression test.
   - Full parity matrix sign-off.
