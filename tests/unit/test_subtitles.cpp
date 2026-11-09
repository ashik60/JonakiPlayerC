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
    std::string m_bengaliSrtPath;

private slots:
    void initTestCase() {
        QDir dataDir("tests/test_data");
        if (!dataDir.exists()) dataDir = QDir("F:/Projects/JonakiPlayer/tests/test_data");

        m_videoPath = dataDir.absoluteFilePath("test_video_aac.mp4").toStdString();
        m_srtPath = dataDir.absoluteFilePath("test_sub.srt").toStdString();
        m_assPath = dataDir.absoluteFilePath("test_sub.ass").toStdString();
        m_vttPath = dataDir.absoluteFilePath("test_sub.vtt").toStdString();
        m_unicodeSrtPath = dataDir.absoluteFilePath("test_sub_unicode.srt").toStdString();
        m_bengaliSrtPath = dataDir.absoluteFilePath("test_sub_bengali.srt").toStdString();

        QVERIFY(QFileInfo::exists(QString::fromStdString(m_videoPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_srtPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_assPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_vttPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_unicodeSrtPath)));
        QVERIFY(QFileInfo::exists(QString::fromStdString(m_bengaliSrtPath)));
    }

    void testExternalSubtitleLoading() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_videoPath));

        // Wait for media to load
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);
        QVERIFY(engine.getDuration() > 0.0);

        // Load external SRT
        engine.loadExternalSubtitle(m_srtPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Load external ASS
        engine.loadExternalSubtitle(m_assPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Load external VTT
        engine.loadExternalSubtitle(m_vttPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

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

    void testBengaliSubtitleRendering() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_videoPath));

        // Wait for media to load and begin playback
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);
        QVERIFY(engine.getDuration() > 0.0);

        // Load Bengali subtitle containing conjuncts (যুক্তাক্ষর) and vowel signs (কারচিহ্ন)
        engine.loadExternalSubtitle(m_bengaliSrtPath);

        // Wait for subtitle track to be registered in track list
        bool hasSubTrack = false;
        attempts = 0;
        while (!hasSubTrack && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            auto tracks = engine.getTracks();
            for (const auto& trk : tracks) {
                if (trk.type == "sub") {
                    hasSubTrack = true;
                    break;
                }
            }
            attempts++;
        }
        QVERIFY(hasSubTrack);

        // Verify subtitle visibility is enabled
        engine.setSubtitleVisibility(true);
        QCOMPARE(engine.isSubtitleVisible(), true);

        // Seek to 1s where Bengali subtitle text is active
        engine.seek(1.0, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        engine.shutdown();
    }

    void testSubtitleTrackSwitching() {
        MpvPlaybackEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.loadMedia(m_videoPath));

        // Wait for media to load
        int attempts = 0;
        while ((engine.getState() != PlaybackState::Playing || engine.getDuration() <= 0.0) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            attempts++;
        }
        QCOMPARE(engine.getState(), PlaybackState::Playing);
        QVERIFY(engine.getDuration() > 0.0);

        // Load English and Bengali subtitles
        engine.loadExternalSubtitle(m_srtPath);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        engine.loadExternalSubtitle(m_bengaliSrtPath);

        std::vector<int> subTrackIds;
        attempts = 0;
        while (subTrackIds.size() < 2 && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            subTrackIds.clear();
            auto tracks = engine.getTracks();
            for (const auto& trk : tracks) {
                if (trk.type == "sub") {
                    subTrackIds.push_back(trk.id);
                }
            }
            attempts++;
        }
        QVERIFY(subTrackIds.size() >= 2);

        // Switch to first subtitle track
        QVERIFY(engine.selectTrack("sub", subTrackIds[0]));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Switch to second subtitle track
        QVERIFY(engine.selectTrack("sub", subTrackIds[1]));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify audio track selection also functions cleanly
        auto tracks = engine.getTracks();
        std::vector<int> audioTrackIds;
        for (const auto& trk : tracks) {
            if (trk.type == "audio") {
                audioTrackIds.push_back(trk.id);
            }
        }
        if (!audioTrackIds.empty()) {
            QVERIFY(engine.selectTrack("audio", audioTrackIds[0]));
        }

        engine.shutdown();
    }

    void testDeterministicSubtitlePriority() {
        MpvPlaybackEngine engine;
        SubtitleManager subMgr(&engine);

        QDir dir("build/test_output_prio");
        dir.mkpath(".");
        QString mediaFile = dir.absoluteFilePath("video.mp4");
        QString srtFile = dir.absoluteFilePath("video.srt");
        QString assFile = dir.absoluteFilePath("video.ass");
        QString vttFile = dir.absoluteFilePath("video.vtt");
        QString bnSrtFile = dir.absoluteFilePath("video.bn.srt");
        QString enSrtFile = dir.absoluteFilePath("video.en.srt");
        QString commentaryFile = dir.absoluteFilePath("video.commentary.srt");
        QString unrelated = dir.absoluteFilePath("other.srt");

        QFile(mediaFile).open(QIODevice::WriteOnly);
        QFile(srtFile).open(QIODevice::WriteOnly);
        QFile(assFile).open(QIODevice::WriteOnly);
        QFile(vttFile).open(QIODevice::WriteOnly);
        QFile(bnSrtFile).open(QIODevice::WriteOnly);
        QFile(enSrtFile).open(QIODevice::WriteOnly);
        QFile(commentaryFile).open(QIODevice::WriteOnly);
        QFile(unrelated).open(QIODevice::WriteOnly);

        QStringList matches = subMgr.findMatchingSubtitles(mediaFile.toStdString());
        QCOMPARE(matches.size(), 6);

        // Check tier 0 exact matches first, sorted by format rank: ASS > SRT > VTT
        QCOMPARE(matches[0], assFile);
        QCOMPARE(matches[1], srtFile);
        QCOMPARE(matches[2], vttFile);

        // Check tier 1 language tag matches next, sorted alphabetically: bn > en
        QCOMPARE(matches[3], bnSrtFile);
        QCOMPARE(matches[4], enSrtFile);

        // Check tier 2 other suffix matches
        QCOMPARE(matches[5], commentaryFile);
    }
};

QTEST_MAIN(TestSubtitles)
#include "test_subtitles.moc"
