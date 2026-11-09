#include "JonakiApplication.h"
#include "../ui/MainWindow.h"
#include "../playback/IPlaybackEngine.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QTimer>
#include <QFileInfo>
#include <QPixmap>

namespace Jonaki {

JonakiApplication::JonakiApplication(int& argc, char** argv)
    : QApplication(argc, argv) {
}

JonakiApplication::~JonakiApplication() = default;

bool JonakiApplication::init() {
    // Configure paths
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);
    QString logFile = appDataDir + "/JonakiPlayer.log";
    Logger::init(logFile.toStdString());

    Logger::info("Starting JonakiPlayer...");

    // Load Settings
    QString settingsFile = appDataDir + "/JonakiPlayer.ini";
    SettingsManager::instance().loadFromFile(settingsFile.toStdString());

    // Create & Show Main Window
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->show();

    // Check command line arguments for media file and screenshot capture
    QString mediaToOpen;
    QString captureFile;
    const QStringList args = arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (args[i] == "--capture" && i + 1 < args.size()) {
            captureFile = args[++i];
        } else if (!args[i].startsWith("-") && mediaToOpen.isEmpty()) {
            mediaToOpen = args[i];
        }
    }

    if (!mediaToOpen.isEmpty() && m_mainWindow && m_mainWindow->engine()) {
        m_mainWindow->engine()->loadMedia(mediaToOpen.toStdString());
    }

    if (!captureFile.isEmpty()) {
        QTimer::singleShot(2000, [this, captureFile]() {
            if (m_mainWindow) {
                QPixmap pix = m_mainWindow->grab();
                QDir().mkpath(QFileInfo(captureFile).absolutePath());
                pix.save(captureFile, "PNG");
                Logger::info("Captured window pixmap to " + captureFile.toStdString());
            }
            quit();
        });
    }

    return true;
}

} // namespace Jonaki
