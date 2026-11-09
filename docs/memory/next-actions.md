# Next Actions Register

This file maintains the immediate, ordered implementation tasks for active sessions.

---

## Immediate Next Steps (M0 Completion)

1. [ ] **Verify Toolchain Installation:** Ensure `g++ -v`, `cmake --version`, `ninja --version`, `qmake6 -v`, and `libmpv` headers/libs exist in `C:\msys64\ucrt64`.
2. [ ] **Setup Build Environment Wrapper / PATH:** Configure shell scripts to execute builds using `C:\msys64\ucrt64\bin`.
3. [ ] **Verify GSD SPEC Planning Lock:** Finalize `.gsd/SPEC.md` (`Status: FINALIZED`) incorporating the complete product specification.
4. [ ] **Git Initial Commit for M0:** Add all foundation files, documentation, memory logs, and GSD templates.
5. [ ] **Produce M0 Completion Report:** Generate `docs/milestones/milestone-reports/milestone_M0_completion_report.md`.

---

## Phase M1 Actions (Reconnaissance & Parity Matrix)

1. [ ] **Catalog PotPlayer Command IDs:** Extract all `CMD_*` and `ID_*` commands from `Language/English.ini` and `Menus/SimpleMenu.xml`.
2. [ ] **Extract Reference Skin Assets:** Unpack `Default.dsf` textures into `resources/reference/skin_assets/` (internal reference only for dimensions and slices).
3. [ ] **Construct Feature Parity Matrix:** Populate `docs/feature-parity/feature-matrix.md` with every verified category (Playback, Playlist, Subtitles, Audio, Video, Settings, Shortcuts).
4. [ ] **Record Reverse Engineering Findings:** Update `docs/reverse-engineering/evidence-register.md`.
