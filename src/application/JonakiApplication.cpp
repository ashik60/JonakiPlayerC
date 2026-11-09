#include "JonakiApplication.h"
#include "../ui/MainWindow.h"
#include "../core/Logger.h"
#include "../settings/SettingsManager.h"
#include <QStandardPaths>
#include <QDir>

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

    return true;
}

} // namespace Jonaki
