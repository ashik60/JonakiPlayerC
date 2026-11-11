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
    QString urlWithCreds = "http://admin:secretPass123@example.com/stream.mp4";
    QString sanitized = UrlValidator::sanitizeForLogging(urlWithCreds);
    QVERIFY(!sanitized.contains("secretPass123"));
    QVERIFY(!sanitized.contains("admin"));
    QVERIFY(sanitized.contains("***:***@example.com"));
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

QTEST_MAIN(TestOpenUrlDialog)
#include "test_open_url_dialog.moc"
