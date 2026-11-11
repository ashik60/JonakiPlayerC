#include "AudioPreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <sstream>

namespace Jonaki {

AudioPreferencesPage::AudioPreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void AudioPreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void AudioPreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: Audio Output & Speakers (Dialog 356)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("Audio Render Output & Speaker Channels (Dialog 356)", page0);
    auto* f0 = new QFormLayout(grp0);
    m_cmbOutputDevice = new QComboBox(grp0);
    m_cmbOutputDevice->addItems({"Default (DirectSound / WASAPI Auto)", "WASAPI (Exclusive)", "WASAPI (Shared)", "DirectSound"});
    m_cmbSpeakerChannels = new QComboBox(grp0);
    m_cmbSpeakerChannels->addItems({"Stereo (2.0 channels)", "5.1 Surround", "7.1 Surround", "Mono (1.0 channel)"});
    m_chkExpandStereo = new QCheckBox("Expand stereo to center and surround channels", grp0);

    f0->addRow("Audio Output Device:", m_cmbOutputDevice);
    f0->addRow("Speaker Configuration:", m_cmbSpeakerChannels);
    f0->addRow("", m_chkExpandStereo);
    l0->addWidget(grp0);
    l0->addStretch();

    // ==========================================
    // Sub-page 1: Volume Normalizer (dynaudnorm)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Dynamic Volume Normalizer (dynaudnorm)", page1);
    auto* f1 = new QFormLayout(grp1);
    m_chkNormalization = new QCheckBox("Enable real-time dynamic volume normalization (Shift+N)", grp1);
    m_spnNormTargetPeak = new QDoubleSpinBox(grp1);
    m_spnNormTargetPeak->setRange(0.50, 1.00);
    m_spnNormTargetPeak->setSingleStep(0.05);
    m_spnNormTargetPeak->setValue(0.95);
    m_spnNormMaxGain = new QSpinBox(grp1);
    m_spnNormMaxGain->setRange(1, 30);
    m_spnNormMaxGain->setValue(10);
    m_spnNormMaxGain->setSuffix(" dB");

    f1->addRow("", m_chkNormalization);
    f1->addRow("Target Peak Level:", m_spnNormTargetPeak);
    f1->addRow("Maximum Gain Amplification:", m_spnNormMaxGain);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: 10-Band Equalizer (Dialog 363)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("10-Band Graphic Equalizer (Dialog 363)", page2);
    auto* v2 = new QVBoxLayout(grp2);

    auto* topH = new QHBoxLayout();
    m_chkEqualizerEnabled = new QCheckBox("Enable 10-band graphic equalizer (Shift+E)", grp2);
    m_cmbEqPresets = new QComboBox(grp2);
    m_cmbEqPresets->addItems({"Flat (Default)", "Rock", "Pop", "Classical", "Jazz", "Vocal"});
    m_btnResetEq = new QPushButton("Reset (0 dB)", grp2);
    topH->addWidget(m_chkEqualizerEnabled);
    topH->addStretch();
    topH->addWidget(new QLabel("Preset:", grp2));
    topH->addWidget(m_cmbEqPresets);
    topH->addWidget(m_btnResetEq);
    v2->addLayout(topH);

    auto* sliderH = new QHBoxLayout();
    const char* bandNames[] = {"31Hz", "62Hz", "125Hz", "250Hz", "500Hz", "1kHz", "2kHz", "4kHz", "8kHz", "16kHz"};
    m_eqSliders.resize(10);
    m_eqLabels.resize(10);

