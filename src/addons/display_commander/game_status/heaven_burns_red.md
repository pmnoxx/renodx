# Heaven Burns Red (HBR) - Game Status

## Game Information

- **Game Name**: Heaven Burns Red (ヘブンバーンズレッド)
- **Platform**: PC (Steam)
- **Engine**: Unity
- **Version Tested**: Latest (as of 2024-12-19)
- **Addon Version**: Display Commander (latest)

## Compatibility Status

🟡 **Partially Compatible** - Core functionality works but audio issues present

## Known Issues

### 🔴 **Critical Issues**

#### 1. Audio Not Audible Despite 100% Volume
- **Status**: Confirmed Issue
- **Description**: Game audio cannot be heard even when volume is set to 100% in Display Commander
- **Impact**: Users cannot hear game audio, affecting gameplay experience
- **Date Identified**: 2024-12-19
- **Related**: Audio management, volume control

#### 2. Audio Muting Not Working
- **Status**: Confirmed Issue
- **Description**: Audio muting functionality does not work properly in HBR
- **Impact**: Background audio muting and manual mute controls ineffective
- **Date Identified**: 2024-12-19
- **Related**: Audio management, background monitoring

### 🟠 **High Priority Issues**

#### 1. Audio Session Detection
- **Status**: Investigation Required
- **Description**: Display Commander may not be properly detecting HBR's audio sessions
- **Impact**: Audio controls cannot target the correct process
- **Date Identified**: 2024-12-19
- **Related**: Windows audio session management

### 🟡 **Medium Priority Issues**

None currently identified.

### 🟢 **Low Priority Issues**

None currently identified.

## Testing Results

### Audio Functionality Tests

| Feature | Status | Notes |
|---------|--------|-------|
| Volume Control | ❌ Broken | Volume set to 100% but no audio heard |
| Manual Mute | ❌ Broken | Mute toggle has no effect |
| Background Mute | ❌ Broken | Background muting not working |
| Audio Session Detection | ❌ Failed | Cannot detect HBR audio sessions |

### Other Functionality Tests

| Feature | Status | Notes |
|---------|--------|-------|
| Window Management | ✅ Working | Borderless mode and window controls work |
| Reflex Integration | ⚠️ Partial | ETW markers working but features marked as non-functional |
| Display Settings | ✅ Working | Resolution and display controls functional |

## Investigation Notes

### Audio Issue Analysis

1. **Process ID Detection**: HBR may be using a different process structure for audio
2. **Audio Session Creation**: Unity games may create audio sessions differently
3. **COM Interface Access**: HBR may be blocking or interfering with audio session enumeration
4. **Audio Device Selection**: Game may be using non-standard audio endpoints

### Potential Causes

1. **Unity Audio System**: HBR uses Unity's audio system which may have different behavior
2. **Anti-Cheat**: Game may have anti-cheat measures blocking audio session access
3. **Audio Driver**: Specific audio driver compatibility issues
4. **Process Isolation**: HBR may be running audio in a separate process

## Workarounds

### Current Workarounds

1. **Use System Volume**: Control audio through Windows system volume instead of addon
2. **Game Audio Settings**: Adjust audio settings within HBR's own menu
3. **Audio Device Selection**: Try different audio output devices

### Recommended Actions

1. **Disable Audio Features**: Turn off audio management in Display Commander for HBR
2. **Use Alternative Methods**: Consider other audio management tools for this game
3. **Report to Developers**: Contact HBR developers about audio session access

### Debugging Tools

1. **Audio Debug Button**: Use the "Debug Audio Sessions" button in Display Commander's Audio section
2. **Process Analysis**: The debug tool will show all audio sessions and identify HBR's process
3. **Session Information**: Detailed logging of audio session states, names, and properties

## Configuration Recommendations

### Display Commander Settings

```
Audio Volume: 100% (note: not working)
Audio Mute: Off (note: not working)
Mute in Background: Off (note: not working)
```

### System Settings

- Use Windows volume mixer for HBR
- Check HBR's in-game audio settings
- Verify audio device selection in Windows

## Related Issues

- **TODO**: `audio_management.md` - Audio session management improvements
- **TODO**: `hooks.md` - Process detection and hooking improvements
- **General**: Unity engine compatibility with Windows audio APIs

## Next Steps

### Immediate Actions

1. **Disable Audio Features**: Turn off audio management for HBR to prevent confusion
2. **Investigate Process Structure**: Analyze HBR's process tree for audio sessions
3. **Test with Other Unity Games**: Compare behavior across different Unity titles

### Long-term Solutions

1. **Unity Audio Support**: Add specific support for Unity audio systems
2. **Process Detection**: Improve process identification for games with complex audio structures
3. **Fallback Mechanisms**: Implement fallback audio control methods

## Notes

- HBR is a Unity-based game which may have different audio behavior than other games
- Audio issues appear to be related to process detection rather than addon functionality
- Consider adding Unity-specific audio handling in future versions
- Test with other Unity games to determine if this is a Unity-specific issue

## Updates

- **2024-12-19**: Initial issue identification and documentation
- **2024-12-19**: Created game status file and investigation notes
