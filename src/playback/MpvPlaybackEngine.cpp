#include "MpvPlaybackEngine.h"
#include "../core/Logger.h"
#include <sstream>
#include <cstring>
#include <algorithm>

namespace Jonaki {

MpvPlaybackEngine::MpvPlaybackEngine() = default;

MpvPlaybackEngine::~MpvPlaybackEngine() {
    shutdown();
}

bool MpvPlaybackEngine::isInitialized() const {
    return m_initialized.load();
}

bool MpvPlaybackEngine::initialize() {
    if (m_initialized.load()) {
        return true;
    }

#if defined(HAVE_MPV)
    m_mpv = mpv_create();
    if (!m_mpv) {
        Logger::error("Failed to create libmpv instance.");
        if (m_onError) m_onError(PlaybackError::DecoderInitializationFailed, "Failed to create mpv context");
        return false;
    }

    // Configure core options
    mpv_set_option_string(m_mpv, "terminal", "no");
    mpv_set_option_string(m_mpv, "msg-level", "all=warn");
    mpv_set_option_string(m_mpv, "keep-open", "always");
    mpv_set_option_string(m_mpv, "hwdec", "auto-safe");
    mpv_set_option_string(m_mpv, "audio-pitch-correction", "yes");
    mpv_set_option_string(m_mpv, "sub-auto", "exact");
    mpv_set_option_string(m_mpv, "sub-font", "sans-serif");
    mpv_set_option_string(m_mpv, "sub-font-size", "38");
    mpv_set_option_string(m_mpv, "sub-color", "#FFFFFFFF");
    mpv_set_option_string(m_mpv, "sub-border-color", "#FF000000");
    mpv_set_option_string(m_mpv, "sub-border-size", "2.0");
    mpv_set_option_string(m_mpv, "osd-font", "Tahoma");
    mpv_set_option_string(m_mpv, "osd-font-size", "24");
    mpv_set_option_string(m_mpv, "osd-color", "#FFFFFFFF");
    mpv_set_option_string(m_mpv, "osd-border-color", "#FF000000");

    // Observe properties
    mpv_observe_property(m_mpv, 1, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 2, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 3, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 4, "track-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 5, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 6, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 7, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 8, "hwdec-current", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 9, "video-codec", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 10, "ab-loop-a", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 11, "ab-loop-b", MPV_FORMAT_STRING);

    int status = mpv_initialize(m_mpv);
    if (status < 0) {
        std::string errStr = mpv_error_string(status);
        Logger::error("Failed to initialize libmpv engine: " + errStr);
        if (m_onError) m_onError(PlaybackError::DecoderInitializationFailed, errStr);
        mpv_destroy(m_mpv);
        m_mpv = nullptr;
        return false;
    }

    m_initialized.store(true);
    m_running.store(true);
    m_eventThread = std::thread(&MpvPlaybackEngine::eventLoopWorker, this);

    Logger::info("libmpv engine initialized successfully with async event thread.");
    return true;
#else
    m_initialized.store(true);
    Logger::info("MpvPlaybackEngine initialized in stub mode (HAVE_MPV not defined).");
    return true;
#endif
}

void MpvPlaybackEngine::shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    m_running.store(false);

#if defined(HAVE_MPV)
    if (m_mpv) {
        // Wake up event loop
        mpv_wakeup(m_mpv);
    }
#endif

    if (m_eventThread.joinable()) {
        m_eventThread.join();
    }

#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
#endif

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = PlaybackState::Stopped;
        m_currentTime = 0.0;
        m_duration = 0.0;
        m_tracks.clear();
    }

    m_initialized.store(false);
    Logger::info("libmpv engine shut down cleanly.");
}

void MpvPlaybackEngine::eventLoopWorker() {
#if defined(HAVE_MPV)
    while (m_running.load() && m_mpv) {
        mpv_event* event = mpv_wait_event(m_mpv, 0.05);
        if (!event) continue;

        if (event->event_id == MPV_EVENT_SHUTDOWN || !m_running.load()) {
            break;
        }

        handleMpvEvent(event);
    }
#endif
}

