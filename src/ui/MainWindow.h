#pragma once

#include <QMainWindow>
#include <memory>

namespace Jonaki {

class IPlaybackEngine;
class MpvPlaybackEngine;
class VideoOpenGLWidget;
class TitleBarWidget;
class BottomControlBar;
class FramelessWindowHelper;
class PlaylistController;
class PlaylistDrawer;

/**
 * @brief Main window for JonakiPlayer.
 *
 * Implements the measured 723x399 PotPlayer default layout:
 * - 31px custom frameless title bar
 * - OpenGL video surface with mpv rendering
 * - 62px bottom control bar (22px seekbar + 40px buttons)
 * - Complete context menu hierarchy & keyboard shortcuts
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    IPlaybackEngine* engine() const;

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    void setupUi();
    void setupEngine();
    void setupShortcuts();
    void connectSignals();

    std::unique_ptr<MpvPlaybackEngine> m_engine;
    std::unique_ptr<FramelessWindowHelper> m_framelessHelper;

    TitleBarWidget* m_titleBar = nullptr;
    VideoOpenGLWidget* m_videoWidget = nullptr;
    BottomControlBar* m_controlBar = nullptr;

    std::unique_ptr<PlaylistController> m_playlistController;
    PlaylistDrawer* m_playlistDrawer = nullptr;
};

} // namespace Jonaki
