#include "MainWindow.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include "../playback/MpvPlaybackEngine.h"
#include "../video/VideoOpenGLWidget.h"
#include "../input/ActionManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QKeyEvent>
#include <QMessageBox>

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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupEngine();
    setupUi();
    connectSignals();
    setupShortcuts();
}

MainWindow::~MainWindow() = default;

IPlaybackEngine* MainWindow::engine() const {
    return m_engine.get();
}

void MainWindow::setupEngine() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    if (!m_engine->initialize()) {
        Logger::error("Failed to initialize MpvPlaybackEngine in MainWindow.");
    }
}

void MainWindow::setupUi() {
    setWindowTitle(QString::fromUtf8(APP_NAME.data()) + " v" + QString::fromUtf8(APP_VERSION.data()));

    auto& settings = SettingsManager::instance();
    // Use measured PotPlayer idle dimension (723 x 399) as baseline
    int w = settings.getInt("Window", "Width", 723);
    int h = settings.getInt("Window", "Height", 399);
    resize(w, h);
    setMinimumSize(320, 200);
    setAcceptDrops(true);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. Video surface
    m_videoWidget = new VideoOpenGLWidget(this);
    m_videoWidget->setEngine(m_engine.get());
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_videoWidget, 1);

    // 2. Control bar container
    auto* controlBar = new QWidget(this);
    controlBar->setFixedHeight(62);
    controlBar->setStyleSheet("background-color: #1e1e1e; color: #ffffff;");
    auto* controlLayout = new QVBoxLayout(controlBar);
    controlLayout->setContentsMargins(8, 4, 8, 4);
    controlLayout->setSpacing(4);

    // Top sub-row: Seekbar + Time label
    auto* seekLayout = new QHBoxLayout();
    seekLayout->setContentsMargins(0, 0, 0, 0);
    seekLayout->setSpacing(8);

    m_seekSlider = new QSlider(Qt::Horizontal, controlBar);
    m_seekSlider->setRange(0, 1000);
    m_seekSlider->setValue(0);
    seekLayout->addWidget(m_seekSlider, 1);

    m_lblTime = new QLabel("00:00 / 00:00", controlBar);
    m_lblTime->setStyleSheet("color: #cccccc; font-family: Tahoma; font-size: 11px;");
    seekLayout->addWidget(m_lblTime);
    controlLayout->addLayout(seekLayout);

    // Bottom sub-row: Buttons + Volume
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(6);

    m_btnPlayPause = new QPushButton("▶ Play", controlBar);
    m_btnPlayPause->setFixedWidth(75);
    btnLayout->addWidget(m_btnPlayPause);

    m_btnStop = new QPushButton("■ Stop", controlBar);
    m_btnStop->setFixedWidth(60);
    btnLayout->addWidget(m_btnStop);

    m_btnOpenFile = new QPushButton("📂 Open", controlBar);
    m_btnOpenFile->setFixedWidth(65);
    btnLayout->addWidget(m_btnOpenFile);

    btnLayout->addStretch(1);

    m_btnMute = new QPushButton("🔊", controlBar);
    m_btnMute->setFixedWidth(32);
    btnLayout->addWidget(m_btnMute);

    m_volumeSlider = new QSlider(Qt::Horizontal, controlBar);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(settings.getInt("Playback", "Volume", 100));
    m_volumeSlider->setFixedWidth(80);
    btnLayout->addWidget(m_volumeSlider);

    controlLayout->addLayout(btnLayout);
    mainLayout->addWidget(controlBar, 0);

    Logger::info("MainWindow UI and video surface created.");
}

void MainWindow::connectSignals() {
    connect(m_btnPlayPause, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);
    connect(m_btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(m_btnOpenFile, &QPushButton::clicked, this, &MainWindow::onOpenFileClicked);
    connect(m_btnMute, &QPushButton::clicked, this, &MainWindow::onMuteClicked);

    connect(m_seekSlider, &QSlider::sliderMoved, this, &MainWindow::onSeekSliderMoved);
    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]() { m_userSeeking = true; });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]() {
        m_userSeeking = false;
        double dur = m_engine->getDuration();
        if (dur > 0) {
            double targetTime = (m_seekSlider->value() / 1000.0) * dur;
            m_engine->seek(targetTime, true);
        }
    });

    connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeSliderMoved);

    // Playback Engine callbacks
    m_engine->setOnStateChanged([this](PlaybackState state) {
        QMetaObject::invokeMethod(this, [this, state]() {
            if (state == PlaybackState::Playing) {
                m_btnPlayPause->setText("❚❚ Pause");
            } else {
                m_btnPlayPause->setText("▶ Play");
            }
        });
    });

    m_engine->setOnTimeChanged([this](double currentTime, double duration) {
        QMetaObject::invokeMethod(this, [this, currentTime, duration]() {
            if (!m_userSeeking && duration > 0) {
                int pos = static_cast<int>((currentTime / duration) * 1000.0);
                m_seekSlider->setValue(pos);
            }
            m_lblTime->setText(formatTime(currentTime) + " / " + formatTime(duration));
        });
    });

    m_engine->setOnError([this](PlaybackError err, const std::string& msg) {
        (void)err;
        QMetaObject::invokeMethod(this, [this, msg]() {
            Logger::error("Playback Error: " + msg);
        });
    });
}

