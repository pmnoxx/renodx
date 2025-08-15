# Game Status Index

This file provides an overview of all documented game compatibility status for the Display Commander addon.

## Game Compatibility Overview

### 🟢 **Fully Compatible Games**
- No games currently listed as fully compatible

### 🟡 **Partially Compatible Games**

#### 1. [Heaven Burns Red (HBR)](heaven_burns_red.md)
- **Status**: Partially Compatible
- **Engine**: Unity
- **Platform**: PC (Steam)
- **Key Issues**: Audio functionality completely broken
- **Working Features**: Window management, display settings, Reflex ETW markers
- **Last Updated**: 2024-12-19

### 🔴 **Not Compatible Games**
- No games currently listed as not compatible

### ⚠️ **Untested Games**
- Most games have not been tested yet
- Status unknown for most titles

## Game Categories

### **By Engine**

#### Unity Games
- [Heaven Burns Red (HBR)](heaven_burns_red.md) - 🟡 Partially Compatible

#### Unreal Engine Games
- No games tested yet

#### Other Engines
- No games tested yet

### **By Platform**

#### PC (Steam)
- [Heaven Burns Red (HBR)](heaven_burns_red.md) - 🟡 Partially Compatible

#### PC (Other)
- No games tested yet

#### Other Platforms
- No games tested yet

## Testing Priority

### **High Priority for Testing**
1. **Popular Unity Games** - To determine if HBR issues are Unity-specific
2. **Popular Unreal Games** - To establish baseline compatibility
3. **Games with Known Audio Issues** - To test audio management improvements

### **Medium Priority for Testing**
1. **Different Game Types** - RPGs, FPS, Strategy, etc.
2. **Various Publishers** - Different anti-cheat and security measures
3. **Older Games** - To test compatibility with legacy systems

### **Low Priority for Testing**
1. **Obscure or Niche Games** - Limited user base
2. **Games with Known Compatibility Issues** - Other tools already documented issues

## How to Add New Games

1. **Create a new file** in the `game_status/` folder
2. **Use the template** from `heaven_burns_red.md`
3. **Update this index** to include the new game
4. **Test thoroughly** before marking as compatible
5. **Document all issues** found during testing

## Template for New Games

```markdown
# Game Name - Game Status

## Game Information
- **Game Name**: [Full Game Name]
- **Platform**: [Platform(s)]
- **Engine**: [Game Engine]
- **Version Tested**: [Version Number]
- **Addon Version**: Display Commander [Version]

## Compatibility Status
[🟢/🟡/🔴/⚠️] [Status Description]

## Known Issues
[Document all issues found]

## Testing Results
[Document test results for each feature]

## Workarounds
[Document any available workarounds]

## Next Steps
[Document planned actions and improvements]
```

## Notes

- Update this index when adding new game status files
- Keep compatibility status current and accurate
- Link to relevant TODO files for technical issues
- Include testing dates and version information
- Document workarounds and solutions when available

## Last Updated

- **2024-12-19**: Initial creation with HBR documentation
- **2024-12-19**: Added game status documentation system