void MpvPlaybackEngine::handleMpvEvent(void* eventPtr) {
#if defined(HAVE_MPV)
    mpv_event* event = static_cast<mpv_event*>(eventPtr);
    if (!event) return;

    switch (event->event_id) {
        case MPV_EVENT_START_FILE: {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_state = PlaybackState::Playing;
            if (m_onStateChanged) m_onStateChanged(m_state);
            break;
        }
        case MPV_EVENT_FILE_LOADED: {
            Logger::info("Media file loaded and demuxed successfully.");
            updateTracks();
            break;
        }
        case MPV_EVENT_END_FILE: {
            mpv_event_end_file* endEvent = static_cast<mpv_event_end_file*>(event->data);
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_state = PlaybackState::Stopped;
            m_currentTime = 0.0;
            if (endEvent && endEvent->reason == MPV_END_FILE_REASON_ERROR) {
                std::string errStr = mpv_error_string(endEvent->error);
                Logger::error("Playback end with error: " + errStr);
                if (m_onError) m_onError(PlaybackError::InvalidMedia, errStr);
            }
            if (m_onStateChanged) m_onStateChanged(m_state);
            break;
        }
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property* prop = static_cast<mpv_event_property*>(event->data);
            if (!prop) break;

            if (std::strcmp(prop->name, "time-pos") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    double t = *static_cast<double*>(prop->data);
                    double d = 0.0;
                    {
                        std::lock_guard<std::mutex> lock(m_stateMutex);
                        m_currentTime = t;
                        d = m_duration;
                    }
                    if (m_onTimeChanged) m_onTimeChanged(t, d);
                }
            } else if (std::strcmp(prop->name, "duration") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    double d = *static_cast<double*>(prop->data);
                    double t = 0.0;
                    {
                        std::lock_guard<std::mutex> lock(m_stateMutex);
                        m_duration = d;
                        t = m_currentTime;
                    }
                    if (m_onTimeChanged) m_onTimeChanged(t, d);
                }
            } else if (std::strcmp(prop->name, "pause") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    int paused = *static_cast<int*>(prop->data);
                    PlaybackState newState = paused ? PlaybackState::Paused : PlaybackState::Playing;
                    {
                        std::lock_guard<std::mutex> lock(m_stateMutex);
                        m_state = newState;
                    }
                    if (m_onStateChanged) m_onStateChanged(newState);
                }
            } else if (std::strcmp(prop->name, "track-list") == 0) {
                updateTracks();
            } else if (std::strcmp(prop->name, "volume") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    m_volume = static_cast<int>(*static_cast<double*>(prop->data));
                }
            } else if (std::strcmp(prop->name, "mute") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    m_mute = (*static_cast<int*>(prop->data) != 0);
                }
            } else if (std::strcmp(prop->name, "speed") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    m_speed = *static_cast<double*>(prop->data);
                }
            } else if (std::strcmp(prop->name, "hwdec-current") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    const char* hw = *static_cast<char**>(prop->data);
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    m_hwdecActive = hw ? hw : "no";
                }
            } else if (std::strcmp(prop->name, "video-codec") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    const char* vc = *static_cast<char**>(prop->data);
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    m_videoCodec = vc ? vc : "";
                }
            } else if (std::strcmp(prop->name, "ab-loop-a") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    const char* aVal = *static_cast<char**>(prop->data);
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    if (aVal && std::strcmp(aVal, "no") != 0 && std::strlen(aVal) > 0) {
                        try {
                            m_loopA = std::stod(aVal);
                        } catch (...) {
                            m_loopA = -1.0;
                        }
                    } else {
                        m_loopA = -1.0;
                    }
                    m_loopActive = (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA);
                }
            } else if (std::strcmp(prop->name, "ab-loop-b") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    const char* bVal = *static_cast<char**>(prop->data);
                    std::lock_guard<std::mutex> lock(m_stateMutex);
                    if (bVal && std::strcmp(bVal, "no") != 0 && std::strlen(bVal) > 0) {
                        try {
                            m_loopB = std::stod(bVal);
                        } catch (...) {
                            m_loopB = -1.0;
                        }
                    } else {
                        m_loopB = -1.0;
                    }
                    m_loopActive = (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA);
                }
            }
            break;
        }
        default:
            break;
    }
#else
    (void)eventPtr;
#endif
}

