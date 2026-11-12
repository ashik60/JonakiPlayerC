#pragma once

#include <string>
#include <vector>
#include <QStringList>

namespace Jonaki {

class IPlaybackEngine;

/**
 * @brief Manages subtitle discovery, auto-matching adjacent files,
 * font & style preferences, and synchronization offsets.
 */
class SubtitleManager {
public:
    explicit SubtitleManager(IPlaybackEngine* engine);

    // Auto-discovery of subtitle files matching media base name
    QStringList findMatchingSubtitles(const std::string& mediaPath) const;
    void autoLoadMatchingSubtitles(const std::string& mediaPath);

    // Track controls
    void toggleVisibility();
    void setVisibility(bool visible);
    bool isVisible() const;

    void adjustDelay(double deltaSeconds); // +/- 0.5s
    void resetDelay();
    double currentDelay() const;

    // Network subtitle configuration
    void setNetworkSubtitleProcessingEnabled(bool enabled) { m_networkSubtitleProcessing = enabled; }
    bool isNetworkSubtitleProcessingEnabled() const { return m_networkSubtitleProcessing; }

private:
    IPlaybackEngine* m_engine = nullptr;
    bool m_networkSubtitleProcessing = true;
};

} // namespace Jonaki
