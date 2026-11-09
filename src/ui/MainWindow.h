#pragma once

#include <QMainWindow>
#include <memory>

namespace Jonaki {

class IPlaybackEngine;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupUi();
    void setupShortcuts();
    void setupEngine();

    std::unique_ptr<IPlaybackEngine> m_engine;
};

} // namespace Jonaki
