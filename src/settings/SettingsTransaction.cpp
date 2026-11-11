#include "SettingsTransaction.h"
#include "../core/Logger.h"
#include <fstream>
#include <sstream>
#include <map>

namespace Jonaki {

std::string SettingsTransaction::makeKey(const std::string& section, const std::string& key) {
    return section + "/" + key;
}

SettingsTransaction::SettingsTransaction(SettingsManager& manager)
    : m_manager(manager) {
}

SettingValue SettingsTransaction::getValue(const std::string& section, const std::string& key, const SettingValue& defaultValue) const {
    std::string fullKey = makeKey(section, key);
    auto it = m_staged.find(fullKey);
    if (it != m_staged.end()) {
        return it->second;
    }
    return m_manager.getValue(section, key, defaultValue);
}

bool SettingsTransaction::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    } else if (std::holds_alternative<int>(val)) {
        return std::get<int>(val) != 0;
    } else if (std::holds_alternative<std::string>(val)) {
        std::string s = std::get<std::string>(val);
        return (s == "1" || s == "true" || s == "TRUE" || s == "yes");
    }
    return defaultValue;
}

int SettingsTransaction::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val);
    } else if (std::holds_alternative<double>(val)) {
        return static_cast<int>(std::get<double>(val));
    } else if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val) ? 1 : 0;
    } else if (std::holds_alternative<std::string>(val)) {
        try {
            return std::stoi(std::get<std::string>(val));
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

double SettingsTransaction::getDouble(const std::string& section, const std::string& key, double defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<double>(val)) {
        return std::get<double>(val);
    } else if (std::holds_alternative<int>(val)) {
        return static_cast<double>(std::get<int>(val));
    } else if (std::holds_alternative<std::string>(val)) {
        try {
            return std::stod(std::get<std::string>(val));
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string SettingsTransaction::getString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    auto val = getValue(section, key, defaultValue);
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    } else if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val) ? "true" : "false";
    } else if (std::holds_alternative<int>(val)) {
        return std::to_string(std::get<int>(val));
    } else if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    }
    return defaultValue;
}

void SettingsTransaction::setValue(const std::string& section, const std::string& key, SettingValue val) {
    std::string fullKey = makeKey(section, key);
    auto current = m_manager.getValue(section, key, {});
    m_staged[fullKey] = val;
    if (val != current) {
        m_dirtyKeys.insert(fullKey);
    } else {
        m_dirtyKeys.erase(fullKey);
    }
}

void SettingsTransaction::setBool(const std::string& section, const std::string& key, bool val) {
    setValue(section, key, val);
}

void SettingsTransaction::setInt(const std::string& section, const std::string& key, int val) {
    setValue(section, key, val);
}

void SettingsTransaction::setDouble(const std::string& section, const std::string& key, double val) {
    setValue(section, key, val);
}

void SettingsTransaction::setString(const std::string& section, const std::string& key, const std::string& val) {
    setValue(section, key, val);
}

bool SettingsTransaction::isDirty() const {
    return !m_dirtyKeys.empty();
}

bool SettingsTransaction::isKeyDirty(const std::string& section, const std::string& key) const {
    return m_dirtyKeys.find(makeKey(section, key)) != m_dirtyKeys.end();
}

size_t SettingsTransaction::dirtyCount() const {
    return m_dirtyKeys.size();
}

void SettingsTransaction::commit() {
    for (const auto& keyStr : m_dirtyKeys) {
        auto it = m_staged.find(keyStr);
        if (it != m_staged.end()) {
            size_t slashPos = keyStr.find('/');
            if (slashPos != std::string::npos) {
                std::string section = keyStr.substr(0, slashPos);
                std::string key = keyStr.substr(slashPos + 1);
                m_manager.setValue(section, key, it->second);
            }
        }
    }
    m_dirtyKeys.clear();
}

void SettingsTransaction::discard() {
    m_staged.clear();
    m_dirtyKeys.clear();
}

