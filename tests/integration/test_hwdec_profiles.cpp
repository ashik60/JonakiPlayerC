#include <QTest>
#include <QSignalSpy>
#include <QDir>
#include <QFile>

#include "../../src/playback/MpvPlaybackEngine.h"
#include "../../src/settings/SettingsManager.h"

using namespace Jonaki;

class TestHwdecProfiles : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDefaultHwdecProfile();
    void testSetHwdecProfileWhileStopped();
    void testSetHwdecProfileDuringPlayback();
    void testSetHwdecProfileWhilePaused();
    void testSwitchToSoftwareDecoding();
    void testUnsupportedProfileHandlingDoesNotCrash();
    void testPositionAndStatePreservedAcrossRuntimeSwitch();
    void testActiveHwdecInspection();

private:
    std::unique_ptr<MpvPlaybackEngine> m_engine;
    QString m_testMedia;
};

void TestHwdecProfiles::initTestCase() {
    SettingsManager::instance().resetToDefaults();
    m_testMedia = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(QFile::exists(m_testMedia));
}

void TestHwdecProfiles::cleanupTestCase() {
    SettingsManager::instance().resetToDefaults();
}

void TestHwdecProfiles::init() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    QVERIFY(m_engine->initialize());
}

void TestHwdecProfiles::cleanup() {
    if (m_engine) {
        m_engine->shutdown();
        m_engine.reset();
    }
}

void TestHwdecProfiles::testDefaultHwdecProfile() {
    // Default profile must be auto-safe
    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("auto-safe"));
}

void TestHwdecProfiles::testSetHwdecProfileWhileStopped() {
    m_engine->setHwdecProfile("d3d11va-copy");
    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("d3d11va-copy"));

    m_engine->setHwdecProfile("no");
    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("no"));
}

void TestHwdecProfiles::testSetHwdecProfileDuringPlayback() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Switch profile at runtime while playing
    m_engine->setHwdecProfile("no");
    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("no"));

    // Verify playback keeps playing without crash or stopping
    QTest::qWait(500);
    QCOMPARE(m_engine->getState(), PlaybackState::Playing);
    QVERIFY(m_engine->getCurrentTime() > 0.1);
}

void TestHwdecProfiles::testSetHwdecProfileWhilePaused() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    m_engine->pause();
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Paused, 3000);

    m_engine->setHwdecProfile("d3d11va-copy");
    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("d3d11va-copy"));

    // Paused state must be preserved
    QCOMPARE(m_engine->getState(), PlaybackState::Paused);
}

void TestHwdecProfiles::testSwitchToSoftwareDecoding() {
    m_engine->setHwdecProfile("no");
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);

    QCOMPARE(m_engine->getRequestedHwdecProfile(), std::string("no"));
    QCOMPARE(m_engine->getActiveHwdecDecoder(), std::string("no"));
}

void TestHwdecProfiles::testUnsupportedProfileHandlingDoesNotCrash() {
    m_engine->setHwdecProfile("non_existent_accelerator");
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Should fall back cleanly without terminating playback
    QVERIFY(m_engine->getCurrentTime() > 0.05);
}

void TestHwdecProfiles::testPositionAndStatePreservedAcrossRuntimeSwitch() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    m_engine->seek(1.5, true);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 1.0, 5000);

    double beforeTime = m_engine->getCurrentTime();
    m_engine->setHwdecProfile("auto-safe");

    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= beforeTime, 3000);
    QCOMPARE(m_engine->getState(), PlaybackState::Playing);
}

void TestHwdecProfiles::testActiveHwdecInspection() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    std::string active = m_engine->getActiveHwdecDecoder();
    // active is either "no" (software) or an active hardware decoder (e.g. "d3d11va", "d3d11va-copy")
    QVERIFY(!active.empty());

    // Wait for video codec property update
    QTRY_VERIFY_WITH_TIMEOUT(!m_engine->getVideoCodec().empty(), 3000);
    std::string codec = m_engine->getVideoCodec();
    // May be reported as "h264" or full description "H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10"
    std::string lowerCodec;
    lowerCodec.reserve(codec.size());
    for (char c : codec) {
        lowerCodec.push_back(static_cast<char>(::tolower(static_cast<unsigned char>(c))));
    }
    QVERIFY(lowerCodec.find("h264") != std::string::npos || lowerCodec.find("h.264") != std::string::npos || lowerCodec.find("avc") != std::string::npos);
}

QTEST_MAIN(TestHwdecProfiles)
#include "test_hwdec_profiles.moc"
