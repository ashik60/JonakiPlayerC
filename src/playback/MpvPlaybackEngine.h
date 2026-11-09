#pragma once

#include "IPlaybackEngine.h"
#include <memory>
#include <atomic>

#if defined(HAVE_MPV)
#include <mpv/client.h>
#include <mpv/render_gl.h>
#endif

namespace Jonaki {

class MpvPlaybackEngine : public IPlaybackEngine {
public:
    MpvPlaybackEngine();
    ~MpvPlaybackEngine() override;

    bool initialize() override;
    void shutdown() override;

    bool loadMedia(const std::string& pathOrUrl) override;
    void play() override;
    void pause() override;
    void togglePlayPause() override;
    void stop() override;

    void seek(double seconds, bool exact = false) override;
    void seekRelative(double seconds) override;
    void stepFrame(int direction) override;
    double getCurrentTime() const override;
    double getDuration() const override;
    PlaybackState getState() const override;

    void setSpeed(double speed) override;
    double getSpeed() const override;
    void setVolume(int volume) override;
    int getVolume() const override;
    void setMute(bool mute) override;
    bool isMute() const override;

    std::vector<TrackInfo> getTracks() const override;
    bool selectTrack(const std::string& type, int trackId) override;

    void setSubtitleDelay(double seconds) override;
    double getSubtitleDelay() const override;
    void loadExternalSubtitle(const std::string& subPath) override;

    void setAudioDelay(double seconds) override;
    double getAudioDelay() const override;

    void setOnStateChanged(StateChangedCallback cb) override { m_onStateChanged = std::move(cb); }
    void setOnTimeChanged(TimeChangedCallback cb) override { m_onTimeChanged = std::move(cb); }
    void setOnTrackListChanged(TrackListChangedCallback cb) override { m_onTrackListChanged = std::move(cb); }

private:
#if defined(HAVE_MPV)
    mpv_handle* m_mpv = nullptr;
#endif

    std::atomic<PlaybackState> m_state{PlaybackState::Stopped};
    double m_currentTime = 0.0;
    double m_duration = 0.0;
    double m_speed = 1.0;
    int m_volume = 100;
    bool m_mute = false;
    double m_subDelay = 0.0;
    double m_audioDelay = 0.0;

    StateChangedCallback m_onStateChanged;
    TimeChangedCallback m_onTimeChanged;
    TrackListChangedCallback m_onTrackListChanged;
};

} // namespace Jonaki
