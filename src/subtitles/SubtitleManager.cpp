#include "SubtitleManager.h"
#include "../playback/IPlaybackEngine.h"
#include "../core/Logger.h"

#include <QFileInfo>
#include <QDir>
#include <sstream>
#include <iomanip>

namespace Jonaki {

SubtitleManager::SubtitleManager(IPlaybackEngine* engine)
    : m_engine(engine) {
}

QStringList SubtitleManager::findMatchingSubtitles(const std::string& mediaPath) const {
    QStringList matches;
    QFileInfo mediaInfo(QString::fromStdString(mediaPath));
    if (!mediaInfo.exists()) return matches;

    QDir parentDir = mediaInfo.dir();
    QString baseName = mediaInfo.completeBaseName();

    static const QStringList subExts = {
        "srt", "ass", "ssa", "vtt", "sub", "smi", "idx", "txt"
    };

    const auto entries = parentDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (const auto& entry : entries) {
        QString ext = entry.suffix().toLower();
        if (subExts.contains(ext)) {
            QString name = entry.completeBaseName();
            // Direct basename match: e.g. "movie.srt" or "movie.en.srt"
            if (name == baseName || name.startsWith(baseName + ".")) {
                matches.append(entry.absoluteFilePath());
            }
        }
    }

    return matches;
}

void SubtitleManager::autoLoadMatchingSubtitles(const std::string& mediaPath) {
    if (!m_engine) return;
    QStringList subs = findMatchingSubtitles(mediaPath);
    for (const auto& sub : subs) {
        Logger::info("Auto-loading matching subtitle: " + sub.toStdString());
        m_engine->loadExternalSubtitle(sub.toStdString());
    }
}

void SubtitleManager::toggleVisibility() {
    if (!m_engine) return;
    bool newVis = !m_engine->isSubtitleVisible();
    m_engine->setSubtitleVisibility(newVis);
    m_engine->showOsdMessage(newVis ? "Subtitles: Show" : "Subtitles: Hide");
}

void SubtitleManager::setVisibility(bool visible) {
    if (!m_engine) return;
    m_engine->setSubtitleVisibility(visible);
    m_engine->showOsdMessage(visible ? "Subtitles: Show" : "Subtitles: Hide");
}

bool SubtitleManager::isVisible() const {
    return m_engine ? m_engine->isSubtitleVisible() : false;
}

void SubtitleManager::adjustDelay(double deltaSeconds) {
    if (!m_engine) return;
    double newDelay = m_engine->getSubtitleDelay() + deltaSeconds;
    m_engine->setSubtitleDelay(newDelay);

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Subtitle Sync: " << (newDelay >= 0 ? "+" : "") << newDelay << " s";
    m_engine->showOsdMessage(ss.str());
}

void SubtitleManager::resetDelay() {
    if (!m_engine) return;
    m_engine->setSubtitleDelay(0.0);
    m_engine->showOsdMessage("Subtitle Sync: 0.0 s (Default)");
}

double SubtitleManager::currentDelay() const {
    return m_engine ? m_engine->getSubtitleDelay() : 0.0;
}

} // namespace Jonaki
