#include <QTest>
#include "../../src/settings/SettingsManager.h"
#include <QDir>
#include <QFile>

class TestSettings : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        Jonaki::SettingsManager::instance().resetToDefaults();
    }

    void testDefaultValues() {
        auto& mgr = Jonaki::SettingsManager::instance();
        QCOMPARE(mgr.getInt("Window", "Width"), 960);
        QCOMPARE(mgr.getInt("Window", "Height"), 540);
        QCOMPARE(mgr.getInt("Playback", "Volume"), 100);
        QCOMPARE(mgr.getBool("Playback", "Mute"), false);
        QCOMPARE(mgr.getDouble("Playback", "Speed"), 1.0);
        QCOMPARE(mgr.getString("Video", "AspectRatio"), "Default");
    }

    void testSetAndGet() {
        auto& mgr = Jonaki::SettingsManager::instance();
        mgr.setInt("Custom", "TestInt", 42);
        mgr.setBool("Custom", "TestBool", true);
        mgr.setDouble("Custom", "TestDouble", 3.14159);
        mgr.setString("Custom", "TestString", "PotPlayer Parity");

        QCOMPARE(mgr.getInt("Custom", "TestInt"), 42);
        QCOMPARE(mgr.getBool("Custom", "TestBool"), true);
        QCOMPARE(mgr.getDouble("Custom", "TestDouble"), 3.14159);
        QCOMPARE(mgr.getString("Custom", "TestString"), "PotPlayer Parity");
    }

    void testSaveAndLoad() {
        auto& mgr = Jonaki::SettingsManager::instance();
        QString tempPath = QDir::temp().filePath("jonaki_settings_test.ini");
        std::string stdPath = tempPath.toStdString();

        mgr.setInt("SavedSection", "KeyA", 1234);
        mgr.setString("SavedSection", "KeyB", "TestVal");

        bool saved = mgr.saveToFile(stdPath);
        QVERIFY(saved);
        QVERIFY(QFile::exists(tempPath));

        // Reset to defaults and verify keys are gone
        mgr.resetToDefaults();
        QCOMPARE(mgr.getInt("SavedSection", "KeyA", -1), -1);

        // Load back
        bool loaded = mgr.loadFromFile(stdPath);
        QVERIFY(loaded);
        QCOMPARE(mgr.getInt("SavedSection", "KeyA"), 1234);
        QCOMPARE(mgr.getString("SavedSection", "KeyB"), "TestVal");

        QFile::remove(tempPath);
    }
};

QTEST_MAIN(TestSettings)
#include "test_settings.moc"
