#include "GeneralPreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>

namespace Jonaki {

GeneralPreferencesPage::GeneralPreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void GeneralPreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void GeneralPreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: General Settings (Dialog 354)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("General Window & Behavioral Settings", page0);
    auto* v0 = new QVBoxLayout(grp0);
    m_chkSnapToEdge = new QCheckBox("Snap window to edge of screen and adjacent windows", grp0);
    m_chkSnapToEdge->setObjectName("chkSnapToEdge");
    m_chkShowTooltips = new QCheckBox("Show informative tooltips on control bar buttons", grp0);
    m_chkStoreIniFile = new QCheckBox("Store settings into .ini file (portable mode)", grp0);
    m_chkPauseOnFocusLost = new QCheckBox("Pause playback when window loses focus", grp0);
    m_chkPauseOnMinimize = new QCheckBox("Pause playback when window is minimized", grp0);
    m_chkAutoRotate = new QCheckBox("Auto-rotate video based on container metadata", grp0);

    v0->addWidget(m_chkSnapToEdge);
    v0->addWidget(m_chkShowTooltips);
    v0->addWidget(m_chkStoreIniFile);
    v0->addWidget(m_chkPauseOnFocusLost);
    v0->addWidget(m_chkPauseOnMinimize);
    v0->addWidget(m_chkAutoRotate);
    v0->addStretch();
    l0->addWidget(grp0);

    // ==========================================
    // Sub-page 1: Startup (Dialog 354/355)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Startup & Initial Geometry", page1);
    auto* f1 = new QFormLayout(grp1);
    m_cmbStartupWindowSize = new QComboBox(grp1);
    m_cmbStartupWindowSize->addItems({"960x540 (Default)", "723x399 (PotPlayer Baseline)", "Last Closed Size", "Fit to Video Size"});
    m_chkRestorePosition = new QCheckBox("Restore last window position on startup", grp1);
    m_spnDefaultVolume = new QSpinBox(grp1);
    m_spnDefaultVolume->setRange(0, 100);
    m_spnDefaultVolume->setValue(100);

    f1->addRow("Startup Window Size:", m_cmbStartupWindowSize);
    f1->addRow("", m_chkRestorePosition);
    f1->addRow("Default Volume on Startup:", m_spnDefaultVolume);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: OSD (Dialog 375)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("On-Screen Display (OSD) Preferences", page2);
    auto* f2 = new QFormLayout(grp2);
    m_chkOsdShowFilename = new QCheckBox("Show file name when opening media", grp2);
    m_chkOsdShowPlaybackTime = new QCheckBox("Show current playback time & duration on seek", grp2);
    m_chkOsdShowCodecInfo = new QCheckBox("Show video codec and hardware decoder in OSD", grp2);
    m_spnOsdDurationMs = new QSpinBox(grp2);
    m_spnOsdDurationMs->setRange(300, 5000);
    m_spnOsdDurationMs->setSingleStep(100);
    m_spnOsdDurationMs->setSuffix(" ms");
    m_fntOsdFont = new QFontComboBox(grp2);
    m_spnOsdFontSize = new QSpinBox(grp2);
    m_spnOsdFontSize->setRange(12, 48);

    f2->addRow(m_chkOsdShowFilename);
    f2->addRow(m_chkOsdShowPlaybackTime);
    f2->addRow(m_chkOsdShowCodecInfo);
    f2->addRow("Display Duration:", m_spnOsdDurationMs);
    f2->addRow("OSD Font Family:", m_fntOsdFont);
    f2->addRow("OSD Font Size:", m_spnOsdFontSize);
    l2->addWidget(grp2);
    l2->addStretch();

    // ==========================================
    // Sub-page 3: Logo (Dialog 378)
    // ==========================================
    auto* page3 = new QWidget(this);
    auto* l3 = new QVBoxLayout(page3);
    l3->setContentsMargins(0, 0, 0, 0);

    auto* grp3 = new QGroupBox("Logo & Branding", page3);
    auto* f3 = new QFormLayout(grp3);
    m_cmbLogoType = new QComboBox(grp3);
    m_cmbLogoType->addItems({"Jonaki Default", "PotPlayer Classic Skinned", "Blank"});
    m_chkLogoShowOnLoad = new QCheckBox("Show animated logo during file loading", grp3);

    f3->addRow("Idle Logo Display:", m_cmbLogoType);
    f3->addRow("", m_chkLogoShowOnLoad);
    l3->addWidget(grp3);
    l3->addStretch();

