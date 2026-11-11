#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <vector>

namespace Jonaki {

class AudioPreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit AudioPreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: Audio Output & Speakers (Dialog 356)
    QComboBox* m_cmbOutputDevice = nullptr;
    QComboBox* m_cmbSpeakerChannels = nullptr;
    QCheckBox* m_chkExpandStereo = nullptr;

    // Sub-page 1: Volume Normalizer (dynaudnorm)
    QCheckBox* m_chkNormalization = nullptr;
    QDoubleSpinBox* m_spnNormTargetPeak = nullptr;
    QSpinBox* m_spnNormMaxGain = nullptr;

    // Sub-page 2: 10-Band Equalizer (Dialog 363)
    QCheckBox* m_chkEqualizerEnabled = nullptr;
    QComboBox* m_cmbEqPresets = nullptr;
    QPushButton* m_btnResetEq = nullptr;
    std::vector<QSlider*> m_eqSliders;
    std::vector<QLabel*> m_eqLabels;

    // Sub-page 3: Audio Delay & Codec (Dialog 361)
    QDoubleSpinBox* m_spnAudioDelay = nullptr;
    QCheckBox* m_chkPassThrough = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
