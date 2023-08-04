#pragma once

#include <functional>  // for std::function
#include <QCoreApplication>  // for Q_DECLARE_TR_FUNCTIONS
#include <QSharedPointer>
#include <QString>

class App;
class MenuWindow;
class OpenableWidget;
class QWidget;


// An item on one of the CamCOPS menus.

class MenuItem
{
    Q_DECLARE_TR_FUNCTIONS(MenuItem)

public:
    using ActionFunction = std::function<void()>;

public:
    MenuItem(const QString& title, const ActionFunction& func);

    // Item title.
    QString title() const;

    // Item subtitle.
    QString subtitle() const;

    // Creates and returns an (unowned) widget representing the row.
    QWidget* rowWidget(App& app) const;

    // The menu item has been chosen; act on it.
    void act(App& app) const;

protected:
    QString m_title;
    ActionFunction m_func;

private:
    void setDefaults();
};
