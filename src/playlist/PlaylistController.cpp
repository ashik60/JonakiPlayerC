#include "PlaylistController.h"
#include "ResumeStore.h"
#include "../playback/IPlaybackEngine.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include "../network/UrlValidator.h"

#include <QDirIterator>
#include <QFileInfo>
#include <random>

namespace Jonaki {

PlaylistController::PlaylistController(IPlaybackEngine* engine, QObject* parent)
    : QObject(parent), m_engine(engine) {
    syncWithEngine();
}

void PlaylistController::syncWithEngine() {
    if (!m_engine) return;

    // Track state transitions to auto-advance at end-of-file
    m_engine->setOnStateChanged([this](PlaybackState state) {
        if (state == PlaybackState::Stopped) {
            // Save position if stopped mid-playback
            const PlaylistItem* item = m_model.getItem(m_currentIndex);
            if (item) {
                double curTime = m_engine->getCurrentTime();
                double dur = m_engine->getDuration();
                if (curTime > 5.0 && (dur - curTime) > 10.0) {
                    ResumeStore::instance().savePosition(
                        item->filePath,
                        static_cast<int64_t>(curTime * 1000),
                        static_cast<int64_t>(dur * 1000)
                    );
                } else if (curTime > 0.0 && (dur - curTime) <= 10.0) {
                    // Played to the end -> clear resume position
                    ResumeStore::instance().clearPosition(item->filePath);
                }
            }
        }
    });
}

bool PlaylistController::isSupportedMediaFile(const QString& path) const {
    static const QStringList extensions = {
        "mp4", "mkv", "avi", "mov", "wmv", "flv", "webm", "ts", "m2ts",
        "mp3", "flac", "wav", "aac", "ogg", "m4a", "opus"
    };
    QFileInfo fi(path);
    return extensions.contains(fi.suffix().toLower());
}

void PlaylistController::addPath(const QString& path) {
    QFileInfo fi(path);
    if (fi.isDir()) {
        addDirectoryRecursive(path);
    } else if (isSupportedMediaFile(path)) {
        PlaylistItem item;
        item.id = m_model.rowCount() + 1;
        item.filePath = path.toStdString();
        item.title = fi.completeBaseName().toStdString();
        m_model.addItem(item);
    }
}

void PlaylistController::addPaths(const QStringList& paths) {
    for (const auto& p : paths) {
        addPath(p);
    }
}

void PlaylistController::addDirectoryRecursive(const QString& dirPath) {
    QDirIterator it(dirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file = it.next();
        if (isSupportedMediaFile(file)) {
            PlaylistItem item;
            item.id = m_model.rowCount() + 1;
            item.filePath = file.toStdString();
            item.title = QFileInfo(file).completeBaseName().toStdString();
            m_model.addItem(item);
        }
    }
}

bool PlaylistController::playIndex(int index) {
    const PlaylistItem* item = m_model.getItem(index);
    if (!item || !m_engine) return false;

    m_currentIndex = index;
    bool success = m_engine->loadMedia(item->filePath);
    if (success) {
        QString pathStr = QString::fromStdString(item->filePath);
        if (UrlValidator::isValidMediaUrl(pathStr)) {
            bool storeEnteredOnly = SettingsManager::instance().getBool("Network", "StoreEnteredUrlsOnly", true);
            if (!storeEnteredOnly) {
                QString historyRaw = QString::fromStdString(SettingsManager::instance().getString("Network", "RecentUrls", ""));
                QStringList urls = historyRaw.split(";;", Qt::SkipEmptyParts);
                int existing = urls.indexOf(pathStr);
                if (existing >= 0) urls.removeAt(existing);
                urls.prepend(pathStr);
                while (urls.size() > 20) urls.removeLast();
                SettingsManager::instance().setString("Network", "RecentUrls", urls.join(";;").toStdString());
            }
        }

        // Check for saved resume position
        int64_t resumePosMs = 0;
        int64_t durMs = 0;
        if (ResumeStore::instance().getPosition(item->filePath, resumePosMs, durMs) && resumePosMs > 5000) {
            double seekTargetSec = resumePosMs / 1000.0;
            Logger::info("Restoring resume position: " + std::to_string(seekTargetSec) + "s for " + UrlValidator::sanitizeForLogging(pathStr).toStdString());
            m_engine->seek(seekTargetSec, true);
        }

        emit currentItemChanged(m_currentIndex, item);
    }
    return success;
}

bool PlaylistController::playNext() {
    int total = m_model.rowCount();
    if (total == 0) return false;

    int nextIdx = m_currentIndex;
    switch (m_order) {
        case PlaybackOrder::Sequential:
            nextIdx++;
            if (nextIdx >= total) {
                emit playbackFinished();
                return false;
            }
            break;
        case PlaybackOrder::RepeatOne:
            break;
        case PlaybackOrder::RepeatAll:
            nextIdx = (nextIdx + 1) % total;
            break;
        case PlaybackOrder::Shuffle: {
            if (total > 1) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, total - 1);
                nextIdx = dis(gen);
            }
            break;
        }
    }

    return playIndex(nextIdx);
}

bool PlaylistController::playPrevious() {
    int total = m_model.rowCount();
    if (total == 0) return false;

    int prevIdx = m_currentIndex - 1;
    if (prevIdx < 0) {
        prevIdx = (m_order == PlaybackOrder::RepeatAll) ? total - 1 : 0;
    }
    return playIndex(prevIdx);
}

bool PlaylistController::loadDpl(const QString& filePath) {
    PlaylistData data;
    if (!DplPlaylistParser::parseFile(filePath.toStdString(), data)) {
        return false;
    }

    m_model.setItems(data.items);
    m_activeDplPath = filePath;

    if (!data.playName.empty()) {
        for (int i = 0; i < static_cast<int>(data.items.size()); ++i) {
            if (data.items[i].filePath == data.playName) {
                m_currentIndex = i;
                break;
            }
        }
    }
    return true;
}

bool PlaylistController::saveDpl(const QString& filePath) {
    PlaylistData data;
    const auto& items = m_model.items();
    data.items = items;
    data.topIndex = 0;
    data.savePlayPos = 1;

    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(items.size())) {
        data.playName = items[m_currentIndex].filePath;
        if (m_engine) {
            data.playTimeMs = static_cast<int64_t>(m_engine->getCurrentTime() * 1000);
        }
    }

    return DplPlaylistParser::serializeToFile(filePath.toStdString(), data);
}

} // namespace Jonaki
