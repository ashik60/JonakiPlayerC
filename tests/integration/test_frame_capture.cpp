#include <QTest>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QClipboard>
#include <QGuiApplication>
#include <QSignalSpy>
#include <QElapsedTimer>

#include "../../src/playback/MpvPlaybackEngine.h"
#include "../../src/ui/FrameCaptureHelper.h"
#include "../../src/settings/SettingsManager.h"

using namespace Jonaki;

class TestFrameCapture : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCaptureWithoutMediaReturnsError();
    void testUniqueFilePathAndCollisionHandling();
    void testDirectoryCreationAndPermissionCheck();
    void testPngGenerationAndDimensions();
    void testSourceVsSubtitleCapture();
    void testUnicodeOutputPathHandling();
    void testClipboardImageCapture();
    void testGuiResponsivenessAsyncCapture();

private:
    std::unique_ptr<MpvPlaybackEngine> m_engine;
    QString m_testOutputDir;
};

void TestFrameCapture::initTestCase() {
    SettingsManager::instance().resetToDefaults();
    m_testOutputDir = QDir::currentPath() + "/test_captures";
    QDir().mkpath(m_testOutputDir);
}

void TestFrameCapture::cleanupTestCase() {
    QDir(m_testOutputDir).removeRecursively();
}

void TestFrameCapture::init() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    m_engine->initialize();
}

void TestFrameCapture::cleanup() {
    if (m_engine) {
        m_engine->shutdown();
        m_engine.reset();
    }
}

void TestFrameCapture::testCaptureWithoutMediaReturnsError() {
    // Engine is initialized but no media is loaded -> capture must fail cleanly
    std::string outPath = (m_testOutputDir + "/no_media.png").toStdString();
    bool result = m_engine->captureFrame(outPath, CaptureMode::Video);
    QVERIFY(!result);
    QVERIFY(!QFile::exists(QString::fromStdString(outPath)));
}

void TestFrameCapture::testUniqueFilePathAndCollisionHandling() {
    QString dir = m_testOutputDir + "/collision_test";
    QDir().mkpath(dir);

    QString path1 = FrameCaptureHelper::generateUniqueFilePath(dir, "JonakiTest");
    QVERIFY(!path1.isEmpty());

    // Create a dummy file at path1
    QFile f1(path1);
    QVERIFY(f1.open(QIODevice::WriteOnly));
    f1.write("dummy");
    f1.close();

    // Generate again in same directory with same base: must produce unique candidate
    QString path2 = FrameCaptureHelper::generateUniqueFilePath(dir, "JonakiTest");
    QVERIFY(path1 != path2);
    QVERIFY(!QFile::exists(path2));

    QFile::remove(path1);
    QDir(dir).removeRecursively();
}

void TestFrameCapture::testDirectoryCreationAndPermissionCheck() {
    QString deepDir = m_testOutputDir + "/a/b/c/nested";
    QVERIFY(FrameCaptureHelper::ensureDirectoryExists(deepDir));
    QVERIFY(QDir(deepDir).exists());
}

void TestFrameCapture::testPngGenerationAndDimensions() {
    QString mediaPath = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(m_engine->loadMedia(mediaPath.toStdString()));

    // Wait for playback and decode to advance
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    QString outPath = m_testOutputDir + "/test_dim.png";
    QFile::remove(outPath);

    bool ok = m_engine->captureFrame(outPath.toStdString(), CaptureMode::Video);
    QVERIFY(ok);
    QVERIFY(QFile::exists(outPath));

    QImage img(outPath);
    QVERIFY(!img.isNull());
    QCOMPARE(img.width(), 640);
    QCOMPARE(img.height(), 360);
    QVERIFY(img.format() != QImage::Format_Invalid);

    QFile::remove(outPath);
}

void TestFrameCapture::testSourceVsSubtitleCapture() {
    QString mediaPath = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QString subPath = "F:/Projects/JonakiPlayer/tests/test_data/test_sub.srt";

    QVERIFY(m_engine->loadMedia(mediaPath.toStdString()));
    // Wait for media to be loaded before adding subtitle track
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    m_engine->loadExternalSubtitle(subPath.toStdString());
    m_engine->setSubtitleVisibility(true);

    // Seek to 1.5s where subtitle "This is test subtitle line 1" is active
    m_engine->seek(1.5, true);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 1.0, 5000);

    QString sourceSnap = m_testOutputDir + "/snap_source.png";
    QString subSnap = m_testOutputDir + "/snap_sub.png";
    QFile::remove(sourceSnap);
    QFile::remove(subSnap);

    QVERIFY(m_engine->captureFrame(sourceSnap.toStdString(), CaptureMode::Video));
    QVERIFY(m_engine->captureFrame(subSnap.toStdString(), CaptureMode::Subtitles));

    QImage imgSource(sourceSnap);
    QImage imgSub(subSnap);
    QVERIFY(!imgSource.isNull());
    QVERIFY(!imgSub.isNull());

    QCOMPARE(imgSource.size(), imgSub.size());

    // Both files exist and are valid PNG images
    QVERIFY(QFileInfo(sourceSnap).size() > 1000);
    QVERIFY(QFileInfo(subSnap).size() > 1000);

    QFile::remove(sourceSnap);
    QFile::remove(subSnap);
}

void TestFrameCapture::testUnicodeOutputPathHandling() {
    QString mediaPath = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(m_engine->loadMedia(mediaPath.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Unicode path containing Bengali script
    QString unicodeDir = m_testOutputDir + QString::fromUtf8("/বাংলা_ক্যাপচার_ফোল্ডার");
    QVERIFY(FrameCaptureHelper::ensureDirectoryExists(unicodeDir));

    QString unicodePath = unicodeDir + QString::fromUtf8("/স্ক্রিনশট_পরীক্ষা_২০২৬.png");
    QFile::remove(unicodePath);

    bool ok = m_engine->captureFrame(unicodePath.toStdString(), CaptureMode::Video);
    QVERIFY(ok);
    QVERIFY(QFile::exists(unicodePath));

    QImage img(unicodePath);
    QVERIFY(!img.isNull());
    QCOMPARE(img.width(), 640);
    QCOMPARE(img.height(), 360);

    QFile::remove(unicodePath);
    QDir(unicodeDir).removeRecursively();
}

void TestFrameCapture::testClipboardImageCapture() {
    QString mediaPath = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(m_engine->loadMedia(mediaPath.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Capture to clipboard via helper
    bool ok = FrameCaptureHelper::captureToClipboard(m_engine.get(), CaptureMode::Video);
    QVERIFY(ok);

    QImage clipImg = QGuiApplication::clipboard()->image();
    QVERIFY(!clipImg.isNull());
    QCOMPARE(clipImg.width(), 640);
    QCOMPARE(clipImg.height(), 360);
}

void TestFrameCapture::testGuiResponsivenessAsyncCapture() {
    QString mediaPath = "F:/Projects/JonakiPlayer/tests/test_data/test_video_aac.mp4";
    QVERIFY(m_engine->loadMedia(mediaPath.toStdString()));
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Asynchronous capture helper must return immediately without blocking GUI
    QElapsedTimer timer;
    timer.start();
    bool started = FrameCaptureHelper::captureToFile(m_engine.get(), CaptureMode::Subtitles);
    qint64 elapsed = timer.elapsed();

    QVERIFY(started);
    // Non-blocking requirement: dispatch must return within < 100 ms
    QVERIFY(elapsed < 100);

    // Wait briefly for worker thread to complete file write
    QTest::qWait(500);
}

QTEST_MAIN(TestFrameCapture)
#include "test_frame_capture.moc"
