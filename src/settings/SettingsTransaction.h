#pragma once

#include "SettingsManager.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace Jonaki {

/**
 * @brief Transactional staging buffer for application settings.
 *
 * Buffers configuration edits in memory during dialog sessions.
 * Allows committing changes (Apply/OK), discarding uncommitted edits (Cancel),
 * resetting to defaults, and exporting to standalone INI files.
 */
class SettingsTransaction {
public:
    explicit SettingsTransaction(SettingsManager& manager = SettingsManager::instance());
    ~SettingsTransaction() = default;

    // Getters: queries staged map first, falls back to manager or defaultValue
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& section, const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    SettingValue getValue(const std::string& section, const std::string& key, const SettingValue& defaultValue = {}) const;

    // Setters: updates staged map and marks key as dirty
    void setBool(const std::string& section, const std::string& key, bool val);
    void setInt(const std::string& section, const std::string& key, int val);
    void setDouble(const std::string& section, const std::string& key, double val);
    void setString(const std::string& section, const std::string& key, const std::string& val);
    void setValue(const std::string& section, const std::string& key, SettingValue val);

    // Transaction state
    bool isDirty() const;
    bool isKeyDirty(const std::string& section, const std::string& key) const;
    size_t dirtyCount() const;

    // Transaction operations
    void commit();
    void discard();
    void resetToDefaults();
    bool exportToFile(const std::string& filePath) const;

private:
    SettingsManager& m_manager;
    std::unordered_map<std::string, SettingValue> m_staged;
    std::unordered_set<std::string> m_dirtyKeys;

    static std::string makeKey(const std::string& section, const std::string& key);
};

} // namespace Jonaki
