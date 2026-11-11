#include <QTest>
#include <QSignalSpy>
#include <QTreeWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QFile>
#include <QDir>

#include "../../src/settings/SettingsManager.h"
#include "../../src/settings/SettingsTransaction.h"
#include "../../src/ui/PreferencesDialog.h"
#include "../../src/ui/preferences/GeneralPreferencesPage.h"
#include "../../src/ui/preferences/PlaybackPreferencesPage.h"
#include "../../src/ui/preferences/VideoPreferencesPage.h"
#include "../../src/ui/preferences/AudioPreferencesPage.h"
#include "../../src/ui/preferences/SubtitlePreferencesPage.h"
#include "../../src/ui/preferences/FilterControlPreferencesPage.h"

using namespace Jonaki;

class TestPreferencesDialog : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    // 1. Transaction unit tests
    void testTransactionStagingAndCommit();
    void testTransactionDiscard();
    void testTransactionResetToDefaults();
    void testTransactionExportToFile();

    // 2. Dialog GUI & Tree navigation tests
    void testDialogGeometryAndTreeHierarchy();
    void testPageNavigationAcrossAllLeafNodes();

    // 3. Transactional GUI semantics (Apply, Cancel, OK, Reset)
    void testApplyAndCancelSemantics();
    void testResetCurrentPageSemantics();
};

void TestPreferencesDialog::initTestCase() {
    SettingsManager::instance().resetToDefaults();
}

void TestPreferencesDialog::cleanupTestCase() {
    QFile::remove("test_export_preferences.ini");
}

void TestPreferencesDialog::init() {
    SettingsManager::instance().resetToDefaults();
}

void TestPreferencesDialog::testTransactionStagingAndCommit() {
    auto& sm = SettingsManager::instance();
    sm.setBool("General", "SnapToEdge", true);

    SettingsTransaction tx(sm);
    QCOMPARE(tx.getBool("General", "SnapToEdge", false), true);
    QVERIFY(!tx.isDirty());

    // Modify in transaction
    tx.setBool("General", "SnapToEdge", false);
    QVERIFY(tx.isDirty());
    QCOMPARE(tx.getBool("General", "SnapToEdge", true), false);

    // Underlying manager must still be untouched!
    QCOMPARE(sm.getBool("General", "SnapToEdge", true), true);

    // Commit changes
    tx.commit();
    QVERIFY(!tx.isDirty());
    QCOMPARE(sm.getBool("General", "SnapToEdge", true), false);
}

void TestPreferencesDialog::testTransactionDiscard() {
    auto& sm = SettingsManager::instance();
    sm.setInt("Playback", "ShortJumpSec", 5);

    SettingsTransaction tx(sm);
    tx.setInt("Playback", "ShortJumpSec", 15);
    QVERIFY(tx.isDirty());

    // Discard
    tx.discard();
    QVERIFY(!tx.isDirty());
    QCOMPARE(tx.getInt("Playback", "ShortJumpSec", 5), 5);
    QCOMPARE(sm.getInt("Playback", "ShortJumpSec", 5), 5);
}

void TestPreferencesDialog::testTransactionResetToDefaults() {
    auto& sm = SettingsManager::instance();
    sm.setInt("Playback", "ShortJumpSec", 99);

    SettingsTransaction tx(sm);
    tx.resetToDefaults();
    QCOMPARE(tx.getInt("Playback", "ShortJumpSec", 0), 5);
    QCOMPARE(tx.getBool("General", "SnapToEdge", false), true);
    QCOMPARE(tx.getString("Video", "AspectRatio", ""), std::string("Default"));
}

