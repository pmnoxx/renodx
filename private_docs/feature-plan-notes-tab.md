# Feature plan: Notes tab

## Summary

Add a **Notes** tab to the RenoDX overlay (not shown by default) where the user can write and store game-specific notes in a separate file.

## Requirements

- Notes tab in the settings overlay; **Settings** tab remains the default (Notes not shown by default).
- Notes stored in a **separate file** (not in ReShade config).
- Path: `Display_Commander\Games\<GAME_NAME>\notes.txt`
- **Game name** = folder name containing the game executable.
  - Example: `D:\SteamLibrary\steamapps\common\SAMURAI MAIDEN\Game.exe` → game name = `SAMURAI MAIDEN`  
  - File: `...\common\Display_Commander\Games\SAMURAI MAIDEN\notes.txt`

## Implementation

### 1. Game notes path (platform)

- **File:** `src/utils/platform.hpp`
- Add `GetGameNotesPath(process_path)`:
  - `process_path.parent_path()` = game directory (e.g. `...\SAMURAI MAIDEN`).
  - `game_name` = `game_dir.filename().string()`.
  - Return `game_dir.parent_path() / "Display_Commander" / "Games" / game_name / "notes.txt"`.
- If `game_name` is empty, use `"Unknown"`.

### 2. Overlay tab bar (settings)

- **File:** `src/utils/settings.hpp`
- Wrap overlay content in `ImGui::BeginTabBar("RenoDXOverlayTabs")` with two tabs:
  - **Settings** (first = default): existing preset/settings UI unchanged.
  - **Notes** (second): notes UI (see below).

### 3. Notes tab UI and file I/O

- **File:** `src/utils/settings.hpp` (inside Notes tab)
- State:
  - Static buffer for note text (e.g. 256 KB).
  - `notes_loaded` so we load from file only once when Notes tab is first opened.
  - `notes_dirty` when buffer was edited (show “(unsaved)” and enable Save).
- Load:
  - On first open of Notes tab: read `GetGameNotesPath()` into buffer; create parent dirs only on first Save.
- UI:
  - `ImGui::InputTextMultiline` for editing (allow tab input).
  - “Save” button: create `Display_Commander\Games\<GAME_NAME>\` if needed, write buffer to `notes.txt`, clear `notes_dirty`.
- Dependencies: `#include "./platform.hpp"`, `<fstream>`, `<sstream>`, `<cstring>`, `<filesystem>` (via platform if needed).

### 4. Edge cases

- Missing file: treat as empty notes (no error).
- Very long file: truncate to buffer size on load (e.g. 256 KB).
- Path with no write access: Save will fail; no crash (optional: show a short message in UI if desired later).

## Files touched

| File | Change |
|------|--------|
| `src/utils/platform.hpp` | Add `GetGameNotesPath()` |
| `src/utils/settings.hpp` | Tab bar, Notes tab, load/save notes from/to file |

## Testing

- Run addon with exe in `...\SAMURAI MAIDEN\` (or any game folder).
- Open overlay → default tab is Settings.
- Switch to Notes → empty or existing `notes.txt` content.
- Edit, click Save → file appears at `...\Display_Commander\Games\SAMURAI MAIDEN\notes.txt`.
- Reopen overlay, open Notes again → same content (loaded once per session).