void MpvPlaybackEngine::updateTracks() {
#if defined(HAVE_MPV)
    if (!m_mpv) return;

    mpv_node node;
    if (mpv_get_property(m_mpv, "track-list", MPV_FORMAT_NODE, &node) >= 0) {
        if (node.format == MPV_FORMAT_NODE_ARRAY && node.u.list) {
            std::vector<TrackInfo> newTracks;
            for (int i = 0; i < node.u.list->num; i++) {
                mpv_node* item = &node.u.list->values[i];
                if (item->format == MPV_FORMAT_NODE_MAP && item->u.list) {
                    TrackInfo info;
                    for (int j = 0; j < item->u.list->num; j++) {
                        const char* key = item->u.list->keys[j];
                        mpv_node* val = &item->u.list->values[j];
                        if (std::strcmp(key, "id") == 0 && val->format == MPV_FORMAT_INT64) {
                            info.id = static_cast<int>(val->u.int64);
                        } else if (std::strcmp(key, "type") == 0 && val->format == MPV_FORMAT_STRING) {
                            info.type = val->u.string;
                        } else if (std::strcmp(key, "title") == 0 && val->format == MPV_FORMAT_STRING) {
                            info.title = val->u.string;
                        } else if (std::strcmp(key, "lang") == 0 && val->format == MPV_FORMAT_STRING) {
                            info.language = val->u.string;
                        } else if (std::strcmp(key, "codec") == 0 && val->format == MPV_FORMAT_STRING) {
                            info.codec = val->u.string;
                        } else if (std::strcmp(key, "selected") == 0 && val->format == MPV_FORMAT_FLAG) {
                            info.selected = (val->u.flag != 0);
                        }
                    }
                    newTracks.push_back(info);
                }
            }
            {
                std::lock_guard<std::mutex> lock(m_stateMutex);
                m_tracks = newTracks;
            }
            Logger::info("Tracks updated, total: " + std::to_string(newTracks.size()));
            if (m_onTrackListChanged) m_onTrackListChanged(newTracks);
        }
        mpv_free_node_contents(&node);
    }
#endif
}

bool MpvPlaybackEngine::loadMedia(const std::string& pathOrUrl) {
    if (!m_initialized.load()) {
        if (!initialize()) {
            return false;
        }
    }

    Logger::info("MpvPlaybackEngine loading media: " + pathOrUrl);

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_currentMediaUri = pathOrUrl;
        m_loopA = -1.0;
        m_loopB = -1.0;
        m_loopActive = false;
    }

#if defined(HAVE_MPV)
    if (!m_mpv) return false;

    const char* cmd[] = {"loadfile", pathOrUrl.c_str(), "replace", nullptr};
    int err = mpv_command_async(m_mpv, 0, cmd);
    if (err < 0) {
        std::string errStr = mpv_error_string(err);
        Logger::error("Failed to load media file: " + errStr);
        if (m_onError) m_onError(PlaybackError::FileNotFound, errStr);
        return false;
    }
#endif

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = PlaybackState::Playing;
        m_currentTime = 0.0;
    }

    if (m_onStateChanged) m_onStateChanged(PlaybackState::Playing);
    return true;
}

void MpvPlaybackEngine::play() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = 0;
        mpv_set_property_async(m_mpv, 0, "pause", MPV_FORMAT_FLAG, &flag);
    }
#endif
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = PlaybackState::Playing;
    }
    if (m_onStateChanged) m_onStateChanged(PlaybackState::Playing);
}

void MpvPlaybackEngine::pause() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = 1;
        mpv_set_property_async(m_mpv, 0, "pause", MPV_FORMAT_FLAG, &flag);
    }
#endif
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = PlaybackState::Paused;
    }
    if (m_onStateChanged) m_onStateChanged(PlaybackState::Paused);
}

void MpvPlaybackEngine::togglePlayPause() {
    PlaybackState cur;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        cur = m_state;
    }
    if (cur == PlaybackState::Playing) {
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
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = PlaybackState::Stopped;
        m_currentTime = 0.0;
    }
    if (m_onStateChanged) m_onStateChanged(PlaybackState::Stopped);
}

void MpvPlaybackEngine::seek(double seconds, bool exact) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string secStr = std::to_string(seconds);
        const char* cmd[] = {"seek", secStr.c_str(), exact ? "exact" : "absolute", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#else
    (void)exact;
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
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
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_currentTime += seconds;
    if (m_currentTime < 0.0) m_currentTime = 0.0;
}

