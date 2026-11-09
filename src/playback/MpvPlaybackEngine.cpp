#include "MpvPlaybackEngine.h"
#include "../core/Logger.h"
#include <QtGlobal>

namespace Jonaki {

MpvPlaybackEngine::MpvPlaybackEngine() = default;

MpvPlaybackEngine::~MpvPlaybackEngine() {
    shutdown();
}

bool MpvPlaybackEngine::initialize() {
#if defined(HAVE_MPV)
    m_mpv = mpv_create();
    if (!m_mpv) {
        Logger::error("Failed to create mpv context.");
        return false;
    }

    // Configure default mpv options
    mpv_set_option_string(m_mpv, "terminal", "yes");
    mpv_set_option_string(m_mpv, "msg-level", "all=warn");
    mpv_set_option_string(m_mpv, "vo", "gpu");
    mpv_set_option_string(m_mpv, "hwdec", "auto-safe");
    mpv_set_option_string(m_mpv, "keep-open", "yes");

    int status = mpv_initialize(m_mpv);
    if (status < 0) {
        Logger::error("Failed to initialize mpv engine: " + std::string(mpv_error_string(status)));
        mpv_destroy(m_mpv);
        m_mpv = nullptr;
        return false;
    }

    Logger::info("libmpv engine initialized successfully.");
    return true;
#else
    Logger::info("MpvPlaybackEngine initialized in stub mode (HAVE_MPV not set).");
    return true;
#endif
}

void MpvPlaybackEngine::shutdown() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
        Logger::info("libmpv engine destroyed.");
    }
#endif
    m_state = PlaybackState::Stopped;
}

bool MpvPlaybackEngine::loadMedia(const std::string& pathOrUrl) {
    Logger::info("Loading media: " + pathOrUrl);
#if defined(HAVE_MPV)
    if (!m_mpv) return false;
    const char* cmd[] = {"loadfile", pathOrUrl.c_str(), nullptr};
    int err = mpv_command_async(m_mpv, 0, cmd);
    if (err < 0) {
        Logger::error("Failed to load file: " + std::string(mpv_error_string(err)));
        return false;
    }
#endif
    m_state = PlaybackState::Playing;
    if (m_onStateChanged) m_onStateChanged(m_state);
    return true;
}

void MpvPlaybackEngine::play() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = 0;
        mpv_set_property_async(m_mpv, 0, "pause", MPV_FORMAT_FLAG, &flag);
    }
#endif
    m_state = PlaybackState::Playing;
    if (m_onStateChanged) m_onStateChanged(m_state);
}

void MpvPlaybackEngine::pause() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = 1;
        mpv_set_property_async(m_mpv, 0, "pause", MPV_FORMAT_FLAG, &flag);
    }
#endif
    m_state = PlaybackState::Paused;
    if (m_onStateChanged) m_onStateChanged(m_state);
}

void MpvPlaybackEngine::togglePlayPause() {
    if (m_state == PlaybackState::Playing) {
        pause();
    } else {
        play();
    }
}

void MpvPlaybackEngine::stop() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        const char* cmd[] = {"stop", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#endif
    m_state = PlaybackState::Stopped;
    m_currentTime = 0.0;
    if (m_onStateChanged) m_onStateChanged(m_state);
}

void MpvPlaybackEngine::seek(double seconds, bool exact) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string secStr = std::to_string(seconds);
        const char* cmd[] = {"seek", secStr.c_str(), exact ? "exact" : "relative-percent", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#else
    Q_UNUSED(exact);
#endif
    m_currentTime = seconds;
}

void MpvPlaybackEngine::seekRelative(double seconds) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string secStr = std::to_string(seconds);
        const char* cmd[] = {"seek", secStr.c_str(), "relative", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#endif
    m_currentTime += seconds;
}

void MpvPlaybackEngine::stepFrame(int direction) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        const char* cmd[] = {direction >= 0 ? "frame-step" : "frame-back-step", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#else
    Q_UNUSED(direction);
#endif
}

double MpvPlaybackEngine::getCurrentTime() const {
    return m_currentTime;
}

double MpvPlaybackEngine::getDuration() const {
    return m_duration;
}

PlaybackState MpvPlaybackEngine::getState() const {
    return m_state;
}

void MpvPlaybackEngine::setSpeed(double speed) {
    m_speed = speed;
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE, &m_speed);
    }
#endif
}

double MpvPlaybackEngine::getSpeed() const {
    return m_speed;
}

void MpvPlaybackEngine::setVolume(int volume) {
    m_volume = volume;
#if defined(HAVE_MPV)
    if (m_mpv) {
        double volDbl = static_cast<double>(volume);
        mpv_set_property_async(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE, &volDbl);
    }
#endif
}

int MpvPlaybackEngine::getVolume() const {
    return m_volume;
}

void MpvPlaybackEngine::setMute(bool mute) {
    m_mute = mute;
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = mute ? 1 : 0;
        mpv_set_property_async(m_mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
    }
#endif
}

bool MpvPlaybackEngine::isMute() const {
    return m_mute;
}

std::vector<TrackInfo> MpvPlaybackEngine::getTracks() const {
    return {};
}

bool MpvPlaybackEngine::selectTrack(const std::string& type, int trackId) {
    Q_UNUSED(type);
    Q_UNUSED(trackId);
    return true;
}

void MpvPlaybackEngine::setSubtitleDelay(double seconds) {
    m_subDelay = seconds;
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "sub-delay", MPV_FORMAT_DOUBLE, &m_subDelay);
    }
#endif
}

double MpvPlaybackEngine::getSubtitleDelay() const {
    return m_subDelay;
}

void MpvPlaybackEngine::loadExternalSubtitle(const std::string& subPath) {
    Logger::info("Loading external subtitle: " + subPath);
#if defined(HAVE_MPV)
    if (m_mpv) {
        const char* cmd[] = {"sub-add", subPath.c_str(), "select", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#endif
}

void MpvPlaybackEngine::setAudioDelay(double seconds) {
    m_audioDelay = seconds;
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "audio-delay", MPV_FORMAT_DOUBLE, &m_audioDelay);
    }
#endif
}

double MpvPlaybackEngine::getAudioDelay() const {
    return m_audioDelay;
}

} // namespace Jonaki
