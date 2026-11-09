#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace Jonaki {

/**
 * @brief Represents the playback state of the media engine.
 */
enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

/**
 * @brief Detailed information about an elementary media stream.
 */
struct TrackInfo {
    int id = -1;
    std::string type;     // "video", "audio", "sub"
    std::string title;
    std::string language;
    std::string codec;
    bool selected = false;
};

/**
 * @brief Common playback error categories.
 */
enum class PlaybackError {
    None,
    FileNotFound,
    InvalidMedia,
    DecoderInitializationFailed,
    NetworkError,
    Unknown
};

/**
 * @brief Pure abstract playback engine interface.
 * 
 * This interface isolates all multimedia operations from specific engine
 * implementations (such as libmpv, GStreamer, or DirectShow), ensuring clean
 * decoupling and independent testability.
 */
class IPlaybackEngine {
public:
    virtual ~IPlaybackEngine() = default;

    // --- Lifecycle ---
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // --- Media Controls ---
    virtual bool loadMedia(const std::string& pathOrUrl) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void togglePlayPause() = 0;
    virtual void stop() = 0;

    // --- Seeking & Timing ---
    virtual void seek(double seconds, bool exact = false) = 0;
    virtual void seekRelative(double seconds) = 0;
    virtual void stepFrame(int direction) = 0; // +1 forward, -1 backward
    virtual double getCurrentTime() const = 0;
    virtual double getDuration() const = 0;
    virtual PlaybackState getState() const = 0;

    // --- Speed & Audio Volume ---
    virtual void setSpeed(double speed) = 0;
    virtual double getSpeed() const = 0;
    virtual void setVolume(int volume) = 0; // 0 to 100 (or up to 200)
    virtual int getVolume() const = 0;
    virtual void setMute(bool mute) = 0;
    virtual bool isMute() const = 0;

    // --- Stream & Track Management ---
    virtual std::vector<TrackInfo> getTracks() const = 0;
    virtual bool selectTrack(const std::string& type, int trackId) = 0;

    // --- Subtitles ---
    virtual void setSubtitleDelay(double seconds) = 0;
    virtual double getSubtitleDelay() const = 0;
    virtual void loadExternalSubtitle(const std::string& subPath) = 0;

    // --- Audio Delay ---
    virtual void setAudioDelay(double seconds) = 0;
    virtual double getAudioDelay() const = 0;

    // --- Video Aspect Ratio & Zoom ---
    virtual void setAspectRatio(const std::string& ratio) = 0; // e.g., "default", "16:9", "4:3"
    virtual std::string getAspectRatio() const = 0;

    // --- Callbacks & Observers ---
    using StateChangedCallback = std::function<void(PlaybackState)>;
    using TimeChangedCallback = std::function<void(double currentTime, double duration)>;
    using TrackListChangedCallback = std::function<void(const std::vector<TrackInfo>&)>;
    using ErrorCallback = std::function<void(PlaybackError error, const std::string& message)>;

    virtual void setOnStateChanged(StateChangedCallback cb) = 0;
    virtual void setOnTimeChanged(TimeChangedCallback cb) = 0;
    virtual void setOnTrackListChanged(TrackListChangedCallback cb) = 0;
    virtual void setOnError(ErrorCallback cb) = 0;
};

} // namespace Jonaki
