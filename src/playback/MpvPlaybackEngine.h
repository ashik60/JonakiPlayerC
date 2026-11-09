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
    void setSubtitleVisibility(bool visible) override;
    bool isSubtitleVisible() const override;

    // --- On-Screen Display (OSD) ---
    void showOsdMessage(const std::string& text, int durationMs = 1500) override;
    void setAudioDelay(double seconds) override;
    double getAudioDelay() const override;

    // --- Equalizer ---
    void setEqualizerBand(int bandIndex, double gainDb) override;
    double getEqualizerBand(int bandIndex) const override;
    void setEqualizerEnabled(bool enabled) override;
    bool isEqualizerEnabled() const override;
    void resetEqualizer() override;

    // --- Dynamic Audio Normalization ---
    void setAudioNormalization(bool enabled) override;
    bool isAudioNormalizationEnabled() const override;

    // --- Video Controls ---
    void setDeinterlacing(const std::string& mode) override;
    std::string getDeinterlacing() const override;
    void setAspectRatio(const std::string& ratio) override;
    std::string getAspectRatio() const override;
    std::string getHwdecActive() const override;
    std::string getVideoCodec() const override;

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
    void applyAudioFilters();

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
    bool m_subVisibility = true;
    double m_audioDelay = 0.0;
    bool m_equalizerEnabled = false;
    std::vector<double> m_equalizerBands = std::vector<double>(10, 0.0);
    bool m_audioNormalizationEnabled = false;
    std::string m_deinterlaceMode = "off";
    std::string m_aspectRatio = "default";
    std::string m_hwdecActive = "no";
    std::string m_videoCodec;
    std::vector<TrackInfo> m_tracks;

    StateChangedCallback m_onStateChanged;
    TimeChangedCallback m_onTimeChanged;
    TrackListChangedCallback m_onTrackListChanged;
    ErrorCallback m_onError;
};

} // namespace Jonaki
