#include <QTest>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QClipboard>
#include <QGuiApplication>

#include "../../src/ui/OpenUrlDialog.h"
#include "../../src/network/UrlValidator.h"
#include "../../src/settings/SettingsManager.h"

using namespace Jonaki;

class TestOpenUrlDialog : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    void testUrlValidatorValidSchemes();
    void testUrlValidatorInvalidOrEmpty();
    void testUrlValidatorPreservesQueryAndFragment();
    void testUrlValidatorMasksCredentials();
    void testUrlValidatorHlsDetection();

    void testDialogInitialLayoutAndControls();
    void testDialogUrlValidationOnAccept();
    void testDialogClipboardAutoPasteEnabled();
    void testDialogClipboardAutoPasteDisabled();
    void testDialogPreservesManualInputOverClipboard();
    void testDialogHistoryPersistenceAndDeduplication();
    void testDialogClearHistory();
    void testDialogDeleteSelectedHistory();
    void testCaptureDialogScreenshot();
    void testUrlValidatorCaseInsensitiveAndPercentEncodedCdnParameters();
    void testProcessSubtitlesCheckboxWiring();
    void testStoreOnlyEnteredUrlsManualEntry();
};

void TestOpenUrlDialog::initTestCase() {
    SettingsManager::instance().resetToDefaults();
}

void TestOpenUrlDialog::cleanupTestCase() {
    SettingsManager::instance().resetToDefaults();
}

void TestOpenUrlDialog::init() {
    SettingsManager::instance().resetToDefaults();
}

void TestOpenUrlDialog::testUrlValidatorValidSchemes() {
    QString err;
    QVERIFY(UrlValidator::isValidMediaUrl("http://example.com/video.mp4", &err));
    QVERIFY(err.isEmpty());

    QVERIFY(UrlValidator::isValidMediaUrl("https://stream.example.org:8080/live/master.m3u8", &err));
    QVERIFY(err.isEmpty());

    QVERIFY(UrlValidator::isValidMediaUrl("rtmp://live.stream.net/app/streamKey", &err));
    QVERIFY(UrlValidator::isValidMediaUrl("rtmps://secure.stream.net/app/streamKey", &err));
}

void TestOpenUrlDialog::testUrlValidatorInvalidOrEmpty() {
    QString err;
    QVERIFY(!UrlValidator::isValidMediaUrl("", &err));
    QCOMPARE(err, QString("URL cannot be empty."));

    QVERIFY(!UrlValidator::isValidMediaUrl("ftp://files.example.com/movie.mkv", &err));
    QVERIFY(err.contains("Unsupported URL scheme"));

    QVERIFY(!UrlValidator::isValidMediaUrl("file:///C:/movies/video.mp4", &err));
    QVERIFY(err.contains("Unsupported URL scheme"));

    QVERIFY(!UrlValidator::isValidMediaUrl("http://", &err));
    QVERIFY(err.contains("missing a valid host"));

    QVERIFY(!UrlValidator::isValidMediaUrl("not_a_url", &err));
}

void TestOpenUrlDialog::testUrlValidatorPreservesQueryAndFragment() {
    QString complexUrl = "https://example.com/playlist.m3u8?token=xyz123&expire=18900000#track=audio1";
    QString err;
    QVERIFY(UrlValidator::isValidMediaUrl(complexUrl, &err));
    QCOMPARE(UrlValidator::normalizeUrl(complexUrl), complexUrl);
}

void TestOpenUrlDialog::testUrlValidatorMasksCredentials() {
    QString urlWithCreds = "http://admin:secretPass123@example.com/stream.mp4?token=secretToken456&signature=sig789&user=john";
    QString sanitized = UrlValidator::sanitizeForLogging(urlWithCreds);
    QVERIFY(!sanitized.contains("secretPass123"));
    QVERIFY(!sanitized.contains("admin"));
    QVERIFY(sanitized.contains("***:***@example.com"));
    QVERIFY(!sanitized.contains("secretToken456"));
    QVERIFY(!sanitized.contains("sig789"));
    QVERIFY(sanitized.contains("token=***"));
    QVERIFY(sanitized.contains("signature=***"));
    QVERIFY(sanitized.contains("user=john"));
}

void TestOpenUrlDialog::testUrlValidatorHlsDetection() {
    QVERIFY(UrlValidator::isHlsUrl("http://example.com/live/index.m3u8"));
    QVERIFY(UrlValidator::isHlsUrl("https://cdn.net/hls/master.M3U8?sig=abc"));
    QVERIFY(!UrlValidator::isHlsUrl("http://example.com/video.mp4"));
}

