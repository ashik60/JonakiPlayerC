#pragma once

#include "PreferencesPageBase.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

namespace Jonaki {

class VideoPreferencesPage : public PreferencesPageBase {
    Q_OBJECT

public:
    explicit VideoPreferencesPage(QWidget* parent = nullptr);

    void loadFrom(const SettingsTransaction& tx) override;
    void saveTo(SettingsTransaction& tx) override;
    void resetToDefaults(SettingsTransaction& tx) override;
    bool isDirty() const override;
    void showSubPage(int subPageIndex) override;

private:
    void setupUi();

    QStackedWidget* m_subStack = nullptr;

    // Sub-page 0: Video Settings (Dialog 366)
    QComboBox* m_cmbAspectRatio = nullptr;
    QComboBox* m_cmbDeinterlaceMode = nullptr;

    // Sub-page 1: Video Renderer & HWDEC (Dialog 355/360)
    QComboBox* m_cmbHwDec = nullptr;
    QComboBox* m_cmbColorRange = nullptr;
    QLabel* m_lblActiveHwdec = nullptr;

    // Sub-page 2: Color & Level Control (Dialog 371)
    QSlider* m_sldBrightness = nullptr;
    QSpinBox* m_spnBrightness = nullptr;
    QSlider* m_sldContrast = nullptr;
    QSpinBox* m_spnContrast = nullptr;
    QSlider* m_sldSaturation = nullptr;
    QSpinBox* m_spnSaturation = nullptr;
    QSlider* m_sldHue = nullptr;
    QSpinBox* m_spnHue = nullptr;
    QPushButton* m_btnResetColor = nullptr;

    // Sub-page 3: Resizing & Cropping (Dialog 369/374)
    QComboBox* m_cmbResizeMethod = nullptr;
    QCheckBox* m_chkCorrectAspectOnCapture = nullptr;

    bool m_dirty = false;
    void markDirty();
};

} // namespace Jonaki
