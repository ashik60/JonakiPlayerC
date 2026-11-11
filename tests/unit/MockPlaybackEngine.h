#pragma once

#include "playback/IPlaybackEngine.h"
#include <vector>
#include <string>

namespace Jonaki {

/**
 * @brief Pure Mock implementation of IPlaybackEngine for deterministic unit testing.
 *
 * Does not depend on external hardware, display servers, or libmpv runtime.
 */
class MockPlaybackEngine : public IPlaybackEngine {
public:
    MockPlaybackEngine() = default;
    ~MockPlaybackEngine() override = default;

    bool initialize() override {
        m_initialized = true;
        return true;
    }

    void shutdown() override {
        m_initialized = false;
        m_state = PlaybackState::Stopped;
    }

    bool isInitialized() const override {
        return m_initialized;
    }

    bool loadMedia(const std::string& pathOrUrl) override {
        if (!m_initialized) return false;
        if (pathOrUrl.empty() || pathOrUrl == "invalid_file_nonexistent.xyz") {
            if (m_onError) m_onError(PlaybackError::FileNotFound, "File not found");
            return false;
        }
        m_loadedPath = pathOrUrl;
        m_state = PlaybackState::Playing;
        m_duration = 120.0;
        m_currentTime = 0.0;
        if (m_onStateChanged) m_onStateChanged(m_state);
        return true;
    }

    void play() override {
        m_state = PlaybackState::Playing;
        if (m_onStateChanged) m_onStateChanged(m_state);
    }

    void pause() override {
        m_state = PlaybackState::Paused;
        if (m_onStateChanged) m_onStateChanged(m_state);
    }

    void togglePlayPause() override {
        if (m_state == PlaybackState::Playing) {
            pause();
        } else {
            play();
        }
    }

    void stop() override {
        m_state = PlaybackState::Stopped;
        m_currentTime = 0.0;
        if (m_onStateChanged) m_onStateChanged(m_state);
    }

    void seek(double seconds, bool exact = false) override {
        (void)exact;
        m_currentTime = seconds;
        if (m_currentTime > m_duration) m_currentTime = m_duration;
        if (m_currentTime < 0.0) m_currentTime = 0.0;
        if (m_onTimeChanged) m_onTimeChanged(m_currentTime, m_duration);
    }

    void seekRelative(double seconds) override {
        seek(m_currentTime + seconds);
    }

    void stepFrame(int direction) override {
        seekRelative(direction > 0 ? 0.033 : -0.033);
    }

    double getCurrentTime() const override { return m_currentTime; }
    double getDuration() const override { return m_duration; }
    PlaybackState getState() const override { return m_state; }

    void setSpeed(double speed) override { m_speed = speed; }
    double getSpeed() const override { return m_speed; }

    void setVolume(int volume) override { m_volume = volume; }
    int getVolume() const override { return m_volume; }

    void setMute(bool mute) override { m_mute = mute; }
    bool isMute() const override { return m_mute; }

    std::vector<TrackInfo> getTracks() const override { return m_tracks; }
    bool selectTrack(const std::string& type, int trackId) override {
        (void)type; (void)trackId;
        return true;
    }

    void setSubtitleDelay(double seconds) override { m_subDelay = seconds; }
    double getSubtitleDelay() const override { return m_subDelay; }
    void loadExternalSubtitle(const std::string& subPath) override { (void)subPath; }
    void setSubtitleVisibility(bool visible) override { m_subVisibility = visible; }
    bool isSubtitleVisible() const override { return m_subVisibility; }

    void showOsdMessage(const std::string& text, int durationMs = 1500) override {
        (void)text; (void)durationMs;
    }

    void setAudioDelay(double seconds) override { m_audioDelay = seconds; }
    double getAudioDelay() const override { return m_audioDelay; }

