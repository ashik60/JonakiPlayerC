#include "BottomControlBar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

namespace Jonaki {

static QString formatTime(double seconds) {
    if (seconds < 0) seconds = 0;
    int totalSec = static_cast<int>(seconds);
    int h = totalSec / 3600;
    int m = (totalSec % 3600) / 60;
    int s = totalSec % 60;

    if (h > 0) {
        return QString::asprintf("%02d:%02d:%02d", h, m, s);
    }
    return QString::asprintf("%02d:%02d", m, s);
}

BottomControlBar::BottomControlBar(QWidget* parent)
    : QWidget(parent) {
    setFixedHeight(62); // 22px seek row + 40px buttons row

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSeekRow();
    setupButtonRow();

    mainLayout->addWidget(m_seekRow);
    mainLayout->addWidget(m_buttonRow);
}

void BottomControlBar::setupSeekRow() {
    m_seekRow = new QWidget(this);
    m_seekRow->setFixedHeight(22);

    auto* layout = new QHBoxLayout(m_seekRow);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(6);

    // Timeline seekbar
    m_seekSlider = new QSlider(Qt::Horizontal, m_seekRow);
    m_seekSlider->setRange(0, 1000);
    m_seekSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #2d2d30; border-radius: 2px; } "
        "QSlider::sub-page:horizontal { background: #f0b414; border-radius: 2px; } "
        "QSlider::handle:horizontal { background: #d0d0d5; border: 1px solid #646468; width: 10px; margin: -3px 0; border-radius: 3px; } "
        "QSlider::handle:horizontal:hover { background: #ffffff; }"
    );
    connect(m_seekSlider, &QSlider::sliderMoved, this, [this](int pos) {
        m_userSeeking = true;
        emit seekMoved(pos);
    });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]() {
        m_userSeeking = false;
        emit seekMoved(m_seekSlider->value());
    });
    layout->addWidget(m_seekSlider, 1);

    // Mute Button (13x10)
    m_btnMute = new QPushButton("🔊", m_seekRow);
    m_btnMute->setFixedSize(18, 16);
    m_btnMute->setStyleSheet(
        "QPushButton { background: transparent; color: #a0a0a0; border: none; font-size: 10px; } "
        "QPushButton:hover { color: #ffffff; }"
    );
    connect(m_btnMute, &QPushButton::clicked, this, &BottomControlBar::muteToggled);
    layout->addWidget(m_btnMute);

    // Volume Slider (70x10)
    m_volumeSlider = new QSlider(Qt::Horizontal, m_seekRow);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->setFixedWidth(70);
    m_volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #2d2d30; border-radius: 2px; } "
        "QSlider::sub-page:horizontal { background: #f0b414; border-radius: 2px; } "
        "QSlider::handle:horizontal { background: #c8c8cd; width: 8px; margin: -2px 0; border-radius: 2px; } "
        "QSlider::handle:horizontal:hover { background: #ffffff; }"
    );
    connect(m_volumeSlider, &QSlider::sliderMoved, this, &BottomControlBar::volumeMoved);
    layout->addWidget(m_volumeSlider);
}

void BottomControlBar::setupButtonRow() {
    m_buttonRow = new QWidget(this);
    m_buttonRow->setFixedHeight(40);

    auto* layout = new QHBoxLayout(m_buttonRow);
    layout->setContentsMargins(6, 0, 8, 0);
    layout->setSpacing(4);

    auto makeBtn = [](const QString& text, int w = 41, int h = 36) {
        auto* btn = new QPushButton(text);
        btn->setFixedSize(w, h);
        btn->setStyleSheet(
            "QPushButton { background: transparent; color: #d0d0d0; border: none; font-size: 13px; font-weight: bold; border-radius: 3px; } "
            "QPushButton:hover { background: #323236; color: #ffffff; } "
            "QPushButton:pressed { background: #18181a; }"
        );
        return btn;
    };

    m_btnPlay = makeBtn("▶");
    connect(m_btnPlay, &QPushButton::clicked, this, &BottomControlBar::playClicked);
    layout->addWidget(m_btnPlay);

    m_btnPause = makeBtn("❚❚");
    m_btnPause->setVisible(false);
    connect(m_btnPause, &QPushButton::clicked, this, &BottomControlBar::pauseClicked);
    layout->addWidget(m_btnPause);

    m_btnStop = makeBtn("■");
    connect(m_btnStop, &QPushButton::clicked, this, &BottomControlBar::stopClicked);
    layout->addWidget(m_btnStop);

    m_btnPrev = makeBtn("⏮");
    connect(m_btnPrev, &QPushButton::clicked, this, &BottomControlBar::prevClicked);
    layout->addWidget(m_btnPrev);

    m_btnNext = makeBtn("⏭");
    connect(m_btnNext, &QPushButton::clicked, this, &BottomControlBar::nextClicked);
    layout->addWidget(m_btnNext);

    m_btnOpen = makeBtn("📂");
    connect(m_btnOpen, &QPushButton::clicked, this, &BottomControlBar::openFileClicked);
    layout->addWidget(m_btnOpen);

    // Time Indicators
    layout->addSpacing(8);
    m_lblCurrentTime = new QLabel("00:00:00", m_buttonRow);
    m_lblCurrentTime->setStyleSheet("color: #dcdcdc; font-family: Tahoma; font-size: 11px;");
    layout->addWidget(m_lblCurrentTime);

    auto* sep = new QLabel("/", m_buttonRow);
    sep->setStyleSheet("color: #707070; font-family: Tahoma; font-size: 11px;");
    layout->addWidget(sep);

    m_lblTotalTime = new QLabel("00:00:00", m_buttonRow);
    m_lblTotalTime->setStyleSheet("color: #909090; font-family: Tahoma; font-size: 11px;");
    layout->addWidget(m_lblTotalTime);

    layout->addStretch(1);

    // Right drawers: Playlist & Control box
    m_btnControlBox = makeBtn("🎛", 26, 26);
    connect(m_btnControlBox, &QPushButton::clicked, this, &BottomControlBar::controlBoxClicked);
    layout->addWidget(m_btnControlBox);

    m_btnPlaylist = makeBtn("☰", 26, 26);
    connect(m_btnPlaylist, &QPushButton::clicked, this, &BottomControlBar::playlistClicked);
    layout->addWidget(m_btnPlaylist);
}

