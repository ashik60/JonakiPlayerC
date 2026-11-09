#include "DplPlaylistParser.h"
#include "../core/Logger.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Jonaki {

static void trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

bool DplPlaylistParser::parse(const std::string& content, PlaylistData& outData) {
    outData = PlaylistData{};

    std::istringstream stream(content);
    std::string line;

    // Verify first line has DAUMPLAYLIST (allowing for UTF-8 BOM ef bb bf)
    if (!std::getline(stream, line)) {
        return false;
    }
    trim(line);
    // Strip BOM if present
    if (line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB && static_cast<unsigned char>(line[2]) == 0xBF) {
        line = line.substr(3);
    }
    trim(line);

    if (line != "DAUMPLAYLIST") {
        Logger::error("Invalid DPL playlist header: '" + line + "'");
        return false;
    }

    // Temporary map of id -> PlaylistItem to handle unordered key fields
    std::map<int, PlaylistItem> itemMap;

    while (std::getline(stream, line)) {
        trim(line);
        if (line.empty()) continue;

        // Check for item format: <id>*<property>*<value>
        size_t firstStar = line.find('*');
        if (firstStar != std::string::npos) {
            size_t secondStar = line.find('*', firstStar + 1);
            if (secondStar != std::string::npos) {
                std::string idStr = line.substr(0, firstStar);
                std::string prop = line.substr(firstStar + 1, secondStar - firstStar - 1);
                std::string val = line.substr(secondStar + 1);

                try {
                    int id = std::stoi(idStr);
                    PlaylistItem& item = itemMap[id];
                    item.id = id;

                    if (prop == "file") {
                        item.filePath = val;
                    } else if (prop == "title") {
                        item.title = val;
                    } else if (prop == "duration2") {
                        item.durationMs = std::stoll(val);
                    } else if (prop == "start") {
                        item.startPosMs = std::stoll(val);
                    } else {
                        // Preserve all unknown fields
                        item.extraFields[prop] = val;
                    }
                } catch (...) {
                    // Ignore malformed id line
                }
                continue;
            }
        }

        // Check for header properties: <key>=<value>
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string val = line.substr(eqPos + 1);

            if (key == "playname") {
                outData.playName = val;
            } else if (key == "playtime") {
                try { outData.playTimeMs = std::stoll(val); } catch (...) {}
            } else if (key == "topindex") {
                try { outData.topIndex = std::stoi(val); } catch (...) {}
            } else if (key == "saveplaypos") {
                try { outData.savePlayPos = std::stoi(val); } catch (...) {}
            } else if (key == "basefolder") {
                outData.baseFolder = val;
            } else {
                outData.extraHeaderFields[key] = val;
            }
        }
    }

    for (auto& pair : itemMap) {
        outData.items.push_back(std::move(pair.second));
    }

    return true;
}

bool DplPlaylistParser::parseFile(const std::string& filePath, PlaylistData& outData) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        Logger::error("Cannot open DPL file for reading: " + filePath);
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parse(content, outData);
}

std::string DplPlaylistParser::serialize(const PlaylistData& data) {
    std::ostringstream out;
    // Emit UTF-8 BOM to match reference PotPlayer behavior
    out << "\xEF\xBB\xBF";
    out << "DAUMPLAYLIST\r\n";

    if (!data.playName.empty()) {
        out << "playname=" << data.playName << "\r\n";
    }
    if (data.playTimeMs > 0) {
        out << "playtime=" << data.playTimeMs << "\r\n";
    }
    out << "topindex=" << data.topIndex << "\r\n";
    out << "saveplaypos=" << data.savePlayPos << "\r\n";

    if (!data.baseFolder.empty()) {
        out << "basefolder=" << data.baseFolder << "\r\n";
    }

    for (const auto& extra : data.extraHeaderFields) {
        out << extra.first << "=" << extra.second << "\r\n";
    }

    int idx = 1;
    for (const auto& item : data.items) {
        int currentId = (item.id > 0) ? item.id : idx;
        out << currentId << "*file*" << item.filePath << "\r\n";
        if (!item.title.empty()) {
            out << currentId << "*title*" << item.title << "\r\n";
        }
        if (item.durationMs > 0) {
            out << currentId << "*duration2*" << item.durationMs << "\r\n";
        }
        if (item.startPosMs > 0) {
            out << currentId << "*start*" << item.startPosMs << "\r\n";
        }
        for (const auto& extra : item.extraFields) {
            out << currentId << "*" << extra.first << "*" << extra.second << "\r\n";
        }
        idx++;
    }

    return out.str();
}

bool DplPlaylistParser::serializeToFile(const std::string& filePath, const PlaylistData& data) {
    std::string text = serialize(data);
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        Logger::error("Cannot open DPL file for writing: " + filePath);
        return false;
    }
    file.write(text.data(), text.size());
    return true;
}

} // namespace Jonaki
