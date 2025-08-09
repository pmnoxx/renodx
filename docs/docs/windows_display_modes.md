## Windows display modes in renodx: required system calls

This guide summarizes the exact Win32/DXGI APIs to implement window sizing, borderless fullscreen, exclusive fullscreen, and desktop resolution changes in the renodx addon.

### Goals
- Toggle borderless fullscreen reliably on the active monitor
- Toggle exclusive fullscreen via DXGI when needed
- Resize/move the window in windowed mode
- Optionally change desktop resolution/refresh rate

### Prerequisites
- Access to the game/application `HWND`
- If using DXGI, access to `IDXGISwapChain` (D3D11/D3D12)

### Monitor and sizing helpers
- `HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);`
- `GetMonitorInfoW(hMon, &mi); // MONITORINFO{ rcMonitor, rcWork }`
- `GetClientRect(hWnd, &rc)` / `GetWindowRect(hWnd, &rc)`

### Borderless fullscreen (recommended)
Borderless is achieved by removing borders and sizing the window to the monitor bounds.

APIs used:
- `SetWindowLongPtrW(hWnd, GWL_STYLE, ...)` and `SetWindowLongPtrW(hWnd, GWL_EXSTYLE, ...)`
  - Remove: `WS_BORDER | WS_THICKFRAME | WS_DLGFRAME`
  - Use: `WS_POPUP | WS_VISIBLE` (keep `WS_MINIMIZEBOX` if desired)
- `AdjustWindowRectEx(...)` to compute the frame-correct outer rectangle
- `SetWindowPos(hWnd, HWND_TOP, x, y, width, height, SWP_...)` to apply sizing
- `MonitorFromWindow` + `GetMonitorInfoW` to get target monitor rectangle

Typical sequence:
```cpp
// 1) Compute monitor rect
HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
MONITORINFO mi{ sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
RECT target = mi.rcMonitor; // fullscreen borderless size

// 2) Remove borders
LONG_PTR style   = GetWindowLongPtrW(hWnd, GWL_STYLE);
LONG_PTR styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
style   &= ~(WS_BORDER | WS_THICKFRAME | WS_DLGFRAME);
style   |=  (WS_POPUP | WS_VISIBLE);
styleEx &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
SetWindowLongPtrW(hWnd, GWL_STYLE,   style);
SetWindowLongPtrW(hWnd, GWL_EXSTYLE, styleEx);

// 3) Adjust and apply
RECT wnd = target;
AdjustWindowRectEx(&wnd, (DWORD)style, FALSE, (DWORD)styleEx);
SetWindowPos(
  hWnd, HWND_TOP,
  wnd.left, wnd.top,
  wnd.right - wnd.left,
  wnd.bottom - wnd.top,
  SWP_NOZORDER | SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOACTIVATE
);
```
Notes:
- Always include `SWP_FRAMECHANGED` when changing styles.
- Consider respecting the current work-area (`mi.rcWork`) if taskbar should remain visible.

### Windowed resize/move
APIs used:
- `AdjustWindowRectEx(...)` to convert client to window size
- `SetWindowPos(...)` to apply position and dimensions

Sequence:
```cpp
RECT client{ 0, 0, desiredWidth, desiredHeight };
AdjustWindowRectEx(&client, (DWORD)style, FALSE, (DWORD)styleEx);
SetWindowPos(hWnd, nullptr, x, y, client.right - client.left, client.bottom - client.top,
             SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
```

### Exclusive fullscreen (DXGI)
Use when the app and user prefer true exclusive mode.

APIs used:
- `IDXGISwapChain::SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget)`
- Optionally `IDXGISwapChain::ResizeTarget(const DXGI_MODE_DESC* pNewTarget)`

Sequence:
```cpp
// Optional: remove borders first to avoid leftover frames in some cases
// (Same style changes as borderless section.)

// Select output (monitor) if needed; otherwise pass nullptr
ComPtr<IDXGIOutput> output = /* choose via EnumOutputs or from window monitor */;
swapChain->SetFullscreenState(TRUE, output.Get());

// Optionally ensure mode matches desired resolution/refresh
DXGI_MODE_DESC mode{}; mode.Width = w; mode.Height = h; mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM; mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; mode.RefreshRate = { numer, denom };
swapChain->ResizeTarget(&mode);
```
Notes:
- On Windows 10/11, “Fullscreen Optimizations” may alter behavior; removing borders first is a safe practice.
- After toggling exclusive mode, some apps issue a `Present(0, DXGI_PRESENT_TEST)` to prompt the driver to settle mode changes.

### Desktop resolution / refresh change (optional)
Use sparingly; prefer borderless + internal rendering resolution. Only change when absolutely necessary for the experience.

APIs used:
- `ChangeDisplaySettingsExW(LPCWSTR lpszDeviceName, DEVMODEW* lpDevMode, HWND, DWORD dwFlags, LPVOID)`
  - Test with `CDS_TEST`, then apply with `0` or `CDS_FULLSCREEN` / `CDS_UPDATEREGISTRY`
- `EnumDisplaySettingsW` to get current mode and build a `DEVMODEW`

Sequence:
```cpp
DEVMODEW dm{}; dm.dmSize = sizeof(dm);
EnumDisplaySettingsW(deviceName, ENUM_CURRENT_SETTINGS, &dm);

// Set desired resolution/refresh
dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
dm.dmPelsWidth  = w;
dm.dmPelsHeight = h;
dm.dmDisplayFrequency = hz;

if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsExW(deviceName, &dm, 0, CDS_TEST, nullptr))
{
  ChangeDisplaySettingsExW(deviceName, &dm, 0, 0 /* or CDS_FULLSCREEN */, nullptr);
}
```
Notes:
- Provide a path to revert within ~15 seconds if the user cannot confirm the change (fail-safe UI).
- On multi-monitor setups, align with the window’s monitor (`MonitorFromWindow`) and pass that device name.

### Choosing a mode
- Prefer borderless fullscreen: faster transitions, fewer device lost cases, better alt-tab.
- Use exclusive fullscreen only when required (e.g., specific latency or driver features).
- Avoid desktop mode changes unless you must match a non-native mode.

### Flags and style bits quick reference
- Remove for borderless: `WS_BORDER`, `WS_THICKFRAME`, `WS_DLGFRAME`
- Common keep: `WS_POPUP`, `WS_VISIBLE`, `WS_MINIMIZEBOX`
- Extended style trims: remove `WS_EX_CLIENTEDGE`, `WS_EX_WINDOWEDGE`
- `SetWindowPos` flags commonly used: `SWP_SHOWWINDOW`, `SWP_FRAMECHANGED`, `SWP_NOACTIVATE`, `SWP_NOZORDER`

### Error handling tips
- Verify `AdjustWindowRectEx(...)` returns TRUE before using the rect
- Validate `SetFullscreenState` return codes; fall back to borderless on failure
- Guard for device lost after exclusive toggles and handle resize events

### Minimal integration checklist
- Get `HWND` and compute target monitor rect
- Implement borderless toggle (style change + `AdjustWindowRectEx` + `SetWindowPos`
- Wire exclusive toggle via `IDXGISwapChain::SetFullscreenState`
- Optional: Desktop mode change via `ChangeDisplaySettingsExW`
- Handle alt-tab/activation and re-apply styles if the OS/app changes them


