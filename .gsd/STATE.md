---
updated: 2026-11-09T08:35:00+06:00
---

# Project State

## Current Position

**Milestone:** M1 - PotPlayer Reconnaissance & Evidence Mapping
**Phase:** Phase 1 - PotPlayer Reconnaissance
**Status:** completed
**Plan:** M1 Evidence Gathering & Reverse Engineering (Completed)

## Last Action

- Executed Milestone M1 systematic reverse engineering of installed PotPlayer.
- Measured runtime window geometry: 723x399 px main window, 751x564 px preferences dialog.
- Captured empirical screenshot evidence via Win32 PrintWindow.
- Extracted 184 RT_DIALOG resource IDs, 886 menu nodes, and XML layout directives.
- Published application-analysis.md, ui-analysis.md, configuration-analysis.md, playback-analysis.md.
- Published milestone_M1_research_report.md and milestone_M2_implementation_plan.md.

## Next Steps

1. Review M1 research deliverables and M2 implementation plan with Product Owner.
2. Begin Milestone M2 (Architecture & Playback Foundation) upon approval.

## Active Decisions

| Decision | Choice | Made | Affects |
|----------|--------|------|---------|
| TDR-001 Architecture Stack | C++20 + Qt6 + CMake + libmpv + FFmpeg | 2026-11-09 | All phases |
| TDR-002 Skin Engine | Clean-room Qt XML skin parser & spritesheet renderer | 2026-11-09 | M3, M4, M8 |
| TDR-003 Playback Decoupling | Abstract `IPlaybackEngine` interface | 2026-11-09 | M2, M5, M6 |
| TDR-004 Windows Toolchain | MSYS2 UCRT64 toolchain | 2026-11-09 | Build & test workflows |

## Blockers

*None. All tools and prerequisites installed or installing normally.*

## Concerns

- Large size of Qt 6 and libmpv packages (~2.3 GB unpacked) on MSYS2; monitor disk space and installation progress.