void TestOpenUrlDialog::testDialogInitialLayoutAndControls() {
    OpenUrlDialog dlg;
    QCOMPARE(dlg.windowTitle(), QString("Open URL"));

    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    QVERIFY(combo != nullptr);
    QVERIFY(combo->isEditable());

    auto* chkAuto = dlg.findChild<QCheckBox*>("chkAutoPaste");
    QVERIFY(chkAuto != nullptr);

    auto* chkSubs = dlg.findChild<QCheckBox*>("chkProcessSubtitles");
    QVERIFY(chkSubs != nullptr);
    QVERIFY(chkSubs->isChecked());

    auto* btnOpen = dlg.findChild<QPushButton*>("btnOpen");
    QVERIFY(btnOpen != nullptr);
    QVERIFY(btnOpen->isDefault());

    auto* btnCancel = dlg.findChild<QPushButton*>("btnCancel");
    QVERIFY(btnCancel != nullptr);
}

void TestOpenUrlDialog::testDialogUrlValidationOnAccept() {
    OpenUrlDialog dlg;
    dlg.show();
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    auto* errLabel = dlg.findChild<QLabel*>("errorLabel");
    QVERIFY(errLabel != nullptr);
    QVERIFY(!errLabel->isVisible());

    // Set invalid URL and attempt to accept
    combo->setEditText("invalid_url_without_scheme");
    dlg.accept();

    // Dialog should not have closed with accepted result and should show error
    QVERIFY(dlg.result() != QDialog::Accepted);
    QVERIFY(errLabel->isVisible());
    QVERIFY(!errLabel->text().isEmpty());

    // Now set valid URL
    combo->setEditText("https://example.com/valid.mp4");
    dlg.accept();
    QCOMPARE(dlg.result(), (int)QDialog::Accepted);
    QCOMPARE(dlg.getUrl(), QString("https://example.com/valid.mp4"));
}

void TestOpenUrlDialog::testDialogClipboardAutoPasteEnabled() {
    SettingsManager::instance().setBool("Network", "AutoPasteClipboard", true);
    QGuiApplication::clipboard()->setText("https://test-stream.org/live.m3u8");

    OpenUrlDialog dlg;
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    QCOMPARE(combo->currentText(), QString("https://test-stream.org/live.m3u8"));
}

void TestOpenUrlDialog::testDialogClipboardAutoPasteDisabled() {
    SettingsManager::instance().setBool("Network", "AutoPasteClipboard", false);
    QGuiApplication::clipboard()->setText("https://test-stream.org/live.m3u8");

    OpenUrlDialog dlg;
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    QVERIFY(combo->currentText().isEmpty());
}

void TestOpenUrlDialog::testDialogPreservesManualInputOverClipboard() {
    SettingsManager::instance().setBool("Network", "AutoPasteClipboard", true);
    QGuiApplication::clipboard()->setText("https://clipboard.org/clip.mp4");

    OpenUrlDialog dlg;
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    combo->setEditText("https://manual-entered.org/manual.mp4");

    // Copy new text to clipboard while dialog is open
    QGuiApplication::clipboard()->setText("https://another-clipboard.org/another.mp4");

    // Manual input must be preserved and not overwritten
    QCOMPARE(combo->currentText(), QString("https://manual-entered.org/manual.mp4"));
}

void TestOpenUrlDialog::testDialogHistoryPersistenceAndDeduplication() {
    {
        OpenUrlDialog dlg;
        auto* combo = dlg.findChild<QComboBox*>("urlCombo");
        combo->setEditText("https://example.com/stream1.mp4");
        dlg.accept();
    }
    {
        OpenUrlDialog dlg;
        auto* combo = dlg.findChild<QComboBox*>("urlCombo");
        combo->setEditText("https://example.com/stream2.m3u8");
        dlg.accept();
    }
    {
        // Re-enter stream1 - it should move to the top without duplicating
        OpenUrlDialog dlg;
        auto* combo = dlg.findChild<QComboBox*>("urlCombo");
        combo->setEditText("https://example.com/stream1.mp4");
        dlg.accept();
    }

    // Verify stored history in settings
    QString saved = QString::fromStdString(SettingsManager::instance().getString("Network", "RecentUrls", ""));
    QStringList urls = saved.split(";;", Qt::SkipEmptyParts);
    QCOMPARE(urls.size(), 2);
    QCOMPARE(urls[0], QString("https://example.com/stream1.mp4"));
    QCOMPARE(urls[1], QString("https://example.com/stream2.m3u8"));
}

