#include "SubtitleManager.h"
#include "../playback/IPlaybackEngine.h"
#include "../core/Logger.h"

#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>

namespace Jonaki {

SubtitleManager::SubtitleManager(IPlaybackEngine* engine)
    : m_engine(engine) {
}

QStringList SubtitleManager::findMatchingSubtitles(const std::string& mediaPath) const {
    QFileInfo mediaInfo(QString::fromStdString(mediaPath));
    if (!mediaInfo.exists()) return {};

    QDir parentDir = mediaInfo.dir();
    QString baseName = mediaInfo.completeBaseName();

    static const QStringList subExts = {
        "srt", "ass", "ssa", "vtt", "sub", "smi", "idx", "txt"
    };

    struct Candidate {
        QString path;
        int tier;          // 0 = exact base, 1 = language tag (base.lang), 2 = other suffix (base.*)
        int formatRank;    // 0 = ass/ssa, 1 = srt, 2 = vtt, 3 = other
        QString tag;       // language or extra tag
    };

    std::vector<Candidate> candidates;

    const auto entries = parentDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (const auto& entry : entries) {
        QString ext = entry.suffix().toLower();
        if (subExts.contains(ext)) {
            QString name = entry.completeBaseName();
            if (name == baseName) {
                int formatRank = (ext == "ass" || ext == "ssa") ? 0 : (ext == "srt" ? 1 : (ext == "vtt" ? 2 : 3));
                candidates.push_back({entry.absoluteFilePath(), 0, formatRank, ""});
            } else if (name.startsWith(baseName + ".")) {
                QString suffix = name.mid(baseName.length() + 1);
                int formatRank = (ext == "ass" || ext == "ssa") ? 0 : (ext == "srt" ? 1 : (ext == "vtt" ? 2 : 3));
                // If suffix is 2-3 characters (e.g. "en", "bn", "eng"), treat as tier 1 language tag
                int tier = (suffix.length() <= 3 && !suffix.contains('.')) ? 1 : 2;
                candidates.push_back({entry.absoluteFilePath(), tier, formatRank, suffix});
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        if (a.tier != b.tier) return a.tier < b.tier;
        if (a.formatRank != b.formatRank) return a.formatRank < b.formatRank;
        if (a.tag != b.tag) return a.tag < b.tag;
        return a.path < b.path;
    });

    QStringList result;
    for (const auto& c : candidates) {
        result.append(c.path);
    }
    return result;
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
