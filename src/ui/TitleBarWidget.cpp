#include "TitleBarWidget.h"
#include <QPainter>
#include <QMouseEvent>

namespace Jonaki {

TitleBarWidget::TitleBarWidget(QWidget* parent)
    : QWidget(parent) {
    setFixedHeight(31);
    setProperty("isTitleBarDraggable", true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 8, 0);
    layout->setSpacing(4);

    // Main Menu Button (89x30)
    m_btnMainMenu = new QPushButton(this);
    m_btnMainMenu->setFixedSize(89, 28);
    m_btnMainMenu->setText("Jonaki ▼");
    m_btnMainMenu->setStyleSheet(
        "QPushButton { background: #26262a; color: #d0d0d0; border: 1px solid #3c3c40; font-family: Tahoma; font-size: 11px; font-weight: bold; border-radius: 2px; } "
        "QPushButton:hover { background: #323236; color: #ffffff; } "
        "QPushButton:pressed { background: #1c1c1e; }"
    );
    connect(m_btnMainMenu, &QPushButton::clicked, this, &TitleBarWidget::mainMenuClicked);
    layout->addWidget(m_btnMainMenu);

    // Dynamic Title Label
    m_lblTitle = new QLabel(this);
    m_lblTitle->setStyleSheet("color: #a0a0a5; font-family: Tahoma; font-size: 11px;");
    m_lblTitle->setText("JonakiPlayer");
    m_lblTitle->setProperty("isTitleBarDraggable", true);
    layout->addWidget(m_lblTitle, 1);

    // Always on top / Pin Button
    m_btnPin = new QPushButton("📌", this);
    m_btnPin->setFixedSize(16, 16);
    m_btnPin->setStyleSheet(
        "QPushButton { background: transparent; color: #888888; border: none; font-size: 10px; } "
        "QPushButton:hover { color: #ffffff; }"
    );
    connect(m_btnPin, &QPushButton::clicked, this, &TitleBarWidget::pinClicked);
    layout->addWidget(m_btnPin);

    // Minimize Button
    m_btnMin = new QPushButton("─", this);
    m_btnMin->setFixedSize(16, 16);
    m_btnMin->setStyleSheet(
        "QPushButton { background: transparent; color: #a0a0a0; border: none; font-size: 10px; font-weight: bold; } "
        "QPushButton:hover { background: #38383c; color: #ffffff; }"
    );
    connect(m_btnMin, &QPushButton::clicked, this, &TitleBarWidget::minimizeClicked);
    layout->addWidget(m_btnMin);

    // Maximize/Restore Button
    m_btnMax = new QPushButton("▢", this);
    m_btnMax->setFixedSize(16, 16);
    m_btnMax->setStyleSheet(
        "QPushButton { background: transparent; color: #a0a0a0; border: none; font-size: 10px; } "
        "QPushButton:hover { background: #38383c; color: #ffffff; }"
    );
    connect(m_btnMax, &QPushButton::clicked, this, &TitleBarWidget::maximizeRestoreClicked);
    layout->addWidget(m_btnMax);

    // Close Button
    m_btnClose = new QPushButton("✕", this);
    m_btnClose->setFixedSize(16, 16);
    m_btnClose->setStyleSheet(
        "QPushButton { background: transparent; color: #a0a0a0; border: none; font-size: 10px; } "
        "QPushButton:hover { background: #c42b1c; color: #ffffff; }"
    );
    connect(m_btnClose, &QPushButton::clicked, this, &TitleBarWidget::closeClicked);
    layout->addWidget(m_btnClose);
}

void TitleBarWidget::setTitleText(const QString& title) {
    if (m_lblTitle) {
        m_lblTitle->setText(title);
    }
}

void TitleBarWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    // Draw 31px title bar charcoal gradient matching TopCommon.txt
    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0.0, QColor(36, 36, 38));
    grad.setColorAt(1.0, QColor(24, 24, 26));
    p.fillRect(rect(), grad);

    // 1px top highlight and bottom separator
    p.setPen(QColor(58, 58, 60));
    p.drawLine(0, 0, width(), 0);
    p.setPen(QColor(18, 18, 20));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void TitleBarWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeRestoreClicked();
    }
}

} // namespace Jonaki
