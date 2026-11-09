#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace Jonaki {

enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

struct TrackInfo {
    int id = -1;
    std::string type; // "video", "audio", "sub"
    std::string title;
    std::string language;
    std::string codec;
    bool selected = false;
};

class IPlaybackEngine {
public:
    virtual ~IPlaybackEngine() = default;

    // Initialization & Lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    // Media Control
    virtual bool loadMedia(const std::string& pathOrUrl) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void togglePlayPause() = 0;
    virtual void stop() = 0;

    // Seeking & Time
    virtual void seek(double seconds, bool exact = false) = 0;
    virtual void seekRelative(double seconds) = 0;
    virtual void stepFrame(int direction) = 0; // +1 forward, -1 backward
    virtual double getCurrentTime() const = 0;
    virtual double getDuration() const = 0;
    virtual PlaybackState getState() const = 0;

    // Speed & Volume
    virtual void setSpeed(double speed) = 0;
    virtual double getSpeed() const = 0;
    virtual void setVolume(int volume) = 0; // 0 to 100 (or up to 200)
    virtual int getVolume() const = 0;
    virtual void setMute(bool mute) = 0;
    virtual bool isMute() const = 0;

    // Streams & Tracks
    virtual std::vector<TrackInfo> getTracks() const = 0;
    virtual bool selectTrack(const std::string& type, int trackId) = 0;

    // Subtitle Controls
    virtual void setSubtitleDelay(double seconds) = 0;
    virtual double getSubtitleDelay() const = 0;
    virtual void loadExternalSubtitle(const std::string& subPath) = 0;

    // Audio Controls
    virtual void setAudioDelay(double seconds) = 0;
    virtual double getAudioDelay() const = 0;

    // Callbacks
    using StateChangedCallback = std::function<void(PlaybackState)>;
    using TimeChangedCallback = std::function<void(double currentTime, double duration)>;
    using TrackListChangedCallback = std::function<void(const std::vector<TrackInfo>&)>;

    virtual void setOnStateChanged(StateChangedCallback cb) = 0;
    virtual void setOnTimeChanged(TimeChangedCallback cb) = 0;
    virtual void setOnTrackListChanged(TrackListChangedCallback cb) = 0;
};

} // namespace Jonaki
