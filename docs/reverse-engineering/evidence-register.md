# Reverse Engineering Evidence Register

This register logs every empirical investigation conducted on reference PotPlayer installations.

---

## Evidence Index

| Investigation ID | Date | Target Component | Method | Finding Summary | Confidence | Recommendation |
|---|---|---|---|---|---|---|
| **REV-001** | 2026-11-09 | `C:\Program Files\DAUM\PotPlayer` | Static Filesystem Inspection | Full 64-bit installation verified. Found `PotPlayerMini64.exe`, `PotPlayer64.dll`, `ffcodec64.dll`, `MediaDB64.dll`, `ATextOut64.dll`, `Skins/`, `Menus/`, `Language/`. | 100% | Use local files as authoritative reference source. |
| **REV-002** | 2026-11-09 | `Skins/Default.dsf` | Binary & Archive Extraction | `.dsf` format is a standard ZIP archive containing XML layout files (`AudioSkin.xml`, `BottomVideo.txt`, `ChildSkin.xml`) and texture atlas PNGs (`Main.png`, `Control.png`). | 100% | Build custom clean-room `SkinRenderer` in Qt to parse layout directives directly. |
| **REV-003** | 2026-11-09 | `Menus/SimpleMenu.xml` | XML Analysis | Discovered PotPlayer menu hierarchy and exact command ID taxonomy (`ID_BASE_OPEN`, `ID_PLAY_PAUSE`, `ID_FULLSCREEN`, `CMD_POPUPMENU_PLAY`, `ID_OPTION_WINDOW`). | 100% | Adopt command ID taxonomy in JonakiPlayer action map for complete hotkey/menu parity. |
| **REV-004** | 2026-11-09 | `Language/English.ini` | String Table Analysis | 325 KB UTF-16/UTF-8 INI file containing thousands of localized string keys, menu items, settings dialog labels, tooltips, and shortcut descriptions. | 100% | Parse into clean-room string dictionary and translation mapping. |
| **REV-005** | 2026-11-09 | `C:\Users\User\AppData\Roaming\PotPlayerMini64` | User Profile Inspection | Configuration stored in INI files, playlist persistence in `Playlist/`, history database in `MediaDB.sdb`. | 100% | Model JonakiPlayer settings on INI hierarchy (`SettingsManager`). |

---

## Investigation Detail Records

### REV-002: Skin Format (`Default.dsf`)
- **Target:** `C:\Program Files\DAUM\PotPlayer\Skins\Default.dsf`
- **Method:** Inspected byte header (`50 4B 03 04` PK zip signature), unpacked with Python `zipfile`.
- **Findings:**
  - Contains 46 files.
  - Key layout definition: `BottomVideo.txt` specifies `<Static Align='Bottom' Height='40'>` and buttons with `ImageLeft`, `ImageTop`, `ImageWidth`, `ImageHeight`.
  - Spritesheet `Main.png` contains normal, hover, pressed, and disabled states stacked horizontally/vertically.
- **Implementation Strategy:** Implement an XML skin parser that generates a Qt `QWidget` layout tree and paints custom buttons using `QPainter::drawPixmap` with the sprite coordinates.
