#include "settings_wrapper.hpp"
#include "../../addon.hpp"
#include <sstream>
#include <algorithm>

// Windows defines min/max as macros, so we need to undefine them
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace renodx::ui::new_ui {

// SettingBase implementation
SettingBase::SettingBase(const std::string& key, const std::string& section)
    : key_(key), section_(section) {
}

// FloatSetting implementation
FloatSetting::FloatSetting(const std::string& key, float default_value, float min, float max, const std::string& section)
    : SettingBase(key, section), value_(default_value), default_value_(default_value), min_(min), max_(max) {
}

void FloatSetting::Load() {
    float loaded_value;
    if (reshade::get_config_value(nullptr, section_.c_str(), key_.c_str(), loaded_value)) {
        // Clamp the loaded value to valid range
        value_ = std::max(min_, std::min(max_, loaded_value));
    } else {
        // Use default value if not found
        value_ = default_value_;
    }
}

void FloatSetting::Save() {
    reshade::set_config_value(nullptr, section_.c_str(), key_.c_str(), value_);
}

void FloatSetting::SetValue(float value) {
    value_ = std::max(min_, std::min(max_, value));
    Save(); // Auto-save when value changes
}

// IntSetting implementation
IntSetting::IntSetting(const std::string& key, int default_value, int min, int max, const std::string& section)
    : SettingBase(key, section), value_(default_value), default_value_(default_value), min_(min), max_(max) {
}

void IntSetting::Load() {
    int loaded_value;
    if (reshade::get_config_value(nullptr, section_.c_str(), key_.c_str(), loaded_value)) {
        // Clamp the loaded value to valid range
        value_ = std::max(min_, std::min(max_, loaded_value));
    } else {
        // Use default value if not found
        value_ = default_value_;
    }
}

void IntSetting::Save() {
    reshade::set_config_value(nullptr, section_.c_str(), key_.c_str(), value_);
}

void IntSetting::SetValue(int value) {
    value_ = std::max(min_, std::min(max_, value));
    Save(); // Auto-save when value changes
}

// BoolSetting implementation
BoolSetting::BoolSetting(const std::string& key, bool default_value, const std::string& section)
    : SettingBase(key, section), value_(default_value), default_value_(default_value) {
}

void BoolSetting::Load() {
    int loaded_value;
    if (reshade::get_config_value(nullptr, section_.c_str(), key_.c_str(), loaded_value)) {
        value_ = (loaded_value != 0);
    } else {
        // Use default value if not found
        value_ = default_value_;
    }
}

void BoolSetting::Save() {
    reshade::set_config_value(nullptr, section_.c_str(), key_.c_str(), value_ ? 1 : 0);
}

void BoolSetting::SetValue(bool value) {
    value_ = value;
    Save(); // Auto-save when value changes
}

// ComboSetting implementation
ComboSetting::ComboSetting(const std::string& key, int default_value, const std::vector<const char*>& labels, const std::string& section)
    : SettingBase(key, section), value_(default_value), default_value_(default_value), labels_(labels) {
}

void ComboSetting::Load() {
    int loaded_value;
    if (reshade::get_config_value(nullptr, section_.c_str(), key_.c_str(), loaded_value)) {
        // Clamp the loaded value to valid range
        value_ = std::max(0, std::min(static_cast<int>(labels_.size()) - 1, loaded_value));
    } else {
        // Use default value if not found
        value_ = default_value_;
    }
}

void ComboSetting::Save() {
    reshade::set_config_value(nullptr, section_.c_str(), key_.c_str(), value_);
}

void ComboSetting::SetValue(int value) {
    value_ = std::max(0, std::min(static_cast<int>(labels_.size()) - 1, value));
    Save(); // Auto-save when value changes
}

// Wrapper function implementations

bool SliderFloatSetting(FloatSetting& setting, const char* label, const char* format) {
    float value = setting.GetValue();
    bool changed = ImGui::SliderFloat(label, &value, setting.GetMin(), setting.GetMax(), format);
    if (changed) {
        setting.SetValue(value);
    }
    return changed;
}

bool SliderIntSetting(IntSetting& setting, const char* label, const char* format) {
    int value = setting.GetValue();
    bool changed = ImGui::SliderInt(label, &value, setting.GetMin(), setting.GetMax(), format);
    if (changed) {
        setting.SetValue(value);
    }
    return changed;
}

bool CheckboxSetting(BoolSetting& setting, const char* label) {
    bool value = setting.GetValue();
    bool changed = ImGui::Checkbox(label, &value);
    std::stringstream ss;
    ss << "XXX CheckboxSetting: " << label << ", value: " << value << ", changed: " << changed;
    LogInfo(ss.str().c_str());
    if (changed) {
        setting.SetValue(value);
    }
    return changed;
}

bool ComboSettingWrapper(ComboSetting& setting, const char* label) {
    int value = setting.GetValue();
    bool changed = ImGui::Combo(label, &value, setting.GetLabels().data(), static_cast<int>(setting.GetLabels().size()));
    if (changed) {
        setting.SetValue(value);
    }
    return changed;
}

bool ButtonSetting(const char* label, const ImVec2& size) {
    return ImGui::Button(label, size);
}

void TextSetting(const char* text) {
    ImGui::Text("%s", text);
}

void SeparatorSetting() {
    ImGui::Separator();
}

void SpacingSetting() {
    ImGui::Spacing();
}

void LoadTabSettings(const std::vector<SettingBase*>& settings) {
    for (auto* setting : settings) {
        if (setting) {
            setting->Load();
        }
    }
}

} // namespace renodx::ui::new_ui