void SettingsTransaction::resetToDefaults() {
    // Populate staged with standard defaults
    setValue("General", "SnapToEdge", true);
    setValue("General", "ShowTooltips", true);
    setValue("General", "StoreIniFile", true);
    setValue("General", "PauseOnFocusLost", false);
    setValue("General", "PauseOnMinimize", true);
    setValue("General", "AutoRotate", true);

    setValue("Startup", "WindowSize", std::string("960x540"));
    setValue("Startup", "RestorePosition", true);
    setValue("Startup", "DefaultVolume", 100);

    setValue("OSD", "ShowFilename", true);
    setValue("OSD", "ShowPlaybackTime", true);
    setValue("OSD", "ShowCodecInfo", false);
    setValue("OSD", "DurationMs", 1500);
    setValue("OSD", "FontName", std::string("Tahoma"));
    setValue("OSD", "FontSize", 24);

    setValue("Logo", "Type", std::string("Jonaki Default"));
    setValue("Logo", "ShowOnLoad", true);

    setValue("Mouse", "LeftSingleClick", std::string("Play/Pause"));
    setValue("Mouse", "LeftDoubleClick", std::string("Fullscreen"));
    setValue("Mouse", "WheelAction", std::string("Volume"));
    setValue("Mouse", "PreventOutsideDrag", false);

    setValue("Keyboard", "GlobalMediaKeys", true);

    setValue("Skin", "ActiveSkin", std::string("Default Dark"));
    setValue("Skin", "DarkMode", true);

    setValue("Playback", "Volume", 100);
    setValue("Playback", "Mute", false);
    setValue("Playback", "Speed", 1.0);
    setValue("Playback", "AutoResume", true);
    setValue("Playback", "RepeatMode", 0);
    setValue("Playback", "RememberVideoPositions", true);
    setValue("Playback", "RememberAudioPositions", true);
    setValue("Playback", "PreventScreenSaver", true);
    setValue("Playback", "AutoAddSimilarFiles", true);
    setValue("Playback", "JumpKeyframe", true);
    setValue("Playback", "ShortJumpSec", 5);
    setValue("Playback", "MediumJumpSec", 30);
    setValue("Playback", "LongJumpSec", 60);
    setValue("Playback", "MinResumeThresholdSec", 10);

    setValue("Video", "AspectRatio", std::string("Default"));
    setValue("Video", "Deinterlace", false);
    setValue("Video", "DeinterlaceMode", std::string("off"));
    setValue("Video", "HwDec", std::string("auto-safe"));
    setValue("Video", "ColorRange", std::string("Auto"));
    setValue("Video", "Brightness", 0);
    setValue("Video", "Contrast", 0);
    setValue("Video", "Saturation", 66);
    setValue("Video", "Hue", 0);
    setValue("Video", "ResizeMethod", std::string("Original"));
    setValue("Video", "CorrectAspectOnCapture", true);

    setValue("Audio", "Normalization", false);
    setValue("Audio", "NormTargetPeak", 0.95);
    setValue("Audio", "NormMaxGain", 10);
    setValue("Audio", "EqualizerEnabled", false);
    setValue("Audio", "EqualizerBands", std::string("0;0;0;0;0;0;0;0;0;0"));
    setValue("Audio", "AudioDelay", 0.0);
    setValue("Audio", "OutputDevice", std::string("Default"));
    setValue("Audio", "SpeakerChannels", std::string("Stereo"));
    setValue("Audio", "ExpandStereo", false);
    setValue("Audio", "PassThrough", false);

    setValue("Subtitles", "ShowSubtitles", true);
    setValue("Subtitles", "FontSize", 24);
    setValue("Subtitles", "FontName", std::string("sans-serif"));
    setValue("Subtitles", "FontColor", std::string("#FFFFFFFF"));
    setValue("Subtitles", "BorderSize", 2);
    setValue("Subtitles", "LanguagePriority", std::string("eng, ben"));
    setValue("Subtitles", "AutoLoadSameName", true);
    setValue("Subtitles", "SearchSubfolders", true);
    setValue("Subtitles", "DelayStep", 0.5);
    setValue("Subtitles", "VerticalMargin", 90);

    setValue("Filter", "Demuxer", std::string("Auto"));
    setValue("Filter", "NetworkCacheKb", 8192);
    setValue("Filter", "VideoDecoderPriority", std::string("Hardware DXVA/D3D11 First"));
    setValue("Filter", "MultiThreadDecoding", true);
    setValue("Filter", "AudioDecoderEngine", std::string("Built-in FFmpeg"));
}

