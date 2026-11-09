#include "MainWindow.h"
#include "TitleBarWidget.h"
#include "BottomControlBar.h"
#include "FramelessWindowHelper.h"
#include "ContextMenuBuilder.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include "../playback/MpvPlaybackEngine.h"
#include "../video/VideoOpenGLWidget.h"
#include "../input/ActionManager.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMessageBox>

namespace Jonaki {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupEngine();
    setupUi();
    connectSignals();
    setupShortcuts();
}

MainWindow::~MainWindow() {
    if (m_videoWidget) {
        m_videoWidget->cleanupRenderContext();
    }
    if (m_engine) {
        m_engine->shutdown();
    }
}

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

    // Exactly match measured PotPlayer default dimensions: 723 x 399 px
    resize(723, 399);
    setMinimumSize(300, 160);
    setAcceptDrops(true);

    auto* central = new QWidget(this);
    central->setStyleSheet("background: #000000;");
    setCentralWidget(central);

    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Top Custom Title Bar (31px)
    m_titleBar = new TitleBarWidget(this);
    m_titleBar->setTitleText(windowTitle());
    rootLayout->addWidget(m_titleBar);

    // 2. Video Viewport
    m_videoWidget = new VideoOpenGLWidget(this);
    m_videoWidget->setEngine(m_engine.get());
    rootLayout->addWidget(m_videoWidget, 1);

    // 3. Bottom Skinned Controls (62px)
    m_controlBar = new BottomControlBar(this);
    rootLayout->addWidget(m_controlBar);

    // Initialize Frameless Aero-Snap Helper
    m_framelessHelper = std::make_unique<FramelessWindowHelper>(this);
    m_framelessHelper->setDraggableWidget(m_titleBar);

    Logger::info("MainWindow PotPlayer layout (723x399) constructed successfully.");
}

void MainWindow::connectSignals() {
    // Title bar signals
    connect(m_titleBar, &TitleBarWidget::mainMenuClicked, this, [this]() {
        auto menu = ContextMenuBuilder::buildMenu(this);
        menu->exec(m_titleBar->mapToGlobal(QPoint(0, m_titleBar->height())));
    });
    connect(m_titleBar, &TitleBarWidget::minimizeClicked, this, &MainWindow::showMinimized);
    connect(m_titleBar, &TitleBarWidget::maximizeRestoreClicked, this, [this]() {
        if (isMaximized()) showNormal();
        else showMaximized();
    });
    connect(m_titleBar, &TitleBarWidget::closeClicked, this, &MainWindow::close);
    connect(m_titleBar, &TitleBarWidget::pinClicked, this, [this]() {
        bool onTop = (windowFlags() & Qt::WindowStaysOnTopHint);
        setWindowFlag(Qt::WindowStaysOnTopHint, !onTop);
        show();
    });

    // Control bar signals
    connect(m_controlBar, &BottomControlBar::playClicked, this, [this]() {
        m_engine->play();
    });
    connect(m_controlBar, &BottomControlBar::pauseClicked, this, [this]() {
        m_engine->pause();
    });
    connect(m_controlBar, &BottomControlBar::stopClicked, this, [this]() {
        m_engine->stop();
    });
    connect(m_controlBar, &BottomControlBar::openFileClicked, this, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Open Media File");
        if (!file.isEmpty()) {
            m_engine->loadMedia(file.toStdString());
        }
    });
    connect(m_controlBar, &BottomControlBar::seekMoved, this, [this](int pos) {
        double duration = m_engine->getDuration();
        if (duration > 0.0) {
            double targetTime = (pos / 1000.0) * duration;
            m_engine->seek(targetTime, true);
        }
    });
    connect(m_controlBar, &BottomControlBar::volumeMoved, this, [this](int vol) {
        m_engine->setVolume(vol);
    });
    connect(m_controlBar, &BottomControlBar::muteToggled, this, [this]() {
        m_engine->setMute(!m_engine->isMute());
        m_controlBar->setMuted(m_engine->isMute());
    });

    // Engine callbacks
    m_engine->setOnStateChanged([this](PlaybackState state) {
        QMetaObject::invokeMethod(this, [this, state]() {
            m_controlBar->setPlaying(state == PlaybackState::Playing);
        }, Qt::QueuedConnection);
    });

    m_engine->setOnTimeChanged([this](double cur, double dur) {
        QMetaObject::invokeMethod(this, [this, cur, dur]() {
            m_controlBar->setPosition(cur, dur);
        }, Qt::QueuedConnection);
    });

    m_engine->setOnError([this](PlaybackError err, const std::string& msg) {
        (void)err;
        QMetaObject::invokeMethod(this, [this, msg]() {
            QMessageBox::warning(this, "Playback Error", QString::fromStdString(msg));
        }, Qt::QueuedConnection);
    });
}

void MainWindow::setupShortcuts() {
    // Shortcuts handled via keyPressEvent
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Space:
            m_engine->togglePlayPause();
            break;
        case Qt::Key_F:
            m_engine->stepFrame(1);
            break;
        case Qt::Key_D:
            m_engine->stepFrame(-1);
            break;
        case Qt::Key_Right:
            m_engine->seekRelative(5.0);
            break;
        case Qt::Key_Left:
            m_engine->seekRelative(-5.0);
            break;
        case Qt::Key_Up:
            m_engine->setVolume(std::min(100, m_engine->getVolume() + 5));
            m_controlBar->setVolume(m_engine->getVolume());
            break;
        case Qt::Key_Down:
            m_engine->setVolume(std::max(0, m_engine->getVolume() - 5));
            m_controlBar->setVolume(m_engine->getVolume());
            break;
        case Qt::Key_M:
            m_engine->setMute(!m_engine->isMute());
            m_controlBar->setMuted(m_engine->isMute());
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (isFullScreen()) showNormal();
            else showFullScreen();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    auto menu = ContextMenuBuilder::buildMenu(this);
    menu->exec(event->globalPos());
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const auto urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString localPath = urls.first().toLocalFile();
        if (!localPath.isEmpty()) {
            m_engine->loadMedia(localPath.toStdString());
        }
    }
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    if (m_framelessHelper && m_framelessHelper->handleNativeEvent(eventType, message, result)) {
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_videoWidget) {
        m_videoWidget->cleanupRenderContext();
    }
    if (m_engine) {
        m_engine->shutdown();
    }
    QMainWindow::closeEvent(event);
}

} // namespace Jonaki
