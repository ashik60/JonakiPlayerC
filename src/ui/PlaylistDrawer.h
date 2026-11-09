#pragma once

#include <QWidget>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace Jonaki {

class PlaylistController;

/**
 * @brief PotPlayer-style dockable / collapsible right-side playlist panel.
 * Default width 280px, toggled via F8.
 */
class PlaylistDrawer : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistDrawer(PlaylistController* controller, QWidget* parent = nullptr);
    ~PlaylistDrawer() override = default;

signals:
    void closeRequested();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupUi();

    PlaylistController* m_controller = nullptr;
    QListView* m_listView = nullptr;
    QPushButton* m_btnAdd = nullptr;
    QPushButton* m_btnRemove = nullptr;
    QPushButton* m_btnClear = nullptr;
    QPushButton* m_btnOrder = nullptr;
};

} // namespace Jonaki
