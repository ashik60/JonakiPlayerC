#pragma once

#include <QDialog>
#include <QStringList>

class QComboBox;
class QCheckBox;
class QLabel;
class QPushButton;

namespace Jonaki {

/**
 * @brief Open URL Dialog reconstructed from PotPlayer Dialog 310.
 *
 * Baseline layout:
 * - Header prompt: "Input the URL of the stream or file to be played."
 * - Example subtext: "For example, http://www.daum.net/test.mp3"
 * - Editable ComboBox for URL input with recent history
 * - Checkboxes:
 *   - "Auto paste URLs from clipboard"
 *   - "Process subtitles"
 *   - "Store only entered URLs (prevents storing URLs played in playlist)"
 * - Action buttons:
 *   - "Clear all" (history)
 *   - "Delete selected" (history item)
 *   - "Open" (Default / Enter)
 *   - "Cancel" (Escape)
 */
class OpenUrlDialog : public QDialog {
    Q_OBJECT

public:
    explicit OpenUrlDialog(QWidget* parent = nullptr);
    ~OpenUrlDialog() override = default;

    /**
     * @brief Gets the validated URL entered or selected by the user.
     */
    QString getUrl() const;

    /**
     * @brief Whether "Process subtitles" was checked.
     */
    bool isProcessSubtitlesEnabled() const;
    void setProcessSubtitlesEnabled(bool enabled);

    /**
     * @brief Whether "Store only entered URLs" was checked.
     */
    bool isStoreEnteredUrlsOnlyEnabled() const;
    void setStoreEnteredUrlsOnlyEnabled(bool enabled);

    /**
     * @brief Sets the maximum number of history items to retain.
     */
    static constexpr int MaxHistoryCount = 20;

public slots:
    void accept() override;

private slots:
    void onClearAllHistory();
    void onDeleteSelectedHistory();
    void onUrlTextChanged(const QString& text);

private:
    void setupUi();
    void loadHistory();
    void saveHistory();
    void checkClipboardForUrl();

    QComboBox* m_urlCombo{nullptr};
    QLabel* m_errorLabel{nullptr};
    QCheckBox* m_chkAutoPaste{nullptr};
    QCheckBox* m_chkProcessSubtitles{nullptr};
    QCheckBox* m_chkStoreEnteredOnly{nullptr};
    QPushButton* m_btnOpen{nullptr};
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnClearAll{nullptr};
    QPushButton* m_btnDeleteSelected{nullptr};

    QString m_selectedUrl;
    bool m_userEditedText{false};
};

} // namespace Jonaki
