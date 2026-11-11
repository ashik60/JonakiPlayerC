#include "ContextMenuBuilder.h"
#include "MainWindow.h"
#include "FrameCaptureHelper.h"
#include "OpenUrlDialog.h"
#include "../playback/IPlaybackEngine.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <sstream>
#include <iomanip>

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
        parentWindow->openUrlDialog();
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
    audioMenu->addSeparator();
    audioMenu->addAction("Use Equalizer\tShift+E", [parentWindow]() {
        bool eq = !parentWindow->engine()->isEqualizerEnabled();
        parentWindow->engine()->setEqualizerEnabled(eq);
        parentWindow->engine()->showOsdMessage(eq ? "Equalizer: Enabled" : "Equalizer: Disabled");
    });
    audioMenu->addAction("Normalizer (Auto Volume Control)\tShift+N", [parentWindow]() {
        bool norm = !parentWindow->engine()->isAudioNormalizationEnabled();
        parentWindow->engine()->setAudioNormalization(norm);
        parentWindow->engine()->showOsdMessage(norm ? "Normalizer: Enabled" : "Normalizer: Disabled");
    });
    audioMenu->addSeparator();
    auto* audioSyncMenu = audioMenu->addMenu("Audio Sync");
    audioSyncMenu->addAction("Decrease Audio Sync (-0.05s)\tShift+<", [parentWindow]() {
        double d = parentWindow->engine()->getAudioDelay() - 0.05;
        parentWindow->engine()->setAudioDelay(d);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Audio Sync: " << (d >= 0 ? "+" : "") << d << " s";
        parentWindow->engine()->showOsdMessage(ss.str());
    });
    audioSyncMenu->addAction("Increase Audio Sync (+0.05s)\tShift+>", [parentWindow]() {
        double d = parentWindow->engine()->getAudioDelay() + 0.05;
        parentWindow->engine()->setAudioDelay(d);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Audio Sync: " << (d >= 0 ? "+" : "") << d << " s";
        parentWindow->engine()->showOsdMessage(ss.str());
    });
    audioSyncMenu->addAction("Default Audio Sync\tShift+/", [parentWindow]() {
        parentWindow->engine()->setAudioDelay(0.0);
        parentWindow->engine()->showOsdMessage("Audio Sync: 0.00 s (Default)");
    });

    // 4. Video Submenu
    auto* videoMenu = menu->addMenu("Video");
    auto* arMenu = videoMenu->addMenu("Aspect Ratio");
    arMenu->addAction("Cycle Aspect Ratio\tCtrl+F6", [parentWindow]() {
        static const std::vector<std::pair<std::string, std::string>> ratios = {
            {"default", "Default (Source)"},
            {"16:9", "16:9"},
            {"4:3", "4:3"},
            {"16:10", "16:10"},
            {"1:1", "1:1"}
        };
        std::string cur = parentWindow->engine()->getAspectRatio();
        int nextIdx = 0;
        for (size_t i = 0; i < ratios.size(); ++i) {
            if (ratios[i].first == cur) {
                nextIdx = (i + 1) % ratios.size();
                break;
            }
        }
        parentWindow->engine()->setAspectRatio(ratios[nextIdx].first);
        parentWindow->engine()->showOsdMessage("Aspect Ratio: " + ratios[nextIdx].second);
    });
    arMenu->addSeparator();
    arMenu->addAction("Default (Source)", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("default");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: Default (Source)");
    });
    arMenu->addAction("16:9", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("16:9");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 16:9");
    });
    arMenu->addAction("4:3", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("4:3");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 4:3");
    });
    arMenu->addAction("16:10", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("16:10");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 16:10");
    });
    arMenu->addAction("1:1", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("1:1");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 1:1");
    });
    arMenu->addAction("1.85:1", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("1.85:1");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 1.85:1");
    });
    arMenu->addAction("2.35:1", [parentWindow]() {
        parentWindow->engine()->setAspectRatio("2.35:1");
        parentWindow->engine()->showOsdMessage("Aspect Ratio: 2.35:1");
    });

    auto* deintMenu = videoMenu->addMenu("Deinterlacing");
    deintMenu->addAction("Cycle Deinterlacing Mode\tCtrl+I", [parentWindow]() {
        std::string cur = parentWindow->engine()->getDeinterlacing();
        std::string next = (cur == "off" || cur.empty()) ? "auto" : (cur == "auto" ? "on" : "off");
        parentWindow->engine()->setDeinterlacing(next);
        std::string label = (next == "off" ? "Off" : (next == "auto" ? "Auto" : "On"));
        parentWindow->engine()->showOsdMessage("Deinterlacing: " + label);
    });
    deintMenu->addSeparator();
    deintMenu->addAction("Off", [parentWindow]() {
        parentWindow->engine()->setDeinterlacing("off");
        parentWindow->engine()->showOsdMessage("Deinterlacing: Off");
    });
    deintMenu->addAction("Auto", [parentWindow]() {
        parentWindow->engine()->setDeinterlacing("auto");
        parentWindow->engine()->showOsdMessage("Deinterlacing: Auto");
    });
    deintMenu->addAction("On", [parentWindow]() {
        parentWindow->engine()->setDeinterlacing("on");
        parentWindow->engine()->showOsdMessage("Deinterlacing: On");
    });

    videoMenu->addSeparator();

    auto* capMenu = videoMenu->addMenu("Video Capture");
    capMenu->addAction("Save Current Source Frame\tCtrl+E", [parentWindow]() {
        FrameCaptureHelper::captureToFile(parentWindow->engine(), CaptureMode::Video, parentWindow);
    });
    capMenu->addAction("Save Current Screen Frame\tCtrl+Alt+E", [parentWindow]() {
        FrameCaptureHelper::captureToFile(parentWindow->engine(), CaptureMode::Subtitles, parentWindow);
    });
    capMenu->addSeparator();
    capMenu->addAction("Copy Current Source Frame\tCtrl+C", [parentWindow]() {
        FrameCaptureHelper::captureToClipboard(parentWindow->engine(), CaptureMode::Video, parentWindow);
    });
    capMenu->addAction("Copy Current Screen Frame\tCtrl+Alt+C", [parentWindow]() {
        FrameCaptureHelper::captureToClipboard(parentWindow->engine(), CaptureMode::Subtitles, parentWindow);
    });
    capMenu->addSeparator();
    capMenu->addAction("Open Image Storage Folder", [parentWindow]() {
        FrameCaptureHelper::openCaptureFolder(parentWindow);
    });

    // 5. Subtitles Submenu
    auto* subMenu = menu->addMenu("Subtitles");
    subMenu->addAction("Load Subtitle...\tAlt+O", [parentWindow]() {
        QString file = QFileDialog::getOpenFileName(parentWindow, "Open Subtitle File", "", "Subtitles (*.srt *.ass *.ssa *.vtt *.sub *.smi)");
        if (!file.isEmpty()) {
            parentWindow->engine()->loadExternalSubtitle(file.toStdString());
            parentWindow->engine()->showOsdMessage("Subtitle loaded: " + QFileInfo(file).fileName().toStdString());
        }
    });
    subMenu->addAction("Show/Hide Subtitles\tAlt+H", [parentWindow]() {
        bool vis = !parentWindow->engine()->isSubtitleVisible();
        parentWindow->engine()->setSubtitleVisibility(vis);
        parentWindow->engine()->showOsdMessage(vis ? "Subtitles: Show" : "Subtitles: Hide");
    });
    subMenu->addSeparator();
    subMenu->addAction("Subtitle Sync: Slower (-0.5s)\t<", [parentWindow]() {
        double d = parentWindow->engine()->getSubtitleDelay() - 0.5;
        parentWindow->engine()->setSubtitleDelay(d);
        parentWindow->engine()->showOsdMessage("Subtitle Sync: " + std::to_string(d) + " s");
    });
    subMenu->addAction("Subtitle Sync: Faster (+0.5s)\t>", [parentWindow]() {
        double d = parentWindow->engine()->getSubtitleDelay() + 0.5;
        parentWindow->engine()->setSubtitleDelay(d);
        parentWindow->engine()->showOsdMessage("Subtitle Sync: " + std::to_string(d) + " s");
    });
    subMenu->addAction("Subtitle Sync: Reset\t/", [parentWindow]() {
        parentWindow->engine()->setSubtitleDelay(0.0);
        parentWindow->engine()->showOsdMessage("Subtitle Sync: 0.0 s (Default)");
    });

    menu->addSeparator();

    // 6. Window Controls
    menu->addAction("Toggle Fullscreen\tEnter", [parentWindow]() {
        if (parentWindow->isFullScreen()) parentWindow->showNormal();
        else parentWindow->showFullScreen();
    });

    menu->addSeparator();

    // 6. Preferences & About
    menu->addAction("Preferences...\tF5", [parentWindow]() {
        parentWindow->openPreferencesDialog();
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
