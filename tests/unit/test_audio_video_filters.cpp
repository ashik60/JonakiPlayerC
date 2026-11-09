#include <QTest>
#include <QFileInfo>
#include <QDir>
#include <thread>
#include <chrono>

#include "playback/MpvPlaybackEngine.h"
#include "settings/SettingsManager.h"

using namespace Jonaki;

class TestAudioVideoFilters : public QObject {
    Q_OBJECT

private:
    std::string m_videoPath;
    std::string m_audioPath;
    std::string m_interlacedPath;

private slots:
    void initTestCase() {
        QDir dataDir("tests/test_data");
        if (!dataDir.exists()) dataDir = QDir("F:/Projects/JonakiPlayer/tests/test_data");

        m_videoPath = dataDir.absoluteFilePath("test_video_aac.mp4").toStdString();
        m_audioPath = dataDir.absoluteFilePath("test_audio.mp3").toStdString();
        m_interlacedPath = dataDir.absoluteFilePath("test_interlaced.mp4").toStdString();

        QVERIFY(QFileInfo::exists(QString::fromStdString(m_videoPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_audioPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_interlacedPath)));
    }

    void testEqualizer10Bands() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_audioPath));

        // Wait for media to load
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);

        // Initially disabled with 0dB gain across all 10 bands
        QCOMPARE(engine.isEqualizerEnabled(), false);
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(engine.getEqualizerBand(i), 0.0);
        }

        // Set gains across all 10 bands
        double testGains[10] = {-3.0, -1.5, 0.0, 2.0, 4.5, 6.0, 3.5, 1.0, -2.0, -4.0};
        for (int i = 0; i < 10; ++i) {
            engine.setEqualizerBand(i, testGains[i]);
            QCOMPARE(engine.getEqualizerBand(i), testGains[i]);
        }

        // Test gain clamping limits: [-12.0, +12.0] dB
        engine.setEqualizerBand(0, 25.0);
        QCOMPARE(engine.getEqualizerBand(0), 12.0);

        engine.setEqualizerBand(9, -35.0);
        QCOMPARE(engine.getEqualizerBand(9), -12.0);

        // Enable equalizer
        engine.setEqualizerEnabled(true);
        QCOMPARE(engine.isEqualizerEnabled(), true);

        // Reset equalizer
        engine.resetEqualizer();
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(engine.getEqualizerBand(i), 0.0);
        }

        engine.shutdown();
    }

    void testDynamicAudioNormalizationCoexistence() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_audioPath));

        // Wait for media to load
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);

        // Configure Equalizer
        engine.setEqualizerBand(4, 6.0); // 500 Hz +6dB
        engine.setEqualizerEnabled(true);
        QCOMPARE(engine.isEqualizerEnabled(), true);
        QCOMPARE(engine.getEqualizerBand(4), 6.0);

        // Enable Dynamic Audio Normalization
        QCOMPARE(engine.isAudioNormalizationEnabled(), false);
        engine.setAudioNormalization(true);
        QCOMPARE(engine.isAudioNormalizationEnabled(), true);

        // Equalizer settings must be preserved exactly without corruption
        QCOMPARE(engine.isEqualizerEnabled(), true);
        QCOMPARE(engine.getEqualizerBand(4), 6.0);

        // Disable Equalizer; Normalization must remain active
        engine.setEqualizerEnabled(false);
        QCOMPARE(engine.isEqualizerEnabled(), false);
        QCOMPARE(engine.isAudioNormalizationEnabled(), true);

        // Disable Normalization; both are now disabled cleanly
        engine.setAudioNormalization(false);
        QCOMPARE(engine.isAudioNormalizationEnabled(), false);

        engine.shutdown();
    }

    void testDeinterlacingModes() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_interlacedPath));

        // Wait for media to load
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);

        // Default mode is off
        QCOMPARE(engine.getDeinterlacing(), "off");

        // Set to Auto
        engine.setDeinterlacing("auto");
        QCOMPARE(engine.getDeinterlacing(), "auto");

        // Set to Forced On
        engine.setDeinterlacing("on");
        QCOMPARE(engine.getDeinterlacing(), "on");

        // Set back to Off
        engine.setDeinterlacing("off");
        QCOMPARE(engine.getDeinterlacing(), "off");

        engine.shutdown();
    }

    void testAudioDelayAdjustment() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());

        // Default audio delay is 0.0s
        QCOMPARE(engine.getAudioDelay(), 0.0);

        // Slower sync by 0.05s
        engine.setAudioDelay(-0.05);
        QCOMPARE(engine.getAudioDelay(), -0.05);

        // Faster sync by 0.10s (resulting in +0.05s)
        engine.setAudioDelay(0.05);
        QCOMPARE(engine.getAudioDelay(), 0.05);

        // Clamping to sensible limits [-10.0, 10.0]
        engine.setAudioDelay(15.0);
        QCOMPARE(engine.getAudioDelay(), 10.0);

        engine.setAudioDelay(-15.0);
        QCOMPARE(engine.getAudioDelay(), -10.0);

        // Reset to 0.0s
        engine.setAudioDelay(0.0);
        QCOMPARE(engine.getAudioDelay(), 0.0);

        engine.shutdown();
    }

    void testAspectRatioScalingModes() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());

        // Default ratio
        QCOMPARE(engine.getAspectRatio(), "default");

        // Standard aspect ratios
        static const std::vector<std::string> ratios = {
            "16:9", "4:3", "16:10", "1:1", "1.85:1", "2.35:1", "default"
        };

        for (const auto& r : ratios) {
            engine.setAspectRatio(r);
            QCOMPARE(engine.getAspectRatio(), r);
        }

        // Custom ratio
        engine.setAspectRatio("2.39:1");
        QCOMPARE(engine.getAspectRatio(), "2.39:1");

        engine.shutdown();
    }

    void testSettingsPersistence() {
        auto& settings = SettingsManager::instance();

        // Write audio/video options
        settings.setBool("Audio", "EqualizerEnabled", true);
        settings.setString("Audio", "EqualizerBands", "1.0;2.0;3.0;4.0;5.0;6.0;5.0;4.0;3.0;2.0");
        settings.setBool("Audio", "Normalization", true);
        settings.setDouble("Audio", "AudioDelay", 0.05);
        settings.setString("Video", "DeinterlaceMode", "auto");
        settings.setString("Video", "AspectRatio", "16:9");

        // Save to file
        std::string testFile = "build/test_output/av_settings.ini";
        QDir("build/test_output").mkpath(".");
        QVERIFY(settings.saveToFile(testFile));

        // Reset and reload
        settings.resetToDefaults();
        QCOMPARE(settings.getBool("Audio", "EqualizerEnabled"), false);

        QVERIFY(settings.loadFromFile(testFile));
        QCOMPARE(settings.getBool("Audio", "EqualizerEnabled"), true);
        QCOMPARE(settings.getString("Audio", "EqualizerBands"), "1.0;2.0;3.0;4.0;5.0;6.0;5.0;4.0;3.0;2.0");
        QCOMPARE(settings.getBool("Audio", "Normalization"), true);
        QCOMPARE(settings.getDouble("Audio", "AudioDelay"), 0.05);
        QCOMPARE(settings.getString("Video", "DeinterlaceMode"), "auto");
        QCOMPARE(settings.getString("Video", "AspectRatio"), "16:9");
    }
};

QTEST_MAIN(TestAudioVideoFilters)
#include "test_audio_video_filters.moc"
