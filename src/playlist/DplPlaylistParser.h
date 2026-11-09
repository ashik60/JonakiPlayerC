#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <map>

namespace Jonaki {

/**
 * @brief Represents an individual media entry in a Daum Playlist (.dpl).
 */
struct PlaylistItem {
    int id = 0;                          // 1-based index
    std::string filePath;                // 1*file*...
    std::string title;                   // 1*title*...
    int64_t durationMs = 0;              // 1*duration2*... (milliseconds)
    int64_t startPosMs = 0;              // 1*start*... (playback offset / resume pos)
    
    // Preserved unknown properties to avoid destructive round-trips
    // e.g. "1*played*1", "1*sub*...", etc.
    std::map<std::string, std::string> extraFields;
};

/**
 * @brief In-memory model representing a complete Daum Playlist file.
 */
struct PlaylistData {
    std::string playName;                // playname=... (currently playing file)
    int64_t playTimeMs = 0;              // playtime=... (playback position)
    int topIndex = 0;                    // topindex=... (scroll/view index)
    int savePlayPos = 0;                 // saveplaypos=... (1 = save, 0 = default)
    std::string baseFolder;              // basefolder=...

    std::vector<PlaylistItem> items;

    // Preserved header metadata not explicitly modeled
    std::map<std::string, std::string> extraHeaderFields;
};

/**
 * @brief High-fidelity, round-trip compatible parser and serializer for
 * Daum PotPlayer .dpl playlist files.
 */
class DplPlaylistParser {
public:
    static bool parse(const std::string& content, PlaylistData& outData);
    static bool parseFile(const std::string& filePath, PlaylistData& outData);

    static std::string serialize(const PlaylistData& data);
    static bool serializeToFile(const std::string& filePath, const PlaylistData& data);
};

} // namespace Jonaki
