# keyboard_hook.cpp - TODO and Issues

## Overview
This file contains keyboard hook functionality and window message logging hooks. Most hook functionality has been moved to the `hooks/` subdirectory.

## Known Issues

### 🔴 Critical Issues
None currently identified.

### 🟠 High Priority Issues
None currently identified.

### 🟡 Medium Priority Issues

#### 1. Incomplete Hook Migration
- **Status**: In Progress
- **Description**: Some hook functions still remain in this file and need to be moved to appropriate hook files
- **Impact**: Code organization and maintainability
- **Date Identified**: 2024-12-19

#### 2. CreateWindowEx Hook Implementation
- **Status**: TODO
- **Description**: The `CreateWindowExW_Hook` function is prepared but requires a proper hooking library (like Detours) for full implementation
- **Impact**: Window creation hooks are not fully functional
- **Date Identified**: 2024-12-19

### 🟢 Low Priority Issues
None currently identified.

## TODO Items

### 🔧 Immediate Tasks

#### 1. Complete Hook Migration
- [ ] Move remaining hook functions to appropriate hook files
- [ ] Update includes and dependencies
- [ ] Remove duplicate code
- [ ] Test that all functionality still works

#### 2. Implement Proper CreateWindowEx Hooking
- [ ] Research and select appropriate hooking library
- [ ] Implement proper function hooking for `CreateWindowExW`
- [ ] Test window creation interception
- [ ] Document hooking approach

### 📋 Future Improvements

#### 1. Code Organization
- [ ] Consider if this file should be renamed to `window_message_logging.cpp`
- [ ] Move remaining hook-related code to appropriate hook files
- [ ] Keep only window message logging functionality here

#### 2. Error Handling
- [ ] Improve error handling in hook installation functions
- [ ] Add more detailed logging for debugging
- [ ] Implement fallback mechanisms for failed hooks

## Technical Debt

### 1. Mixed Responsibilities
- **Issue**: File contains both keyboard hooks and window message logging
- **Impact**: Violates single responsibility principle
- **Solution**: Separate into focused modules

### 2. Incomplete Hook Implementation
- **Issue**: Some hooks are prepared but not fully implemented
- **Impact**: Reduced functionality
- **Solution**: Complete implementation or remove placeholder code

## Performance Considerations

### 1. Hook Installation
- **Issue**: Hook installation happens during initialization
- **Impact**: Potential startup delay
- **Solution**: Consider lazy loading for non-critical hooks

### 2. Message Logging
- **Issue**: Window message logging may impact performance
- **Impact**: Reduced frame rates during heavy window activity
- **Solution**: Make logging configurable and disable by default

## Code Quality Issues

### 1. File Size
- **Issue**: File is still quite large (1037 lines)
- **Impact**: Harder to maintain and understand
- **Solution**: Continue refactoring and moving code to appropriate modules

### 2. Dependencies
- **Issue**: File has many external dependencies
- **Impact**: Tight coupling, harder to test
- **Solution**: Reduce dependencies through better abstraction

## Related Files
- `hooks/alt_tab_hook.cpp` - Alt-Tab suppression functionality
- `hooks/window_creation_hook.cpp` - Window creation hooks
- `hooks/window_subclassing_hook.cpp` - Window subclassing hooks
- `hooks/borderless_style_enforcement.cpp` - Borderless style enforcement

## Notes
- This file is in transition from the old monolithic structure to the new modular hook system
- Most critical functionality has been moved to the hooks subdirectory
- Remaining code should be reviewed for further refactoring opportunities
