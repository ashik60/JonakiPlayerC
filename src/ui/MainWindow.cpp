#include "MainWindow.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include "../playback/MpvPlaybackEngine.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QKeyEvent>

namespace Jonaki {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUi();
    setupEngine();
    setupShortcuts();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle(QString::fromUtf8(APP_NAME.data()) + " v" + QString::fromUtf8(APP_VERSION.data()));

    auto& settings = SettingsManager::instance();
    int w = settings.getInt("Window", "Width", UI::DEFAULT_WINDOW_WIDTH);
    int h = settings.getInt("Window", "Height", UI::DEFAULT_WINDOW_HEIGHT);
    resize(w, h);
    setMinimumSize(UI::MIN_WINDOW_WIDTH, UI::MIN_WINDOW_HEIGHT);

    setAcceptDrops(true);
    Logger::info("MainWindow UI initialized.");
}

void MainWindow::setupEngine() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    if (m_engine->initialize()) {
        Logger::info("Playback engine successfully initialized in MainWindow.");
    } else {
        Logger::warning("Playback engine initialized in fallback mode.");
    }
}

void MainWindow::setupShortcuts() {
    // Shortcuts will be registered via ShortcutManager in M4
}

void MainWindow::closeEvent(QCloseEvent* event) {
    auto& settings = SettingsManager::instance();
    settings.setInt("Window", "Width", width());
    settings.setInt("Window", "Height", height());

    if (m_engine) {
        m_engine->shutdown();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        if (m_engine) {
            m_engine->togglePlayPause();
        }
        event->accept();
        return;
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
            Logger::info("Loading dropped media file: " + localFile.toStdString());
            m_engine->loadMedia(localFile.toStdString());
        }
    }
    event->acceptProposedAction();
}

} // namespace Jonaki
