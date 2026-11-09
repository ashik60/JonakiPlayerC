#pragma once

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

namespace Jonaki {

class BottomControlBar : public QWidget {
    Q_OBJECT

public:
    explicit BottomControlBar(QWidget* parent = nullptr);

    void setPlaying(bool playing);
    void setPosition(double currentSec, double totalSec);
    void setVolume(int vol);
    void setMuted(bool muted);

signals:
    void playClicked();
    void pauseClicked();
    void stopClicked();
    void prevClicked();
    void nextClicked();
    void openFileClicked();
    void playlistClicked();
    void controlBoxClicked();
    void seekMoved(int position);
    void volumeMoved(int volume);
    void muteToggled();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupSeekRow();
    void setupButtonRow();

    QWidget* m_seekRow = nullptr;
    QWidget* m_buttonRow = nullptr;

    QSlider* m_seekSlider = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QPushButton* m_btnMute = nullptr;

    QPushButton* m_btnPlay = nullptr;
    QPushButton* m_btnPause = nullptr;
    QPushButton* m_btnStop = nullptr;
    QPushButton* m_btnPrev = nullptr;
    QPushButton* m_btnNext = nullptr;
    QPushButton* m_btnOpen = nullptr;

    QLabel* m_lblCurrentTime = nullptr;
    QLabel* m_lblTotalTime = nullptr;

    QPushButton* m_btnPlaylist = nullptr;
    QPushButton* m_btnControlBox = nullptr;

    bool m_userSeeking = false;
};

} // namespace Jonaki
