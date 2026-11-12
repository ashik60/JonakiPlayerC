#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include <mpv/client.h>

#include "../../src/playback/MpvPlaybackEngine.h"
#include "../../src/ui/BottomControlBar.h"

using namespace Jonaki;

class TestABRepeat : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testSetPointABoundaries();
    void testSetPointBBoundaries();
    void testBoundaryValidationOrder();
    void testClearPointA();
    void testClearPointB();
    void testClearBothPoints();
    void testToggleLoopWhenConfigured();
    void testMediaLoadClearsLoopPoints();
    void testBottomControlBarMarkerUpdates();
    void testActualPlaybackWrappingBtoA();
    void testSeekingWhileLoopActive();
    void testChangingMediaClearsNativeLoopProperties();

private:
    std::unique_ptr<MpvPlaybackEngine> m_engine;
    QString m_testMedia;
};

void TestABRepeat::initTestCase() {
    m_testMedia = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(QFile::exists(m_testMedia));
}

void TestABRepeat::cleanupTestCase() {
}

void TestABRepeat::init() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    QVERIFY(m_engine->initialize());
}

void TestABRepeat::cleanup() {
    if (m_engine) {
        m_engine->shutdown();
        m_engine.reset();
    }
}

void TestABRepeat::testSetPointABoundaries() {
    QCOMPARE(m_engine->getLoopPointA(), -1.0);
    m_engine->setLoopPointA(1.5);
    QCOMPARE(m_engine->getLoopPointA(), 1.5);
    QVERIFY(!m_engine->isLoopActive()); // Loop not active until Point B set
}

void TestABRepeat::testSetPointBBoundaries() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.5);
    QCOMPARE(m_engine->getLoopPointA(), 1.0);
    QCOMPARE(m_engine->getLoopPointB(), 3.5);
    QVERIFY(m_engine->isLoopActive());
}

void TestABRepeat::testBoundaryValidationOrder() {
    // If Point B is set less than or equal to Point A, loop should not be active
    m_engine->setLoopPointA(4.0);
    m_engine->setLoopPointB(2.0);
    QVERIFY(!m_engine->isLoopActive());
}

void TestABRepeat::testClearPointA() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.0);
    QVERIFY(m_engine->isLoopActive());

    m_engine->clearLoopPointA();
    QCOMPARE(m_engine->getLoopPointA(), -1.0);
    QCOMPARE(m_engine->getLoopPointB(), 3.0);
    QVERIFY(!m_engine->isLoopActive());
}

void TestABRepeat::testClearPointB() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.0);
    QVERIFY(m_engine->isLoopActive());

    m_engine->clearLoopPointB();
    QCOMPARE(m_engine->getLoopPointA(), 1.0);
    QCOMPARE(m_engine->getLoopPointB(), -1.0);
    QVERIFY(!m_engine->isLoopActive());
}

void TestABRepeat::testClearBothPoints() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.0);
    QVERIFY(m_engine->isLoopActive());

    m_engine->clearLoop();
    QCOMPARE(m_engine->getLoopPointA(), -1.0);
    QCOMPARE(m_engine->getLoopPointB(), -1.0);
    QVERIFY(!m_engine->isLoopActive());
}

void TestABRepeat::testToggleLoopWhenConfigured() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.0);
    QVERIFY(m_engine->isLoopActive());

    m_engine->toggleLoop();
    QVERIFY(!m_engine->isLoopActive());

    m_engine->toggleLoop();
    QVERIFY(m_engine->isLoopActive());
}

void TestABRepeat::testMediaLoadClearsLoopPoints() {
    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(3.0);
    QVERIFY(m_engine->isLoopActive());

    m_engine->loadMedia(m_testMedia.toStdString());
    QCOMPARE(m_engine->getLoopPointA(), -1.0);
    QCOMPARE(m_engine->getLoopPointB(), -1.0);
    QVERIFY(!m_engine->isLoopActive());
}

void TestABRepeat::testBottomControlBarMarkerUpdates() {
    BottomControlBar bar;
    bar.resize(400, 62);
    bar.setLoopPoints(1.0, 3.0, 10.0);

    // Call paintEvent via repaint to ensure no crash in marker coordinate calculations
    bar.repaint();

    // Reset loop points
    bar.setLoopPoints(-1.0, -1.0, 10.0);
    bar.repaint();
}

void TestABRepeat::testActualPlaybackWrappingBtoA() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Set loop [0.5, 1.2]
    m_engine->setLoopPointA(0.5);
    m_engine->setLoopPointB(1.2);
    QVERIFY(m_engine->isLoopActive());

    // Verify native mpv properties are set
    char* propA = mpv_get_property_string(m_engine->rawHandle(), "ab-loop-a");
    char* propB = mpv_get_property_string(m_engine->rawHandle(), "ab-loop-b");
    QVERIFY(propA != nullptr);
    QVERIFY(propB != nullptr);
    QVERIFY(QString(propA).startsWith("0.5"));
    QVERIFY(QString(propB).startsWith("1.2"));
    mpv_free(propA);
    mpv_free(propB);

    // Seek close to Point B (1.0s)
    m_engine->seek(1.0, true);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 0.95, 3000);

    // Playback should advance past 1.2 and wrap back to ~0.5s
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 0.5 && m_engine->getCurrentTime() <= 0.9, 5000);
}

void TestABRepeat::testSeekingWhileLoopActive() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    m_engine->setLoopPointA(0.5);
    m_engine->setLoopPointB(2.0);
    QVERIFY(m_engine->isLoopActive());

    // Seek inside loop
    m_engine->seek(1.5, true);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 1.4 && m_engine->getCurrentTime() <= 1.8, 3000);
}

void TestABRepeat::testChangingMediaClearsNativeLoopProperties() {
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    m_engine->setLoopPointA(1.0);
    m_engine->setLoopPointB(2.5);
    QVERIFY(m_engine->isLoopActive());

    // Load media again (simulating opening a new track)
    QVERIFY(m_engine->loadMedia(m_testMedia.toStdString()));

    // Verify internal state reset
    QCOMPARE(m_engine->getLoopPointA(), -1.0);
    QCOMPARE(m_engine->getLoopPointB(), -1.0);
    QVERIFY(!m_engine->isLoopActive());

    // Verify native mpv properties are "no"
    char* propA = mpv_get_property_string(m_engine->rawHandle(), "ab-loop-a");
    char* propB = mpv_get_property_string(m_engine->rawHandle(), "ab-loop-b");
    QVERIFY(propA != nullptr);
    QVERIFY(propB != nullptr);
    QCOMPARE(QString(propA), QString("no"));
    QCOMPARE(QString(propB), QString("no"));
    mpv_free(propA);
    mpv_free(propB);
}

QTEST_MAIN(TestABRepeat)
#include "test_ab_repeat.moc"
