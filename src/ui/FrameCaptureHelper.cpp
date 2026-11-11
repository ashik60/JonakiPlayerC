#include "FrameCaptureHelper.h"
#include "../settings/SettingsManager.h"
#include "../core/Logger.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QUuid>
#include <QImage>
#include <QGuiApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <thread>

namespace Jonaki {

QString FrameCaptureHelper::defaultCaptureDirectory() {
    QString picDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (picDir.isEmpty()) {
        picDir = QDir::homePath() + "/Pictures";
    }
    return picDir + "/JonakiPlayer";
}

QString FrameCaptureHelper::getCaptureDirectory() {
    std::string custom = SettingsManager::instance().getString("Capture", "Directory", "");
    if (!custom.empty()) {
        return QString::fromStdString(custom);
    }
    return defaultCaptureDirectory();
}

void FrameCaptureHelper::setCaptureDirectory(const QString& dir) {
    SettingsManager::instance().setString("Capture", "Directory", dir.toStdString());
}

bool FrameCaptureHelper::ensureDirectoryExists(const QString& dirPath) {
    QDir d(dirPath);
    if (!d.exists()) {
        if (!d.mkpath(".")) {
            Logger::error("Failed to create capture directory: " + dirPath.toStdString());
            return false;
        }
    }
    QFileInfo info(dirPath);
    return info.isDir() && info.isWritable();
}

QString FrameCaptureHelper::generateUniqueFilePath(const QString& dirPath, const QString& prefix, const QString& ext) {
    ensureDirectoryExists(dirPath);

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
    QString baseName = QString("%1_%2").arg(prefix, timestamp);
    QString candidate = QString("%1/%2.%3").arg(dirPath, baseName, ext);

    // Collision invariant: Never overwrite existing files
    int counter = 1;
    while (QFileInfo::exists(candidate)) {
        candidate = QString("%1/%2_%3.%4").arg(dirPath, baseName, QString::number(counter).rightJustified(2, '0'), ext);
        counter++;
    }

    return QDir::toNativeSeparators(candidate);
}

bool FrameCaptureHelper::captureToFile(IPlaybackEngine* engine, CaptureMode mode, QWidget* parent) {
    (void)parent;
    if (!engine || !engine->isInitialized() || engine->getState() == PlaybackState::Stopped) {
        if (engine) engine->showOsdMessage("Cannot capture frame: No active playback");
        return false;
    }

    QString dir = getCaptureDirectory();
    if (!ensureDirectoryExists(dir)) {
        engine->showOsdMessage("Capture failed: Cannot access destination folder");
        return false;
    }

    QString filePath = generateUniqueFilePath(dir);
    std::string utf8Path = filePath.toStdString();

    // Execute capture asynchronously to keep GUI thread responsive
    std::thread([engine, utf8Path, mode, filePath]() {
        bool ok = engine->captureFrame(utf8Path, mode);
        if (ok && QFileInfo::exists(filePath) && QFileInfo(filePath).size() > 0) {
            QString name = QFileInfo(filePath).fileName();
            engine->showOsdMessage("Saved frame: " + name.toStdString(), 2000);
        } else {
            engine->showOsdMessage("Frame capture failed", 2000);
        }
    }).detach();

    return true;
}

bool FrameCaptureHelper::captureToClipboard(IPlaybackEngine* engine, CaptureMode mode, QWidget* parent) {
    (void)parent;
    if (!engine || !engine->isInitialized() || engine->getState() == PlaybackState::Stopped) {
        if (engine) engine->showOsdMessage("Cannot copy frame: No active playback");
        return false;
    }

    QString tempDir = QDir::tempPath();
    QString tempFileName = QString("jonaki_clip_%1.png").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    QString tempPath = QDir::toNativeSeparators(tempDir + "/" + tempFileName);

    bool ok = engine->captureFrame(tempPath.toStdString(), mode);
    if (!ok || !QFileInfo::exists(tempPath) || QFileInfo(tempPath).size() == 0) {
        engine->showOsdMessage("Failed to capture frame for clipboard");
        if (QFileInfo::exists(tempPath)) QFile::remove(tempPath);
        return false;
    }

    QImage img(tempPath);
    QFile::remove(tempPath); // Clean up temp file immediately

    if (img.isNull()) {
        engine->showOsdMessage("Failed to decode captured frame for clipboard");
        return false;
    }

    QGuiApplication::clipboard()->setImage(img);
    QString modeLabel = (mode == CaptureMode::Video) ? "Source frame" : "Screen frame";
    QString osdMsg = QString("%1 copied to clipboard (%2x%3)").arg(modeLabel).arg(img.width()).arg(img.height());
    engine->showOsdMessage(osdMsg.toStdString(), 2000);

    return true;
}

void FrameCaptureHelper::openCaptureFolder(QWidget* parent) {
    (void)parent;
    QString dir = getCaptureDirectory();
    ensureDirectoryExists(dir);
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

} // namespace Jonaki
