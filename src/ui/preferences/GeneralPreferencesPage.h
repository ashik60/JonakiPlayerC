#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QFontComboBox>
#include <QLabel>

namespace Jonaki {

class GeneralPreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit GeneralPreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: General Settings (Dialog 354)
    QCheckBox* m_chkSnapToEdge = nullptr;
    QCheckBox* m_chkShowTooltips = nullptr;
    QCheckBox* m_chkStoreIniFile = nullptr;
    QCheckBox* m_chkPauseOnFocusLost = nullptr;
    QCheckBox* m_chkPauseOnMinimize = nullptr;
    QCheckBox* m_chkAutoRotate = nullptr;

    // Sub-page 1: Startup (Dialog 354/355)
    QComboBox* m_cmbStartupWindowSize = nullptr;
    QCheckBox* m_chkRestorePosition = nullptr;
    QSpinBox* m_spnDefaultVolume = nullptr;

    // Sub-page 2: OSD (Dialog 375)
    QCheckBox* m_chkOsdShowFilename = nullptr;
    QCheckBox* m_chkOsdShowPlaybackTime = nullptr;
    QCheckBox* m_chkOsdShowCodecInfo = nullptr;
    QSpinBox* m_spnOsdDurationMs = nullptr;
    QFontComboBox* m_fntOsdFont = nullptr;
    QSpinBox* m_spnOsdFontSize = nullptr;

    // Sub-page 3: Logo (Dialog 378)
    QComboBox* m_cmbLogoType = nullptr;
    QCheckBox* m_chkLogoShowOnLoad = nullptr;

    // Sub-page 4: Mouse (Dialog 366)
    QComboBox* m_cmbMouseLeftSingle = nullptr;
    QComboBox* m_cmbMouseLeftDouble = nullptr;
    QComboBox* m_cmbMouseWheel = nullptr;
    QCheckBox* m_chkMousePreventOutsideDrag = nullptr;

    // Sub-page 5: Keyboard / Hotkeys (Dialog 367)
    QCheckBox* m_chkGlobalMediaKeys = nullptr;

    // Sub-page 6: Skin / Theme (Dialog 376)
    QComboBox* m_cmbActiveSkin = nullptr;
    QCheckBox* m_chkDarkMode = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
