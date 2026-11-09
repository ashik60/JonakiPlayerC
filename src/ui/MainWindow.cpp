#include "MainWindow.h"
#include "TitleBarWidget.h"
#include "BottomControlBar.h"
#include "FramelessWindowHelper.h"
#include "ContextMenuBuilder.h"
#include "PlaylistDrawer.h"
#include "../playlist/PlaylistController.h"
#include "../playlist/ResumeStore.h"
#include "../subtitles/SubtitleManager.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include "../playback/MpvPlaybackEngine.h"
#include "../video/VideoOpenGLWidget.h"
#include "../input/ActionManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QDir>
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

    // Initialize Resume Store SQLite database in AppData
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);
    QString sdbPath = appDataDir + "/MediaInfo.sdb";
    ResumeStore::instance().initialize(sdbPath.toStdString());

    m_playlistController = std::make_unique<PlaylistController>(m_engine.get(), this);
    m_subtitleManager = std::make_unique<SubtitleManager>(m_engine.get());
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

    // 2. Middle area: Video Viewport + Dockable Playlist Drawer
    auto* middleContainer = new QWidget(central);
    auto* middleLayout = new QHBoxLayout(middleContainer);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleLayout->setSpacing(0);

    m_videoWidget = new VideoOpenGLWidget(middleContainer);
    m_videoWidget->setEngine(m_engine.get());
    middleLayout->addWidget(m_videoWidget, 1);

    m_playlistDrawer = new PlaylistDrawer(m_playlistController.get(), middleContainer);
    m_playlistDrawer->setVisible(false); // Collapsed by default, toggled via F8 or bottom button
    middleLayout->addWidget(m_playlistDrawer);

    rootLayout->addWidget(middleContainer, 1);

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
            if (m_subtitleManager) {
                m_subtitleManager->autoLoadMatchingSubtitles(file.toStdString());
            }
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

    connect(m_controlBar, &BottomControlBar::prevClicked, this, [this]() {
        if (m_playlistController) m_playlistController->playPrevious();
    });
    connect(m_controlBar, &BottomControlBar::nextClicked, this, [this]() {
        if (m_playlistController) m_playlistController->playNext();
    });
    connect(m_controlBar, &BottomControlBar::playlistClicked, this, [this]() {
        if (m_playlistDrawer) {
            bool visible = !m_playlistDrawer->isVisible();
            m_playlistDrawer->setVisible(visible);
            if (visible) {
                resize(width() + m_playlistDrawer->width(), height());
            } else {
                resize(std::max(723, width() - m_playlistDrawer->width()), height());
            }
        }
    });

    connect(m_playlistDrawer, &PlaylistDrawer::closeRequested, this, [this]() {
        if (m_playlistDrawer && m_playlistDrawer->isVisible()) {
            m_playlistDrawer->setVisible(false);
            resize(std::max(723, width() - m_playlistDrawer->width()), height());
        }
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
        case Qt::Key_F8:
            if (m_playlistDrawer) {
                bool vis = !m_playlistDrawer->isVisible();
                m_playlistDrawer->setVisible(vis);
                if (vis) {
                    resize(width() + m_playlistDrawer->width(), height());
                } else {
                    resize(std::max(723, width() - m_playlistDrawer->width()), height());
                }
            }
            break;
        case Qt::Key_PageDown:
            if (m_playlistController) m_playlistController->playNext();
            break;
        case Qt::Key_PageUp:
            if (m_playlistController) m_playlistController->playPrevious();
            break;
        case Qt::Key_Less:
        case Qt::Key_Comma:
            if (m_subtitleManager) m_subtitleManager->adjustDelay(-0.5);
            break;
        case Qt::Key_Greater:
        case Qt::Key_Period:
            if (m_subtitleManager) m_subtitleManager->adjustDelay(0.5);
            break;
        case Qt::Key_Slash:
            if (m_subtitleManager) m_subtitleManager->resetDelay();
            break;
        case Qt::Key_H:
            if (event->modifiers() & Qt::AltModifier) {
                if (m_subtitleManager) m_subtitleManager->toggleVisibility();
            }
            break;
        case Qt::Key_O:
            if (event->modifiers() & Qt::AltModifier) {
                QString file = QFileDialog::getOpenFileName(this, "Open Subtitle File", "", "Subtitles (*.srt *.ass *.ssa *.vtt *.sub *.smi)");
                if (!file.isEmpty() && m_engine) {
                    m_engine->loadExternalSubtitle(file.toStdString());
                    m_engine->showOsdMessage("Subtitle loaded: " + QFileInfo(file).fileName().toStdString());
                }
            }
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
    if (!urls.isEmpty() && m_playlistController) {
        QStringList paths;
        for (const auto& u : urls) {
            QString lp = u.toLocalFile();
            if (!lp.isEmpty()) paths.append(lp);
        }
        m_playlistController->addPaths(paths);
        if (m_playlistController->currentIndex() == -1 && m_playlistController->model()->rowCount() > 0) {
            m_playlistController->playIndex(0);
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
