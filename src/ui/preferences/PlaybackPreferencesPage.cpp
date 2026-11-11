#include "PlaybackPreferencesPage.h"
#include "../../settings/SettingsTransaction.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

namespace Jonaki {

PlaybackPreferencesPage::PlaybackPreferencesPage(QWidget* parent)
    : PreferencesPageBase(parent) {
    setupUi();
}

void PlaybackPreferencesPage::markDirty() {
    m_dirty = true;
    emit pageModified();
}

void PlaybackPreferencesPage::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_subStack = new QStackedWidget(this);

    // ==========================================
    // Sub-page 0: Playback Settings (Dialog 358)
    // ==========================================
    auto* page0 = new QWidget(this);
    auto* l0 = new QVBoxLayout(page0);
    l0->setContentsMargins(0, 0, 0, 0);

    auto* grp0 = new QGroupBox("Playback History & Window Behavior", page0);
    auto* v0 = new QVBoxLayout(grp0);
    m_chkRememberVideoPositions = new QCheckBox("Remember video playback positions in SQLite database", grp0);
    m_chkRememberAudioPositions = new QCheckBox("Remember audio playback positions", grp0);
    m_chkPreventScreenSaver = new QCheckBox("Prevent screensaver and display sleep during playback", grp0);
    m_chkAutoAddSimilarFiles = new QCheckBox("Automatically add similar files in directory to playlist", grp0);

    v0->addWidget(m_chkRememberVideoPositions);
    v0->addWidget(m_chkRememberAudioPositions);
    v0->addWidget(m_chkPreventScreenSaver);
    v0->addWidget(m_chkAutoAddSimilarFiles);
    v0->addStretch();
    l0->addWidget(grp0);

    // ==========================================
    // Sub-page 1: Time Jump (Dialog 364)
    // ==========================================
    auto* page1 = new QWidget(this);
    auto* l1 = new QVBoxLayout(page1);
    l1->setContentsMargins(0, 0, 0, 0);

    auto* grp1 = new QGroupBox("Timeline Seeking & Jump Step Intervals", page1);
    auto* f1 = new QFormLayout(grp1);
    m_chkJumpKeyframe = new QCheckBox("Jump to nearest keyframe for instantaneous seeks (recommended)", grp1);
    m_spnShortJumpSec = new QSpinBox(grp1);
    m_spnShortJumpSec->setRange(1, 60);
    m_spnShortJumpSec->setSuffix(" sec (Left/Right Arrows)");
    m_spnMediumJumpSec = new QSpinBox(grp1);
    m_spnMediumJumpSec->setRange(5, 300);
    m_spnMediumJumpSec->setSuffix(" sec (Ctrl+Left/Right Arrows)");
    m_spnLongJumpSec = new QSpinBox(grp1);
    m_spnLongJumpSec->setRange(10, 600);
    m_spnLongJumpSec->setSuffix(" sec (Shift+Left/Right Arrows)");

    f1->addRow("", m_chkJumpKeyframe);
    f1->addRow("Short Jump Interval:", m_spnShortJumpSec);
    f1->addRow("Medium Jump Interval:", m_spnMediumJumpSec);
    f1->addRow("Long Jump Interval:", m_spnLongJumpSec);
    l1->addWidget(grp1);
    l1->addStretch();

    // ==========================================
    // Sub-page 2: Repeat & Resume (Dialog 358/377)
    // ==========================================
    auto* page2 = new QWidget(this);
    auto* l2 = new QVBoxLayout(page2);
    l2->setContentsMargins(0, 0, 0, 0);

    auto* grp2 = new QGroupBox("Repeat Modes & Auto-Resume Thresholds", page2);
    auto* f2 = new QFormLayout(grp2);
    m_cmbRepeatMode = new QComboBox(grp2);
    m_cmbRepeatMode->addItems({"0: Do Not Repeat (Stop at end)", "1: Repeat Current Track", "2: Repeat Entire Playlist"});
    m_chkAutoResume = new QCheckBox("Automatically resume playback from recorded position", grp2);
    m_spnMinResumeThresholdSec = new QSpinBox(grp2);
    m_spnMinResumeThresholdSec->setRange(1, 120);
    m_spnMinResumeThresholdSec->setSuffix(" sec");

    f2->addRow("Default Repeat Mode:", m_cmbRepeatMode);
    f2->addRow("", m_chkAutoResume);
    f2->addRow("Minimum Duration to Record Resume Position:", m_spnMinResumeThresholdSec);
    l2->addWidget(grp2);
    l2->addStretch();

    m_subStack->addWidget(page0);
    m_subStack->addWidget(page1);
    m_subStack->addWidget(page2);

    mainLayout->addWidget(m_subStack);