void MpvPlaybackEngine::stepFrame(int direction) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        const char* cmd[] = {direction >= 0 ? "frame-step" : "frame-back-step", nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#else
    (void)direction;
#endif
}

double MpvPlaybackEngine::getCurrentTime() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_currentTime;
}

double MpvPlaybackEngine::getDuration() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_duration;
}

PlaybackState MpvPlaybackEngine::getState() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_state;
}

void MpvPlaybackEngine::setSpeed(double speed) {
    if (speed < 0.1) speed = 0.1;
    if (speed > 5.0) speed = 5.0;

#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE, &speed);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_speed = speed;
}

double MpvPlaybackEngine::getSpeed() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_speed;
}

void MpvPlaybackEngine::setVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 200) volume = 200;

#if defined(HAVE_MPV)
    if (m_mpv) {
        double volDbl = static_cast<double>(volume);
        mpv_set_property_async(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE, &volDbl);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_volume = volume;
}

int MpvPlaybackEngine::getVolume() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_volume;
}

void MpvPlaybackEngine::setMute(bool mute) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = mute ? 1 : 0;
        mpv_set_property_async(m_mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_mute = mute;
}

bool MpvPlaybackEngine::isMute() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_mute;
}

std::vector<TrackInfo> MpvPlaybackEngine::getTracks() const {
#if defined(HAVE_MPV)
    if (m_mpv) {
        const_cast<MpvPlaybackEngine*>(this)->updateTracks();
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_tracks;
}

bool MpvPlaybackEngine::selectTrack(const std::string& type, int trackId) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string prop = (type == "audio" ? "aid" : (type == "sub" ? "sid" : "vid"));
        int64_t tid = trackId;
        mpv_set_property_async(m_mpv, 0, prop.c_str(), MPV_FORMAT_INT64, &tid);
        return true;
    }
#else
    (void)type;
    (void)trackId;
#endif
    return false;
}

void MpvPlaybackEngine::setSubtitleDelay(double seconds) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "sub-delay", MPV_FORMAT_DOUBLE, &seconds);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_subDelay = seconds;
}

double MpvPlaybackEngine::getSubtitleDelay() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_subDelay;
}

void MpvPlaybackEngine::loadExternalSubtitle(const std::string& subPath) {
    Logger::info("Loading external subtitle track: " + subPath);
#if defined(HAVE_MPV)
    if (m_mpv) {
        const char* cmd[] = {"sub-add", subPath.c_str(), "select", nullptr};
        int err = mpv_command(m_mpv, cmd);
        if (err < 0) {
            Logger::error("Failed to load subtitle: " + std::string(mpv_error_string(err)));
        } else {
            updateTracks();
        }
    }
#endif
}

void MpvPlaybackEngine::setSubtitleVisibility(bool visible) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        int flag = visible ? 1 : 0;
        mpv_set_property_async(m_mpv, 0, "sub-visibility", MPV_FORMAT_FLAG, &flag);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_subVisibility = visible;
}

bool MpvPlaybackEngine::isSubtitleVisible() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_subVisibility;
}

void MpvPlaybackEngine::showOsdMessage(const std::string& text, int durationMs) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string durStr = std::to_string(durationMs);
        const char* cmd[] = {"show-text", text.c_str(), durStr.c_str(), nullptr};
        mpv_command_async(m_mpv, 0, cmd);
    }
#else
    (void)text;
    (void)durationMs;
#endif
}

void MpvPlaybackEngine::setAudioDelay(double seconds) {
    double clamped = std::clamp(seconds, -10.0, 10.0);
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_async(m_mpv, 0, "audio-delay", MPV_FORMAT_DOUBLE, &clamped);
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_audioDelay = clamped;
}

double MpvPlaybackEngine::getAudioDelay() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_audioDelay;
}