    // ==========================================
    // Sub-page 4: Mouse (Dialog 366)
    // ==========================================
    auto* page4 = new QWidget(this);
    auto* l4 = new QVBoxLayout(page4);
    l4->setContentsMargins(0, 0, 0, 0);

    auto* grp4 = new QGroupBox("Mouse Click & Gesture Actions", page4);
    auto* f4 = new QFormLayout(grp4);
    m_cmbMouseLeftSingle = new QComboBox(grp4);
    m_cmbMouseLeftSingle->addItems({"Play/Pause", "Fullscreen", "None"});
    m_cmbMouseLeftDouble = new QComboBox(grp4);
    m_cmbMouseLeftDouble->addItems({"Fullscreen", "Play/Pause", "None"});
    m_cmbMouseWheel = new QComboBox(grp4);
    m_cmbMouseWheel->addItems({"Volume", "Seek (5 sec)", "None"});
    m_chkMousePreventOutsideDrag = new QCheckBox("Prevent window dragging outside title bar", grp4);

    f4->addRow("Left Button - Single Click:", m_cmbMouseLeftSingle);
    f4->addRow("Left Button - Double Click:", m_cmbMouseLeftDouble);
    f4->addRow("Mouse Wheel:", m_cmbMouseWheel);
    f4->addRow("", m_chkMousePreventOutsideDrag);
    l4->addWidget(grp4);
    l4->addStretch();

    // ==========================================
    // Sub-page 5: Keyboard / Hotkeys (Dialog 367)
    // ==========================================
    auto* page5 = new QWidget(this);
    auto* l5 = new QVBoxLayout(page5);
    l5->setContentsMargins(0, 0, 0, 0);

    auto* grp5 = new QGroupBox("Keyboard Shortcut Mappings", page5);
    auto* v5 = new QVBoxLayout(grp5);
    m_chkGlobalMediaKeys = new QCheckBox("Enable global multimedia control keys (Play, Pause, Stop)", grp5);
    v5->addWidget(m_chkGlobalMediaKeys);

    auto* tblShortcuts = new QTableWidget(12, 2, grp5);
    tblShortcuts->setHorizontalHeaderLabels({"Action", "Default Shortcut"});
    tblShortcuts->horizontalHeader()->setStretchLastSection(true);
    tblShortcuts->verticalHeader()->setVisible(false);
    tblShortcuts->setEditTriggers(QAbstractItemView::NoEditTriggers);

    struct ShortcutRow { const char* act; const char* key; };
    const ShortcutRow sRows[] = {
        {"Play / Pause", "Space"},
        {"Preferences Window", "F5"},
        {"Open File", "Ctrl+O"},
        {"Open URL / Stream", "Ctrl+U"},
        {"Save Source Frame", "Ctrl+E"},
        {"Save Screen Frame (with subs)", "Ctrl+Alt+E"},
        {"Copy Frame to Clipboard", "Ctrl+C"},
        {"A-B Repeat: Set Point A", "["},
        {"A-B Repeat: Set Point B", "]"},
        {"A-B Repeat: Start / Stop Loop", "\\"},
        {"Audio Sync Delay Adjust", "Shift+< / Shift+>"},
        {"Subtitle Sync Delay Adjust", "< / >"}
    };
    for (int i = 0; i < 12; ++i) {
        tblShortcuts->setItem(i, 0, new QTableWidgetItem(sRows[i].act));
        tblShortcuts->setItem(i, 1, new QTableWidgetItem(sRows[i].key));
    }
    v5->addWidget(tblShortcuts);
    l5->addWidget(grp5);

    // ==========================================
    // Sub-page 6: Skin / Theme (Dialog 376)
    // ==========================================
    auto* page6 = new QWidget(this);
    auto* l6 = new QVBoxLayout(page6);
    l6->setContentsMargins(0, 0, 0, 0);

    auto* grp6 = new QGroupBox("Skin & Visual Theme", page6);
    auto* f6 = new QFormLayout(grp6);
    m_cmbActiveSkin = new QComboBox(grp6);
    m_cmbActiveSkin->addItems({"Default Dark (Skinned)", "Classic Compact", "High Contrast"});
    m_chkDarkMode = new QCheckBox("Enforce system dark palette on menus & dialogs", grp6);

    f6->addRow("Active Skin:", m_cmbActiveSkin);
    f6->addRow("", m_chkDarkMode);
    l6->addWidget(grp6);
    l6->addStretch();

