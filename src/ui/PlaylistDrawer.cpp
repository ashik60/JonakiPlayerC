#include "PlaylistDrawer.h"
#include "../playlist/PlaylistController.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

namespace Jonaki {

PlaylistDrawer::PlaylistDrawer(PlaylistController* controller, QWidget* parent)
    : QWidget(parent), m_controller(controller) {
    setFixedWidth(280);
    setAcceptDrops(true);
    setupUi();
}

void PlaylistDrawer::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Header bar
    auto* header = new QWidget(this);
    header->setFixedHeight(28);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(4, 0, 4, 0);

    auto* lblTitle = new QLabel("Playlist", header);
    lblTitle->setStyleSheet("color: #d0d0d0; font-family: Tahoma; font-size: 11px; font-weight: bold;");
    headerLayout->addWidget(lblTitle);
    headerLayout->addStretch(1);

    auto* btnClose = new QPushButton("✕", header);
    btnClose->setFixedSize(18, 18);
    btnClose->setStyleSheet("QPushButton { background: transparent; color: #a0a0a0; border: none; } QPushButton:hover { color: #ffffff; }");
    connect(btnClose, &QPushButton::clicked, this, &PlaylistDrawer::closeRequested);
    headerLayout->addWidget(btnClose);
    mainLayout->addWidget(header);

    // List view
    m_listView = new QListView(this);
    m_listView->setStyleSheet(
        "QListView { background: #18181a; color: #d0d0d0; border: 1px solid #2a2a2e; font-family: Tahoma; font-size: 11px; } "
        "QListView::item { padding: 4px; border-bottom: 1px solid #202022; } "
        "QListView::item:hover { background: #26262a; } "
        "QListView::item:selected { background: #3c3c42; color: #f0b414; font-weight: bold; }"
    );

    if (m_controller) {
        m_listView->setModel(m_controller->model());
        connect(m_listView, &QListView::doubleClicked, this, [this](const QModelIndex& index) {
            if (m_controller) {
                m_controller->playIndex(index.row());
            }
        });
        connect(m_controller, &PlaylistController::currentItemChanged, this, [this](int idx, const PlaylistItem*) {
            if (m_listView && idx >= 0) {
                QModelIndex modelIdx = m_controller->model()->index(idx, 0);
                m_listView->setCurrentIndex(modelIdx);
            }
        });
    }
    mainLayout->addWidget(m_listView, 1);

    // Bottom toolbar
    auto* toolBar = new QWidget(this);
    toolBar->setFixedHeight(32);
    auto* toolLayout = new QHBoxLayout(toolBar);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(4);

    auto makeBtn = [](const QString& text) {
        auto* btn = new QPushButton(text);
        btn->setStyleSheet(
            "QPushButton { background: #26262a; color: #c0c0c0; border: 1px solid #3a3a3e; font-family: Tahoma; font-size: 10px; border-radius: 2px; padding: 3px 6px; } "
            "QPushButton:hover { background: #323238; color: #ffffff; }"
        );
        return btn;
    };

    m_btnAdd = makeBtn("+ Add");
    connect(m_btnAdd, &QPushButton::clicked, this, [this]() {
        QStringList files = QFileDialog::getOpenFileNames(this, "Add Files to Playlist");
        if (!files.isEmpty() && m_controller) {
            m_controller->addPaths(files);
        }
    });
    toolLayout->addWidget(m_btnAdd);

    m_btnRemove = makeBtn("- Del");
    connect(m_btnRemove, &QPushButton::clicked, this, [this]() {
        QModelIndex idx = m_listView->currentIndex();
        if (idx.isValid() && m_controller) {
            m_controller->model()->removeItem(idx.row());
        }
    });
    toolLayout->addWidget(m_btnRemove);

    m_btnClear = makeBtn("Clear");
    connect(m_btnClear, &QPushButton::clicked, this, [this]() {
        if (m_controller) {
            m_controller->model()->clear();
        }
    });
    toolLayout->addWidget(m_btnClear);

    m_btnOrder = makeBtn("Seq");
    connect(m_btnOrder, &QPushButton::clicked, this, [this]() {
        if (!m_controller) return;
        auto cur = m_controller->playbackOrder();
        if (cur == PlaybackOrder::Sequential) {
            m_controller->setPlaybackOrder(PlaybackOrder::RepeatAll);
            m_btnOrder->setText("All");
        } else if (cur == PlaybackOrder::RepeatAll) {
            m_controller->setPlaybackOrder(PlaybackOrder::RepeatOne);
            m_btnOrder->setText("1");
        } else if (cur == PlaybackOrder::RepeatOne) {
            m_controller->setPlaybackOrder(PlaybackOrder::Shuffle);
            m_btnOrder->setText("Shf");
        } else {
            m_controller->setPlaybackOrder(PlaybackOrder::Sequential);
            m_btnOrder->setText("Seq");
        }
    });
    toolLayout->addWidget(m_btnOrder);

    mainLayout->addWidget(toolBar);

    setStyleSheet("background: #202022; border-left: 1px solid #303034;");
}

void PlaylistDrawer::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PlaylistDrawer::dropEvent(QDropEvent* event) {
    if (!m_controller) return;
    const auto urls = event->mimeData()->urls();
    QStringList paths;
    for (const auto& u : urls) {
        QString p = u.toLocalFile();
        if (!p.isEmpty()) paths.append(p);
    }
    m_controller->addPaths(paths);
}

} // namespace Jonaki
