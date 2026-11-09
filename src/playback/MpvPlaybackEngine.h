#pragma once

#include "IPlaybackEngine.h"
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#if defined(HAVE_MPV)
#include <mpv/client.h>
#include <mpv/render_gl.h>
#endif

namespace Jonaki {

/**
 * @brief Concrete implementation of IPlaybackEngine using libmpv.
 *
 * Implements full asynchronous event loop handling, hardware decoding configuration,
 * property observation, and track introspection via libmpv C API.
 */
class MpvPlaybackEngine : public IPlaybackEngine {
public:
    MpvPlaybackEngine();
    ~MpvPlaybackEngine() override;

    // --- Lifecycle ---
    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override;

    // --- Media Controls ---
    bool loadMedia(const std::string& pathOrUrl) override;
    void play() override;
    void pause() override;
    void togglePlayPause() override;
    void stop() override;

    // --- Seeking & Timing ---
    void seek(double seconds, bool exact = false) override;
    void seekRelative(double seconds) override;
    void stepFrame(int direction) override;
    double getCurrentTime() const override;
    double getDuration() const override;
    PlaybackState getState() const override;

    // --- Speed & Volume ---
    void setSpeed(double speed) override;
    double getSpeed() const override;
    void setVolume(int volume) override;
    int getVolume() const override;
    void setMute(bool mute) override;
    bool isMute() const override;

    // --- Tracks ---
    std::vector<TrackInfo> getTracks() const override;
    bool selectTrack(const std::string& type, int trackId) override;

    // --- Subtitles & Audio Delay ---
    void setSubtitleDelay(double seconds) override;
    double getSubtitleDelay() const override;
    void loadExternalSubtitle(const std::string& subPath) override;
    void setAudioDelay(double seconds) override;
    double getAudioDelay() const override;

    // --- Video Controls ---
    void setAspectRatio(const std::string& ratio) override;
    std::string getAspectRatio() const override;

    // --- Callbacks ---
    void setOnStateChanged(StateChangedCallback cb) override { m_onStateChanged = std::move(cb); }
    void setOnTimeChanged(TimeChangedCallback cb) override { m_onTimeChanged = std::move(cb); }
    void setOnTrackListChanged(TrackListChangedCallback cb) override { m_onTrackListChanged = std::move(cb); }
    void setOnError(ErrorCallback cb) override { m_onError = std::move(cb); }

    // Direct access to underlying mpv_handle for rendering contexts
#if defined(HAVE_MPV)
    mpv_handle* rawHandle() const { return m_mpv; }
#endif

private:
    void eventLoopWorker();
    void handleMpvEvent(void* eventPtr);
    void updateTracks();

#if defined(HAVE_MPV)
    mpv_handle* m_mpv = nullptr;
#endif

    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_running{false};
    std::thread m_eventThread;

    mutable std::mutex m_stateMutex;
    PlaybackState m_state{PlaybackState::Stopped};
    double m_currentTime = 0.0;
    double m_duration = 0.0;
    double m_speed = 1.0;
    int m_volume = 100;
    bool m_mute = false;
    double m_subDelay = 0.0;
    double m_audioDelay = 0.0;
    std::string m_aspectRatio = "default";
    std::vector<TrackInfo> m_tracks;

    StateChangedCallback m_onStateChanged;
    TimeChangedCallback m_onTimeChanged;
    TrackListChangedCallback m_onTrackListChanged;
    ErrorCallback m_onError;
};

} // namespace Jonaki
