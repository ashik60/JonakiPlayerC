#pragma once

#include "PlaylistModel.h"
#include <QObject>
#include <QStringList>

namespace Jonaki {

class IPlaybackEngine;

enum class PlaybackOrder {
    Sequential,
    RepeatOne,
    RepeatAll,
    Shuffle
};

/**
 * @brief Controller managing playlist navigation, file scanning, playback synchronization,
 * and persistence.
 */
class PlaylistController : public QObject {
    Q_OBJECT

public:
    explicit PlaylistController(IPlaybackEngine* engine, QObject* parent = nullptr);
    ~PlaylistController() override = default;

    PlaylistModel* model() { return &m_model; }
    const PlaylistModel* model() const { return &m_model; }

    int currentIndex() const { return m_currentIndex; }
    PlaybackOrder playbackOrder() const { return m_order; }
    void setPlaybackOrder(PlaybackOrder order) { m_order = order; }

    // Navigation
    bool playIndex(int index);
    bool playNext();
    bool playPrevious();

    // Adding media
    void addPath(const QString& path);
    void addPaths(const QStringList& paths);
    void addDirectoryRecursive(const QString& dirPath);

    // Persistence
    bool loadDpl(const QString& filePath);
    bool saveDpl(const QString& filePath);

signals:
    void currentItemChanged(int index, const PlaylistItem* item);
    void playbackFinished();

private:
    void syncWithEngine();
    bool isSupportedMediaFile(const QString& path) const;

    IPlaybackEngine* m_engine = nullptr;
    PlaylistModel m_model;
    int m_currentIndex = -1;
    PlaybackOrder m_order = PlaybackOrder::Sequential;

    QString m_activeDplPath;
};

} // namespace Jonaki
