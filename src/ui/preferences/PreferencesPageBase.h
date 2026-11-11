#pragma once

#include <QWidget>

namespace Jonaki {

class SettingsTransaction;

/**
 * @brief Base class for all Preferences category pages.
 *
 * Each category page provides transactional loading, saving, and default reset,
 * and tracks dirty state. It supports navigation between sub-pages corresponding
 * to child items in the Preferences tree.
 */
class PreferencesPageBase : public QWidget {
    Q_OBJECT

public:
    explicit PreferencesPageBase(QWidget* parent = nullptr) : QWidget(parent) {}
    ~PreferencesPageBase() override = default;

    virtual void loadFrom(const SettingsTransaction& tx) = 0;
    virtual void saveTo(SettingsTransaction& tx) = 0;
    virtual void resetToDefaults(SettingsTransaction& tx) = 0;
    virtual bool isDirty() const = 0;
    virtual void showSubPage(int subPageIndex) = 0;

signals:
    void pageModified();
};

} // namespace Jonaki