    // Add all 7 sub-pages to the internal stack
    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);
    m_subStack->addWidget(page3);
    m_subStack->addWidget(page4);
    m_subStack->addWidget(page5);
    m_subStack->addWidget(page6);

    mainLayout->addWidget(m_subStack);

    // Wire modification signals
    auto connectWidget = [this](auto* w) {
        connect(w, &QCheckBox::toggled, this, &GeneralPreferencesPage::markDirty);
    };
    connectWidget(m_chkSnapToEdge);
    connectWidget(m_chkShowTooltips);
    connectWidget(m_chkStoreIniFile);
    connectWidget(m_chkPauseOnFocusLost);
    connectWidget(m_chkPauseOnMinimize);
    connectWidget(m_chkAutoRotate);
    connectWidget(m_chkRestorePosition);
    connectWidget(m_chkOsdShowFilename);
    connectWidget(m_chkOsdShowPlaybackTime);
    connectWidget(m_chkOsdShowCodecInfo);
    connectWidget(m_chkLogoShowOnLoad);
    connectWidget(m_chkMousePreventOutsideDrag);
    connectWidget(m_chkGlobalMediaKeys);
    connectWidget(m_chkDarkMode);

    connect(m_cmbStartupWindowSize, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_spnDefaultVolume, &QSpinBox::valueChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_spnOsdDurationMs, &QSpinBox::valueChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_fntOsdFont, &QFontComboBox::currentFontChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_spnOsdFontSize, &QSpinBox::valueChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_cmbLogoType, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_cmbMouseLeftSingle, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_cmbMouseLeftDouble, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_cmbMouseWheel, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
    connect(m_cmbActiveSkin, &QComboBox::currentIndexChanged, this, &GeneralPreferencesPage::markDirty);
}

void GeneralPreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void GeneralPreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_chkSnapToEdge->setChecked(tx.getBool("General", "SnapToEdge", true));
    m_chkShowTooltips->setChecked(tx.getBool("General", "ShowTooltips", true));
    m_chkStoreIniFile->setChecked(tx.getBool("General", "StoreIniFile", true));
    m_chkPauseOnFocusLost->setChecked(tx.getBool("General", "PauseOnFocusLost", false));
    m_chkPauseOnMinimize->setChecked(tx.getBool("General", "PauseOnMinimize", true));
    m_chkAutoRotate->setChecked(tx.getBool("General", "AutoRotate", true));

    m_cmbStartupWindowSize->setCurrentText(QString::fromStdString(tx.getString("Startup", "WindowSize", "960x540 (Default)")));
    m_chkRestorePosition->setChecked(tx.getBool("Startup", "RestorePosition", true));
    m_spnDefaultVolume->setValue(tx.getInt("Startup", "DefaultVolume", 100));

    m_chkOsdShowFilename->setChecked(tx.getBool("OSD", "ShowFilename", true));
    m_chkOsdShowPlaybackTime->setChecked(tx.getBool("OSD", "ShowPlaybackTime", true));
    m_chkOsdShowCodecInfo->setChecked(tx.getBool("OSD", "ShowCodecInfo", false));
    m_spnOsdDurationMs->setValue(tx.getInt("OSD", "DurationMs", 1500));
    m_fntOsdFont->setCurrentFont(QFont(QString::fromStdString(tx.getString("OSD", "FontName", "Tahoma"))));
    m_spnOsdFontSize->setValue(tx.getInt("OSD", "FontSize", 24));

    m_cmbLogoType->setCurrentText(QString::fromStdString(tx.getString("Logo", "Type", "Jonaki Default")));
    m_chkLogoShowOnLoad->setChecked(tx.getBool("Logo", "ShowOnLoad", true));

    m_cmbMouseLeftSingle->setCurrentText(QString::fromStdString(tx.getString("Mouse", "LeftSingleClick", "Play/Pause")));
    m_cmbMouseLeftDouble->setCurrentText(QString::fromStdString(tx.getString("Mouse", "LeftDoubleClick", "Fullscreen")));
    m_cmbMouseWheel->setCurrentText(QString::fromStdString(tx.getString("Mouse", "WheelAction", "Volume")));
    m_chkMousePreventOutsideDrag->setChecked(tx.getBool("Mouse", "PreventOutsideDrag", false));

    m_chkGlobalMediaKeys->setChecked(tx.getBool("Keyboard", "GlobalMediaKeys", true));

    m_cmbActiveSkin->setCurrentText(QString::fromStdString(tx.getString("Skin", "ActiveSkin", "Default Dark (Skinned)")));
    m_chkDarkMode->setChecked(tx.getBool("Skin", "DarkMode", true));

    m_dirty = false;
}

void GeneralPreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setBool("General", "SnapToEdge", m_chkSnapToEdge->isChecked());
    tx.setBool("General", "ShowTooltips", m_chkShowTooltips->isChecked());
    tx.setBool("General", "StoreIniFile", m_chkStoreIniFile->isChecked());
    tx.setBool("General", "PauseOnFocusLost", m_chkPauseOnFocusLost->isChecked());
    tx.setBool("General", "PauseOnMinimize", m_chkPauseOnMinimize->isChecked());
    tx.setBool("General", "AutoRotate", m_chkAutoRotate->isChecked());

    tx.setString("Startup", "WindowSize", m_cmbStartupWindowSize->currentText().toStdString());
    tx.setBool("Startup", "RestorePosition", m_chkRestorePosition->isChecked());
    tx.setInt("Startup", "DefaultVolume", m_spnDefaultVolume->value());

    tx.setBool("OSD", "ShowFilename", m_chkOsdShowFilename->isChecked());
    tx.setBool("OSD", "ShowPlaybackTime", m_chkOsdShowPlaybackTime->isChecked());
    tx.setBool("OSD", "ShowCodecInfo", m_chkOsdShowCodecInfo->isChecked());
    tx.setInt("OSD", "DurationMs", m_spnOsdDurationMs->value());
    tx.setString("OSD", "FontName", m_fntOsdFont->currentFont().family().toStdString());
    tx.setInt("OSD", "FontSize", m_spnOsdFontSize->value());

    tx.setString("Logo", "Type", m_cmbLogoType->currentText().toStdString());
    tx.setBool("Logo", "ShowOnLoad", m_chkLogoShowOnLoad->isChecked());

    tx.setString("Mouse", "LeftSingleClick", m_cmbMouseLeftSingle->currentText().toStdString());
    tx.setString("Mouse", "LeftDoubleClick", m_cmbMouseLeftDouble->currentText().toStdString());
    tx.setString("Mouse", "WheelAction", m_cmbMouseWheel->currentText().toStdString());
    tx.setBool("Mouse", "PreventOutsideDrag", m_chkMousePreventOutsideDrag->isChecked());

    tx.setBool("Keyboard", "GlobalMediaKeys", m_chkGlobalMediaKeys->isChecked());

    tx.setString("Skin", "ActiveSkin", m_cmbActiveSkin->currentText().toStdString());
    tx.setBool("Skin", "DarkMode", m_chkDarkMode->isChecked());

    m_dirty = false;
}

void GeneralPreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setBool("General", "SnapToEdge", true);
    tx.setBool("General", "ShowTooltips", true);
    tx.setBool("General", "StoreIniFile", true);
    tx.setBool("General", "PauseOnFocusLost", false);
    tx.setBool("General", "PauseOnMinimize", true);
    tx.setBool("General", "AutoRotate", true);

    tx.setString("Startup", "WindowSize", "960x540 (Default)");
    tx.setBool("Startup", "RestorePosition", true);
    tx.setInt("Startup", "DefaultVolume", 100);

    tx.setBool("OSD", "ShowFilename", true);
    tx.setBool("OSD", "ShowPlaybackTime", true);
    tx.setBool("OSD", "ShowCodecInfo", false);
    tx.setInt("OSD", "DurationMs", 1500);
    tx.setString("OSD", "FontName", "Tahoma");
    tx.setInt("OSD", "FontSize", 24);

    tx.setString("Logo", "Type", "Jonaki Default");
    tx.setBool("Logo", "ShowOnLoad", true);

    tx.setString("Mouse", "LeftSingleClick", "Play/Pause");
    tx.setString("Mouse", "LeftDoubleClick", "Fullscreen");
    tx.setString("Mouse", "WheelAction", "Volume");
    tx.setBool("Mouse", "PreventOutsideDrag", false);

    tx.setBool("Keyboard", "GlobalMediaKeys", true);

    tx.setString("Skin", "ActiveSkin", "Default Dark (Skinned)");
    tx.setBool("Skin", "DarkMode", true);

    loadFrom(tx);
    markDirty();
}

bool GeneralPreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
