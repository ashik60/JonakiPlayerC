#include <QTest>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSignalSpy>
#include <QThread>

#include "../../src/playback/MpvPlaybackEngine.h"
#include "../../src/network/UrlValidator.h"

using namespace Jonaki;

/**
 * @brief Embedded minimal HTTP server for testing network media streaming and error conditions.
 */
class LocalHttpServer : public QTcpServer {
    Q_OBJECT

public:
    explicit LocalHttpServer(const QString& baseDir, QObject* parent = nullptr)
        : QTcpServer(parent), m_baseDir(baseDir) {}

    quint16 serverPort() const {
        return QTcpServer::serverPort();
    }

    QString getUrl(const QString& path) const {
        return QString("http://127.0.0.1:%1%2").arg(serverPort()).arg(path);
    }

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        auto* socket = new QTcpSocket(this);
        if (!socket->setSocketDescriptor(socketDescriptor)) {
            delete socket;
            return;
        }

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            QByteArray request = socket->readAll();
            QString reqStr = QString::fromUtf8(request);
            QStringList lines = reqStr.split("\r\n");
            if (lines.isEmpty()) {
                socket->disconnectFromHost();
                return;
            }

            QString firstLine = lines[0];
            QStringList tokens = firstLine.split(' ');
            if (tokens.size() < 2 || tokens[0] != "GET") {
                socket->disconnectFromHost();
                return;
            }

            QString path = tokens[1].split('?')[0]; // Strip query params

            // 1. Simulate 302 Redirect
            if (path == "/redirect_to_mp4") {
                QByteArray resp = "HTTP/1.1 302 Found\r\n"
                                  "Location: /test_video_aac.mp4\r\n"
                                  "Content-Length: 0\r\n\r\n";
                socket->write(resp);
                socket->flush();
                socket->disconnectFromHost();
                return;
            }

            // 2. Simulate 404 Not Found
            if (path == "/non_existent.mp4" || path == "/missing_segment.ts") {
                QByteArray resp = "HTTP/1.1 404 Not Found\r\n"
                                  "Content-Type: text/plain\r\n"
                                  "Content-Length: 9\r\n\r\nNot Found";
                socket->write(resp);
                socket->flush();
                socket->disconnectFromHost();
                return;
            }

            // 3. Simulate Invalid M3U8
            if (path == "/invalid.m3u8") {
                QByteArray resp = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: application/vnd.apple.mpegurl\r\n"
                                  "Content-Length: 26\r\n\r\n"
                                  "THIS_IS_NOT_A_VALID_M3U8\n";
                socket->write(resp);
                socket->flush();
                socket->disconnectFromHost();
                return;
            }

            // 4. File Serving
            QString filePath = m_baseDir + path;
            QFile file(filePath);
            if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
                QByteArray resp = "HTTP/1.1 404 Not Found\r\n"
                                  "Content-Length: 0\r\n\r\n";
                socket->write(resp);
                socket->flush();
                socket->disconnectFromHost();
                return;
            }

            QByteArray body = file.readAll();
            QString contentType = "application/octet-stream";
            if (path.endsWith(".mp4")) contentType = "video/mp4";
            else if (path.endsWith(".m3u8")) contentType = "application/vnd.apple.mpegurl";
            else if (path.endsWith(".ts")) contentType = "video/MP2T";

            QByteArray header = QString("HTTP/1.1 200 OK\r\n"
                                        "Content-Type: %1\r\n"
                                        "Content-Length: %2\r\n"
                                        "Connection: close\r\n\r\n")
                                    .arg(contentType)
                                    .arg(body.size()).toUtf8();

            socket->write(header);
            socket->write(body);
            socket->flush();
            socket->disconnectFromHost();
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
    }

private:
    QString m_baseDir;
};

class TestNetworkStreaming : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDirectHttpMp4Playback();
    void testHlsVodStreaming();
    void testHttpRedirectHandling();
    void testHttp404ErrorHandling();
    void testInvalidM3u8Handling();
    void testMissingHlsSegmentErrorRecovery();
    void testEngineShutdownDuringNetworkStream();

private:
    std::unique_ptr<LocalHttpServer> m_server;
    std::unique_ptr<MpvPlaybackEngine> m_engine;
    QString m_serverBaseDir;
};