void BottomControlBar::setPlaying(bool playing) {
    m_btnPlay->setVisible(!playing);
    m_btnPause->setVisible(playing);
}

void BottomControlBar::setPosition(double currentSec, double totalSec) {
    if (m_lblCurrentTime) m_lblCurrentTime->setText(formatTime(currentSec));
    if (m_lblTotalTime) m_lblTotalTime->setText(formatTime(totalSec));

    if (!m_userSeeking && m_seekSlider && totalSec > 0.0) {
        int pos = static_cast<int>((currentSec / totalSec) * 1000.0);
        m_seekSlider->blockSignals(true);
        m_seekSlider->setValue(pos);
        m_seekSlider->blockSignals(false);
    }
}

void BottomControlBar::setVolume(int vol) {
    if (m_volumeSlider) {
        m_volumeSlider->blockSignals(true);
        m_volumeSlider->setValue(vol);
        m_volumeSlider->blockSignals(false);
    }
}

void BottomControlBar::setMuted(bool muted) {
    if (m_btnMute) {
        m_btnMute->setText(muted ? "🔇" : "🔊");
    }
}

void BottomControlBar::setLoopPoints(double aSec, double bSec, double totalSec) {
    m_loopASec = aSec;
    m_loopBSec = bSec;
    m_totalDurationSec = totalSec;
    update();
}

void BottomControlBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    // Draw 22px seek rail row background
    p.fillRect(0, 0, width(), 22, QColor(32, 32, 34));
    p.setPen(QColor(45, 45, 48));
    p.drawLine(0, 0, width(), 0);

    // Draw A-B loop timeline markers on the seek rail if active
    if (m_seekSlider && m_totalDurationSec > 0.0) {
        QRect sliderGeo = m_seekSlider->geometry();
        int railLeft = m_seekRow->x() + sliderGeo.x() + 5;
        int railWidth = sliderGeo.width() - 10;
        int railY = m_seekRow->y() + sliderGeo.y();

        if (m_loopASec >= 0.0 && m_loopASec <= m_totalDurationSec) {
            int markerAX = railLeft + static_cast<int>((m_loopASec / m_totalDurationSec) * railWidth);
            p.setPen(QPen(QColor(0, 200, 255), 2)); // Cyan for Point A
            p.drawLine(markerAX, railY + 2, markerAX, railY + 18);
            p.fillRect(markerAX - 2, railY + 1, 5, 4, QColor(0, 200, 255));
        }

        if (m_loopBSec >= 0.0 && m_loopBSec <= m_totalDurationSec) {
            int markerBX = railLeft + static_cast<int>((m_loopBSec / m_totalDurationSec) * railWidth);
            p.setPen(QPen(QColor(255, 80, 80), 2)); // Red/Orange for Point B
            p.drawLine(markerBX, railY + 2, markerBX, railY + 18);
            p.fillRect(markerBX - 2, railY + 15, 5, 4, QColor(255, 80, 80));
        }
    }

    // Draw 40px buttons row background
    p.fillRect(0, 22, width(), 40, QColor(24, 24, 26));
    p.setPen(QColor(38, 38, 40));
    p.drawLine(0, 22, width(), 22);
}

} // namespace Jonaki
