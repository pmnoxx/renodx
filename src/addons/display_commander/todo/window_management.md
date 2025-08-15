# window_management.cpp - TODO and Issues

## Overview
This file handles window style changes, position management, and borderless mode enforcement. It contains the core logic for applying window style changes and managing window properties.

## Known Issues

### 🔴 Critical Issues
None currently identified.

### 🟠 High Priority Issues

#### 1. Window Top Bar Still Appearing
- **Status**: Known Issue
- **Description**: Game windows still appear with top bar despite attempts to suppress styles
- **Impact**: Borderless mode not working as intended
- **Date Identified**: 2024-12-19
- **Related**: Log shows `style_changed=0` and "Failed to get original window procedure" warnings

#### 2. Style Application Failures
- **Status**: In Progress
- **Description**: Some window style changes fail to apply properly
- **Impact**: Inconsistent borderless behavior
- **Date Identified**: 2024-12-19

### 🟡 Medium Priority Issues

#### 1. Window Creation Hook Retry Mechanism
- **Status**: Removed
- **Description**: Window creation hook retry mechanism was removed during refactoring
- **Impact**: May miss windows created after initial hook installation
- **Date Identified**: 2024-12-19
- **Solution**: Implemented in separate window creation detection hooks

#### 2. Style Change Verification
- **Status**: TODO
- **Description**: Style changes are applied but not always verified
- **Impact**: Unclear if styles were actually applied
- **Date Identified**: 2024-12-19

### 🟢 Low Priority Issues
None currently identified.

## TODO Items

### 🔧 Immediate Tasks

#### 1. Fix Top Bar Suppression
- [ ] Investigate why window styles are not being properly suppressed
- [ ] Check if window creation hooks are working correctly
- [ ] Verify style application timing
- [ ] Test with different games and window creation methods

#### 2. Improve Style Application
- [ ] Add better error handling for failed style changes
- [ ] Implement style change verification
- [ ] Add fallback mechanisms for style application
- [ ] Log detailed information about style changes

### 📋 Future Improvements

#### 1. Window Style Management
- [ ] Implement more robust style change detection
- [ ] Add support for more window styles and properties
- [ ] Implement style change history tracking
- [ ] Add style change validation

#### 2. Performance Optimization
- [ ] Optimize style change frequency
- [ ] Implement style change batching
- [ ] Add adaptive style change timing
- [ ] Consider async style application

#### 3. Error Recovery
- [ ] Implement automatic retry for failed style changes
- [ ] Add fallback style application methods
- [ ] Implement style change rollback
- [ ] Add health monitoring for style management

## Technical Debt

### 1. Style Application Complexity
- **Issue**: Style application logic is complex and may have edge cases
- **Impact**: Potential bugs and maintenance overhead
- **Solution**: Simplify and document the style application system

### 2. Error Handling
- **Issue**: Limited error handling for style application failures
- **Impact**: Difficult to debug issues
- **Solution**: Add comprehensive error handling and logging

### 3. Style Change Verification
- **Issue**: Style changes are not always verified after application
- **Impact**: Unclear if changes were successful
- **Solution**: Implement proper verification and logging

## Performance Considerations

### 1. Style Change Frequency
- **Issue**: Style changes may be applied too frequently
- **Impact**: Performance overhead and potential flickering
- **Solution**: Implement configurable change frequency and batching

### 2. Style Change Verification
- **Issue**: Verifying every style change may impact performance
- **Impact**: Reduced frame rates during heavy style changes
- **Solution**: Implement selective verification and caching

### 3. Window Enumeration
- **Issue**: Enumerating all windows may be expensive
- **Impact**: Performance impact during style enforcement
- **Solution**: Implement efficient window tracking and caching

## Code Quality Issues

### 1. Function Complexity
- **Issue**: Some functions are quite complex and handle multiple responsibilities
- **Impact**: Harder to maintain and debug
- **Solution**: Break down complex functions into smaller, focused functions

### 2. Error Logging
- **Issue**: Error logging could be more detailed and helpful
- **Impact**: Difficult to diagnose issues
- **Solution**: Improve error messages and add context information

### 3. Code Duplication
- **Issue**: Some style application logic may be duplicated
- **Impact**: Maintenance overhead and potential inconsistencies
- **Solution**: Consolidate common style application logic

## Related Files
- `hooks/borderless_style_enforcement.cpp` - Borderless style enforcement
- `hooks/window_creation_detection.cpp` - Window creation detection
- `ui_settings.cpp` - Window management UI controls

## Notes
- This file is critical for the borderless mode functionality
- Style application failures are a common source of user issues
- Consider implementing more robust style change detection and application
- Test with various games and window creation methods to ensure compatibility
