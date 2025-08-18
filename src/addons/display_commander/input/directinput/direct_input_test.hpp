#pragma once

#include <Windows.h>
#include <dinput.h>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <thread>

namespace renodx::input::direct_input::test {

class DirectInputTester {
public:
    DirectInputTester();
    ~DirectInputTester();

    // Initialize the tester and install hooks
    bool Initialize();
    
    // Clean up hooks and resources
    void Shutdown();

    // Test functionality
    void RunMouseTest();
    void RunKeyboardTest();
    void RunInputBlockingTest();
    
    // Status and results
    bool IsTestRunning() const { return m_test_running.load(); }
    const std::string& GetLastTestResult() const { return m_last_test_result; }
    
    // Test configuration
    void SetTestDuration(int milliseconds) { m_test_duration_ms = milliseconds; }
    int GetTestDuration() const { return m_test_duration_ms; }
    
    // Device management
    void TrackDevice(IDirectInputDevice8* device);
    void UntrackDevice(IDirectInputDevice8* device);
    size_t GetTrackedDeviceCount() const { return m_tracked_devices.size(); }
    
    // Hook status queries
    bool IsDirectInput8CreateHookActive() const { return m_original_DirectInput8Create != nullptr; }
    bool IsDirectInputCreateAHookActive() const { return m_original_DirectInputCreateA != nullptr; }
    bool IsDirectInputCreateWHookActive() const { return m_original_DirectInputCreateW != nullptr; }
    bool IsDirectInputCreateExHookActive() const { return m_original_DirectInputCreateEx != nullptr; }
    
    // Manual retry trigger
    bool RetryLoadModules() { return TryLoadDirectInputModules(); }
    
    // Thread status
    bool IsRetryThreadRunning() const { return m_retry_thread_running.load(); }

private:
    // Background retry thread management
    void StartRetryThread();
    void StopRetryThread();

    // Hook functions for DirectInput APIs
    static HRESULT WINAPI HookDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, LPUNKNOWN punkOuter);
    static HRESULT WINAPI HookDirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
    
    // Device-level hook functions
    static HRESULT WINAPI HookCreateDevice(IDirectInput8* This, REFGUID rguid, LPDIRECTINPUTDEVICE *ppDevice, LPUNKNOWN pUnkOuter);
    static HRESULT WINAPI HookSetDeviceData(IDirectInputDevice8* This, DWORD cbObjectData, LPDIOBJECTDATAFORMAT rgdod, DWORD dwFlags, DWORD dwInOut);
    static HRESULT WINAPI HookGetDeviceState(IDirectInputDevice8* This, DWORD cbData, LPVOID lpvData);

    // Install hooks
    bool InstallHooks();
    void UninstallHooks();
    bool InstallDeviceHooks(IDirectInputDevice8* device);
    void UninstallDeviceHooks();
    
    // Module loading retry
    bool TryLoadDirectInputModules();
    bool AreAnyCreationHooksActive() const {
        return IsDirectInput8CreateHookActive() || IsDirectInputCreateAHookActive() ||
               IsDirectInputCreateWHookActive() || IsDirectInputCreateExHookActive();
    }

    // Test helpers
    void LogTestEvent(const std::string& event);
    void SimulateTestInput();

    // Original function pointers
    using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
    using DirectInputCreateA_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, LPDIRECTINPUTA*, LPUNKNOWN);
    using DirectInputCreateW_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, LPDIRECTINPUTW*, LPUNKNOWN);
    using DirectInputCreateEx_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
    using CreateDevice_t = HRESULT(STDMETHODCALLTYPE*)(IDirectInput8*, REFGUID, LPDIRECTINPUTDEVICE*, LPUNKNOWN);
    using SetDeviceData_t = HRESULT(STDMETHODCALLTYPE*)(IDirectInputDevice8*, DWORD, LPDIOBJECTDATAFORMAT, DWORD, DWORD);
    using GetDeviceState_t = HRESULT(STDMETHODCALLTYPE*)(IDirectInputDevice8*, DWORD, LPVOID);

    DirectInput8Create_t m_original_DirectInput8Create = nullptr;
    DirectInputCreateA_t m_original_DirectInputCreateA = nullptr;
    DirectInputCreateW_t m_original_DirectInputCreateW = nullptr;
    DirectInputCreateEx_t m_original_DirectInputCreateEx = nullptr;
    CreateDevice_t m_original_CreateDevice = nullptr;
    SetDeviceData_t m_original_SetDeviceData = nullptr;
    GetDeviceState_t m_original_GetDeviceState = nullptr;

    // Test state
    std::atomic<bool> m_test_running{false};
    std::string m_last_test_result;
    int m_test_duration_ms = 5000; // 5 seconds default
    
    // Hook state
    bool m_hooks_installed = false;
    bool m_device_hooks_installed = false;
    std::atomic<bool> m_retry_thread_running{false};
    std::thread m_retry_thread;
    
    // Device tracking
    std::vector<IDirectInputDevice8*> m_tracked_devices;
    
    // Test statistics
    struct TestStats {
        int mouse_events_intercepted = 0;
        int keyboard_events_intercepted = 0;
        int input_blocks_performed = 0;
        int devices_created = 0;
        std::vector<std::string> intercepted_events;
    };
    
    TestStats m_current_test_stats;
    
    // Global instance for static access
    static DirectInputTester* s_instance;
};

// Global instance
extern std::unique_ptr<DirectInputTester> g_directInputTester;

} // namespace renodx::input::direct_input::test
