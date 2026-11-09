#include <QTest>
#include <QFileInfo>
#include <QDir>
#include <thread>
#include <chrono>

#include "playback/MpvPlaybackEngine.h"
#include "subtitles/SubtitleManager.h"

using namespace Jonaki;

class TestSubtitles : public QObject {
    Q_OBJECT

private:
    std::string m_videoPath;
    std::string m_srtPath;
    std::string m_assPath;
    std::string m_vttPath;
    std::string m_unicodeSrtPath;

private slots:
    void initTestCase() {
        QDir dataDir("tests/test_data");
        if (!dataDir.exists()) dataDir = QDir("F:/Projects/JonakiPlayer/tests/test_data");

        m_videoPath = dataDir.absoluteFilePath("test_video_aac.mp4").toStdString();
        m_srtPath = dataDir.absoluteFilePath("test_sub.srt").toStdString();
        m_assPath = dataDir.absoluteFilePath("test_sub.ass").toStdString();
        m_vttPath = dataDir.absoluteFilePath("test_sub.vtt").toStdString();
        m_unicodeSrtPath = dataDir.absoluteFilePath("test_sub_unicode.srt").toStdString();

        QVERIFY(QFileInfo::exists(QString::fromStdString(m_videoPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_srtPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_assPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_vttPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_unicodeSrtPath)));
    }

    void testExternalSubtitleLoading() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_videoPath));

        // Load external SRT
        engine.loadExternalSubtitle(m_srtPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Load external ASS
        engine.loadExternalSubtitle(m_assPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Load external VTT
        engine.loadExternalSubtitle(m_vttPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Subtitle visibility toggle
        engine.setSubtitleVisibility(false);
        QCOMPARE(engine.isSubtitleVisible(), false);

        engine.setSubtitleVisibility(true);
        QCOMPARE(engine.isSubtitleVisible(), true);

        engine.shutdown();
    }

    void testSubtitleDelayAndOsd() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_videoPath));

        SubtitleManager subMgr(&engine);
        QCOMPARE(subMgr.currentDelay(), 0.0);

        // Adjust delay +0.5s
        subMgr.adjustDelay(0.5);
        QCOMPARE(subMgr.currentDelay(), 0.5);

        // Adjust delay -1.0s (results in -0.5s)
        subMgr.adjustDelay(-1.0);
        QCOMPARE(subMgr.currentDelay(), -0.5);

        // Reset delay
        subMgr.resetDelay();
        QCOMPARE(subMgr.currentDelay(), 0.0);

        // Show OSD notification
        engine.showOsdMessage("Test OSD Message", 1000);

        engine.shutdown();
    }

    void testSubtitleAutoMatching() {
        MpvPlaybackEngine engine;
        SubtitleManager subMgr(&engine);

        // Create temporary media file and matching subtitle
        QDir dir("build/test_output");
        dir.mkpath(".");
        QString mediaFile = dir.absoluteFilePath("sample_episode.mkv");
        QString matchingSrt = dir.absoluteFilePath("sample_episode.srt");
        QString matchingEnSrt = dir.absoluteFilePath("sample_episode.en.srt");
        QString nonMatchingSrt = dir.absoluteFilePath("other_movie.srt");

        QFile(mediaFile).open(QIODevice::WriteOnly);
        QFile(matchingSrt).open(QIODevice::WriteOnly);
        QFile(matchingEnSrt).open(QIODevice::WriteOnly);
        QFile(nonMatchingSrt).open(QIODevice::WriteOnly);

        QStringList matches = subMgr.findMatchingSubtitles(mediaFile.toStdString());
        QCOMPARE(matches.size(), 2);
        QVERIFY(matches.contains(matchingSrt));
        QVERIFY(matches.contains(matchingEnSrt));
        QVERIFY(!matches.contains(nonMatchingSrt));
    }
};

QTEST_MAIN(TestSubtitles)
#include "test_subtitles.moc"