    for (int i = 0; i < 10; ++i) {
        auto* bandV = new QVBoxLayout();
        auto* valLbl = new QLabel("0dB", grp2);
        valLbl->setAlignment(Qt::AlignCenter);

        auto* sld = new QSlider(Qt::Vertical, grp2);
        sld->setRange(-12, 12);
        sld->setValue(0);
        sld->setTickPosition(QSlider::TicksBothSides);
        sld->setTickInterval(3);

        auto* nameLbl = new QLabel(bandNames[i], grp2);
        nameLbl->setAlignment(Qt::AlignCenter);
        nameLbl->setStyleSheet("font-size: 10px; color: #a0a0a0;");

        connect(sld, &QSlider::valueChanged, this, [this, valLbl](int val) {
            valLbl->setText(QString("%1dB").arg(val));
            markDirty();
        });

        bandV->addWidget(valLbl);
        bandV->addWidget(sld);
        bandV->addWidget(nameLbl);
        sliderH->addLayout(bandV);

        m_eqSliders[i] = sld;
        m_eqLabels[i] = valLbl;
    }
    v2->addLayout(sliderH);
    l2->addWidget(grp2);

    connect(m_btnResetEq, &QPushButton::clicked, this, [this]() {
        for (auto* sld : m_eqSliders) {
            sld->setValue(0);
        }
        markDirty();
    });

    connect(m_cmbEqPresets, &QComboBox::currentIndexChanged, this, [this](int idx) {
        const int presets[][10] = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},         // Flat
            {5, 3, -1, -2, -1, 2, 4, 6, 7, 7},       // Rock
            {-1, 1, 3, 4, 3, 0, -1, 1, 3, 4},       // Pop
            {5, 4, 3, 2, -2, -2, 0, 2, 3, 4},       // Classical
            {3, 2, 0, 2, -1, -1, 0, 1, 3, 4},       // Jazz
            {-2, -3, -1, 2, 4, 4, 3, 1, -1, -2}     // Vocal
        };
        if (idx >= 0 && idx < 6) {
            for (int i = 0; i < 10; ++i) {
                m_eqSliders[i]->setValue(presets[idx][i]);
            }
            markDirty();
        }
    });

    // ==========================================
    // Sub-page 3: Audio Delay & Codec (Dialog 361)
    // ==========================================
    auto* page3 = new QWidget(this);
    auto* l3 = new QVBoxLayout(page3);
    l3->setContentsMargins(0, 0, 0, 0);

    auto* grp3 = new QGroupBox("Audio Synchronization & Pass-Through", page3);
    auto* f3 = new QFormLayout(grp3);
    m_spnAudioDelay = new QDoubleSpinBox(grp3);
    m_spnAudioDelay->setRange(-10.0, 10.0);
    m_spnAudioDelay->setSingleStep(0.05);
    m_spnAudioDelay->setDecimals(2);
    m_spnAudioDelay->setSuffix(" sec (Shift+< / Shift+>)");
    m_chkPassThrough = new QCheckBox("Enable AC3/DTS/TrueHD digital pass-through (S/PDIF / HDMI)", grp3);

    f3->addRow("Default Audio Sync Delay:", m_spnAudioDelay);
    f3->addRow("", m_chkPassThrough);
    l3->addWidget(grp3);
    l3->addStretch();

    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);
    m_subStack->addWidget(page3);

    mainLayout->addWidget(m_subStack);

    // Signals
    connect(m_cmbOutputDevice, &QComboBox::currentIndexChanged, this, &AudioPreferencesPage::markDirty);
    connect(m_cmbSpeakerChannels, &QComboBox::currentIndexChanged, this, &AudioPreferencesPage::markDirty);
    connect(m_chkExpandStereo, &QCheckBox::toggled, this, &AudioPreferencesPage::markDirty);
    connect(m_chkNormalization, &QCheckBox::toggled, this, &AudioPreferencesPage::markDirty);
    connect(m_spnNormTargetPeak, &QDoubleSpinBox::valueChanged, this, &AudioPreferencesPage::markDirty);
    connect(m_spnNormMaxGain, &QSpinBox::valueChanged, this, &AudioPreferencesPage::markDirty);
    connect(m_chkEqualizerEnabled, &QCheckBox::toggled, this, &AudioPreferencesPage::markDirty);
    connect(m_spnAudioDelay, &QDoubleSpinBox::valueChanged, this, &AudioPreferencesPage::markDirty);
    connect(m_chkPassThrough, &QCheckBox::toggled, this, &AudioPreferencesPage::markDirty);
}

void AudioPreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void AudioPreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_cmbOutputDevice->setCurrentText(QString::fromStdString(tx.getString("Audio", "OutputDevice", "Default (DirectSound / WASAPI Auto)")));
    m_cmbSpeakerChannels->setCurrentText(QString::fromStdString(tx.getString("Audio", "SpeakerChannels", "Stereo (2.0 channels)")));
    m_chkExpandStereo->setChecked(tx.getBool("Audio", "ExpandStereo", false));

    m_chkNormalization->setChecked(tx.getBool("Audio", "Normalization", false));
    m_spnNormTargetPeak->setValue(tx.getDouble("Audio", "NormTargetPeak", 0.95));
    m_spnNormMaxGain->setValue(tx.getInt("Audio", "NormMaxGain", 10));

    m_chkEqualizerEnabled->setChecked(tx.getBool("Audio", "EqualizerEnabled", false));

    std::string bandsStr = tx.getString("Audio", "EqualizerBands", "0;0;0;0;0;0;0;0;0;0");
    std::stringstream ss(bandsStr);
    std::string token;
    int idx = 0;
    while (std::getline(ss, token, ';') && idx < 10) {
        try {
            int val = std::stoi(token);
            m_eqSliders[idx]->setValue(val);
        } catch (...) {
            m_eqSliders[idx]->setValue(0);
        }
        idx++;
    }

    m_spnAudioDelay->setValue(tx.getDouble("Audio", "AudioDelay", 0.0));
    m_chkPassThrough->setChecked(tx.getBool("Audio", "PassThrough", false));

    m_dirty = false;
}

void AudioPreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setString("Audio", "OutputDevice", m_cmbOutputDevice->currentText().toStdString());
    tx.setString("Audio", "SpeakerChannels", m_cmbSpeakerChannels->currentText().toStdString());
    tx.setBool("Audio", "ExpandStereo", m_chkExpandStereo->isChecked());

    tx.setBool("Audio", "Normalization", m_chkNormalization->isChecked());
    tx.setDouble("Audio", "NormTargetPeak", m_spnNormTargetPeak->value());
    tx.setInt("Audio", "NormMaxGain", m_spnNormMaxGain->value());

    tx.setBool("Audio", "EqualizerEnabled", m_chkEqualizerEnabled->isChecked());

    std::string bandsStr;
    for (size_t i = 0; i < m_eqSliders.size(); ++i) {
        if (i > 0) bandsStr += ";";
        bandsStr += std::to_string(m_eqSliders[i]->value());
    }
    tx.setString("Audio", "EqualizerBands", bandsStr);

    tx.setDouble("Audio", "AudioDelay", m_spnAudioDelay->value());
    tx.setBool("Audio", "PassThrough", m_chkPassThrough->isChecked());

    m_dirty = false;
}

void AudioPreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setString("Audio", "OutputDevice", "Default (DirectSound / WASAPI Auto)");
    tx.setString("Audio", "SpeakerChannels", "Stereo (2.0 channels)");
    tx.setBool("Audio", "ExpandStereo", false);
    tx.setBool("Audio", "Normalization", false);
    tx.setDouble("Audio", "NormTargetPeak", 0.95);
    tx.setInt("Audio", "NormMaxGain", 10);
    tx.setBool("Audio", "EqualizerEnabled", false);
    tx.setString("Audio", "EqualizerBands", "0;0;0;0;0;0;0;0;0;0");
    tx.setDouble("Audio", "AudioDelay", 0.0);
    tx.setBool("Audio", "PassThrough", false);

    loadFrom(tx);
    markDirty();
}

bool AudioPreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