void MpvPlaybackEngine::applyAudioFilters() {
    std::string eqStr;
    if (m_equalizerEnabled) {
        static const int centerFreqs[10] = {31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000};
        std::ostringstream ss;
        ss << "lavfi=[";
        for (int i = 0; i < 10; ++i) {
            if (i > 0) ss << ",";
            ss << "equalizer=f=" << centerFreqs[i] << ":width_type=o:w=1:g=" << m_equalizerBands[i];
        }
        ss << "]";
        eqStr = ss.str();
    }

    std::string normStr;
    if (m_audioNormalizationEnabled) {
        normStr = "lavfi=[dynaudnorm=f=500:g=31:p=0.95:m=10]";
    }

    std::string combinedFilter;
    if (!eqStr.empty() && !normStr.empty()) {
        combinedFilter = eqStr + "," + normStr;
    } else if (!eqStr.empty()) {
        combinedFilter = eqStr;
    } else if (!normStr.empty()) {
        combinedFilter = normStr;
    }

#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "af", combinedFilter.c_str());
        Logger::info("Applied audio filter graph: " + (combinedFilter.empty() ? "(cleared)" : combinedFilter));
    }
#endif
}

void MpvPlaybackEngine::setEqualizerBand(int bandIndex, double gainDb) {
    if (bandIndex < 0 || bandIndex >= 10) return;
    double clamped = std::clamp(gainDb, -12.0, 12.0);
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_equalizerBands[bandIndex] = clamped;
    }
    applyAudioFilters();
}

double MpvPlaybackEngine::getEqualizerBand(int bandIndex) const {
    if (bandIndex < 0 || bandIndex >= 10) return 0.0;
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_equalizerBands[bandIndex];
}

void MpvPlaybackEngine::setEqualizerEnabled(bool enabled) {
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_equalizerEnabled = enabled;
    }
    applyAudioFilters();
}

bool MpvPlaybackEngine::isEqualizerEnabled() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_equalizerEnabled;
}

void MpvPlaybackEngine::resetEqualizer() {
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_equalizerBands = std::vector<double>(10, 0.0);
    }
    applyAudioFilters();
}

void MpvPlaybackEngine::setAudioNormalization(bool enabled) {
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_audioNormalizationEnabled = enabled;
    }
    applyAudioFilters();
}

bool MpvPlaybackEngine::isAudioNormalizationEnabled() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_audioNormalizationEnabled;
}

void MpvPlaybackEngine::setDeinterlacing(const std::string& mode) {
    std::string mpvMode = "no";
    if (mode == "on" || mode == "yes") {
        mpvMode = "yes";
    } else if (mode == "auto") {
        mpvMode = "auto";
    } else {
        mpvMode = "no";
    }

#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "deinterlace", mpvMode.c_str());
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_deinterlaceMode = mode;
}

std::string MpvPlaybackEngine::getDeinterlacing() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_deinterlaceMode;
}

void MpvPlaybackEngine::setAspectRatio(const std::string& ratio) {
#if defined(HAVE_MPV)
    if (m_mpv) {
        std::string val = (ratio == "default" || ratio.empty()) ? "-1" : ratio;
        mpv_set_property_string(m_mpv, "video-aspect-override", val.c_str());
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_aspectRatio = ratio;
}

std::string MpvPlaybackEngine::getAspectRatio() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_aspectRatio;
}

std::string MpvPlaybackEngine::getHwdecActive() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_hwdecActive;
}

std::string MpvPlaybackEngine::getVideoCodec() const {
#if defined(HAVE_MPV)
    if (m_mpv) {
        char* vc = mpv_get_property_string(m_mpv, "video-codec");
        if (vc) {
            std::string res(vc);
            mpv_free(vc);
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_videoCodec = res;
            return res;
        }
        char* vf = mpv_get_property_string(m_mpv, "video-format");
        if (vf) {
            std::string res(vf);
            mpv_free(vf);
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_videoCodec = res;
            return res;
        }
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    if (!m_videoCodec.empty()) {
        return m_videoCodec;
    }
    for (const auto& track : m_tracks) {
        if (track.type == "video" && !track.codec.empty()) {
            m_videoCodec = track.codec;
            return m_videoCodec;
        }
    }
    return m_videoCodec;
}

void MpvPlaybackEngine::setHwdecProfile(const std::string& profile) {
    Logger::info("Requesting hwdec profile change: " + profile);
#if defined(HAVE_MPV)
    if (m_mpv) {
        int err = mpv_set_property_string(m_mpv, "hwdec", profile.c_str());
        if (err < 0) {
            Logger::warning("Failed to set hwdec property: " + std::string(mpv_error_string(err)));
        }
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_requestedHwdec = profile;
}

std::string MpvPlaybackEngine::getRequestedHwdecProfile() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_requestedHwdec;
}

std::string MpvPlaybackEngine::getActiveHwdecDecoder() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_hwdecActive;
}

