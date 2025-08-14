#include "dxgi_device_info.hpp"
#include "addon.hpp"
#include <dxgi1_6.h>
#include <d3d11.h>

#pragma comment (lib, "dxgi.lib")

// Global instance declaration (defined in globals.cpp)
extern std::unique_ptr<DXGIDeviceInfoManager> g_dxgiDeviceInfoManager;

DXGIDeviceInfoManager::DXGIDeviceInfoManager() : initialized_(false) {
}

DXGIDeviceInfoManager::~DXGIDeviceInfoManager() {
    Cleanup();
}

bool DXGIDeviceInfoManager::Initialize() {
    if (initialized_) {
        return true;
    }

    // Create DXGI factory
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory_));
    if (FAILED(hr)) {
        LogWarn("Failed to create DXGI factory");
        return false;
    }

    // Enumerate adapters
    if (!EnumerateAdapters()) {
        LogWarn("Failed to enumerate DXGI adapters");
        return false;
    }

    initialized_ = true;
    LogInfo("DXGI Device Info Manager initialized successfully");
    return true;
}

void DXGIDeviceInfoManager::Refresh() {
    if (!initialized_) {
        return;
    }

    adapters_.clear();
    EnumerateAdapters();
    LogDebug("DXGI device information refreshed");
}

void DXGIDeviceInfoManager::Cleanup() {
    adapters_.clear();
    factory_.Reset();
    initialized_ = false;
}

bool DXGIDeviceInfoManager::EnumerateAdapters() {
    if (!factory_) {
        return false;
    }

    int adapter_idx = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;

    while (SUCCEEDED(factory_->EnumAdapters(adapter_idx++, &adapter))) {
        DXGIAdapterInfo adapter_info = {};
        
        // Get adapter description
        DXGI_ADAPTER_DESC desc = {};
        if (SUCCEEDED(adapter->GetDesc(&desc))) {
            // Convert wide string to UTF-8
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, nullptr, 0, nullptr, nullptr);
            if (size_needed > 0) {
                std::string description(size_needed - 1, 0);
                WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, &description[0], size_needed, nullptr, nullptr);
                adapter_info.description = description;
            }
            
            adapter_info.name = "Adapter " + std::to_string(adapter_idx - 1);
            adapter_info.dedicated_video_memory = desc.DedicatedVideoMemory;
            adapter_info.dedicated_system_memory = desc.DedicatedSystemMemory;
            adapter_info.shared_system_memory = desc.SharedSystemMemory;
            adapter_info.adapter_luid = desc.AdapterLuid;
            // Note: DXGI_ADAPTER_DESC doesn't have Flags member, so we'll set this based on other criteria
            adapter_info.is_software = (desc.DedicatedVideoMemory == 0);
        }

        // Enumerate outputs for this adapter
        EnumerateOutputs(adapter.Get(), adapter_info);
        
        adapters_.push_back(std::move(adapter_info));
        adapter = nullptr; // Reset for next iteration
    }

    return !adapters_.empty();
}

bool DXGIDeviceInfoManager::EnumerateOutputs(IDXGIAdapter* adapter, DXGIAdapterInfo& adapter_info) {
    if (!adapter) {
        return false;
    }

    int output_idx = 0;
    Microsoft::WRL::ComPtr<IDXGIOutput> output;

    while (SUCCEEDED(adapter->EnumOutputs(output_idx++, &output))) {
        DXGIOutputInfo output_info = {};
        
        // Get basic output description
        DXGI_OUTPUT_DESC desc = {};
        if (SUCCEEDED(output->GetDesc(&desc))) {
            // Convert wide string to UTF-8
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, desc.DeviceName, -1, nullptr, 0, nullptr, nullptr);
            if (size_needed > 0) {
                std::string device_name(size_needed - 1, 0);
                WideCharToMultiByte(CP_UTF8, 0, desc.DeviceName, -1, &device_name[0], size_needed, nullptr, nullptr);
                output_info.device_name = device_name;
            } else {
                output_info.device_name = "Unknown Device";
            }
            output_info.desktop_coordinates = desc.DesktopCoordinates;
            output_info.is_attached = desc.AttachedToDesktop;
            output_info.rotation = desc.Rotation;
            
            // Get monitor name if available
            if (desc.Monitor) {
                MONITORINFOEXW monitor_info = {};
                monitor_info.cbSize = sizeof(MONITORINFOEXW);
                if (GetMonitorInfoW(desc.Monitor, &monitor_info)) {
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, monitor_info.szDevice, -1, nullptr, 0, nullptr, nullptr);
                    if (size_needed > 0) {
                        std::string monitor_name(size_needed - 1, 0);
                        WideCharToMultiByte(CP_UTF8, 0, monitor_info.szDevice, -1, &monitor_name[0], size_needed, nullptr, nullptr);
                        output_info.monitor_name = monitor_name;
                    }
                }
            }
        }

        // Try to get advanced output information (DXGI 1.6)
        Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
        if (SUCCEEDED(output.As(&output6))) {
            DXGI_OUTPUT_DESC1 desc1 = {};
            if (SUCCEEDED(output6->GetDesc1(&desc1))) {
                // HDR information
                output_info.supports_hdr10 = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
                output_info.max_luminance = desc1.MaxLuminance;
                output_info.min_luminance = desc1.MinLuminance;
                // Note: These members don't exist in DXGI_OUTPUT_DESC1, so we'll set them to 0
                output_info.max_frame_average_light_level = 0.0f;
                output_info.max_content_light_level = 0.0f;
                
                // Color space information
                output_info.color_space = static_cast<DXGI_COLOR_SPACE_TYPE>(desc1.ColorSpace);
                output_info.supports_wide_color_gamut = (desc1.ColorSpace != DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
                
                // Get refresh rate from supported modes
                DXGI_MODE_DESC mode_desc = {};
                mode_desc.Width = desc1.DesktopCoordinates.right - desc1.DesktopCoordinates.left;
                mode_desc.Height = desc1.DesktopCoordinates.bottom - desc1.DesktopCoordinates.top;
                mode_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                
                DXGI_MODE_DESC closest_mode = {};
                if (SUCCEEDED(output6->FindClosestMatchingMode(&mode_desc, &closest_mode, nullptr))) {
                    output_info.refresh_rate = closest_mode.RefreshRate;
                }
            }
        }

        // Enumerate supported modes
        UINT num_modes = 0;
        if (SUCCEEDED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes, nullptr))) {
            if (num_modes > 0) {
                std::vector<DXGI_MODE_DESC> modes(num_modes);
                if (SUCCEEDED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes, modes.data()))) {
                    output_info.supported_modes = std::move(modes);
                }
            }
        }

        adapter_info.outputs.push_back(std::move(output_info));
        output = nullptr; // Reset for next iteration
    }

    return true;
}
