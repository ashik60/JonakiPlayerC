#include "PreferencesDialog.h"
#include "preferences/GeneralPreferencesPage.h"
#include "preferences/PlaybackPreferencesPage.h"
#include "preferences/VideoPreferencesPage.h"
#include "preferences/AudioPreferencesPage.h"
#include "preferences/SubtitlePreferencesPage.h"
#include "preferences/FilterControlPreferencesPage.h"
#include "../settings/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

namespace Jonaki {

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
    , m_tx(SettingsManager::instance()) {
    setupUi();
    buildCategoryTree();

    // Load transactional settings into all pages
    m_generalPage->loadFrom(m_tx);
    m_playbackPage->loadFrom(m_tx);
    m_videoPage->loadFrom(m_tx);
    m_audioPage->loadFrom(m_tx);
    m_subtitlePage->loadFrom(m_tx);
    m_filterPage->loadFrom(m_tx);

    updateApplyButtonState();
}

void PreferencesDialog::setupUi() {
    setWindowTitle("Preferences");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Set 751x564 baseline geometry at standard 96 DPI
    resize(751, 564);
    setMinimumSize(640, 480);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(8);

    auto* splitter = new QSplitter(Qt::Horizontal, this);

    // Left navigation tree
    m_categoryTree = new QTreeWidget(splitter);
    m_categoryTree->setHeaderHidden(true);
    m_categoryTree->setMinimumWidth(180);
    m_categoryTree->setMaximumWidth(280);
    m_categoryTree->setRootIsDecorated(true);
    m_categoryTree->setAnimated(true);

    // Right page stack
    m_pageStack = new QStackedWidget(splitter);
    m_generalPage = new GeneralPreferencesPage(m_pageStack);
    m_playbackPage = new PlaybackPreferencesPage(m_pageStack);
    m_videoPage = new VideoPreferencesPage(m_pageStack);
    m_audioPage = new AudioPreferencesPage(m_pageStack);
    m_subtitlePage = new SubtitlePreferencesPage(m_pageStack);
    m_filterPage = new FilterControlPreferencesPage(m_pageStack);

    m_pageStack->addWidget(m_generalPage);
    m_pageStack->addWidget(m_playbackPage);
    m_pageStack->addWidget(m_videoPage);
    m_pageStack->addWidget(m_audioPage);
    m_pageStack->addWidget(m_subtitlePage);
    m_pageStack->addWidget(m_filterPage);

    splitter->addWidget(m_categoryTree);
    splitter->addWidget(m_pageStack);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    rootLayout->addWidget(splitter);

    // Bottom action bar (Dialog 324 layout)
    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(0, 4, 0, 0);

    m_btnReset = new QPushButton("&Reset", this);
    m_btnExport = new QPushButton("&Export Settings...", this);

    m_btnOk = new QPushButton("OK", this);
    m_btnOk->setDefault(true);
    m_btnCancel = new QPushButton("Cancel", this);
    m_btnApply = new QPushButton("&Apply", this);

    bottomLayout->addWidget(m_btnReset);
    bottomLayout->addWidget(m_btnExport);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_btnOk);
    bottomLayout->addWidget(m_btnCancel);
    bottomLayout->addWidget(m_btnApply);

    rootLayout->addLayout(bottomLayout);

    // Wire tree navigation
    connect(m_categoryTree, &QTreeWidget::currentItemChanged, this, &PreferencesDialog::onTreeItemChanged);

    // Wire bottom actions
    connect(m_btnApply, &QPushButton::clicked, this, &PreferencesDialog::applySettings);
    connect(m_btnOk, &QPushButton::clicked, this, &PreferencesDialog::accept);
    connect(m_btnCancel, &QPushButton::clicked, this, &PreferencesDialog::reject);
    connect(m_btnReset, &QPushButton::clicked, this, &PreferencesDialog::resetCurrentPage);
    connect(m_btnExport, &QPushButton::clicked, this, &PreferencesDialog::exportSettings);

    // Wire dirty change notification from pages
    auto connectPage = [this](PreferencesPageBase* page) {
        connect(page, &PreferencesPageBase::pageModified, this, &PreferencesDialog::onPageModified);
    };
    connectPage(m_generalPage);
    connectPage(m_playbackPage);
    connectPage(m_videoPage);
    connectPage(m_audioPage);
    connectPage(m_subtitlePage);
    connectPage(m_filterPage);
}

