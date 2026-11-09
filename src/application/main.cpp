#include "JonakiApplication.h"
#include "../core/Logger.h"

int main(int argc, char* argv[]) {
    try {
        Jonaki::JonakiApplication app(argc, argv);
        if (!app.init()) {
            Jonaki::Logger::error("Application initialization failed.");
            return 1;
        }
        return app.exec();
    } catch (const std::exception& e) {
        Jonaki::Logger::error(std::string("Unhandled exception: ") + e.what());
        return 1;
    } catch (...) {
        Jonaki::Logger::error("Unknown fatal error encountered.");
        return 1;
    }
}
