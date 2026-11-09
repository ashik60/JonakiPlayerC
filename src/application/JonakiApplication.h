#pragma once

#include <QApplication>
#include <memory>

namespace Jonaki {

class MainWindow;

class JonakiApplication : public QApplication {
    Q_OBJECT

public:
    JonakiApplication(int& argc, char** argv);
    ~JonakiApplication() override;

    bool init();

private:
    std::unique_ptr<MainWindow> m_mainWindow;
};

} // namespace Jonaki
