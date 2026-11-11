#include "VideoPreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

namespace Jonaki {

VideoPreferencesPage::VideoPreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void VideoPreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void VideoPreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: Video Settings (Dialog 366)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("Aspect Ratio & Deinterlacing", page0);
    auto* f0 = new QFormLayout(grp0);
    m_cmbAspectRatio = new QComboBox(grp0);
    m_cmbAspectRatio->addItems({"Default", "16:9", "4:3", "16:10", "1:1", "1.85:1", "2.35:1"});
    m_cmbDeinterlaceMode = new QComboBox(grp0);
    m_cmbDeinterlaceMode->addItems({"off", "auto", "on"});

    f0->addRow("Default Aspect Ratio:", m_cmbAspectRatio);
    f0->addRow("Deinterlace Mode:", m_cmbDeinterlaceMode);
    l0->addWidget(grp0);
    l0->addStretch();

    // ==========================================
    // Sub-page 1: Video Renderer & HWDEC (Dialog 355/360)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Hardware Accelerated Decoding & Renderer", page1);
    auto* f1 = new QFormLayout(grp1);
    m_cmbHwDec = new QComboBox(grp1);
#if defined(Q_OS_WIN)
    m_cmbHwDec->addItems({
        "auto-safe (Default Safe Auto)",
        "d3d11va-copy (Direct3D11 Copy-Back)",
        "d3d11va (Direct3D11 Native)",
        "dxva2-copy (DXVA2 Copy-Back)",
        "dxva2 (DXVA2 Native)",
        "no (Software Decoding)"
    });
#else
    m_cmbHwDec->addItems({
        "auto-safe (Default Safe Auto)",
        "vaapi-copy (VA-API Copy-Back)",
        "vaapi (VA-API Native)",
        "nvdec-copy (NVIDIA NVDEC Copy-Back)",
        "nvdec (NVIDIA NVDEC Native)",
        "no (Software Decoding)"
    });
#endif

    m_cmbColorRange = new QComboBox(grp1);
    m_cmbColorRange->addItems({"Auto", "Limited (16-235)", "Full (0-255)"});
    m_lblActiveHwdec = new QLabel("Active hardware decoder: None (Software)", grp1);
    m_lblActiveHwdec->setStyleSheet("color: #80c080; font-weight: bold;");

    f1->addRow("Hardware Decoding Profile:", m_cmbHwDec);
    f1->addRow("Output Color Range:", m_cmbColorRange);
    f1->addRow("Decoder Status:", m_lblActiveHwdec);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: Color & Level Control (Dialog 371)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("Color Adjustments & Luminance Levels (Dialog 371)", page2);
    auto* f2 = new QFormLayout(grp2);

    auto makeSliderPair = [this, grp2](QSlider*& sld, QSpinBox*& spn, int min, int max, int def) {
        sld = new QSlider(Qt::Horizontal, grp2);
        sld->setRange(min, max);
        sld->setValue(def);
        spn = new QSpinBox(grp2);
        spn->setRange(min, max);
        spn->setValue(def);

        connect(sld, &QSlider::valueChanged, spn, &QSpinBox::setValue);
        connect(spn, &QSpinBox::valueChanged, sld, &QSlider::setValue);
        connect(sld, &QSlider::valueChanged, this, &VideoPreferencesPage::markDirty);

        auto* h = new QHBoxLayout();
        h->addWidget(sld);
        h->addWidget(spn);
        return h;
    };

    f2->addRow("Brightness:", makeSliderPair(m_sldBrightness, m_spnBrightness, -100, 100, 0));
    f2->addRow("Contrast:", makeSliderPair(m_sldContrast, m_spnContrast, -100, 100, 0));
    f2->addRow("Saturation:", makeSliderPair(m_sldSaturation, m_spnSaturation, 0, 100, 66));
    f2->addRow("Hue:", makeSliderPair(m_sldHue, m_spnHue, -100, 100, 0));

    m_btnResetColor = new QPushButton("Reset Color Controls to Defaults", grp2);
    f2->addRow("", m_btnResetColor);
    l2->addWidget(grp2);
    l2->addStretch();

    connect(m_btnResetColor, &QPushButton::clicked, this, [this]() {
        m_sldBrightness->setValue(0);
        m_sldContrast->setValue(0);
        m_sldSaturation->setValue(66);
        m_sldHue->setValue(0);
        markDirty();
    });

    // ==========================================
    // Sub-page 3: Resizing & Cropping (Dialog 369/374)
    // ==========================================
    auto* page3 = new QWidget(this);
    auto* l3 = new QVBoxLayout(page3);
    l3->setContentsMargins(0, 0, 0, 0);