    void setEqualizerBand(int bandIndex, double gainDb) override {
        if (bandIndex >= 0 && bandIndex < 10) {
            m_equalizerBands[bandIndex] = std::clamp(gainDb, -12.0, 12.0);
        }
    }
    double getEqualizerBand(int bandIndex) const override {
        if (bandIndex >= 0 && bandIndex < 10) {
            return m_equalizerBands[bandIndex];
        }
        return 0.0;
    }
    void setEqualizerEnabled(bool enabled) override { m_equalizerEnabled = enabled; }
    bool isEqualizerEnabled() const override { return m_equalizerEnabled; }
    void resetEqualizer() override {
        m_equalizerBands = std::vector<double>(10, 0.0);
    }

    void setAudioNormalization(bool enabled) override { m_audioNormalizationEnabled = enabled; }
    bool isAudioNormalizationEnabled() const override { return m_audioNormalizationEnabled; }

    void setDeinterlacing(const std::string& mode) override { m_deinterlaceMode = mode; }
    std::string getDeinterlacing() const override { return m_deinterlaceMode; }

    void setAspectRatio(const std::string& ratio) override { m_aspectRatio = ratio; }
    std::string getAspectRatio() const override { return m_aspectRatio; }

    void setHwdecProfile(const std::string& profile) override { m_requestedHwdec = profile; }
    std::string getRequestedHwdecProfile() const override { return m_requestedHwdec; }
    std::string getActiveHwdecDecoder() const override { return m_activeHwdec; }
    std::string getHwdecActive() const override { return m_activeHwdec; }
    void setActiveHwdecDecoder(const std::string& act) { m_activeHwdec = act; }
    std::string getVideoCodec() const override { return "h264"; }

    // --- A-B Repeat Loop ---
    void setLoopPointA(double seconds) override {
        m_loopA = seconds;
        if (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA) m_loopActive = true;
    }
    void setLoopPointB(double seconds) override {
        m_loopB = seconds;
        if (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA) m_loopActive = true;
    }
    void clearLoopPointA() override { m_loopA = -1.0; m_loopActive = false; }
    void clearLoopPointB() override { m_loopB = -1.0; m_loopActive = false; }
    void clearLoop() override { m_loopA = -1.0; m_loopB = -1.0; m_loopActive = false; }
    double getLoopPointA() const override { return m_loopA; }
    double getLoopPointB() const override { return m_loopB; }
    bool isLoopActive() const override { return m_loopActive; }
    void toggleLoop() override {
        if (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA) {
            m_loopActive = !m_loopActive;
        } else {
            clearLoop();
        }
    }

    bool captureFrame(const std::string& outputPath, CaptureMode mode = CaptureMode::Subtitles) override {
        if (!m_initialized || m_loadedPath.empty() || m_state == PlaybackState::Stopped) {
            return false;
        }
        m_lastCapturedPath = outputPath;
        m_lastCaptureMode = mode;
        return true;
    }

    const std::string& lastCapturedPath() const { return m_lastCapturedPath; }
    CaptureMode lastCaptureMode() const { return m_lastCaptureMode; }

    void setOnStateChanged(StateChangedCallback cb) override { m_onStateChanged = std::move(cb); }
    void setOnTimeChanged(TimeChangedCallback cb) override { m_onTimeChanged = std::move(cb); }
    void setOnTrackListChanged(TrackListChangedCallback cb) override { m_onTrackListChanged = std::move(cb); }
    void setOnError(ErrorCallback cb) override { m_onError = std::move(cb); }

    const std::string& loadedPath() const { return m_loadedPath; }

private:
    bool m_initialized = false;
    PlaybackState m_state = PlaybackState::Stopped;
    std::string m_loadedPath;
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
    std::vector<TrackInfo> m_tracks;
    std::string m_lastCapturedPath;
    CaptureMode m_lastCaptureMode = CaptureMode::Subtitles;
    std::string m_requestedHwdec = "auto-safe";
    std::string m_activeHwdec = "no";
    double m_loopA = -1.0;
    double m_loopB = -1.0;
    bool m_loopActive = false;

    StateChangedCallback m_onStateChanged;
    TimeChangedCallback m_onTimeChanged;
    TrackListChangedCallback m_onTrackListChanged;
    ErrorCallback m_onError;
};

} // namespace Jonaki
