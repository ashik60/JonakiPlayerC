#pragma once

#include "DplPlaylistParser.h"
#include <QAbstractListModel>
#include <vector>
#include <memory>

namespace Jonaki {

/**
 * @brief Qt Model representing items in the current playlist queue.
 */
class PlaylistModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum PlaylistRoles {
        FilePathRole = Qt::UserRole + 1,
        TitleRole,
        DurationRole,
        StartPosRole
    };

    explicit PlaylistModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItems(const std::vector<PlaylistItem>& items);
    const std::vector<PlaylistItem>& items() const { return m_items; }

    void addItem(const PlaylistItem& item);
    void insertItem(int row, const PlaylistItem& item);
    void removeItem(int row);
    void moveItem(int fromRow, int toRow);
    void clear();

    const PlaylistItem* getItem(int row) const;

private:
    std::vector<PlaylistItem> m_items;
};

} // namespace Jonaki