    auto* grp3 = new QGroupBox("Video Resizing & Frame Capture Margins", page3);
    auto* f3 = new QFormLayout(grp3);
    m_cmbResizeMethod = new QComboBox(grp3);
    m_cmbResizeMethod->addItems({"Original", "Match Window Size", "Fit Inside Window"});
    m_chkCorrectAspectOnCapture = new QCheckBox("Correct display aspect ratio when capturing frames", grp3);

    f3->addRow("Video Scaling Method:", m_cmbResizeMethod);
    f3->addRow("", m_chkCorrectAspectOnCapture);
    l3->addWidget(grp3);
    l3->addStretch();

    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);
    m_subStack->addWidget(page3);

    mainLayout->addWidget(m_subStack);

    // Signals
    connect(m_cmbAspectRatio, &QComboBox::currentIndexChanged, this, &VideoPreferencesPage::markDirty);
    connect(m_cmbDeinterlaceMode, &QComboBox::currentIndexChanged, this, &VideoPreferencesPage::markDirty);
    connect(m_cmbHwDec, &QComboBox::currentIndexChanged, this, &VideoPreferencesPage::markDirty);
    connect(m_cmbColorRange, &QComboBox::currentIndexChanged, this, &VideoPreferencesPage::markDirty);
    connect(m_cmbResizeMethod, &QComboBox::currentIndexChanged, this, &VideoPreferencesPage::markDirty);
    connect(m_chkCorrectAspectOnCapture, &QCheckBox::toggled, this, &VideoPreferencesPage::markDirty);
}

void VideoPreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void VideoPreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_cmbAspectRatio->setCurrentText(QString::fromStdString(tx.getString("Video", "AspectRatio", "Default")));
    m_cmbDeinterlaceMode->setCurrentText(QString::fromStdString(tx.getString("Video", "DeinterlaceMode", "off")));

    std::string hwdec = tx.getString("Video", "HwDec", "auto-safe");
    for (int i = 0; i < m_cmbHwDec->count(); ++i) {
        if (m_cmbHwDec->itemText(i).startsWith(QString::fromStdString(hwdec))) {
            m_cmbHwDec->setCurrentIndex(i);
            break;
        }
    }

    m_cmbColorRange->setCurrentText(QString::fromStdString(tx.getString("Video", "ColorRange", "Auto")));
    m_sldBrightness->setValue(tx.getInt("Video", "Brightness", 0));
    m_sldContrast->setValue(tx.getInt("Video", "Contrast", 0));
    m_sldSaturation->setValue(tx.getInt("Video", "Saturation", 66));
    m_sldHue->setValue(tx.getInt("Video", "Hue", 0));

    m_cmbResizeMethod->setCurrentText(QString::fromStdString(tx.getString("Video", "ResizeMethod", "Original")));
    m_chkCorrectAspectOnCapture->setChecked(tx.getBool("Video", "CorrectAspectOnCapture", true));

    m_dirty = false;
}

void VideoPreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setString("Video", "AspectRatio", m_cmbAspectRatio->currentText().toStdString());
    tx.setString("Video", "DeinterlaceMode", m_cmbDeinterlaceMode->currentText().toStdString());
    tx.setBool("Video", "Deinterlace", m_cmbDeinterlaceMode->currentText() != "off");

    QString hwText = m_cmbHwDec->currentText();
    QString hwCode = hwText.split(" ").first();
    tx.setString("Video", "HwDec", hwCode.toStdString());

    tx.setString("Video", "ColorRange", m_cmbColorRange->currentText().toStdString());
    tx.setInt("Video", "Brightness", m_sldBrightness->value());
    tx.setInt("Video", "Contrast", m_sldContrast->value());
    tx.setInt("Video", "Saturation", m_sldSaturation->value());
    tx.setInt("Video", "Hue", m_sldHue->value());

    tx.setString("Video", "ResizeMethod", m_cmbResizeMethod->currentText().toStdString());
    tx.setBool("Video", "CorrectAspectOnCapture", m_chkCorrectAspectOnCapture->isChecked());

    m_dirty = false;
}

void VideoPreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setString("Video", "AspectRatio", "Default");
    tx.setBool("Video", "Deinterlace", false);
    tx.setString("Video", "DeinterlaceMode", "off");
    tx.setString("Video", "HwDec", "auto-safe");
    tx.setString("Video", "ColorRange", "Auto");
    tx.setInt("Video", "Brightness", 0);
    tx.setInt("Video", "Contrast", 0);
    tx.setInt("Video", "Saturation", 66);
    tx.setInt("Video", "Hue", 0);
    tx.setString("Video", "ResizeMethod", "Original");
    tx.setBool("Video", "CorrectAspectOnCapture", true);

    loadFrom(tx);
    markDirty();
}

bool VideoPreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
