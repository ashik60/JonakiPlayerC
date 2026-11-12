#include "OpenUrlDialog.h"
#include "../network/UrlValidator.h"
#include "../settings/SettingsManager.h"
#include "../core/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGuiApplication>
#include <QClipboard>
#include <QFont>

namespace Jonaki {

OpenUrlDialog::OpenUrlDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    loadHistory();

    // Check clipboard if auto-paste is enabled
    if (m_chkAutoPaste->isChecked()) {
        checkClipboardForUrl();
    }
}

void OpenUrlDialog::setupUi() {
    setWindowTitle("Open URL");
    setObjectName("OpenUrlDialog");
    setModal(true);
    resize(480, 220);
    setMinimumWidth(400);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(8);

    // Prompt Header
    auto* promptLabel = new QLabel("Input the URL of the stream or file to be played.", this);
    QFont promptFont = promptLabel->font();
    promptFont.setBold(true);
    promptLabel->setFont(promptFont);
    mainLayout->addWidget(promptLabel);

    // Example subtext
    auto* exampleLabel = new QLabel("For example, http://www.daum.net/test.mp3", this);
    QPalette exPal = exampleLabel->palette();
    exPal.setColor(QPalette::WindowText, Qt::gray);
    exampleLabel->setPalette(exPal);
    mainLayout->addWidget(exampleLabel);

    // URL ComboBox (editable)
    m_urlCombo = new QComboBox(this);
    m_urlCombo->setObjectName("urlCombo");
    m_urlCombo->setEditable(true);
    m_urlCombo->lineEdit()->setPlaceholderText("http://...");
    m_urlCombo->lineEdit()->setClearButtonEnabled(true);
    mainLayout->addWidget(m_urlCombo);

    // Error label for validation feedback
    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setStyleSheet("color: #d9534f; font-size: 11px;");
    m_errorLabel->setVisible(false);
    mainLayout->addWidget(m_errorLabel);

    // Checkboxes
    m_chkAutoPaste = new QCheckBox("Auto paste URLs from clipboard", this);
    m_chkAutoPaste->setObjectName("chkAutoPaste");
    mainLayout->addWidget(m_chkAutoPaste);

    m_chkProcessSubtitles = new QCheckBox("Process subtitles", this);
    m_chkProcessSubtitles->setObjectName("chkProcessSubtitles");
    m_chkProcessSubtitles->setChecked(true);
    mainLayout->addWidget(m_chkProcessSubtitles);

    m_chkStoreEnteredOnly = new QCheckBox("Store only entered URLs (prevents storing URLs played in playlist)", this);
    m_chkStoreEnteredOnly->setObjectName("chkStoreEnteredOnly");
    m_chkStoreEnteredOnly->setChecked(true);
    mainLayout->addWidget(m_chkStoreEnteredOnly);

    // Button Row (History management & Action buttons)
    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(6);

    m_btnClearAll = new QPushButton("Clear all", this);
    m_btnClearAll->setObjectName("btnClearAll");
    buttonRow->addWidget(m_btnClearAll);

    m_btnDeleteSelected = new QPushButton("Delete selected", this);
    m_btnDeleteSelected->setObjectName("btnDeleteSelected");
    buttonRow->addWidget(m_btnDeleteSelected);

    buttonRow->addStretch();

    m_btnOpen = new QPushButton("Open", this);
    m_btnOpen->setObjectName("btnOpen");
    m_btnOpen->setDefault(true);
    buttonRow->addWidget(m_btnOpen);

    m_btnCancel = new QPushButton("Cancel", this);
    m_btnCancel->setObjectName("btnCancel");
    buttonRow->addWidget(m_btnCancel);

    mainLayout->addLayout(buttonRow);

    // Connect signals
    connect(m_btnOpen, &QPushButton::clicked, this, &OpenUrlDialog::accept);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnClearAll, &QPushButton::clicked, this, &OpenUrlDialog::onClearAllHistory);
    connect(m_btnDeleteSelected, &QPushButton::clicked, this, &OpenUrlDialog::onDeleteSelectedHistory);
    connect(m_urlCombo->lineEdit(), &QLineEdit::textChanged, this, &OpenUrlDialog::onUrlTextChanged);
}

