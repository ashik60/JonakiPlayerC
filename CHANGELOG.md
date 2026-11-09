# CHANGELOG.md

All notable changes to **JonakiPlayer** will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased] - 2026-11-09

### Added
- **Project Structure:** Established clean-room directory hierarchy for core, playback, ui, audio, video, subtitle, settings, input, platform abstractions, tests, and documentation.
- **Reverse Engineering Integration:**
  - Installed and configured `rea@6.8.0` with Antigravity MCP integration in `~/.gemini/config/mcp_config.json`.
  - Registered the REA reverse-engineering skill and 229 native analysis tools.
- **GSD (Get Shit Done) Framework:**
  - Imported and validated GSD v1.6.0 workflows (27 workflows), subagents (5 subagents), and skills (12 skills).
  - Configured canonical rules in `PROJECT_RULES.md` and `GSD-STYLE.md`.
- **Environment & Reference Reconnaissance:**
  - Discovered and verified local Daum PotPlayer 64-bit installation at `C:\Program Files\DAUM\PotPlayer`.
  - Analyzed and verified `Skins/Default.dsf` as a standard ZIP archive containing complete UI layouts, XML definitions (`BottomVideo.txt`, `AudioSkin.xml`), and sprite coordinates (`Main.png`).
  - Cataloged `Language/English.ini` and `Menus/SimpleMenu.xml` containing PotPlayer internal command IDs and menus.
- **Documentation & Memory System:**
  - Created `AGENTS.md`, `README.md`, `PROJECT_CONTEXT.md`, `ROADMAP.md`, and persistent memory registers in `docs/memory/`.
