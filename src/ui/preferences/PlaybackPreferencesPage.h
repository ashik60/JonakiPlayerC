#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

namespace Jonaki {

class PlaybackPreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit PlaybackPreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: Playback Settings (Dialog 358)
    QCheckBox* m_chkRememberVideoPositions = nullptr;
    QCheckBox* m_chkRememberAudioPositions = nullptr;
    QCheckBox* m_chkPreventScreenSaver = nullptr;
    QCheckBox* m_chkAutoAddSimilarFiles = nullptr;

    // Sub-page 1: Time Jump (Dialog 364)
    QCheckBox* m_chkJumpKeyframe = nullptr;
    QSpinBox* m_spnShortJumpSec = nullptr;
    QSpinBox* m_spnMediumJumpSec = nullptr;
    QSpinBox* m_spnLongJumpSec = nullptr;

    // Sub-page 2: Repeat & Resume (Dialog 358/377)
    QComboBox* m_cmbRepeatMode = nullptr;
    QCheckBox* m_chkAutoResume = nullptr;
    QSpinBox* m_spnMinResumeThresholdSec = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
