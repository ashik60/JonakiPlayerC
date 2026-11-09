#include <QTest>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

#include "playlist/DplPlaylistParser.h"
#include "playlist/ResumeStore.h"
#include "playlist/PlaylistModel.h"

using namespace Jonaki;

class TestPlaylist : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        QDir().mkpath("build/test_output");
        ResumeStore::instance().initialize("build/test_output/test_mediainfo.sdb");
    }

    void cleanupTestCase() {
        ResumeStore::instance().close();
    }

    void testDplParsingRoundTrip() {
        // Multi-entry synthetic DPL with extra fields
        std::string sampleDpl = 
            "\xEF\xBB\xBF"
            "DAUMPLAYLIST\r\n"
            "playname=F:\\Media\\movie1.mkv\r\n"
            "playtime=123450\r\n"
            "topindex=0\r\n"
            "saveplaypos=1\r\n"
            "1*file*F:\\Media\\movie1.mkv\r\n"
            "1*title*Movie One (2024)\r\n"
            "1*duration2*7200000\r\n"
            "1*start*123450\r\n"
            "1*played*1\r\n"
            "2*file*F:\\Media\\movie2.mp4\r\n"
            "2*title*Movie Two\r\n"
            "2*duration2*5400000\r\n"
            "2*start*0\r\n";

        PlaylistData data;
        QVERIFY(DplPlaylistParser::parse(sampleDpl, data));

        QCOMPARE(data.playName, std::string("F:\\Media\\movie1.mkv"));
        QCOMPARE(data.playTimeMs, static_cast<int64_t>(123450));
        QCOMPARE(data.savePlayPos, 1);
        QCOMPARE(data.items.size(), static_cast<size_t>(2));

        // Check item 1
        QCOMPARE(data.items[0].id, 1);
        QCOMPARE(data.items[0].filePath, std::string("F:\\Media\\movie1.mkv"));
        QCOMPARE(data.items[0].title, std::string("Movie One (2024)"));
        QCOMPARE(data.items[0].durationMs, static_cast<int64_t>(7200000));
        QCOMPARE(data.items[0].startPosMs, static_cast<int64_t>(123450));
        QCOMPARE(data.items[0].extraFields.count("played"), static_cast<size_t>(1));
        QCOMPARE(data.items[0].extraFields["played"], std::string("1"));

        // Round-trip serialization
        std::string serialized = DplPlaylistParser::serialize(data);
        PlaylistData roundTripData;
        QVERIFY(DplPlaylistParser::parse(serialized, roundTripData));

        QCOMPARE(roundTripData.playName, data.playName);
        QCOMPARE(roundTripData.playTimeMs, data.playTimeMs);
        QCOMPARE(roundTripData.items.size(), static_cast<size_t>(2));
        QCOMPARE(roundTripData.items[0].extraFields["played"], std::string("1"));
    }

    void testReferencePotPlayerDpl() {
        QString refPath = "C:/Users/User/AppData/Roaming/PotPlayerMini64/Playlist/PotPlayerMini64.dpl";
        if (QFile::exists(refPath)) {
            PlaylistData data;
            QVERIFY(DplPlaylistParser::parseFile(refPath.toStdString(), data));
            QVERIFY(!data.items.empty());
            QVERIFY(!data.playName.empty());
            QVERIFY(data.items[0].durationMs > 0);
        }
    }

    void testResumeStorePersistence() {
        std::string testPath = "F:\\Movies\\TestMovie.mkv";
        int64_t expectedPos = 45200;
        int64_t expectedDur = 180000;

        QVERIFY(ResumeStore::instance().savePosition(testPath, expectedPos, expectedDur));

        int64_t readPos = 0;
        int64_t readDur = 0;
        QVERIFY(ResumeStore::instance().getPosition(testPath, readPos, readDur));
        QCOMPARE(readPos, expectedPos);
        QCOMPARE(readDur, expectedDur);

        // Clear
        QVERIFY(ResumeStore::instance().clearPosition(testPath));
        QCOMPARE(ResumeStore::instance().getPosition(testPath, readPos, readDur), false);
    }

    void testPlaylistModelOperations() {
        PlaylistModel model;
        QCOMPARE(model.rowCount(), 0);

        PlaylistItem item1{1, "video1.mp4", "Video 1", 60000, 0, {}};
        PlaylistItem item2{2, "video2.mp4", "Video 2", 120000, 0, {}};

        model.addItem(item1);
        model.addItem(item2);
        QCOMPARE(model.rowCount(), 2);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("Video 1"));
        QCOMPARE(model.data(model.index(1, 0), Qt::DisplayRole).toString(), QString("Video 2"));

        // Move
        model.moveItem(0, 1);
        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("Video 2"));

        // Remove
        model.removeItem(0);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("Video 1"));

        // Clear
        model.clear();
        QCOMPARE(model.rowCount(), 0);
    }
};

QTEST_MAIN(TestPlaylist)
#include "test_playlist.moc"
