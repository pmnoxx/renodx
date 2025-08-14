#include "dxgi_device_info.hpp"
#include "addon.hpp"
#include <dxgi1_6.h>
#include <d3d11.h>

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")

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

bool DXGIDeviceInfoManager::ResetHDRMetadata(const std::string& output_device_name, float max_cll) {
    if (!initialized_ || !factory_) {
        return false;
    }

    // Find the output with the specified device name
    for (const auto& adapter : adapters_) {
        for (const auto& output : adapter.outputs) {
            if (output.device_name == output_device_name && output.supports_hdr10) {
                // Create a temporary swapchain to reset HDR metadata
                return ResetHDRMetadataForOutput(output, max_cll);
            }
        }
    }
    
    LogWarn("HDR metadata reset: Output not found or doesn't support HDR10");
    return false;
}

bool DXGIDeviceInfoManager::ResetHDRMetadataForOutput(const DXGIOutputInfo& output, float max_cll) {
    // Create a temporary window for the swapchain
    int width = output.desktop_coordinates.right - output.desktop_coordinates.left;
    int height = output.desktop_coordinates.bottom - output.desktop_coordinates.top;
    
    // Create a temporary window
    HWND hwnd = CreateWindowW(
        L"static", L"HDR Metadata Reset", 
        WS_VISIBLE | WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        output.desktop_coordinates.left, output.desktop_coordinates.top,
        width, height, 0, 0, 0, 0
    );
    
    if (!hwnd) {
        LogWarn("HDR metadata reset: Failed to create temporary window");
        return false;
    }

    bool success = false;
    
    try {
        // Create D3D11 device and swapchain
        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
        
        D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
        D3D_FEATURE_LEVEL feature_level;
        
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            feature_levels, 1, D3D11_SDK_VERSION,
            &device, &feature_level, &context
        );
        
        if (SUCCEEDED(hr)) {
            // Create swapchain
            DXGI_SWAP_CHAIN_DESC swap_desc = {};
            swap_desc.BufferDesc.Width = width;
            swap_desc.BufferDesc.Height = height;
            swap_desc.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
            swap_desc.BufferDesc.RefreshRate = { 0, 0 };
            swap_desc.BufferCount = 3;
            swap_desc.Windowed = TRUE;
            swap_desc.OutputWindow = hwnd;
            swap_desc.SampleDesc = { 1, 0 };
            swap_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swap_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
            swap_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            
            hr = factory_->CreateSwapChain(device.Get(), &swap_desc, &swapchain);
            
            if (SUCCEEDED(hr)) {
                // Get IDXGISwapChain4 for HDR metadata
                Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain4;
                hr = swapchain.As(&swapchain4);
                
                if (SUCCEEDED(hr)) {
                    // Set HDR10 colorspace
                    hr = swapchain4->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
                    
                    if (SUCCEEDED(hr)) {
                        // Create HDR metadata
                        DXGI_HDR_METADATA_HDR10 metadata = {};
                        
                        // Use provided max_cll or get from output info
                        float max_luminance = (max_cll > 0.0f) ? max_cll : output.max_luminance;
                        
                        metadata.MinMasteringLuminance = 0;
                        metadata.MaxMasteringLuminance = static_cast<UINT>(max_luminance * 10000);
                        metadata.MaxFrameAverageLightLevel = static_cast<UINT16>(max_luminance * 10000);
                        metadata.MaxContentLightLevel = static_cast<UINT16>(max_luminance * 10000);
                        
                        // Set color primaries (using standard HDR10 values if not available)
                        metadata.WhitePoint[0] = 3127;   // 0.3127 * 10000
                        metadata.WhitePoint[1] = 3290;   // 0.3290 * 10000
                        metadata.RedPrimary[0] = 6800;    // 0.68 * 10000
                        metadata.RedPrimary[1] = 3200;    // 0.32 * 10000
                        metadata.GreenPrimary[0] = 2650;  // 0.265 * 10000
                        metadata.GreenPrimary[1] = 6900;  // 0.69 * 10000
                        metadata.BluePrimary[0] = 1500;   // 0.15 * 10000
                        metadata.BluePrimary[1] = 600;    // 0.06 * 10000
                        
                        // Set HDR metadata
                        hr = swapchain4->SetHDRMetaData(
                            DXGI_HDR_METADATA_TYPE_HDR10, 
                            sizeof(metadata), 
                            &metadata
                        );
                        
                        if (SUCCEEDED(hr)) {
                            // Present a few frames to ensure metadata is applied
                            swapchain4->Present(1, 0);
                            Sleep(100);
                            swapchain4->Present(1, 0);
                            Sleep(100);
                            swapchain4->Present(1, 0);
                            
                            LogInfo(("HDR metadata reset successful for output: " + output.device_name).c_str());
                            success = true;
                        } else {
                            LogWarn("HDR metadata reset: Failed to set HDR metadata");
                        }
                    } else {
                        LogWarn("HDR metadata reset: Failed to set HDR10 colorspace");
                    }
                } else {
                    LogWarn("HDR metadata reset: Failed to get IDXGISwapChain4");
                }
            } else {
                LogWarn("HDR metadata reset: Failed to create swapchain");
            }
        } else {
            LogWarn("HDR metadata reset: Failed to create D3D11 device");
        }
    } catch (...) {
        LogWarn("HDR metadata reset: Exception occurred during reset");
    }
    
    // Clean up
    if (hwnd) {
        DestroyWindow(hwnd);
    }
    
    return success;
}
