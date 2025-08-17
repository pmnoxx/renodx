#include "custom_fps_limiter.hpp"
#include "../addon.hpp"
#include <algorithm>

namespace renodx::dxgi::fps_limiter {

CustomFpsLimiter::CustomFpsLimiter()
    : m_target_fps(0.0f)
    , m_enabled(false)
    , m_last_time_point(std::chrono::high_resolution_clock::now())
    , m_frame_start_time(std::chrono::high_resolution_clock::now())
    , m_frame_end_time(std::chrono::high_resolution_clock::now())
    , m_last_present_time(std::chrono::high_resolution_clock::now())
    , m_frame_in_progress(false)
{
}

void CustomFpsLimiter::LimitFrameRate()
{
    if (!m_enabled || m_target_fps <= 0.0f) {
        return;
    }

    // Reset state if FPS limit changed (cloned from RenoDX)
    if (m_last_fps_limit != m_target_fps) {
        m_spin_lock_duration = std::chrono::nanoseconds(0);
        m_sleep_latency_history.clear();
        m_last_fps_limit = m_target_fps;
    }

    UpdateTiming();
    ApplySleepAndSpinLock();
}

void CustomFpsLimiter::OnFrameBegin()
{
    // Record when rendering starts
    m_frame_start_time = std::chrono::high_resolution_clock::now();
    m_frame_in_progress = true;
}

void CustomFpsLimiter::OnFrameEnd()
{
    // Record when rendering ends
    m_frame_end_time = std::chrono::high_resolution_clock::now();
    m_frame_in_progress = false;
}

void CustomFpsLimiter::UpdateTiming()
{
    auto now = std::chrono::high_resolution_clock::now();
    
    // Calculate time per frame (cloned from RenoDX)
    const auto time_per_frame = std::chrono::high_resolution_clock::duration(std::chrono::milliseconds(1000)) / static_cast<double>(m_target_fps);
    
    // Calculate next target time based on last time point (FIXED - matches RenoDX exactly)
    const auto next_time_point = m_last_time_point + time_per_frame;
    auto time_till_next_frame = next_time_point - now;
    
    // Debug: Log timing information (can be removed later)
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        auto time_per_frame_ms = std::chrono::duration_cast<std::chrono::microseconds>(time_per_frame).count() / 1000.0;
        auto time_till_next_ms = std::chrono::duration_cast<std::chrono::microseconds>(time_till_next_frame).count() / 1000.0;
        printf("FPS Limiter Debug: target_fps=%.1f, time_per_frame=%.3fms, time_till_next=%.3fms\n", 
               m_target_fps, time_per_frame_ms, time_till_next_ms);
    }
    
    if (time_till_next_frame.count() <= 0) {
        // We're already late, update timing and return
        m_last_time_point = now;
        return;
    }

    // Use sleep for as much as reliably possible (cloned from RenoDX)
    auto sleep_duration = time_till_next_frame - m_spin_lock_duration;
    
    // Debug: Log sleep calculation (can be removed later)
    if (frame_count % 60 == 0) {
        auto sleep_duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(sleep_duration).count() / 1000.0;
        auto spin_lock_ms = std::chrono::duration_cast<std::chrono::microseconds>(m_spin_lock_duration).count() / 1000.0;
        printf("FPS Limiter Debug: sleep_duration=%.3fms, spin_lock_duration=%.3fms\n", 
               sleep_duration_ms, spin_lock_ms);
    }
    
    if (sleep_duration.count() > 0) {
        std::this_thread::sleep_for(sleep_duration);
        auto after_sleep = std::chrono::high_resolution_clock::now();
        auto actual_sleep_duration = after_sleep - now;
        auto sleep_latency = actual_sleep_duration - sleep_duration;

        // Record the sleep latency (cloned from RenoDX)
        m_sleep_latency_history.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_latency));
        auto current_size = m_sleep_latency_history.size() + 1;
        if (current_size > MAX_LATENCY_HISTORY_SIZE) {
            m_sleep_latency_history.pop_front();
            --current_size;
        }

        // Calculate the worst 1% latency (cloned from RenoDX)
        if (current_size >= MAX_LATENCY_HISTORY_SIZE * 0.10f) {
            auto sorted_latencies = m_sleep_latency_history;
            std::ranges::sort(sorted_latencies, std::greater<>());
            auto worst_1_percent = sorted_latencies[current_size * 0.01];
            m_spin_lock_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(worst_1_percent * 1.5);
        }

        m_spin_lock_failures = 0;
        now = after_sleep;
    } else {
        ++m_spin_lock_failures;

        if (m_spin_lock_failures > m_target_fps) {
            // Full second of failures, try increasing (cloned from RenoDX)
            m_spin_lock_failures = 0;

            if (m_sleep_latency_history.empty()) {
                m_spin_lock_duration = std::chrono::nanoseconds(0);
            } else {
                auto sorted_latencies = m_sleep_latency_history;
                std::ranges::sort(sorted_latencies, std::less<>());
                if (sorted_latencies[0] * 1.5 < time_per_frame) {
                    // Decrease spin lock duration by 10% (cloned from RenoDX)
                    m_spin_lock_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_spin_lock_duration * 0.90f);
                }
            }
        }
    }

    // Spin lock until the next time point (cloned from RenoDX)
    while (now < next_time_point) {
        now = std::chrono::high_resolution_clock::now();
    }

    // Update last time point (FIXED - matches RenoDX exactly)
    m_last_time_point = now;
}

void CustomFpsLimiter::ApplySleepAndSpinLock()
{
    // This function is now just a placeholder since UpdateTiming handles everything
    // It's kept for consistency with the header structure
}

void CustomFpsLimiter::SetTargetFps(float fps)
{
    m_target_fps = fps;
    // Note: LogWarn not available, using printf-style logging
    // LogWarn("Custom FPS Limiter: Target FPS set to %f", fps);
}

} // namespace renodx::dxgi::fps_limiter
