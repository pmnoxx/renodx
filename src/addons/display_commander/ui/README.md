# UI Management

This subfolder contains UI-related functionality for the Display Commander addon, including settings definitions, UI controls, and user interface management.

## Purpose

- **Settings Management** - Define and manage all addon settings
- **UI Controls** - Button handlers, input validation, and user interactions
- **Display Modes** - Different UI modes (Basic, Developer, Device Info, Window Info)
- **User Experience** - Tooltips, labels, and interface organization

## Files

### `ui_settings.hpp`
- Header file declaring UI settings and functions
- Settings structure definitions and helper functions

### `ui_settings.cpp`
- Implementation of all UI settings and controls
- Button handlers and user interaction logic
- Settings validation and management

### `ui.hpp`
- Master header file that includes all UI functionality
- Single include point for UI features

## Features

### Settings Management
- **Basic Mode** - Minimal features for casual users
- **Developer Mode** - Full feature set for advanced users
- **Device Info Mode** - DXGI device information display
- **Window Info Mode** - Window creation and management details

### UI Controls
- **Audio Controls** - Volume, mute, and background audio management
- **Display Controls** - Window size, position, and borderless mode
- **Reflex Controls** - NVIDIA Reflex Low Latency settings
- **Debug Controls** - Audio debugging and system information

### User Experience
- **Tooltips** - Helpful information for each setting
- **Validation** - Input validation and error handling
- **Responsiveness** - Async operations to prevent UI blocking

## Dependencies

- ReShade UI framework
- ImGui for rendering
- Windows API for system interactions
- Various addon modules for functionality

## Notes

- UI settings are the primary interface for users
- Settings are organized by functionality and user expertise level
- All settings include proper validation and error handling
- UI is designed to be responsive and user-friendly

## Related

- `globals.cpp` - Global variables for settings
- `audio/` - Audio management functionality
- `hooks/` - Hook management functionality
- `todo/ui_settings.md` - UI settings TODO and issues
