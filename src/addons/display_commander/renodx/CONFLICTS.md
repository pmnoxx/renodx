# Current Conflicts and Issues

This document tracks conflicts and issues that need resolution in the RenoDX proxy layer.

## 🚨 Critical Issues

### 1. Fullscreen Prevention Implementation

**Status**: ✅ **RESOLVED** - No conflict, just need to avoid setting RenoDX variable

**Solution**: 
- Keep Display Commander's fullscreen prevention event handler
- **Do NOT set** `renodx::mods::swapchain::prevent_full_screen` variable
- Let both systems work independently without interference

**Why This Works**:
- Display Commander's event handler prevents fullscreen through ReShade events
- RenoDX's system remains in its default state (doesn't interfere)
- No conflicts, no duplication, both systems can coexist

## 🔍 Other Issues

### 2. Template Instantiation Problems
**Status**: ⚠️ **KNOWN ISSUE** - Low priority

**Problem**: Template functions like `GetDeviceData<T>()` have issues with `void*` types.

**Impact**: Some proxy functions may not work correctly with generic types.

**Solution**: Use specific types instead of `void*` where possible, or provide type-safe alternatives.

### 3. Suppress Top Bar/Border Messages Feature
**Status**: 🚫 **DISABLED BY DEFAULT** - Medium priority

**Problem**: The "Suppress Top Bar/Border Messages" feature in the UI has bugs and is not fully functional.

**Impact**: Users cannot access this feature until the bugs are fixed.

**Solution**: Fix the bugs and re-enable the feature in the UI.

## 📋 Resolution Checklist

- [x] **Resolve fullscreen prevention duplication** (Critical)
  - [x] Choose prevention approach (Display Commander event handler)
  - [x] Avoid setting RenoDX variable to prevent conflicts
  - [ ] Test fullscreen prevention functionality
  - [x] Update documentation
- [ ] **Fix template instantiation issues** (Low priority)
  - [ ] Review all template functions
  - [ ] Provide type-safe alternatives
  - [ ] Update documentation
- [ ] **Complete Suppress Top Bar/Border Messages feature** (Medium priority)
  - [ ] Fix bugs in the feature
  - [ ] Test the feature thoroughly
  - [ ] Re-enable the feature in the UI
  - [ ] Remove "Buggy - Disabled" warning from UI

## 🧪 Testing Required

After resolving conflicts:
1. Test fullscreen prevention with various applications
2. Verify no performance regression
3. Ensure settings UI works correctly
4. Test edge cases (rapid toggling, application crashes, etc.)
