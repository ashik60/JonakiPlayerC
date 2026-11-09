# Reverse Engineering Evidence Register

This register logs every empirical investigation conducted on reference PotPlayer installations.

---

## Evidence Index

| Investigation ID | Date | Target Component | Method | Finding Summary | Classification | Recommendation |
|---|---|---|---|---|---|---|
| **REV-001** | 2026-11-09 | `C:\Program Files\DAUM\PotPlayer` | Filesystem & PE Inspection | Full 64-bit installation verified. Found `PotPlayerMini64.exe`, `PotPlayer64.dll`, `ffcodec64.dll`, `MediaDB64.dll`, `ATextOut64.dll`. | [DIRECTLY-OBSERVED] | Use local installation as authoritative truth. |
| **REV-002** | 2026-11-09 | `Skins/Default.dsf` | Archive Extraction & XML Analysis | Standard ZIP containing 46 assets; layout specified via XML (`BottomVideo.txt`, `TopCommon.txt`, `Common.txt`). | [DIRECTLY-OBSERVED] | Parse XML layout tree and map sprite slices to Qt widgets. |
| **REV-003** | 2026-11-09 | `Menus/SimpleMenu.xml` | XML Analysis | Command ID hierarchy identified (`ID_BASE_OPEN`, `ID_PLAY_PAUSE`, `ID_FULLSCREEN`, `CMD_POPUPMENU_PLAY`). | [DIRECTLY-OBSERVED] | Use command taxonomy in action dispatcher. |
| **REV-004** | 2026-11-09 | `Language/English.ini` | String Table Analysis | 886 hierarchical menu nodes (`[MenuString]`), 167 `[Dialog(N)]` blocks, and filter strings cataloged. | [DIRECTLY-OBSERVED] | Extract into `menu_hierarchy.json` for 100% menu parity. |
| **REV-005** | 2026-11-09 | `HKCU\Software\Daum\PotPlayerMini64` | Registry & Storage Inspection | Settings stored under `Settings` key (`PlaybackMode`, `OnTopMode`, `RememberPosition`, `CaptionStyle`). | [DIRECTLY-OBSERVED] | Replicate settings keys and defaults in `SettingsManager`. |
| **REV-006** | 2026-11-09 | `PotPlayer64.dll` | PE Export Table Analysis | Discovered 43 exported functions (`CreatePotPlayerW`, `SendPotPlayerCmd`, `SetPotPlayerSeek`). | [DIRECTLY-OBSERVED] | Model application architecture around these core responsibilities. |
| **REV-007** | 2026-11-09 | `PotPlayer64.dll` | PE Resource Section (`RT_DIALOG`) | Extracted 184 native dialog IDs corresponding to preferences, file open, and stream dialogs. | [DIRECTLY-OBSERVED] | Map dialog IDs to settings pages in preferences dialog. |
| **REV-008** | 2026-11-09 | Runtime Process Window | Win32 API (`GetWindowRect`, `PrintWindow`) | Default window measured at exactly **723 x 399 px**; Preferences dialog at **751 x 564 px**. Captured screenshots in `docs/reverse-engineering/evidence/`. | [DIRECTLY-OBSERVED] | Adopt 723x399 default window and 751x564 preferences dialog dimensions. |
| **REV-009** | 2026-11-09 | `Playlist/PotPlayerMini64.dpl` | Text Format Analysis | Schema: `DAUMPLAYLIST` header, `playname`, `playtime`, `N*file*`, `N*duration2*`, `N*start*`. | [DIRECTLY-OBSERVED] | Implement clean-room `.dpl` playlist parser & serializer. |
| **REV-010** | 2026-11-09 | `CmdLine64.txt` | Command Line Spec Inspection | Documented CLI switches: `/filedlg`, `/urldlg`, `/autoplay`, `/seek=`, `/sub=`, `/aud=`, `stdin (-)`. | [DIRECTLY-OBSERVED] | Match command line arguments in `main.cpp`. |
| **REV-011** | 2026-11-09 | `BottomVideo.txt` | Sprite Coordinate Mapping | Play (`0, 259`), Pause (`164, 259`), Stop (`0, 300`), Seekbar rail (`0, 179`), Volume (`175, 194`). | [DIRECTLY-OBSERVED] | Bind coordinates in `SkinRenderer`. |
| **REV-012** | 2026-11-09 | MediaDB64.dll & MediaInfo.sdb | Playback History Persistence | Binary database tracking file hash, last seek offset, audio/subtitle track selection. | [INFERRED] | Model via embedded SQLite / structured JSON store. |
| **REV-013** | 2026-11-09 | Audio/Video Filters | Registry & DLL String Inspection | Equalizer has 10 bands (31Hz–16kHz); Normalizer uses dynamic audio normalizer (`dynaudnorm`). | [DIRECTLY-OBSERVED] | Map directly to `libmpv` filters (`af=equalizer`, `af=dynaudnorm`). |
| **REV-014** | 2026-11-09 | Window State & Border Snapping | Runtime Window Observation | Non-client aero snap handled via `WM_NCHITTEST` returning `HTTOP`, `HTLEFT`, `HTCAPTION`. | [INFERRED] | Implement in `src/platform/windows/WindowSnapping.cpp`. |
| **REV-015** | 2026-11-09 | PotPlayer Proprietary Cloud / Broadcast | Registry & String Inspection | Korean broadcast live chat, Kakao TV streaming, and advertising modules. | [UNRESOLVED] | Explicitly out of scope (Non-Goal in SPEC.md). |
