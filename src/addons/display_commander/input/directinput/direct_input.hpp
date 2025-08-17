#pragma once

#include <Windows.h>
#include <dinput.h>
#include <atomic>
#include <memory>
#include <vector>
#include <unordered_set>
#include <mutex>

namespace renodx::input::direct_input {

class DirectInputManager {
public:
    DirectInputManager();
    ~DirectInputManager();

    // Initialize the DirectInput manager and install hooks
    bool Initialize();
    
    // Clean up hooks and resources
    void Shutdown();

    // Global enable/disable
    void SetEnabled(bool enabled);
    bool IsEnabled() const { return m_enabled.load(); }

    // Input blocking control
    void SetBlockMouseInput(bool block);
    void SetBlockKeyboardInput(bool block);
    
    bool IsBlockingMouseInput() const { return m_block_mouse.load(); }
    bool IsBlockingKeyboardInput() const { return m_block_keyboard.load(); }

    // Device management
    void AddDevice(IDirectInputDevice8A* device, DWORD deviceType);
    void RemoveDevice(IDirectInputDevice8A* device);
    size_t GetMouseDeviceCount() const { return m_mouse_devices.size(); }
    size_t GetKeyboardDeviceCount() const { return m_keyboard_devices.size(); }
    size_t GetTotalDeviceCount() const { return m_mouse_devices.size() + m_keyboard_devices.size(); }

    // Static accessors for global input blocking state
    static bool IsBlockingAnyMouseInput();
    static bool IsBlockingAnyKeyboardInput();

private:
    // Hook functions for DirectInput APIs
    static HRESULT WINAPI HookDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

    // Device hook functions
    static HRESULT WINAPI HookIDirectInputDevice8_GetDeviceState(IDirectInputDevice8A *pDevice, DWORD cbData, LPVOID lpvData);
    static HRESULT WINAPI HookIDirectInputDevice8_GetDeviceData(IDirectInputDevice8A *pDevice, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
    static HRESULT WINAPI HookIDirectInputDevice_GetDeviceState(IDirectInputDeviceA *pDevice, DWORD cbData, LPVOID lpvData);
    static HRESULT WINAPI HookIDirectInputDevice_GetDeviceData(IDirectInputDeviceA *pDevice, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);

    // Install hooks for a DirectInput device
    void InstallDeviceHooks(IDirectInput8A *pDI, IDirectInputA *pDIOld);
    void InstallDeviceHooks(IDirectInputA *pDI, IDirectInputW *pDIW);

    // Original function pointers
    using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
    using DirectInputCreateA_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, LPDIRECTINPUTA*, LPUNKNOWN);
    using DirectInputCreateW_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, LPDIRECTINPUTW*, LPUNKNOWN);
    using DirectInputCreateEx_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

    DirectInput8Create_t m_original_DirectInput8Create = nullptr;
    DirectInputCreateA_t m_original_DirectInputCreateA = nullptr;
    DirectInputCreateW_t m_original_DirectInputCreateW = nullptr;
    DirectInputCreateEx_t m_original_DirectInputCreateEx = nullptr;

    // Global enable/disable flag
    std::atomic<bool> m_enabled{true};

    // Input blocking flags
    std::atomic<bool> m_block_mouse{false};
    std::atomic<bool> m_block_keyboard{false};

    // Device tracking
    mutable std::mutex m_devices_mutex;
    std::unordered_set<IDirectInputDevice8A*> m_mouse_devices;
    std::unordered_set<IDirectInputDevice8A*> m_keyboard_devices;

    // Global instance for static access
    static DirectInputManager* s_instance;
};

// Global instance
extern std::unique_ptr<DirectInputManager> g_directInputManager;

} // namespace renodx::input::direct_input