void MainWindow::setupShortcuts() {
    // Basic shortcuts mapped in keyPressEvent
}

void MainWindow::onPlayPauseClicked() {
    if (m_engine) {
        m_engine->togglePlayPause();
    }
}

void MainWindow::onStopClicked() {
    if (m_engine) {
        m_engine->stop();
        m_seekSlider->setValue(0);
        m_lblTime->setText("00:00 / 00:00");
    }
}

void MainWindow::onOpenFileClicked() {
    QString filter = "Media Files (*.mp4 *.mkv *.avi *.mp3 *.flac *.wav *.ogg *.webm *.m4v *.ts);;All Files (*.*)";
    QString filePath = QFileDialog::getOpenFileName(this, "Open Media File", "", filter);
    if (!filePath.isEmpty()) {
        m_engine->loadMedia(filePath.toStdString());
    }
}

void MainWindow::onOpenUrlClicked() {
    bool ok = false;
    QString url = QInputDialog::getText(this, "Open URL", "Enter stream URL (HTTP/HTTPS/HLS):", QLineEdit::Normal, "", &ok);
    if (ok && !url.isEmpty()) {
        m_engine->loadMedia(url.toStdString());
    }
}

void MainWindow::onSeekSliderMoved(int position) {
    double dur = m_engine->getDuration();
    if (dur > 0) {
        double cur = (position / 1000.0) * dur;
        m_lblTime->setText(formatTime(cur) + " / " + formatTime(dur));
    }
}

void MainWindow::onVolumeSliderMoved(int position) {
    if (m_engine) {
        m_engine->setVolume(position);
    }
    auto& settings = SettingsManager::instance();
    settings.setInt("Playback", "Volume", position);
}

void MainWindow::onMuteClicked() {
    if (m_engine) {
        bool newMute = !m_engine->isMute();
        m_engine->setMute(newMute);
        m_btnMute->setText(newMute ? "🔇" : "🔊");
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    auto& settings = SettingsManager::instance();
    settings.setInt("Window", "Width", width());
    settings.setInt("Window", "Height", height());
    settings.setInt("Playback", "Volume", m_volumeSlider->value());

    if (m_engine) {
        m_engine->shutdown();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Space:
            onPlayPauseClicked();
            event->accept();
            return;
        case Qt::Key_Left:
            if (m_engine) m_engine->seekRelative(-5.0);
            event->accept();
            return;
        case Qt::Key_Right:
            if (m_engine) m_engine->seekRelative(5.0);
            event->accept();
            return;
        case Qt::Key_Up:
            m_volumeSlider->setValue(m_volumeSlider->value() + 5);
            event->accept();
            return;
        case Qt::Key_Down:
            m_volumeSlider->setValue(m_volumeSlider->value() - 5);
            event->accept();
            return;
        case Qt::Key_M:
            onMuteClicked();
            event->accept();
            return;
        case Qt::Key_F:
            if (m_engine) m_engine->stepFrame(1);
            event->accept();
            return;
        case Qt::Key_D:
            if (m_engine) m_engine->stepFrame(-1);
            event->accept();
            return;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (isFullScreen()) {
                showNormal();
            } else {
                showFullScreen();
            }
            event->accept();
            return;
        default:
            break;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const auto urls = event->mimeData()->urls();
    if (!urls.isEmpty() && m_engine) {
        QString localFile = urls.first().toLocalFile();
        if (!localFile.isEmpty()) {
            Logger::info("Media dropped: " + localFile.toStdString());
            m_engine->loadMedia(localFile.toStdString());
        }
    }
    event->acceptProposedAction();
}

} // namespace Jonaki
