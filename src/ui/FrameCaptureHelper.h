#pragma once

#include <QString>
#include <QWidget>
#include "../playback/IPlaybackEngine.h"

namespace Jonaki {

/**
 * @brief Manages frame capture operations, directory configuration, collision avoidance,
 * and clipboard snapshot placement in the Qt application layer.
 */
class FrameCaptureHelper {
public:
    static QString defaultCaptureDirectory();
    static QString getCaptureDirectory();
    static void setCaptureDirectory(const QString& dir);
    static bool ensureDirectoryExists(const QString& dirPath);

    static QString generateUniqueFilePath(const QString& dirPath, const QString& prefix = "Jonaki", const QString& ext = "png");

    static bool captureToFile(IPlaybackEngine* engine, CaptureMode mode, QWidget* parent = nullptr);
    static bool captureToClipboard(IPlaybackEngine* engine, CaptureMode mode, QWidget* parent = nullptr);
    static void openCaptureFolder(QWidget* parent = nullptr);

    // Lifecycle coordination & shutdown synchronization
    static void cancelPendingOperations();
    static bool waitForPendingOperations(int timeoutMs = 1000);
    static int activeCapturesCount();
};

} // namespace Jonaki
