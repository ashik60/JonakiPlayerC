#include "SubtitlePreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

namespace Jonaki {

SubtitlePreferencesPage::SubtitlePreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void SubtitlePreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void SubtitlePreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: Subtitle Language & Priority (Dialog 368)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("Subtitle Language Selection & Folder Search (Dialog 368)", page0);
    auto* f0 = new QFormLayout(grp0);
    m_txtLanguagePriority = new QLineEdit("eng, ben", grp0);
    m_chkAutoLoadSameName = new QCheckBox("Auto-load external subtitles matching media filename (.srt, .ass, .vtt)", grp0);
    m_chkSearchSubfolders = new QCheckBox("Search 'subs', 'subtitles', and adjacent subfolders for matches", grp0);

    f0->addRow("Preferred Language Codes:", m_txtLanguagePriority);
    f0->addRow("", m_chkAutoLoadSameName);
    f0->addRow("", m_chkSearchSubfolders);
    l0->addWidget(grp0);
    l0->addStretch();

    // ==========================================
    // Sub-page 1: Font & Styling (Dialog 353)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Font Typography & Styling Overrides (Dialog 353)", page1);
    auto* f1 = new QFormLayout(grp1);
    m_chkShowSubtitles = new QCheckBox("Show subtitles during playback (Alt+H)", grp1);
    m_fntSubtitleFont = new QFontComboBox(grp1);
    m_spnFontSize = new QSpinBox(grp1);
    m_spnFontSize->setRange(10, 96);
    m_spnFontSize->setValue(24);
    m_spnFontSize->setSuffix(" pt");

    m_cmbFontColor = new QComboBox(grp1);
    m_cmbFontColor->addItem("White (#FFFFFFFF)", "#FFFFFFFF");
    m_cmbFontColor->addItem("Yellow (#FFFFFF00)", "#FFFFFF00");
    m_cmbFontColor->addItem("Cyan (#FF00FFFF)", "#FF00FFFF");
    m_cmbFontColor->addItem("Green (#FF00FF00)", "#FF00FF00");

    m_spnBorderSize = new QSpinBox(grp1);
    m_spnBorderSize->setRange(0, 10);
    m_spnBorderSize->setValue(2);
    m_spnBorderSize->setSuffix(" px");

    f1->addRow("", m_chkShowSubtitles);
    f1->addRow("Subtitle Font Family:", m_fntSubtitleFont);
    f1->addRow("Font Size:", m_spnFontSize);
    f1->addRow("Font Color:", m_cmbFontColor);
    f1->addRow("Border / Outline Thickness:", m_spnBorderSize);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: Sync & Placement (Dialog 374)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("Synchronization & Vertical Screen Placement", page2);
    auto* f2 = new QFormLayout(grp2);
    m_spnDelayStep = new QDoubleSpinBox(grp2);
    m_spnDelayStep->setRange(0.1, 5.0);
    m_spnDelayStep->setSingleStep(0.1);
    m_spnDelayStep->setValue(0.5);
    m_spnDelayStep->setSuffix(" sec (< / > hotkeys)");

    m_spnVerticalMargin = new QSpinBox(grp2);
    m_spnVerticalMargin->setRange(0, 100);
    m_spnVerticalMargin->setValue(90);
    m_spnVerticalMargin->setSuffix(" % (from top)");

