#pragma once

#include <QDialog>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <memory>
#include "../settings/SettingsTransaction.h"

namespace Jonaki {

class GeneralPreferencesPage;
class PlaybackPreferencesPage;
class VideoPreferencesPage;
class AudioPreferencesPage;
class SubtitlePreferencesPage;
class FilterControlPreferencesPage;

/**
 * @brief Preferences dialog for JonakiPlayer.
 *
 * Implements the measured 751x564 PotPlayer baseline preferences window:
 * - Dialog 324 layout: Reset, Export Settings, OK, Cancel, Apply.
 * - Master 2-level category tree matching the 6 core categories and 22 sub-pages.
 * - Transactional staging buffer (SettingsTransaction) preventing accidental live mutations.
 */
class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog() override = default;

    void selectCategoryAndSubPage(int categoryIndex, int subPageIndex = 0);
    bool hasUnappliedChanges() const;

signals:
    void settingsApplied();

public slots:
    void applySettings();
    void resetCurrentPage();
    void exportSettings();
    void accept() override;
    void reject() override;

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onPageModified();

private:
    void setupUi();
    void buildCategoryTree();
    void updateApplyButtonState();

    SettingsTransaction m_tx;

    QTreeWidget* m_categoryTree = nullptr;
    QStackedWidget* m_pageStack = nullptr;

    GeneralPreferencesPage* m_generalPage = nullptr;
    PlaybackPreferencesPage* m_playbackPage = nullptr;
    VideoPreferencesPage* m_videoPage = nullptr;
    AudioPreferencesPage* m_audioPage = nullptr;
    SubtitlePreferencesPage* m_subtitlePage = nullptr;
    FilterControlPreferencesPage* m_filterPage = nullptr;

    QPushButton* m_btnReset = nullptr;
    QPushButton* m_btnExport = nullptr;
    QPushButton* m_btnOk = nullptr;
    QPushButton* m_btnCancel = nullptr;
    QPushButton* m_btnApply = nullptr;
};

} // namespace Jonaki