void TestOpenUrlDialog::testDialogClearHistory() {
    SettingsManager::instance().setString("Network", "RecentUrls", "http://u1.com;;http://u2.com");

    OpenUrlDialog dlg;
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    QCOMPARE(combo->count(), 2);

    auto* btnClear = dlg.findChild<QPushButton*>("btnClearAll");
    QVERIFY(btnClear != nullptr);
    btnClear->click();

    QCOMPARE(combo->count(), 0);
    QCOMPARE(QString::fromStdString(SettingsManager::instance().getString("Network", "RecentUrls", "")), QString(""));
}

void TestOpenUrlDialog::testDialogDeleteSelectedHistory() {
    SettingsManager::instance().setString("Network", "RecentUrls", "http://u1.com;;http://u2.com;;http://u3.com");

    OpenUrlDialog dlg;
    auto* combo = dlg.findChild<QComboBox*>("urlCombo");
    QCOMPARE(combo->count(), 3);

    combo->setCurrentIndex(1); // Select http://u2.com
    auto* btnDelete = dlg.findChild<QPushButton*>("btnDeleteSelected");
    QVERIFY(btnDelete != nullptr);
    btnDelete->click();

    QCOMPARE(combo->count(), 2);
    QCOMPARE(combo->itemText(0), QString("http://u1.com"));
    QCOMPARE(combo->itemText(1), QString("http://u3.com"));
}

void TestOpenUrlDialog::testCaptureDialogScreenshot() {
    SettingsManager::instance().setString("Network", "RecentUrls", "https://stream.example.com/live/feed.m3u8;;http://media.example.org/test.mp4");
    OpenUrlDialog dlg;
    dlg.show();
    QTest::qWait(100);

    QPixmap pix = dlg.grab();
    QVERIFY(!pix.isNull());
    QDir().mkpath("F:/Projects/JonakiPlayer/docs/reverse-engineering/evidence");
    QVERIFY(pix.save("F:/Projects/JonakiPlayer/docs/reverse-engineering/evidence/jonaki_open_url_dialog.png"));
}

void TestOpenUrlDialog::testUrlValidatorCaseInsensitiveAndPercentEncodedCdnParameters() {
    // 1. AWS S3 signed parameters (mixed case, percent-encoded, tokens)
    QString awsUrl = "https://s3.amazonaws.com/bucket/stream.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIOSFODNN7EXAMPLE%2F20261112%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20261112T000000Z&X-Amz-Security-Token=AQoDYXdzEJr...&X-Amz-Signature=abcd1234efgh&user=johnDoe&video_id=987";
    QString sanitizedAws = UrlValidator::sanitizeForLogging(awsUrl);
    QVERIFY(!sanitizedAws.contains("AKIAIOSFODNN7EXAMPLE"));
    QVERIFY(!sanitizedAws.contains("AQoDYXdzEJr"));
    QVERIFY(!sanitizedAws.contains("abcd1234efgh"));
    QVERIFY(sanitizedAws.contains("X-Amz-Credential=***"));
    QVERIFY(sanitizedAws.contains("X-Amz-Signature=***"));
    QVERIFY(sanitizedAws.contains("X-Amz-Security-Token=***"));
    QVERIFY(sanitizedAws.contains("user=johnDoe"));
    QVERIFY(sanitizedAws.contains("video_id=987"));

    // 2. Akamai Token Authentication (hdnts, hdnea, uppercase)
    QString akamaiUrl = "https://live.akamai.com/live/feed.m3u8?hdnts=exp=1700000000~acl=/*~data=fast~hmac=98765fedcba&HDNEA=exp=1800000000~hmac=xyz789&channel=sportsHD";
    QString sanitizedAkamai = UrlValidator::sanitizeForLogging(akamaiUrl);
    QVERIFY(!sanitizedAkamai.contains("98765fedcba"));
    QVERIFY(!sanitizedAkamai.contains("xyz789"));
    QVERIFY(sanitizedAkamai.contains("hdnts=***"));
    QVERIFY(sanitizedAkamai.contains("HDNEA=***"));
    QVERIFY(sanitizedAkamai.contains("channel=sportsHD"));

    // 3. CloudFront signed URLs (Policy, Key-Pair-Id, Signature with percent encoding)
    QString cfUrl = "https://d111111abcdef8.cloudfront.net/horizon.mkv?Policy=eyJTdGF0ZW1lbnQi...&Key%2DPair%2DId=APKAJGENFDEXAMPLE&Signature=cfSignature777&title=EpicMovie";
    QString sanitizedCf = UrlValidator::sanitizeForLogging(cfUrl);
    QVERIFY(!sanitizedCf.contains("eyJTdGF0ZW1lbnQi"));
    QVERIFY(!sanitizedCf.contains("APKAJGENFDEXAMPLE"));
    QVERIFY(!sanitizedCf.contains("cfSignature777"));
    QVERIFY(sanitizedCf.contains("Policy=***"));
    QVERIFY(sanitizedCf.contains("Key%2DPair%2DId=***") || sanitizedCf.contains("Key-Pair-Id=***"));
    QVERIFY(sanitizedCf.contains("Signature=***"));
    QVERIFY(sanitizedCf.contains("title=EpicMovie"));

    // 4. Google Cloud Storage signed URLs (GoogleAccessId, x-goog-signature)
    QString gcsUrl = "https://storage.googleapis.com/my-bucket/movie.mp4?GoogleAccessId=service@account.iam.gserviceaccount.com&x-goog-signature=gcsSignature888&x-goog-algorithm=GOOG4-RSA-SHA256&format=mp4";
    QString sanitizedGcs = UrlValidator::sanitizeForLogging(gcsUrl);
    QVERIFY(!sanitizedGcs.contains("service@account.iam.gserviceaccount.com"));
    QVERIFY(!sanitizedGcs.contains("gcsSignature888"));
    QVERIFY(sanitizedGcs.contains("GoogleAccessId=***"));
    QVERIFY(sanitizedGcs.contains("x-goog-signature=***"));
    QVERIFY(sanitizedGcs.contains("format=mp4"));

    // 5. Azure Blob Storage SAS Token (sp, se, sv, sr, sig)
    QString azureUrl = "https://mystorage.blob.core.windows.net/videos/clip.mp4?sp=r&se=2026-11-12T00:00:00Z&sv=2021-06-08&sr=b&sig=azureSignature999&filename=clip.mp4";
    QString sanitizedAzure = UrlValidator::sanitizeForLogging(azureUrl);
    QVERIFY(!sanitizedAzure.contains("azureSignature999"));
    QVERIFY(sanitizedAzure.contains("sig=***"));
    QVERIFY(sanitizedAzure.contains("sp=***"));
    QVERIFY(sanitizedAzure.contains("se=***"));
    QVERIFY(sanitizedAzure.contains("sv=***"));
    QVERIFY(sanitizedAzure.contains("sr=***"));
    QVERIFY(sanitizedAzure.contains("filename=clip.mp4"));
}