void MpvPlaybackEngine::setLoopPointA(double seconds) {
    double clamped = std::max(0.0, seconds);
    std::string secStr = std::to_string(clamped);
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "ab-loop-a", secStr.c_str());
    }
#endif
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_loopA = clamped;
        if (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA) {
            m_loopActive = true;
        }
    }
    showOsdMessage("Point A: " + secStr.substr(0, secStr.find('.') + 3) + "s", 1500);
}

void MpvPlaybackEngine::setLoopPointB(double seconds) {
    double clamped = std::max(0.0, seconds);
    std::string secStr = std::to_string(clamped);
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "ab-loop-b", secStr.c_str());
    }
#endif
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_loopB = clamped;
        if (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA) {
            m_loopActive = true;
        }
    }
    showOsdMessage("Point B: " + secStr.substr(0, secStr.find('.') + 3) + "s", 1500);
}

void MpvPlaybackEngine::clearLoopPointA() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "ab-loop-a", "no");
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_loopA = -1.0;
    m_loopActive = false;
    showOsdMessage("Point A cleared", 1500);
}

void MpvPlaybackEngine::clearLoopPointB() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "ab-loop-b", "no");
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_loopB = -1.0;
    m_loopActive = false;
    showOsdMessage("Point B cleared", 1500);
}

void MpvPlaybackEngine::clearLoop() {
#if defined(HAVE_MPV)
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "ab-loop-a", "no");
        mpv_set_property_string(m_mpv, "ab-loop-b", "no");
    }
#endif
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_loopA = -1.0;
    m_loopB = -1.0;
    m_loopActive = false;
    showOsdMessage("A-B repeat cleared", 1500);
}

double MpvPlaybackEngine::getLoopPointA() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_loopA;
}

double MpvPlaybackEngine::getLoopPointB() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_loopB;
}

bool MpvPlaybackEngine::isLoopActive() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_loopActive;
}

void MpvPlaybackEngine::toggleLoop() {
    bool hasBoth = false;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        hasBoth = (m_loopA >= 0.0 && m_loopB >= 0.0 && m_loopB > m_loopA);
    }

    if (hasBoth) {
        bool nowActive = false;
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_loopActive = !m_loopActive;
            nowActive = m_loopActive;
        }
#if defined(HAVE_MPV)
        if (m_mpv) {
            if (nowActive) {
                mpv_set_property_string(m_mpv, "ab-loop-a", std::to_string(m_loopA).c_str());
                mpv_set_property_string(m_mpv, "ab-loop-b", std::to_string(m_loopB).c_str());
            } else {
                mpv_set_property_string(m_mpv, "ab-loop-a", "no");
                mpv_set_property_string(m_mpv, "ab-loop-b", "no");
            }
        }
#endif
        showOsdMessage(nowActive ? "A-B repeat: Started" : "A-B repeat: Stopped", 1500);
    } else {
        clearLoop();
    }
}

bool MpvPlaybackEngine::captureFrame(const std::string& outputPath, CaptureMode mode) {
    if (!m_initialized.load()) {
        Logger::warning("Cannot capture frame: engine not initialized.");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (m_state == PlaybackState::Stopped) {
            Logger::warning("Cannot capture frame: playback is stopped.");
            return false;
        }
    }

#if defined(HAVE_MPV)
    if (!m_mpv) return false;

    const char* flag = "subtitles";
    switch (mode) {
        case CaptureMode::Video:
            flag = "video";
            break;
        case CaptureMode::Subtitles:
            flag = "subtitles";
            break;
        case CaptureMode::Window:
            flag = "window";
            break;
    }

    // screenshot-to-file <filename> <flags>
    const char* cmd[] = {"screenshot-to-file", outputPath.c_str(), flag, nullptr};
    int err = mpv_command(m_mpv, cmd);
    if (err < 0) {
        std::string errStr = mpv_error_string(err);
        Logger::warning("mpv screenshot-to-file failed: " + errStr + " (path: " + outputPath + ")");
        return false;
    }

    Logger::info("Frame captured successfully: " + outputPath + " [mode=" + std::string(flag) + "]");
    return true;
#else
    (void)outputPath;
    (void)mode;
    return true;
#endif
}

} // namespace Jonaki