void TestPreferencesDialog::testTransactionExportToFile() {
    SettingsTransaction tx(SettingsManager::instance());
    tx.setString("Video", "AspectRatio", "16:9");

    QString exportPath = "test_export_preferences.ini";
    QVERIFY(tx.exportToFile(exportPath.toStdString()));

    QFile file(exportPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = QString::fromUtf8(file.readAll());
    file.close();

    QVERIFY(content.contains("[General]"));
    QVERIFY(content.contains("[Video]"));
    QVERIFY(content.contains("AspectRatio=16:9"));
}

void TestPreferencesDialog::testDialogGeometryAndTreeHierarchy() {
    PreferencesDialog dlg;

    // Check PotPlayer baseline dimensions
    QCOMPARE(dlg.size().width(), 751);
    QCOMPARE(dlg.size().height(), 564);

    auto* tree = dlg.findChild<QTreeWidget*>();
    QVERIFY(tree != nullptr);

    // 6 Master categories
    QCOMPARE(tree->topLevelItemCount(), 6);
    QCOMPARE(tree->topLevelItem(0)->text(0), QString("General"));
    QCOMPARE(tree->topLevelItem(1)->text(0), QString("Playback"));
    QCOMPARE(tree->topLevelItem(2)->text(0), QString("Video"));
    QCOMPARE(tree->topLevelItem(3)->text(0), QString("Audio"));
    QCOMPARE(tree->topLevelItem(4)->text(0), QString("Subtitles"));
    QCOMPARE(tree->topLevelItem(5)->text(0), QString("Filter Control"));

    // Check leaf counts: General=7, Playback=3, Video=4, Audio=4, Subtitles=3, Filter=3
    QCOMPARE(tree->topLevelItem(0)->childCount(), 7);
    QCOMPARE(tree->topLevelItem(1)->childCount(), 3);
    QCOMPARE(tree->topLevelItem(2)->childCount(), 4);
    QCOMPARE(tree->topLevelItem(3)->childCount(), 4);
    QCOMPARE(tree->topLevelItem(4)->childCount(), 3);
    QCOMPARE(tree->topLevelItem(5)->childCount(), 3);
}

void TestPreferencesDialog::testPageNavigationAcrossAllLeafNodes() {
    PreferencesDialog dlg;
    auto* tree = dlg.findChild<QTreeWidget*>();
    auto* stack = dlg.findChild<QStackedWidget*>();
    QVERIFY(tree != nullptr);
    QVERIFY(stack != nullptr);

    // Loop through all 6 categories and all children, ensuring no crashes and correct stack mapping
    for (int cat = 0; cat < tree->topLevelItemCount(); ++cat) {
        auto* top = tree->topLevelItem(cat);
        for (int sub = 0; sub < top->childCount(); ++sub) {
            dlg.selectCategoryAndSubPage(cat, sub);
            QCOMPARE(stack->currentIndex(), cat);
        }
    }
}

void TestPreferencesDialog::testApplyAndCancelSemantics() {
    auto& sm = SettingsManager::instance();
    sm.setBool("General", "SnapToEdge", true);

    PreferencesDialog dlg;
    QVERIFY(!dlg.hasUnappliedChanges());

    // Toggle snap-to-edge checkbox on General page
    auto* chkSnap = dlg.findChild<QCheckBox*>();
    QVERIFY(chkSnap != nullptr);
    chkSnap->setChecked(!chkSnap->isChecked());

    QVERIFY(dlg.hasUnappliedChanges());

    // Clicking Cancel (reject) must discard
    dlg.reject();
    QCOMPARE(sm.getBool("General", "SnapToEdge", true), true);

    // Open again, apply changes
    PreferencesDialog dlg2;
    QSignalSpy spy(&dlg2, &PreferencesDialog::settingsApplied);

    auto* chkSnap2 = dlg2.findChild<QCheckBox*>();
    QVERIFY(chkSnap2 != nullptr);
    chkSnap2->setChecked(false);

    dlg2.applySettings();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(sm.getBool("General", "SnapToEdge", true), false);
    QVERIFY(!dlg2.hasUnappliedChanges());
}

void TestPreferencesDialog::testResetCurrentPageSemantics() {
    auto& sm = SettingsManager::instance();
    sm.setBool("General", "SnapToEdge", false);

    PreferencesDialog dlg;
    dlg.selectCategoryAndSubPage(0, 0); // General Settings
    dlg.resetCurrentPage();

    QVERIFY(dlg.hasUnappliedChanges());
    dlg.applySettings();

    // After reset & apply, snap-to-edge should be back to default true
    QCOMPARE(sm.getBool("General", "SnapToEdge", false), true);
}

QTEST_MAIN(TestPreferencesDialog)
#include "test_preferences_dialog.moc"
