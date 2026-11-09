#include <QTest>
#include "MockPlaybackEngine.h"

using namespace Jonaki;

class TestPlaybackEngineMock : public QObject {
    Q_OBJECT

private slots:
    void testEngineLifecycle() {
        MockPlaybackEngine engine;
        QCOMPARE(engine.isInitialized(), false);
        QCOMPARE(engine.getState(), PlaybackState::Stopped);

        QVERIFY(engine.initialize());
        QCOMPARE(engine.isInitialized(), true);

        engine.shutdown();
        QCOMPARE(engine.isInitialized(), false);
        QCOMPARE(engine.getState(), PlaybackState::Stopped);
    }

    void testStateTransitions() {
        MockPlaybackEngine engine;
        engine.initialize();

        PlaybackState recordedState = PlaybackState::Stopped;
        engine.setOnStateChanged([&](PlaybackState s) {
            recordedState = s;
        });

        // Load media -> Playing
        QVERIFY(engine.loadMedia("test.mp4"));
        QCOMPARE(engine.getState(), PlaybackState::Playing);
        QCOMPARE(recordedState, PlaybackState::Playing);

        // Pause
        engine.pause();
        QCOMPARE(engine.getState(), PlaybackState::Paused);
        QCOMPARE(recordedState, PlaybackState::Paused);

        // TogglePlayPause -> Playing
        engine.togglePlayPause();
        QCOMPARE(engine.getState(), PlaybackState::Playing);
        QCOMPARE(recordedState, PlaybackState::Playing);

        // Stop
        engine.stop();
        QCOMPARE(engine.getState(), PlaybackState::Stopped);
        QCOMPARE(recordedState, PlaybackState::Stopped);
    }

    void testSeekAndTiming() {
        MockPlaybackEngine engine;
        engine.initialize();
        engine.loadMedia("test.mp4"); // duration = 120s

        double notifiedTime = -1.0;
        engine.setOnTimeChanged([&](double cur, double dur) {
            (void)dur;
            notifiedTime = cur;
        });

        engine.seek(45.0, true);
        QCOMPARE(engine.getCurrentTime(), 45.0);
        QCOMPARE(notifiedTime, 45.0);

        engine.seekRelative(-10.0);
        QCOMPARE(engine.getCurrentTime(), 35.0);

        engine.seekRelative(100.0);
        QCOMPARE(engine.getCurrentTime(), 120.0); // clamped to duration
    }

    void testSpeedAndVolume() {
        MockPlaybackEngine engine;
        engine.initialize();

        engine.setSpeed(1.5);
        QCOMPARE(engine.getSpeed(), 1.5);

        engine.setVolume(80);
        QCOMPARE(engine.getVolume(), 80);

        engine.setMute(true);
        QCOMPARE(engine.isMute(), true);
    }

    void testErrorHandling() {
        MockPlaybackEngine engine;
        engine.initialize();

        PlaybackError recordedError = PlaybackError::None;
        std::string recordedMsg;
        engine.setOnError([&](PlaybackError err, const std::string& msg) {
            recordedError = err;
            recordedMsg = msg;
        });

        bool success = engine.loadMedia("invalid_file_nonexistent.xyz");
        QCOMPARE(success, false);
        QCOMPARE(recordedError, PlaybackError::FileNotFound);
        QCOMPARE(recordedMsg.empty(), false);
    }
};

QTEST_MAIN(TestPlaybackEngineMock)
#include "test_playback_mock.moc"
