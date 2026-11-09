#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <mutex>

namespace Jonaki {

using SettingValue = std::variant<bool, int, double, std::string>;

class SettingsManager {
public:
    static SettingsManager& instance();

    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;

    void setValue(const std::string& section, const std::string& key, SettingValue value);
    SettingValue getValue(const std::string& section, const std::string& key, const SettingValue& defaultValue = {}) const;

    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& section, const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;

    void setBool(const std::string& section, const std::string& key, bool val);
    void setInt(const std::string& section, const std::string& key, int val);
    void setDouble(const std::string& section, const std::string& key, double val);
    void setString(const std::string& section, const std::string& key, const std::string& val);

    void resetToDefaults();

private:
    SettingsManager();
    mutable std::mutex m_mutex;
    // Map: "Section/Key" -> SettingValue
    std::unordered_map<std::string, SettingValue> m_settings;

    static std::string makeKey(const std::string& section, const std::string& key);
};

} // namespace Jonaki
