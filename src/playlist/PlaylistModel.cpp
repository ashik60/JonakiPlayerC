#include "PlaylistModel.h"
#include <QFileInfo>

namespace Jonaki {

PlaylistModel::PlaylistModel(QObject* parent)
    : QAbstractListModel(parent) {
}

int PlaylistModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_items.size());
}

QVariant PlaylistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_items.size())) {
        return {};
    }

    const auto& item = m_items[index.row()];

    switch (role) {
        case Qt::DisplayRole: {
            if (!item.title.empty()) {
                return QString::fromStdString(item.title);
            }
            return QFileInfo(QString::fromStdString(item.filePath)).fileName();
        }
        case FilePathRole:
            return QString::fromStdString(item.filePath);
        case TitleRole:
            return QString::fromStdString(item.title);
        case DurationRole:
            return static_cast<qlonglong>(item.durationMs);
        case StartPosRole:
            return static_cast<qlonglong>(item.startPosMs);
        default:
            return {};
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[FilePathRole] = "filePath";
    roles[TitleRole] = "title";
    roles[DurationRole] = "duration";
    roles[StartPosRole] = "startPos";
    return roles;
}

void PlaylistModel::setItems(const std::vector<PlaylistItem>& items) {
    beginResetModel();
    m_items = items;
    endResetModel();
}

void PlaylistModel::addItem(const PlaylistItem& item) {
    int row = static_cast<int>(m_items.size());
    beginInsertRows(QModelIndex(), row, row);
    m_items.push_back(item);
    endInsertRows();
}

void PlaylistModel::insertItem(int row, const PlaylistItem& item) {
    if (row < 0 || row > static_cast<int>(m_items.size())) row = static_cast<int>(m_items.size());
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(m_items.begin() + row, item);
    endInsertRows();
}

void PlaylistModel::removeItem(int row) {
    if (row < 0 || row >= static_cast<int>(m_items.size())) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_items.erase(m_items.begin() + row);
    endRemoveRows();
}

void PlaylistModel::moveItem(int fromRow, int toRow) {
    if (fromRow < 0 || fromRow >= static_cast<int>(m_items.size())) return;
    if (toRow < 0 || toRow >= static_cast<int>(m_items.size()) || fromRow == toRow) return;

    int destChild = (toRow > fromRow) ? toRow + 1 : toRow;
    if (!beginMoveRows(QModelIndex(), fromRow, fromRow, QModelIndex(), destChild)) return;

    auto item = m_items[fromRow];
    m_items.erase(m_items.begin() + fromRow);
    m_items.insert(m_items.begin() + toRow, item);
    endMoveRows();
}

void PlaylistModel::clear() {
    beginResetModel();
    m_items.clear();
    endResetModel();
}

const PlaylistItem* PlaylistModel::getItem(int row) const {
    if (row < 0 || row >= static_cast<int>(m_items.size())) return nullptr;
    return &m_items[row];
}

} // namespace Jonaki
