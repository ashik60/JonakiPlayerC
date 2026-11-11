#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

namespace Jonaki {

class FilterControlPreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit FilterControlPreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: Source / Splitter (Dialog 325/350)
    QComboBox* m_cmbDemuxer = nullptr;
    QSpinBox* m_spnNetworkCacheKb = nullptr;

    // Sub-page 1: Video Decoder Profiles (Dialog 360)
    QComboBox* m_cmbVideoDecoderPriority = nullptr;
    QCheckBox* m_chkMultiThreadDecoding = nullptr;

    // Sub-page 2: Audio Decoder Profiles (Dialog 361)
    QComboBox* m_cmbAudioDecoderEngine = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