    f2->addRow("Delay Adjustment Step:", m_spnDelayStep);
    f2->addRow("Vertical Position Margin:", m_spnVerticalMargin);
    l2->addWidget(grp2);
    l2->addStretch();

    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);

    mainLayout->addWidget(m_subStack);

    // Signals
    connect(m_txtLanguagePriority, &QLineEdit::textChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_chkAutoLoadSameName, &QCheckBox::toggled, this, &SubtitlePreferencesPage::markDirty);
    connect(m_chkSearchSubfolders, &QCheckBox::toggled, this, &SubtitlePreferencesPage::markDirty);
    connect(m_chkShowSubtitles, &QCheckBox::toggled, this, &SubtitlePreferencesPage::markDirty);
    connect(m_fntSubtitleFont, &QFontComboBox::currentFontChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_spnFontSize, &QSpinBox::valueChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_cmbFontColor, &QComboBox::currentIndexChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_spnBorderSize, &QSpinBox::valueChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_spnDelayStep, &QDoubleSpinBox::valueChanged, this, &SubtitlePreferencesPage::markDirty);
    connect(m_spnVerticalMargin, &QSpinBox::valueChanged, this, &SubtitlePreferencesPage::markDirty);
}

void SubtitlePreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void SubtitlePreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_txtLanguagePriority->setText(QString::fromStdString(tx.getString("Subtitles", "LanguagePriority", "eng, ben")));
    m_chkAutoLoadSameName->setChecked(tx.getBool("Subtitles", "AutoLoadSameName", true));
    m_chkSearchSubfolders->setChecked(tx.getBool("Subtitles", "SearchSubfolders", true));

    m_chkShowSubtitles->setChecked(tx.getBool("Subtitles", "ShowSubtitles", true));
    m_fntSubtitleFont->setCurrentFont(QFont(QString::fromStdString(tx.getString("Subtitles", "FontName", "sans-serif"))));
    m_spnFontSize->setValue(tx.getInt("Subtitles", "FontSize", 24));

    QString colorHex = QString::fromStdString(tx.getString("Subtitles", "FontColor", "#FFFFFFFF"));
    int cIdx = m_cmbFontColor->findData(colorHex);
    if (cIdx != -1) m_cmbFontColor->setCurrentIndex(cIdx);

    m_spnBorderSize->setValue(tx.getInt("Subtitles", "BorderSize", 2));
    m_spnDelayStep->setValue(tx.getDouble("Subtitles", "DelayStep", 0.5));
    m_spnVerticalMargin->setValue(tx.getInt("Subtitles", "VerticalMargin", 90));

    m_dirty = false;
}

void SubtitlePreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setString("Subtitles", "LanguagePriority", m_txtLanguagePriority->text().toStdString());
    tx.setBool("Subtitles", "AutoLoadSameName", m_chkAutoLoadSameName->isChecked());
    tx.setBool("Subtitles", "SearchSubfolders", m_chkSearchSubfolders->isChecked());

    tx.setBool("Subtitles", "ShowSubtitles", m_chkShowSubtitles->isChecked());
    tx.setString("Subtitles", "FontName", m_fntSubtitleFont->currentFont().family().toStdString());
    tx.setInt("Subtitles", "FontSize", m_spnFontSize->value());
    tx.setString("Subtitles", "FontColor", m_cmbFontColor->currentData().toString().toStdString());
    tx.setInt("Subtitles", "BorderSize", m_spnBorderSize->value());

    tx.setDouble("Subtitles", "DelayStep", m_spnDelayStep->value());
    tx.setInt("Subtitles", "VerticalMargin", m_spnVerticalMargin->value());

    m_dirty = false;
}

void SubtitlePreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setString("Subtitles", "LanguagePriority", "eng, ben");
    tx.setBool("Subtitles", "AutoLoadSameName", true);
    tx.setBool("Subtitles", "SearchSubfolders", true);
    tx.setBool("Subtitles", "ShowSubtitles", true);
    tx.setString("Subtitles", "FontName", "sans-serif");
    tx.setInt("Subtitles", "FontSize", 24);
    tx.setString("Subtitles", "FontColor", "#FFFFFFFF");
    tx.setInt("Subtitles", "BorderSize", 2);
    tx.setDouble("Subtitles", "DelayStep", 0.5);
    tx.setInt("Subtitles", "VerticalMargin", 90);

    loadFrom(tx);
    markDirty();
}

bool SubtitlePreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