void TestOpenUrlDialog::testProcessSubtitlesCheckboxWiring() {
    OpenUrlDialog dlg;
    auto* chkSubs = dlg.findChild<QCheckBox*>("chkProcessSubtitles");
    QVERIFY(chkSubs != nullptr);

    // Default must be true (Process subtitles is checked)
    QVERIFY(chkSubs->isChecked());
    QVERIFY(dlg.isProcessSubtitlesEnabled());

    // Toggle off
    chkSubs->setChecked(false);
    QVERIFY(!dlg.isProcessSubtitlesEnabled());

    // Programmatic setter
    dlg.setProcessSubtitlesEnabled(true);
    QVERIFY(chkSubs->isChecked());
    QVERIFY(dlg.isProcessSubtitlesEnabled());
}

void TestOpenUrlDialog::testStoreOnlyEnteredUrlsManualEntry() {
    // With StoreEnteredUrlsOnly = true (default)
    SettingsManager::instance().setBool("Network", "StoreEnteredUrlsOnly", true);
    SettingsManager::instance().setString("Network", "RecentUrls", "");

    OpenUrlDialog dlg1;
    auto* combo1 = dlg1.findChild<QComboBox*>("urlCombo");
    combo1->setEditText("https://stream1.example.com/live.m3u8");
    dlg1.accept();

    QString recents1 = QString::fromStdString(SettingsManager::instance().getString("Network", "RecentUrls", ""));
    QCOMPARE(recents1, QString("https://stream1.example.com/live.m3u8"));

    // With StoreEnteredUrlsOnly = false
    SettingsManager::instance().setBool("Network", "StoreEnteredUrlsOnly", false);
    OpenUrlDialog dlg2;
    auto* combo2 = dlg2.findChild<QComboBox*>("urlCombo");
    combo2->setEditText("https://stream2.example.com/live.m3u8");
    dlg2.accept();

    QString recents2 = QString::fromStdString(SettingsManager::instance().getString("Network", "RecentUrls", ""));
    QVERIFY(recents2.contains("https://stream2.example.com/live.m3u8"));
    QVERIFY(recents2.contains("https://stream1.example.com/live.m3u8"));
}

QTEST_MAIN(TestOpenUrlDialog)
#include "test_open_url_dialog.moc"
