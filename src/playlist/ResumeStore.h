#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace Jonaki {

struct ResumeRecord {
    std::string filePath;
    int64_t lastPositionMs = 0;
    int64_t totalDurationMs = 0;
    int width = 0;
    int height = 0;
    double fps = 0.0;
};

/**
 * @brief Manages persistence of media playback resume positions and metadata.
 * Implements clean-room SQLite schema matching PotPlayer's MediaInfo table.
 */
class ResumeStore {
public:
    static ResumeStore& instance();

    bool initialize(const std::string& dbPath);
    void close();

    bool savePosition(const std::string& filePath, int64_t positionMs, int64_t durationMs);
    bool getPosition(const std::string& filePath, int64_t& outPositionMs, int64_t& outDurationMs) const;
    bool clearPosition(const std::string& filePath);

private:
    ResumeStore() = default;
    ~ResumeStore();

    std::string encodeKey(const std::string& path) const;

    mutable std::mutex m_mutex;
    std::string m_dbPath;
    void* m_db = nullptr; // sqlite3* pointer
    bool m_initialized = false;
};

} // namespace Jonaki
