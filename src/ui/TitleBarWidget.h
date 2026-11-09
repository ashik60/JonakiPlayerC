#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

namespace Jonaki {

/**
 * @brief PotPlayer-style custom frameless title bar (exact 31px height).
 * Houses Main Menu button, title label, and system window buttons.
 */
class TitleBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit TitleBarWidget(QWidget* parent = nullptr);

    void setTitleText(const QString& title);

signals:
    void mainMenuClicked();
    void minimizeClicked();
    void maximizeRestoreClicked();
    void closeClicked();
    void pinClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QLabel* m_lblTitle = nullptr;
    QPushButton* m_btnMainMenu = nullptr;
    QPushButton* m_btnPin = nullptr;
    QPushButton* m_btnMin = nullptr;
    QPushButton* m_btnMax = nullptr;
    QPushButton* m_btnClose = nullptr;
};

} // namespace Jonaki
