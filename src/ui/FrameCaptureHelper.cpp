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
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace Jonaki {

static std::atomic<int> s_activeCapturesCount{0};
static std::atomic<bool> s_cancellationRequested{false};
static std::mutex s_activeMutex;
static std::condition_variable s_activeCv;

int FrameCaptureHelper::activeCapturesCount() {
    return s_activeCapturesCount.load();
}

void FrameCaptureHelper::cancelPendingOperations() {
    s_cancellationRequested.store(true);
}

bool FrameCaptureHelper::waitForPendingOperations(int timeoutMs) {
    std::unique_lock<std::mutex> lock(s_activeMutex);
    if (s_activeCapturesCount.load() == 0) return true;
    return s_activeCv.wait_for(lock, std::chrono::milliseconds(timeoutMs), []() {
        return s_activeCapturesCount.load() == 0;
    });
}

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
    s_cancellationRequested.store(false);
    {
        std::lock_guard<std::mutex> lock(s_activeMutex);
        s_activeCapturesCount++;
    }

    bool dispatched = engine->captureFrameAsync(filePath.toStdString(), mode, [filePath, engine](bool ok, const std::string& err) {
        {
            std::lock_guard<std::mutex> lock(s_activeMutex);
            s_activeCapturesCount--;
        }
        s_activeCv.notify_all();

        if (s_cancellationRequested.load()) {
            if (QFileInfo::exists(filePath)) QFile::remove(filePath);
            return;
        }

        if (ok && QFileInfo::exists(filePath) && QFileInfo(filePath).size() > 0) {
            QString name = QFileInfo(filePath).fileName();
            if (engine) engine->showOsdMessage("Saved frame: " + name.toStdString(), 2000);
        } else {
            if (engine) engine->showOsdMessage("Frame capture failed: " + err, 2000);
            if (QFileInfo::exists(filePath) && QFileInfo(filePath).size() == 0) {
                QFile::remove(filePath);
            }
        }
    });

    if (!dispatched) {
        std::lock_guard<std::mutex> lock(s_activeMutex);
        s_activeCapturesCount--;
        s_activeCv.notify_all();
    }
    return dispatched;
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

    s_cancellationRequested.store(false);
    {
        std::lock_guard<std::mutex> lock(s_activeMutex);
        s_activeCapturesCount++;
    }

    bool dispatched = engine->captureFrameAsync(tempPath.toStdString(), mode, [tempPath, engine, mode](bool ok, const std::string& err) {
        {
            std::lock_guard<std::mutex> lock(s_activeMutex);
            s_activeCapturesCount--;
        }
        s_activeCv.notify_all();

        // RAII cleanup guard to guarantee temporary file removal
        struct TempFileGuard {
            QString p;
            ~TempFileGuard() {
                if (QFileInfo::exists(p)) QFile::remove(p);
            }
        } guard{tempPath};

        if (s_cancellationRequested.load() || !ok) {
            if (engine && !s_cancellationRequested.load()) {
                engine->showOsdMessage("Failed to capture frame for clipboard: " + err);
            }
            return;
        }

        if (!QFileInfo::exists(tempPath) || QFileInfo(tempPath).size() == 0) {
            if (engine) engine->showOsdMessage("Failed to capture frame: empty snapshot");
            return;
        }

        QImage img(tempPath);
        if (img.isNull()) {
            if (engine) engine->showOsdMessage("Failed to decode captured frame for clipboard");
            return;
        }

        // QClipboard operations must occur on the main GUI thread
        QMetaObject::invokeMethod(qApp, [img, engine, mode]() {
            QGuiApplication::clipboard()->setImage(img);
            QString modeLabel = (mode == CaptureMode::Video) ? "Source frame" : "Screen frame";
            QString osdMsg = QString("%1 copied to clipboard (%2x%3)").arg(modeLabel).arg(img.width()).arg(img.height());
            if (engine) engine->showOsdMessage(osdMsg.toStdString(), 2000);
        }, Qt::QueuedConnection);
    });

    if (!dispatched) {
        std::lock_guard<std::mutex> lock(s_activeMutex);
        s_activeCapturesCount--;
        s_activeCv.notify_all();
    }
    return dispatched;
}

void FrameCaptureHelper::openCaptureFolder(QWidget* parent) {
    (void)parent;
    QString dir = getCaptureDirectory();
    ensureDirectoryExists(dir);
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

} // namespace Jonaki
