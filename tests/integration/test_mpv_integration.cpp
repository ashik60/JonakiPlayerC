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
        attempts = 0;
        while (engine.getCurrentTime() < 1.0 && attempts < 40) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getCurrentTime() >= 1.0, true);

        // Volume
        engine.setVolume(75);
        QCOMPARE(engine.getVolume(), 75);

        // Stop
        engine.stop();
        attempts = 0;
        while (engine.getState() != PlaybackState::Stopped && attempts < 40) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
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

    void testRepeatedLifecycle() {
        MpvPlaybackEngine engine;
        for (int i = 0; i < 3; ++i) {
            QVERIFY(engine.initialize());
            QVERIFY(engine.isInitialized());
            QVERIFY(engine.loadMedia(m_testAudioPath));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            engine.shutdown();
            QCOMPARE(engine.isInitialized(), false);
            QCOMPARE(engine.getState(), PlaybackState::Stopped);
        }
    }

    void testRapidStateChanges() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_testVideoPath));

        // Rapidly issue commands without crashing or deadlocking
        for (int i = 0; i < 10; ++i) {
            engine.pause();
            engine.play();
            engine.seek(1.0 + (i * 0.2), false);
            engine.setVolume(50 + i * 2);
            engine.setSpeed(0.8 + (i * 0.1));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        QVERIFY(engine.getState() == PlaybackState::Playing || engine.getState() == PlaybackState::Paused);
        engine.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        QCOMPARE(engine.getState(), PlaybackState::Stopped);

        engine.shutdown();
    }

    void testHardwareDecodingDetection() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_testVideoPath));

        // Wait up to 1.5s for playback and decoder initialization
        int attempts = 0;
        while (engine.getVideoCodec().empty() && attempts < 30) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }

        std::string codec = engine.getVideoCodec();
        std::string hwdec = engine.getHwdecActive();
        // Video codec description must contain H.264
        QVERIFY2(codec.find("H.264") != std::string::npos || codec.find("h264") != std::string::npos,
                 "Video codec must resolve to H.264");
        // Distinguish verified hwdec from requested option:
        // hwdec will be one of: "d3d11va-copy", "dxva2-copy", "nvdec-copy", "d3d11va", or "no" if software
        QVERIFY2(!hwdec.empty(), "Active hwdec string must be populated");

        engine.shutdown();
    }
};

QTEST_MAIN(TestMpvPlaybackIntegration)
#include "test_mpv_integration.moc"