void TestNetworkStreaming::initTestCase() {
    m_serverBaseDir = "F:/Projects/JonakiPlayer/tests/test_data";
    m_server = std::make_unique<LocalHttpServer>(m_serverBaseDir);
    QVERIFY(m_server->listen(QHostAddress::LocalHost, 0));
}

void TestNetworkStreaming::cleanupTestCase() {
    if (m_server) {
        m_server->close();
    }
}

void TestNetworkStreaming::init() {
    m_engine = std::make_unique<MpvPlaybackEngine>();
    QVERIFY(m_engine->initialize());
}

void TestNetworkStreaming::cleanup() {
    if (m_engine) {
        m_engine->shutdown();
        m_engine.reset();
    }
}

void TestNetworkStreaming::testDirectHttpMp4Playback() {
    QString streamUrl = m_server->getUrl("/test_video_aac.mp4");
    QVERIFY(UrlValidator::isValidMediaUrl(streamUrl));

    QVERIFY(m_engine->loadMedia(streamUrl.toStdString()));

    // Verify transition to Playing state and playback advancement
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);

    // Verify duration reporting and seeking
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getDuration() > 0.0, 3000);
    m_engine->seek(1.0, true);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() >= 0.5, 3000);

    // Verify Pause and Resume
    m_engine->pause();
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Paused, 3000);
    m_engine->play();
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 3000);
}

void TestNetworkStreaming::testHlsVodStreaming() {
    QString hlsUrl = m_server->getUrl("/hls/playlist.m3u8");
    QVERIFY(UrlValidator::isValidMediaUrl(hlsUrl));
    QVERIFY(UrlValidator::isHlsUrl(hlsUrl));

    QVERIFY(m_engine->loadMedia(hlsUrl.toStdString()));

    // Verify playback starts for HLS stream
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);
    QVERIFY(m_engine->getDuration() > 0.0);
}

void TestNetworkStreaming::testHttpRedirectHandling() {
    QString redirectUrl = m_server->getUrl("/redirect_to_mp4");
    QVERIFY(m_engine->loadMedia(redirectUrl.toStdString()));

    // libmpv/ffmpeg should follow the redirect automatically
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Playing, 5000);
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getCurrentTime() > 0.05, 5000);
}

void TestNetworkStreaming::testHttp404ErrorHandling() {
    QString errorUrl = m_server->getUrl("/non_existent.mp4");
    bool errorTriggered = false;

    m_engine->setOnError([&errorTriggered](PlaybackError err, const std::string& msg) {
        (void)err;
        (void)msg;
        errorTriggered = true;
    });

    QVERIFY(m_engine->loadMedia(errorUrl.toStdString()));

    // Playback should stop and report error
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Stopped, 5000);
}

void TestNetworkStreaming::testInvalidM3u8Handling() {
    QString invalidM3u8Url = m_server->getUrl("/invalid.m3u8");
    m_engine->loadMedia(invalidM3u8Url.toStdString());

    // Demuxing fails and transitions cleanly to Stopped without crash
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Stopped, 5000);
}

void TestNetworkStreaming::testMissingHlsSegmentErrorRecovery() {
    // Construct an M3U8 referencing a missing segment
    QString badPlaylistPath = m_serverBaseDir + "/hls/bad_segment.m3u8";
    QFile f(badPlaylistPath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write("#EXTM3U\n#EXT-X-VERSION:3\n#EXT-X-TARGETDURATION:5\n#EXTINF:5.0,\nmissing_segment.ts\n#EXT-X-ENDLIST\n");
        f.close();
    }

    QString url = m_server->getUrl("/hls/bad_segment.m3u8");
    m_engine->loadMedia(url.toStdString());

    // Should handle segment failure gracefully
    QTRY_VERIFY_WITH_TIMEOUT(m_engine->getState() == PlaybackState::Stopped, 6000);

    QFile::remove(badPlaylistPath);
}

void TestNetworkStreaming::testEngineShutdownDuringNetworkStream() {
    QString streamUrl = m_server->getUrl("/test_video_aac.mp4");
    m_engine->loadMedia(streamUrl.toStdString());
    // Immediately shutdown during buffering/network loading
    m_engine->shutdown();
    QVERIFY(!m_engine->isInitialized());
}

QTEST_MAIN(TestNetworkStreaming)
#include "test_network_streaming.moc"
