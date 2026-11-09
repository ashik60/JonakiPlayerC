#include <QTest>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <thread>
#include <chrono>

#include "playback/MpvPlaybackEngine.h"

using namespace Jonaki;

class TestMpvPlaybackIntegration : public QObject {
    Q_OBJECT

private:
    std::string m_testVideoPath;
    std::string m_testAudioPath;

private slots:
    void initTestCase() {
        // Resolve absolute paths to test media
        QDir testDataDir(QCoreApplication::applicationDirPath() + "/../../tests/test_data");
        if (!testDataDir.exists()) {
            testDataDir = QDir("F:/Projects/JonakiPlayer/tests/test_data");
        }
        m_testVideoPath = testDataDir.absoluteFilePath("test_video_aac.mp4").toStdString();
        m_testAudioPath = testDataDir.absoluteFilePath("test_audio.mp3").toStdString();

        QVERIFY2(QFileInfo::exists(QString::fromStdString(m_testVideoPath)), "test_video_aac.mp4 must exist");
        QVERIFY2(QFileInfo::exists(QString::fromStdString(m_testAudioPath)), "test_audio.mp3 must exist");
    }

    void testMpvLifecycle() {
        MpvPlaybackEngine engine;
        QCOMPARE(engine.isInitialized(), false);

        QVERIFY(engine.initialize());
        QCOMPARE(engine.isInitialized(), true);

        engine.shutdown();
        QCOMPARE(engine.isInitialized(), false);
    }

    void testMpvLoadRealVideo() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());

        bool stateChangedToPlaying = false;
        engine.setOnStateChanged([&](PlaybackState state) {
            if (state == PlaybackState::Playing) {
                stateChangedToPlaying = true;
            }
        });

        QVERIFY(engine.loadMedia(m_testVideoPath));

        // Wait for mpv async event loop to process file
        int attempts = 0;
        while (!stateChangedToPlaying && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }

        QVERIFY2(stateChangedToPlaying, "Engine must reach Playing state on real video");
        QCOMPARE(engine.getState(), PlaybackState::Playing);

        // Pause
        engine.pause();
        attempts = 0;
        while (engine.getState() != PlaybackState::Paused && attempts < 40) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Paused);

        // Seek
        engine.seek(2.0, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCOMPARE(engine.getCurrentTime() >= 1.0, true);

        // Volume
        engine.setVolume(75);
        QCOMPARE(engine.getVolume(), 75);

        // Stop
        engine.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCOMPARE(engine.getState(), PlaybackState::Stopped);

        engine.shutdown();
    }

    void testMpvLoadRealAudio() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());

        QVERIFY(engine.loadMedia(m_testAudioPath));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        engine.setSpeed(1.25);
        QCOMPARE(engine.getSpeed(), 1.25);

        engine.setMute(true);
        QCOMPARE(engine.isMute(), true);

        engine.shutdown();
    }

    void testMpvInvalidFileError() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());

        bool errorReported = false;
        engine.setOnError([&](PlaybackError err, const std::string& msg) {
            (void)err; (void)msg;
            errorReported = true;
        });

        engine.loadMedia("F:/non_existent_folder_abc/file_xyz.mp4");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        QVERIFY2(errorReported, "Invalid file load must trigger error callback");
        engine.shutdown();
    }
};

QTEST_MAIN(TestMpvPlaybackIntegration)
#include "test_mpv_integration.moc"
