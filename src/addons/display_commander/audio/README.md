# Audio Management

This subfolder contains audio management functionality for the Display Commander addon.

## Purpose

- **Process-level audio control** - Mute and volume control for the current process
- **Background audio monitoring** - Automatic audio muting when the game is in the background
- **Audio session management** - Windows audio session enumeration and control

## Files

### `audio_management.hpp`
- Header file declaring audio management functions
- Includes function declarations for muting, volume control, and background monitoring

### `audio_management.cpp`
- Implementation of audio management functionality
- Uses Windows COM interfaces for audio control
- Includes background audio monitoring thread

### `audio.hpp`
- Master header file that includes all audio functionality
- Single include point for audio features

### `audio_debug.hpp` & `audio_debug.cpp`
- Audio debugging and diagnostic functions
- Comprehensive audio session analysis
- Process-specific audio session detection
- Detailed logging for troubleshooting

## Features

### Audio Control
- **SetMuteForCurrentProcess()** - Mute/unmute the current process
- **SetVolumeForCurrentProcess()** - Set volume level (0-100%)
- **RunBackgroundAudioMonitor()** - Background thread for audio monitoring

### Audio Debugging
- **DebugAudioSessions()** - Analyze all active audio sessions
- **DebugAudioSessionForProcess()** - Debug audio sessions for specific process
- **LogAudioSessionInfo()** - Detailed logging of audio session properties

### Background Monitoring
- Monitors window focus state every 300ms
- Automatically mutes audio when game is in background
- Supports manual mute override
- Integrates with background FPS limiting

## Dependencies

- Windows COM interfaces (MMDevice, IAudioSessionManager2)
- Windows audio session APIs
- Process ID management
- Window focus detection

## Notes

- Audio functionality has been moved from the main directory to this subfolder
- Background FPS limiting is currently mixed with audio management (TODO: separate)
- COM resource management could be improved with RAII patterns
- Audio session enumeration may be expensive and could benefit from caching

## Related

- `ui_settings.cpp` - Audio UI controls
- `globals.cpp` - Audio global variables
- `main_entry.cpp` - Audio monitoring thread startup
- `todo/audio_management.md` - TODO and issues documentation
