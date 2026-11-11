#include <QTest>
#include <QSignalSpy>
#include <QFile>

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

QTEST_MAIN(TestABRepeat)
#include "test_ab_repeat.moc"