bool SettingsTransaction::exportToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        Logger::error("Failed to open file for settings export: " + filePath);
        return false;
    }

    // Group all settings by section
    std::map<std::string, std::map<std::string, SettingValue>> sections;
    
    // First, gather from manager defaults
    // Then override with staged values
    auto allKeys = {
        "General/SnapToEdge", "General/ShowTooltips", "General/StoreIniFile",
        "General/PauseOnFocusLost", "General/PauseOnMinimize", "General/AutoRotate",
        "Startup/WindowSize", "Startup/RestorePosition", "Startup/DefaultVolume",
        "OSD/ShowFilename", "OSD/ShowPlaybackTime", "OSD/ShowCodecInfo", "OSD/DurationMs",
        "OSD/FontName", "OSD/FontSize",
        "Logo/Type", "Logo/ShowOnLoad",
        "Mouse/LeftSingleClick", "Mouse/LeftDoubleClick", "Mouse/WheelAction", "Mouse/PreventOutsideDrag",
        "Keyboard/GlobalMediaKeys",
        "Skin/ActiveSkin", "Skin/DarkMode",
        "Playback/Volume", "Playback/Mute", "Playback/Speed", "Playback/AutoResume", "Playback/RepeatMode",
        "Playback/RememberVideoPositions", "Playback/RememberAudioPositions", "Playback/PreventScreenSaver",
        "Playback/AutoAddSimilarFiles", "Playback/JumpKeyframe", "Playback/ShortJumpSec",
        "Playback/MediumJumpSec", "Playback/LongJumpSec", "Playback/MinResumeThresholdSec",
        "Video/AspectRatio", "Video/Deinterlace", "Video/DeinterlaceMode", "Video/HwDec",
        "Video/ColorRange", "Video/Brightness", "Video/Contrast", "Video/Saturation", "Video/Hue",
        "Video/ResizeMethod", "Video/CorrectAspectOnCapture",
        "Audio/Normalization", "Audio/NormTargetPeak", "Audio/NormMaxGain",
        "Audio/EqualizerEnabled", "Audio/EqualizerBands", "Audio/AudioDelay",
        "Audio/OutputDevice", "Audio/SpeakerChannels", "Audio/ExpandStereo", "Audio/PassThrough",
        "Subtitles/ShowSubtitles", "Subtitles/FontSize", "Subtitles/FontName", "Subtitles/FontColor",
        "Subtitles/BorderSize", "Subtitles/LanguagePriority", "Subtitles/AutoLoadSameName",
        "Subtitles/SearchSubfolders", "Subtitles/DelayStep", "Subtitles/VerticalMargin",
        "Filter/Demuxer", "Filter/NetworkCacheKb", "Filter/VideoDecoderPriority",
        "Filter/MultiThreadDecoding", "Filter/AudioDecoderEngine"
    };

    for (const auto& k : allKeys) {
        std::string keyStr(k);
        size_t slash = keyStr.find('/');
        std::string sec = keyStr.substr(0, slash);
        std::string key = keyStr.substr(slash + 1);
        sections[sec][key] = getValue(sec, key, {});
    }

    file << "; JonakiPlayer Configuration Export\n";
    file << "; Generated by SettingsTransaction\n\n";

    for (const auto& [sec, keys] : sections) {
        file << "[" << sec << "]\n";
        for (const auto& [key, val] : keys) {
            file << key << "=";
            if (std::holds_alternative<bool>(val)) {
                file << (std::get<bool>(val) ? "true" : "false");
            } else if (std::holds_alternative<int>(val)) {
                file << std::get<int>(val);
            } else if (std::holds_alternative<double>(val)) {
                file << std::get<double>(val);
            } else if (std::holds_alternative<std::string>(val)) {
                file << std::get<std::string>(val);
            }
            file << "\n";
        }
        file << "\n";
    }

    return true;
}

} // namespace Jonaki
