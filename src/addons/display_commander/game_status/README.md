# Game Status Documentation

This folder contains documentation for how the Display Commander addon works with specific games, including known issues, compatibility notes, and testing results.

## Purpose

- **Track game-specific compatibility** and known issues
- **Document testing results** for different games
- **Provide troubleshooting guidance** for users
- **Maintain compatibility database** across game updates

## File Structure

Each game has its own `.md` file documenting:
- Game information and version tested
- Compatibility status
- Known issues and workarounds
- Testing results and notes
- Configuration recommendations

## How to Use

1. **When testing a new game**: Create a new game status file
2. **When issues are found**: Document them in the appropriate game file
3. **When testing fixes**: Update the status and remove resolved issues
4. **When helping users**: Reference game status files for troubleshooting

## File Naming Convention

- `game_name.md` - Use the game's common name
- Include version information in the file
- Use clear, descriptive issue titles
- Include severity levels (Critical, High, Medium, Low)
- Add timestamps for when issues were identified

## Game Status Categories

### 🟢 **Fully Compatible**
- All features work as expected
- No known issues
- Ready for production use

### 🟡 **Partially Compatible**
- Most features work
- Some known issues or limitations
- May require workarounds

### 🔴 **Not Compatible**
- Major issues prevent normal operation
- Core functionality broken
- Not recommended for use

### ⚠️ **Untested**
- No testing performed yet
- Status unknown
- Use at your own risk

## Notes

- Update game status files when testing new versions
- Link to relevant TODO files for technical issues
- Include user reports and community feedback
- Document workarounds and solutions when available
