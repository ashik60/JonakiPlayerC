#include "SettingsManager.h"
#include "../core/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

namespace Jonaki {

SettingsManager& SettingsManager::instance() {
    static SettingsManager s_instance;
    return s_instance;
}

SettingsManager::SettingsManager() {
    resetToDefaults();
}

std::string SettingsManager::makeKey(const std::string& section, const std::string& key) {
    return section + "/" + key;
}

void SettingsManager::resetToDefaults() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_settings.clear();

    // Default PotPlayer-like values
    m_settings[makeKey("Window", "Width")] = 960;
    m_settings[makeKey("Window", "Height")] = 540;
    m_settings[makeKey("Window", "RememberPosition")] = true;
    m_settings[makeKey("Window", "OnTop")] = 0; // 0=Never, 1=Always, 2=While Playing

    m_settings[makeKey("Playback", "Volume")] = 100;
    m_settings[makeKey("Playback", "Mute")] = false;
    m_settings[makeKey("Playback", "Speed")] = 1.0;
    m_settings[makeKey("Playback", "AutoResume")] = true;
    m_settings[makeKey("Playback", "RepeatMode")] = 0; // 0=None, 1=Repeat One, 2=Repeat All

    m_settings[makeKey("Video", "AspectRatio")] = std::string("Default");
    m_settings[makeKey("Video", "Deinterlace")] = false;
    m_settings[makeKey("Video", "DeinterlaceMode")] = std::string("off");
    m_settings[makeKey("Video", "HwDec")] = std::string("auto-safe");

    m_settings[makeKey("Audio", "Normalization")] = false;
    m_settings[makeKey("Audio", "EqualizerEnabled")] = false;
    m_settings[makeKey("Audio", "EqualizerBands")] = std::string("0;0;0;0;0;0;0;0;0;0");
    m_settings[makeKey("Audio", "AudioDelay")] = 0.0;

    m_settings[makeKey("Subtitles", "ShowSubtitles")] = true;
    m_settings[makeKey("Subtitles", "FontSize")] = 24;
    m_settings[makeKey("Subtitles", "FontName")] = std::string("Arial");
}

bool SettingsManager::loadFromFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::warning("Could not open settings file for reading: " + filePath);
        return false;
    }

    std::string currentSection = "General";
    std::string line;

    while (std::getline(file, line)) {
        // Strip carriage return and leading/trailing whitespace
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos || line[first] == ';' || line[first] == '#') {
            continue;
        }

        size_t last = line.find_last_not_of(" \t");
        std::string trimmed = line.substr(first, (last - first + 1));

        if (trimmed.front() == '[' && trimmed.back() == ']') {
            currentSection = trimmed.substr(1, trimmed.size() - 2);
            continue;
        }

        size_t eqPos = trimmed.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trimmed.substr(0, eqPos);
            std::string val = trimmed.substr(eqPos + 1);

            // Trim key & val
            key.erase(key.find_last_not_of(" \t") + 1);
            if (!val.empty()) {
                val.erase(0, val.find_first_not_of(" \t"));
                val.erase(val.find_last_not_of(" \t") + 1);
            }

            // Guess type (bool, int, double, string)
            if (val == "true" || val == "True" || val == "1") {
                m_settings[makeKey(currentSection, key)] = true;
            } else if (val == "false" || val == "False" || val == "0") {
                m_settings[makeKey(currentSection, key)] = false;
            } else {
                try {
                    size_t idx = 0;
                    int intVal = std::stoi(val, &idx);
                    if (idx == val.size()) {
                        m_settings[makeKey(currentSection, key)] = intVal;
                        continue;
                    }
                    double dblVal = std::stod(val, &idx);
                    if (idx == val.size()) {
                        m_settings[makeKey(currentSection, key)] = dblVal;
                        continue;
                    }
                } catch (...) {}

                m_settings[makeKey(currentSection, key)] = val;
            }
        }
    }

    Logger::info("Settings successfully loaded from: " + filePath);
    return true;
}

bool SettingsManager::saveToFile(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ofstream file(filePath);
    if (!file.is_open()) {
        Logger::error("Could not open settings file for writing: " + filePath);
        return false;
    }

    // Group by section
    std::unordered_map<std::string, std::vector<std::pair<std::string, SettingValue>>> grouped;
    for (const auto& [compKey, val] : m_settings) {
        size_t slash = compKey.find('/');
        if (slash != std::string::npos) {
            std::string sec = compKey.substr(0, slash);
            std::string k = compKey.substr(slash + 1);
            grouped[sec].emplace_back(k, val);
        }
    }

    for (const auto& [section, kvList] : grouped) {
        file << "[" << section << "]\n";
        for (const auto& [k, val] : kvList) {
            file << k << "=";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "1" : "0");
                } else {
                    file << arg;
                }
            }, val);
            file << "\n";
        }
        file << "\n";
    }

    Logger::info("Settings successfully saved to: " + filePath);
    return true;
}

void SettingsManager::setValue(const std::string& section, const std::string& key, SettingValue value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_settings[makeKey(section, key)] = std::move(value);
}

SettingValue SettingsManager::getValue(const std::string& section, const std::string& key, const SettingValue& defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_settings.find(makeKey(section, key));
    if (it != m_settings.end()) {
        return it->second;
    }
    return defaultValue;
}

bool SettingsManager::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
    if (std::holds_alternative<int>(val)) return std::get<int>(val) != 0;
    return defaultValue;
}

int SettingsManager::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<int>(val)) return std::get<int>(val);
    if (std::holds_alternative<double>(val)) return static_cast<int>(std::get<double>(val));
    return defaultValue;
}

double SettingsManager::getDouble(const std::string& section, const std::string& key, double defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<double>(val)) return std::get<double>(val);
    if (std::holds_alternative<int>(val)) return static_cast<double>(std::get<int>(val));
    return defaultValue;
}

std::string SettingsManager::getString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<std::string>(val)) return std::get<std::string>(val);
    return defaultValue;
}

void SettingsManager::setBool(const std::string& section, const std::string& key, bool val) {
    setValue(section, key, val);
}

void SettingsManager::setInt(const std::string& section, const std::string& key, int val) {
    setValue(section, key, val);
}

void SettingsManager::setDouble(const std::string& section, const std::string& key, double val) {
    setValue(section, key, val);
}

void SettingsManager::setString(const std::string& section, const std::string& key, const std::string& val) {
    setValue(section, key, val);
}

} // namespace Jonaki
