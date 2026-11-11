#include "FilterControlPreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

namespace Jonaki {

FilterControlPreferencesPage::FilterControlPreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void FilterControlPreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void FilterControlPreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: Source / Splitter (Dialog 325/350)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("Container Demuxers & Stream Splitters (Dialog 325/350)", page0);
    auto* f0 = new QFormLayout(grp0);
    m_cmbDemuxer = new QComboBox(grp0);
    m_cmbDemuxer->addItems({"Auto (Built-in libmpv / FFmpeg)", "FFmpeg Lavf", "Native MKV Demuxer"});
    m_spnNetworkCacheKb = new QSpinBox(grp0);
    m_spnNetworkCacheKb->setRange(512, 131072);
    m_spnNetworkCacheKb->setSingleStep(1024);
    m_spnNetworkCacheKb->setValue(8192);
    m_spnNetworkCacheKb->setSuffix(" KB");

    f0->addRow("Preferred Demuxer Engine:", m_cmbDemuxer);
    f0->addRow("Network Stream Cache Buffer:", m_spnNetworkCacheKb);
    l0->addWidget(grp0);
    l0->addStretch();

    // ==========================================
    // Sub-page 1: Video Decoder Profiles (Dialog 360)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Video Decoder Priorities & Threading (Dialog 360)", page1);
    auto* f1 = new QFormLayout(grp1);
    m_cmbVideoDecoderPriority = new QComboBox(grp1);
    m_cmbVideoDecoderPriority->addItems({
        "Hardware DXVA/D3D11 First (Auto Fallback)",
        "Direct3D 11 Video Acceleration Native",
        "DirectX Video Acceleration (DXVA2) Native",
        "Software FFmpeg Multi-Threaded Only"
    });
    m_chkMultiThreadDecoding = new QCheckBox("Enable multi-threaded software decoding across all CPU cores", grp1);

    f1->addRow("Decoder Allocation Priority:", m_cmbVideoDecoderPriority);
    f1->addRow("", m_chkMultiThreadDecoding);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: Audio Decoder Profiles (Dialog 361)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("Audio Decoder Engines (Dialog 361)", page2);
    auto* f2 = new QFormLayout(grp2);
    m_cmbAudioDecoderEngine = new QComboBox(grp2);
    m_cmbAudioDecoderEngine->addItems({"Built-in FFmpeg", "DirectShow Filter Pass-through"});

    f2->addRow("Audio Decoder Engine:", m_cmbAudioDecoderEngine);
    l2->addWidget(grp2);
    l2->addStretch();

    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);

    mainLayout->addWidget(m_subStack);

    // Signals
    connect(m_cmbDemuxer, &QComboBox::currentIndexChanged, this, &FilterControlPreferencesPage::markDirty);
    connect(m_spnNetworkCacheKb, &QSpinBox::valueChanged, this, &FilterControlPreferencesPage::markDirty);
    connect(m_cmbVideoDecoderPriority, &QComboBox::currentIndexChanged, this, &FilterControlPreferencesPage::markDirty);
    connect(m_chkMultiThreadDecoding, &QCheckBox::toggled, this, &FilterControlPreferencesPage::markDirty);
    connect(m_cmbAudioDecoderEngine, &QComboBox::currentIndexChanged, this, &FilterControlPreferencesPage::markDirty);
}

void FilterControlPreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void FilterControlPreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_cmbDemuxer->setCurrentText(QString::fromStdString(tx.getString("Filter", "Demuxer", "Auto (Built-in libmpv / FFmpeg)")));
    m_spnNetworkCacheKb->setValue(tx.getInt("Filter", "NetworkCacheKb", 8192));
    m_cmbVideoDecoderPriority->setCurrentText(QString::fromStdString(tx.getString("Filter", "VideoDecoderPriority", "Hardware DXVA/D3D11 First (Auto Fallback)")));
    m_chkMultiThreadDecoding->setChecked(tx.getBool("Filter", "MultiThreadDecoding", true));
    m_cmbAudioDecoderEngine->setCurrentText(QString::fromStdString(tx.getString("Filter", "AudioDecoderEngine", "Built-in FFmpeg")));

    m_dirty = false;
}

void FilterControlPreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setString("Filter", "Demuxer", m_cmbDemuxer->currentText().toStdString());
    tx.setInt("Filter", "NetworkCacheKb", m_spnNetworkCacheKb->value());
    tx.setString("Filter", "VideoDecoderPriority", m_cmbVideoDecoderPriority->currentText().toStdString());
    tx.setBool("Filter", "MultiThreadDecoding", m_chkMultiThreadDecoding->isChecked());
    tx.setString("Filter", "AudioDecoderEngine", m_cmbAudioDecoderEngine->currentText().toStdString());

    m_dirty = false;
}

void FilterControlPreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setString("Filter", "Demuxer", "Auto (Built-in libmpv / FFmpeg)");
    tx.setInt("Filter", "NetworkCacheKb", 8192);
    tx.setString("Filter", "VideoDecoderPriority", "Hardware DXVA/D3D11 First (Auto Fallback)");
    tx.setBool("Filter", "MultiThreadDecoding", true);
    tx.setString("Filter", "AudioDecoderEngine", "Built-in FFmpeg");

    loadFrom(tx);
    markDirty();
}

bool FilterControlPreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