    // Signals
    connect(m_chkRememberVideoPositions, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);
    connect(m_chkRememberAudioPositions, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);
    connect(m_chkPreventScreenSaver, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);
    connect(m_chkAutoAddSimilarFiles, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);
    connect(m_chkJumpKeyframe, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);
    connect(m_chkAutoResume, &QCheckBox::toggled, this, &PlaybackPreferencesPage::markDirty);

    connect(m_spnShortJumpSec, &QSpinBox::valueChanged, this, &PlaybackPreferencesPage::markDirty);
    connect(m_spnMediumJumpSec, &QSpinBox::valueChanged, this, &PlaybackPreferencesPage::markDirty);
    connect(m_spnLongJumpSec, &QSpinBox::valueChanged, this, &PlaybackPreferencesPage::markDirty);
    connect(m_cmbRepeatMode, &QComboBox::currentIndexChanged, this, &PlaybackPreferencesPage::markDirty);
    connect(m_spnMinResumeThresholdSec, &QSpinBox::valueChanged, this, &PlaybackPreferencesPage::markDirty);
}

void PlaybackPreferencesPage::showSubPage(int subPageIndex) {
    if (subPageIndex >= 0 && subPageIndex < m_subStack->count()) {
        m_subStack->setCurrentIndex(subPageIndex);
    }
}

void PlaybackPreferencesPage::loadFrom(const SettingsTransaction& tx) {
    m_chkRememberVideoPositions->setChecked(tx.getBool("Playback", "RememberVideoPositions", true));
    m_chkRememberAudioPositions->setChecked(tx.getBool("Playback", "RememberAudioPositions", true));
    m_chkPreventScreenSaver->setChecked(tx.getBool("Playback", "PreventScreenSaver", true));
    m_chkAutoAddSimilarFiles->setChecked(tx.getBool("Playback", "AutoAddSimilarFiles", true));

    m_chkJumpKeyframe->setChecked(tx.getBool("Playback", "JumpKeyframe", true));
    m_spnShortJumpSec->setValue(tx.getInt("Playback", "ShortJumpSec", 5));
    m_spnMediumJumpSec->setValue(tx.getInt("Playback", "MediumJumpSec", 30));
    m_spnLongJumpSec->setValue(tx.getInt("Playback", "LongJumpSec", 60));

    m_cmbRepeatMode->setCurrentIndex(tx.getInt("Playback", "RepeatMode", 0));
    m_chkAutoResume->setChecked(tx.getBool("Playback", "AutoResume", true));
    m_spnMinResumeThresholdSec->setValue(tx.getInt("Playback", "MinResumeThresholdSec", 10));

    m_dirty = false;
}

void PlaybackPreferencesPage::saveTo(SettingsTransaction& tx) {
    tx.setBool("Playback", "RememberVideoPositions", m_chkRememberVideoPositions->isChecked());
    tx.setBool("Playback", "RememberAudioPositions", m_chkRememberAudioPositions->isChecked());
    tx.setBool("Playback", "PreventScreenSaver", m_chkPreventScreenSaver->isChecked());
    tx.setBool("Playback", "AutoAddSimilarFiles", m_chkAutoAddSimilarFiles->isChecked());

    tx.setBool("Playback", "JumpKeyframe", m_chkJumpKeyframe->isChecked());
    tx.setInt("Playback", "ShortJumpSec", m_spnShortJumpSec->value());
    tx.setInt("Playback", "MediumJumpSec", m_spnMediumJumpSec->value());
    tx.setInt("Playback", "LongJumpSec", m_spnLongJumpSec->value());

    tx.setInt("Playback", "RepeatMode", m_cmbRepeatMode->currentIndex());
    tx.setBool("Playback", "AutoResume", m_chkAutoResume->isChecked());
    tx.setInt("Playback", "MinResumeThresholdSec", m_spnMinResumeThresholdSec->value());

    m_dirty = false;
}

void PlaybackPreferencesPage::resetToDefaults(SettingsTransaction& tx) {
    tx.setBool("Playback", "RememberVideoPositions", true);
    tx.setBool("Playback", "RememberAudioPositions", true);
    tx.setBool("Playback", "PreventScreenSaver", true);
    tx.setBool("Playback", "AutoAddSimilarFiles", true);
    tx.setBool("Playback", "JumpKeyframe", true);
    tx.setInt("Playback", "ShortJumpSec", 5);
    tx.setInt("Playback", "MediumJumpSec", 30);
    tx.setInt("Playback", "LongJumpSec", 60);
    tx.setInt("Playback", "RepeatMode", 0);
    tx.setBool("Playback", "AutoResume", true);
    tx.setInt("Playback", "MinResumeThresholdSec", 10);

    loadFrom(tx);
    markDirty();
}

bool PlaybackPreferencesPage::isDirty() const {
    return m_dirty;
}

} // namespace Jonaki
