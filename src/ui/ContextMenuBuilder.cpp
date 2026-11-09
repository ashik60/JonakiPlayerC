#include "ContextMenuBuilder.h"
#include "MainWindow.h"
#include "../playback/IPlaybackEngine.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

namespace Jonaki {

std::unique_ptr<QMenu> ContextMenuBuilder::buildMenu(MainWindow* parentWindow) {
    auto menu = std::make_unique<QMenu>(parentWindow);
    menu->setStyleSheet(
        "QMenu { background: #202024; color: #dcdcdc; border: 1px solid #3c3c40; font-family: Tahoma; font-size: 11px; padding: 2px; } "
        "QMenu::item { padding: 4px 20px 4px 20px; } "
        "QMenu::item:selected { background: #35353a; color: #ffffff; } "
        "QMenu::separator { height: 1px; background: #38383c; margin: 3px 5px; }"
    );

    // 1. Open Submenu
    auto* openMenu = menu->addMenu("Open");
    openMenu->addAction("Open File(s)...\tCtrl+O", [parentWindow]() {
        QString file = QFileDialog::getOpenFileName(parentWindow, "Open Media File");
        if (!file.isEmpty()) {
            parentWindow->engine()->loadMedia(file.toStdString());
        }
    });
    openMenu->addAction("Open URL...\tCtrl+U", [parentWindow]() {
        bool ok = false;
        QString url = QInputDialog::getText(parentWindow, "Open URL", "Enter media stream URL:", QLineEdit::Normal, "", &ok);
        if (ok && !url.isEmpty()) {
            parentWindow->engine()->loadMedia(url.toStdString());
        }
    });

    menu->addSeparator();

    // 2. Playback Submenu
    auto* playMenu = menu->addMenu("Playback");
    playMenu->addAction("Play / Pause\tSpace", [parentWindow]() {
        parentWindow->engine()->togglePlayPause();
    });
    playMenu->addAction("Stop\tCtrl+Space", [parentWindow]() {
        parentWindow->engine()->stop();
    });
    playMenu->addSeparator();
    playMenu->addAction("Frame Step Forward\tF", [parentWindow]() {
        parentWindow->engine()->stepFrame(1);
    });
    playMenu->addAction("Frame Step Backward\tD", [parentWindow]() {
        parentWindow->engine()->stepFrame(-1);
    });
    playMenu->addSeparator();
    playMenu->addAction("Speed: 1.0x (Normal)\tZ", [parentWindow]() {
        parentWindow->engine()->setSpeed(1.0);
    });
    playMenu->addAction("Speed: Faster (0.1x)\tC", [parentWindow]() {
        parentWindow->engine()->setSpeed(parentWindow->engine()->getSpeed() + 0.1);
    });
    playMenu->addAction("Speed: Slower (0.1x)\tX", [parentWindow]() {
        parentWindow->engine()->setSpeed(std::max(0.2, parentWindow->engine()->getSpeed() - 0.1));
    });

    // 3. Audio Submenu
    auto* audioMenu = menu->addMenu("Audio");
    audioMenu->addAction("Volume Up (+5%)\tUp", [parentWindow]() {
        parentWindow->engine()->setVolume(std::min(100, parentWindow->engine()->getVolume() + 5));
    });
    audioMenu->addAction("Volume Down (-5%)\tDown", [parentWindow]() {
        parentWindow->engine()->setVolume(std::max(0, parentWindow->engine()->getVolume() - 5));
    });
    audioMenu->addAction("Mute / Unmute\tM", [parentWindow]() {
        parentWindow->engine()->setMute(!parentWindow->engine()->isMute());
    });

    // 4. Video Aspect Ratio Submenu
    auto* videoMenu = menu->addMenu("Video");
    videoMenu->addAction("Aspect Ratio: Fit to Window", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("default");
    });
    videoMenu->addAction("Aspect Ratio: 16:9", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("16:9");
    });
    videoMenu->addAction("Aspect Ratio: 4:3", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("4:3");
    });

    menu->addSeparator();

    // 5. Window Controls
    menu->addAction("Toggle Fullscreen\tEnter", [parentWindow]() {
        if (parentWindow->isFullScreen()) parentWindow->showNormal();
        else parentWindow->showFullScreen();
    });

    menu->addSeparator();

    // 6. Preferences & About
    menu->addAction("Preferences...\tF5", [parentWindow]() {
        QMessageBox::information(parentWindow, "Preferences", "Preferences Dialog (751x564) scheduled for Milestone M7.");
    });
    menu->addAction("About JonakiPlayer...\tF1", [parentWindow]() {
        QMessageBox::about(parentWindow, "About JonakiPlayer", "JonakiPlayer v0.1.0\nClean-Room PotPlayer Reimplementation\nC++20, Qt 6, libmpv");
    });

    menu->addSeparator();
    menu->addAction("Exit\tAlt+F4", [parentWindow]() {
        parentWindow->close();
    });

    return menu;
}

} // namespace Jonaki
