#pragma once

#include <QMainWindow>
#include <memory>

class QSlider;
class QLabel;
class QPushButton;
class QTimer;

namespace Jonaki {

class IPlaybackEngine;
class MpvPlaybackEngine;
class VideoOpenGLWidget;

/**
 * @brief Main window for JonakiPlayer.
 *
 * Implements the core video rendering layout, control bar with seekbar,
 * volume slider, time labels, play/pause controls, keyboard shortcuts,
 * and drag-and-drop media loading.
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
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onPlayPauseClicked();
    void onStopClicked();
    void onOpenFileClicked();
    void onOpenUrlClicked();
    void onSeekSliderMoved(int position);
    void onVolumeSliderMoved(int position);
    void onMuteClicked();

private:
    void setupUi();
    void setupEngine();
    void setupShortcuts();
    void connectSignals();

    std::unique_ptr<MpvPlaybackEngine> m_engine;
    VideoOpenGLWidget* m_videoWidget = nullptr;

    // Controls
    QPushButton* m_btnPlayPause = nullptr;
    QPushButton* m_btnStop = nullptr;
    QPushButton* m_btnOpenFile = nullptr;
    QPushButton* m_btnMute = nullptr;
    QSlider* m_seekSlider = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QLabel* m_lblTime = nullptr;

    bool m_userSeeking = false;
};

} // namespace Jonaki
