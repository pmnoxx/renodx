# Current Conflicts and Issues

This document tracks conflicts and issues that need resolution in the RenoDX proxy layer.

## 🚨 Critical Issues

### 1. Fullscreen Prevention Duplication

**Status**: ⚠️ **CONFLICT DETECTED** - Needs immediate resolution

**Problem**: Display Commander has two fullscreen prevention systems:
1. **Display Commander's event handler** in `reshade_events/fullscreen_prevention.cpp`
2. **RenoDX's built-in prevention** via `renodx::mods::swapchain::prevent_full_screen`

**Impact**: 
- Potential conflicts between two prevention systems
- Confusing behavior for users
- Code duplication and maintenance overhead

**Current Behavior**:
- User toggles "Prevent Fullscreen" in Display Commander UI
- This calls `renodx::proxy::SetFullscreenPrevention(true)`
- Which sets `renodx::mods::swapchain::prevent_full_screen = true`
- RenoDX's `OnSetFullscreenState` event handler reads this variable and prevents fullscreen
- Display Commander's event handler also runs and may prevent fullscreen

**Solutions**:

#### Option A: Use RenoDX's Built-in Prevention (Recommended)
- Remove Display Commander's fullscreen prevention event handler
- Keep the proxy call to `SetFullscreenPrevention()`
- Let RenoDX handle all fullscreen prevention logic
- **Pros**: Simpler, no duplication, RenoDX is well-tested
- **Cons**: Less control over prevention behavior

#### Option B: Use Display Commander's Event Handler
- Remove the proxy call to `SetFullscreenPrevention()`
- Keep Display Commander's event handler
- RenoDX's prevention will be disabled
- **Pros**: Full control over prevention logic
- **Cons**: Duplicates RenoDX functionality, more maintenance

#### Option C: Coordinate Both Systems
- Keep both systems but ensure they don't conflict
- Display Commander sets the flag, RenoDX uses it
- **Pros**: Maintains current behavior
- **Cons**: Complex, potential for conflicts

**Recommended Action**: **Option A** - Remove Display Commander's event handler and rely on RenoDX's built-in prevention.

## 🔍 Other Issues

### 2. Template Instantiation Problems
**Status**: ⚠️ **KNOWN ISSUE** - Low priority

**Problem**: Template functions like `GetDeviceData<T>()` have issues with `void*` types.

**Impact**: Some proxy functions may not work correctly with generic types.

**Solution**: Use specific types instead of `void*` where possible, or provide type-safe alternatives.

## 📋 Resolution Checklist

- [ ] **Resolve fullscreen prevention duplication** (Critical)
  - [ ] Choose prevention approach (A, B, or C)
  - [ ] Remove redundant code
  - [ ] Test fullscreen prevention functionality
  - [ ] Update documentation
- [ ] **Fix template instantiation issues** (Low priority)
  - [ ] Review all template functions
  - [ ] Provide type-safe alternatives
  - [ ] Update documentation

## 🧪 Testing Required

After resolving conflicts:
1. Test fullscreen prevention with various applications
2. Verify no performance regression
3. Ensure settings UI works correctly
4. Test edge cases (rapid toggling, application crashes, etc.)
