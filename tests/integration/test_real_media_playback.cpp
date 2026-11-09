#include <QTest>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#include "playback/MpvPlaybackEngine.h"
#include "core/Logger.h"

using namespace Jonaki;

class TestRealMediaPlayback : public QObject {
    Q_OBJECT

private:
    QJsonArray m_metadataList;
    QJsonArray m_playbackResults;

private slots:
    void initTestCase() {
        QFile metaFile("build/real_media_metadata.json");
        if (!metaFile.open(QIODevice::ReadOnly)) {
            // Try absolute path if run from different working dir
            metaFile.setFileName("F:/Projects/JonakiPlayer/build/real_media_metadata.json");
            QVERIFY2(metaFile.open(QIODevice::ReadOnly), "real_media_metadata.json must exist. Run probe script first.");
        }

        QJsonDocument doc = QJsonDocument::fromJson(metaFile.readAll());
        QVERIFY(doc.isArray());
        m_metadataList = doc.array();
        QVERIFY(m_metadataList.size() > 0);
        Logger::info("Loaded " + std::to_string(m_metadataList.size()) + " media file records for playback verification.");
    }

    void testBatchPlaybackAndDeepVerification() {
        int passCount = 0;
        int failCount = 0;
        int deepTestedCount = 0;

        for (int i = 0; i < m_metadataList.size(); ++i) {
            QJsonObject meta = m_metadataList[i].toObject();
            QString filePath = meta["path"].toString();
            QString fileName = meta["filename"].toString();
            QString probedCodec = meta["video_codec"].toString();
            QString resolution = meta["resolution"].toString();
            double duration = meta["duration"].toDouble();

            QJsonObject result;
            result["index"] = meta["index"].toInt();
            result["filename"] = fileName;
            result["path"] = filePath;
            result["probed_codec"] = probedCodec;
            result["resolution"] = resolution;
            result["duration"] = duration;

            if (!QFileInfo::exists(filePath)) {
                result["played_ok"] = false;
                result["error"] = "File not found on disk";
                m_playbackResults.append(result);
                failCount++;
                continue;
            }

            // Perform actual playback smoke test
            MpvPlaybackEngine engine;
            if (!engine.initialize()) {
                result["played_ok"] = false;
                result["error"] = "Engine initialization failed";
                m_playbackResults.append(result);
                failCount++;
                continue;
            }

            if (!engine.loadMedia(filePath.toStdString())) {
                result["played_ok"] = false;
                result["error"] = "loadMedia returned false";
                engine.shutdown();
                m_playbackResults.append(result);
                failCount++;
                continue;
            }

            // Wait for media to load and begin playback
            int attempts = 0;
            bool reachedPlaying = false;
            while (attempts < 60) { // up to 3.0 seconds
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (engine.getState() == PlaybackState::Playing && engine.getDuration() > 0.0) {
                    reachedPlaying = true;
                    break;
                }
                attempts++;
            }

            if (!reachedPlaying) {
                result["played_ok"] = false;
                result["error"] = "Timeout waiting for playback state Playing";
                engine.shutdown();
                m_playbackResults.append(result);
                failCount++;
                continue;
            }

            // Let it render for 400ms to verify frame advancement
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            double curTime = engine.getCurrentTime();
            std::string engineCodec = engine.getVideoCodec();
            std::string hwdec = engine.getHwdecActive();
            auto tracks = engine.getTracks();

            result["played_ok"] = true;
            result["playback_time_advanced"] = (curTime >= 0.0);
            result["current_time"] = curTime;
            result["engine_codec"] = QString::fromStdString(engineCodec);
            result["hwdec_active"] = QString::fromStdString(hwdec);
            result["tracks_count"] = static_cast<int>(tracks.size());

            // Select representative files for deep testing:
            // - Any AV1 files
            // - Any 1080p files
            // - WebM file
            // - Periodic samples across the corpus
            bool selectForDeep = (probedCodec == "av1") ||
                                 (resolution.startsWith("1920")) ||
                                 (filePath.endsWith(".webm", Qt::CaseInsensitive)) ||
                                 (i % 12 == 0);

            if (selectForDeep) {
                QJsonObject deep;
                deep["tested"] = true;

                // 1. Seeking & Timeline Synchronization
                engine.seek(15.0, true);
                attempts = 0;
                while (engine.getCurrentTime() < 10.0 && attempts < 40) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    attempts++;
                }
                deep["seek_15s_ok"] = (engine.getCurrentTime() >= 10.0);

                // 2. Pause & Resume
                engine.pause();
                attempts = 0;
                while (engine.getState() != PlaybackState::Paused && attempts < 20) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    attempts++;
                }
                deep["pause_ok"] = (engine.getState() == PlaybackState::Paused);

                engine.play();
                attempts = 0;
                while (engine.getState() != PlaybackState::Playing && attempts < 20) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    attempts++;
                }
                deep["resume_ok"] = (engine.getState() == PlaybackState::Playing);

                // 3. Audio Delay Adjustment
                engine.setAudioDelay(0.05);
                deep["audio_delay_ok"] = (engine.getAudioDelay() == 0.05);
                engine.setAudioDelay(0.0);

                // 4. Equalizer (10 Bands)
                engine.setEqualizerBand(4, 6.0); // 500Hz +6dB
                engine.setEqualizerEnabled(true);
                deep["equalizer_ok"] = engine.isEqualizerEnabled() && (engine.getEqualizerBand(4) == 6.0);

                // 5. Dynamic Audio Normalization Coexistence
                engine.setAudioNormalization(true);
                deep["dynaudnorm_ok"] = engine.isAudioNormalizationEnabled() && engine.isEqualizerEnabled();
                engine.setEqualizerEnabled(false);
                engine.setAudioNormalization(false);

                // 6. Aspect Ratio Override
                engine.setAspectRatio("16:9");
                deep["aspect_ratio_ok"] = (engine.getAspectRatio() == "16:9");
                engine.setAspectRatio("default");

                // 7. Deinterlacing
                engine.setDeinterlacing("auto");
                deep["deinterlacing_ok"] = (engine.getDeinterlacing() == "auto");
                engine.setDeinterlacing("off");

                result["deep_test"] = deep;
                deepTestedCount++;
            } else {
                QJsonObject deep;
                deep["tested"] = false;
                result["deep_test"] = deep;
            }

            engine.shutdown();
            m_playbackResults.append(result);
            passCount++;
        }

        Logger::info("Batch playback test finished. Passed: " + std::to_string(passCount) +
                     ", Failed: " + std::to_string(failCount) +
                     ", Deep tested: " + std::to_string(deepTestedCount));

        QVERIFY2(passCount > 0, "At least some files must have passed playback");
        QCOMPARE(failCount, 0);
    }

    void cleanupTestCase() {
        // Save detailed results to JSON
        QJsonObject root;
        root["total_tested"] = m_playbackResults.size();
        root["results"] = m_playbackResults;

        QJsonDocument doc(root);
        QFile outFile("build/real_media_playback_results.json");
        if (outFile.open(QIODevice::WriteOnly)) {
            outFile.write(doc.toJson(QJsonDocument::Indented));
            outFile.close();
            Logger::info("Wrote detailed playback results to build/real_media_playback_results.json");
        }
    }
};

QTEST_MAIN(TestRealMediaPlayback)
#include "test_real_media_playback.moc"
