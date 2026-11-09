#pragma once

#include <QMenu>
#include <QString>
#include <memory>

namespace Jonaki {

class MainWindow;

/**
 * @brief Builds and populates the clean-room PotPlayer context menu tree
 * from parsed reference hierarchy data (menu_hierarchy.json).
 */
class ContextMenuBuilder {
public:
    static std::unique_ptr<QMenu> buildMenu(MainWindow* parentWindow);
};

} // namespace Jonaki
