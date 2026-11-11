#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QFontComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>

namespace Jonaki {

class SubtitlePreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit SubtitlePreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: Subtitle Language & Priority (Dialog 368)
    QLineEdit* m_txtLanguagePriority = nullptr;
    QCheckBox* m_chkAutoLoadSameName = nullptr;
    QCheckBox* m_chkSearchSubfolders = nullptr;

    // Sub-page 1: Font & Styling (Dialog 353)
    QCheckBox* m_chkShowSubtitles = nullptr;
    QFontComboBox* m_fntSubtitleFont = nullptr;
    QSpinBox* m_spnFontSize = nullptr;
    QComboBox* m_cmbFontColor = nullptr;
    QSpinBox* m_spnBorderSize = nullptr;

    // Sub-page 2: Sync & Placement (Dialog 374)
    QDoubleSpinBox* m_spnDelayStep = nullptr;
    QSpinBox* m_spnVerticalMargin = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