void PreferencesDialog::buildCategoryTree() {
    m_categoryTree->clear();

    struct TreeDef {
        int catIdx;
        const char* catName;
        std::vector<const char*> subPages;
    };

    const std::vector<TreeDef> defs = {
        {0, "General", {
            "General Settings",
            "Startup",
            "OSD",
            "Logo",
            "Mouse",
            "Keyboard / Hotkeys",
            "Skin / Theme"
        }},
        {1, "Playback", {
            "Playback Settings",
            "Time Jump",
            "Repeat & Resume"
        }},
        {2, "Video", {
            "Video Settings",
            "Video Renderer & HWDEC",
            "Color & Level Control",
            "Resizing & Cropping"
        }},
        {3, "Audio", {
            "Audio Output & Speakers",
            "Volume Normalizer",
            "10-Band Equalizer",
            "Audio Delay & Codec"
        }},
        {4, "Subtitles", {
            "Subtitle Language & Priority",
            "Font & Styling",
            "Sync & Placement"
        }},
        {5, "Filter Control", {
            "Source / Splitter",
            "Video Decoder Profiles",
            "Audio Decoder Profiles"
        }}
    };

    for (const auto& d : defs) {
        auto* topItem = new QTreeWidgetItem(m_categoryTree);
        topItem->setText(0, d.catName);
        topItem->setData(0, Qt::UserRole, d.catIdx);
        topItem->setData(0, Qt::UserRole + 1, 0); // subPage 0

        for (size_t s = 0; s < d.subPages.size(); ++s) {
            auto* childItem = new QTreeWidgetItem(topItem);
            childItem->setText(0, d.subPages[s]);
            childItem->setData(0, Qt::UserRole, d.catIdx);
            childItem->setData(0, Qt::UserRole + 1, static_cast<int>(s));
        }

        topItem->setExpanded(true);
    }

    if (m_categoryTree->topLevelItemCount() > 0) {
        m_categoryTree->setCurrentItem(m_categoryTree->topLevelItem(0));
    }
}

void PreferencesDialog::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    (void)previous;
    if (!current) return;

    int catIdx = current->data(0, Qt::UserRole).toInt();
    int subIdx = current->data(0, Qt::UserRole + 1).toInt();

    if (catIdx >= 0 && catIdx < m_pageStack->count()) {
        m_pageStack->setCurrentIndex(catIdx);
        auto* page = qobject_cast<PreferencesPageBase*>(m_pageStack->widget(catIdx));
        if (page) {
            page->showSubPage(subIdx);
        }
    }
}

void PreferencesDialog::selectCategoryAndSubPage(int categoryIndex, int subPageIndex) {
    if (categoryIndex < 0 || categoryIndex >= m_categoryTree->topLevelItemCount()) return;

    auto* topItem = m_categoryTree->topLevelItem(categoryIndex);
    if (!topItem) return;

    if (subPageIndex >= 0 && subPageIndex < topItem->childCount()) {
        m_categoryTree->setCurrentItem(topItem->child(subPageIndex));
    } else {
        m_categoryTree->setCurrentItem(topItem);
    }
}

void PreferencesDialog::onPageModified() {
    updateApplyButtonState();
}

bool PreferencesDialog::hasUnappliedChanges() const {
    return m_generalPage->isDirty() ||
           m_playbackPage->isDirty() ||
           m_videoPage->isDirty() ||
           m_audioPage->isDirty() ||
           m_subtitlePage->isDirty() ||
           m_filterPage->isDirty() ||
           m_tx.isDirty();
}

void PreferencesDialog::updateApplyButtonState() {
    m_btnApply->setEnabled(hasUnappliedChanges());
}

void PreferencesDialog::applySettings() {
    m_generalPage->saveTo(m_tx);
    m_playbackPage->saveTo(m_tx);
    m_videoPage->saveTo(m_tx);
    m_audioPage->saveTo(m_tx);
    m_subtitlePage->saveTo(m_tx);
    m_filterPage->saveTo(m_tx);

    m_tx.commit();
    SettingsManager::instance().saveToFile("jonaki_settings.ini");

    updateApplyButtonState();
    emit settingsApplied();
}

void PreferencesDialog::accept() {
    if (hasUnappliedChanges()) {
        applySettings();
    }
    QDialog::accept();
}

void PreferencesDialog::reject() {
    m_tx.discard();
    QDialog::reject();
}

void PreferencesDialog::resetCurrentPage() {
    int curIdx = m_pageStack->currentIndex();
    auto* curPage = qobject_cast<PreferencesPageBase*>(m_pageStack->widget(curIdx));
    if (curPage) {
        curPage->resetToDefaults(m_tx);
        updateApplyButtonState();
    }
}

void PreferencesDialog::exportSettings() {
    QString path = QFileDialog::getSaveFileName(
        this,
        "Export Preferences to INI",
        "jonaki_settings.ini",
        "Configuration Files (*.ini);;All Files (*)"
    );

    if (path.isEmpty()) return;

    // Save page edits into transaction before exporting
    m_generalPage->saveTo(m_tx);
    m_playbackPage->saveTo(m_tx);
    m_videoPage->saveTo(m_tx);
    m_audioPage->saveTo(m_tx);
    m_subtitlePage->saveTo(m_tx);
    m_filterPage->saveTo(m_tx);

    if (m_tx.exportToFile(path.toStdString())) {
        QMessageBox::information(this, "Export Succeeded", "Settings successfully exported to:\n" + path);
    } else {
        QMessageBox::critical(this, "Export Failed", "Failed to write settings to:\n" + path);
    }
}

} // namespace Jonaki