void OpenUrlDialog::loadHistory() {
    auto& s = SettingsManager::instance();
    bool autoPaste = s.getBool("Network", "AutoPasteClipboard", true);
    bool processSubs = s.getBool("Network", "ProcessSubtitles", true);
    bool storeEnteredOnly = s.getBool("Network", "StoreEnteredUrlsOnly", true);

    m_chkAutoPaste->setChecked(autoPaste);
    m_chkProcessSubtitles->setChecked(processSubs);
    m_chkStoreEnteredOnly->setChecked(storeEnteredOnly);

    QString historyRaw = QString::fromStdString(s.getString("Network", "RecentUrls", ""));
    QStringList urls = historyRaw.split(";;", Qt::SkipEmptyParts);

    m_urlCombo->clear();
    for (const auto& u : urls) {
        if (!u.trimmed().isEmpty()) {
            m_urlCombo->addItem(u.trimmed());
        }
    }

    if (m_urlCombo->count() > 0) {
        m_urlCombo->setCurrentIndex(0);
        m_urlCombo->lineEdit()->selectAll();
    }
}

void OpenUrlDialog::saveHistory() {
    auto& s = SettingsManager::instance();
    s.setBool("Network", "AutoPasteClipboard", m_chkAutoPaste->isChecked());
    s.setBool("Network", "ProcessSubtitles", m_chkProcessSubtitles->isChecked());
    s.setBool("Network", "StoreEnteredUrlsOnly", m_chkStoreEnteredOnly->isChecked());

    QStringList urls;
    for (int i = 0; i < m_urlCombo->count() && i < MaxHistoryCount; ++i) {
        urls.append(m_urlCombo->itemText(i));
    }
    s.setString("Network", "RecentUrls", urls.join(";;").toStdString());
}

void OpenUrlDialog::checkClipboardForUrl() {
    if (m_userEditedText) return;

    QClipboard* clipboard = QGuiApplication::clipboard();
    if (!clipboard) return;

    QString clipText = clipboard->text().trimmed();
    if (clipText.isEmpty()) return;

    if (UrlValidator::isValidMediaUrl(clipText)) {
        m_urlCombo->setEditText(clipText);
        m_urlCombo->lineEdit()->selectAll();
    }
}

void OpenUrlDialog::onUrlTextChanged(const QString& text) {
    (void)text;
    m_userEditedText = true;
    m_errorLabel->setVisible(false);
}

void OpenUrlDialog::onClearAllHistory() {
    m_urlCombo->clear();
    saveHistory();
}

void OpenUrlDialog::onDeleteSelectedHistory() {
    int idx = m_urlCombo->currentIndex();
    if (idx >= 0) {
        m_urlCombo->removeItem(idx);
        saveHistory();
    }
}

void OpenUrlDialog::accept() {
    QString rawUrl = m_urlCombo->currentText().trimmed();
    QString errorMsg;

    if (!UrlValidator::isValidMediaUrl(rawUrl, &errorMsg)) {
        m_errorLabel->setText(errorMsg);
        m_errorLabel->setVisible(true);
        m_urlCombo->setFocus();
        return;
    }

    m_selectedUrl = rawUrl;

    // Add to history (most-recent first, deduplicated)
    int existingIdx = m_urlCombo->findText(rawUrl);
    if (existingIdx >= 0) {
        m_urlCombo->removeItem(existingIdx);
    }
    m_urlCombo->insertItem(0, rawUrl);
    m_urlCombo->setCurrentIndex(0);

    // Limit count
    while (m_urlCombo->count() > MaxHistoryCount) {
        m_urlCombo->removeItem(m_urlCombo->count() - 1);
    }

    saveHistory();
    QDialog::accept();
}

QString OpenUrlDialog::getUrl() const {
    return m_selectedUrl;
}

bool OpenUrlDialog::isProcessSubtitlesEnabled() const {
    return m_chkProcessSubtitles->isChecked();
}

void OpenUrlDialog::setProcessSubtitlesEnabled(bool enabled) {
    m_chkProcessSubtitles->setChecked(enabled);
}

bool OpenUrlDialog::isStoreEnteredUrlsOnlyEnabled() const {
    return m_chkStoreEnteredOnly->isChecked();
}

void OpenUrlDialog::setStoreEnteredUrlsOnlyEnabled(bool enabled) {
    m_chkStoreEnteredOnly->setChecked(enabled);
}

} // namespace Jonaki
