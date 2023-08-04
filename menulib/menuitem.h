#pragma once

#include <functional>  // for std::function
#include <QCoreApplication>  // for Q_DECLARE_TR_FUNCTIONS
#include <QSharedPointer>
#include <QString>

class App;
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

    // Creates and returns an (unowned) widget representing the row.
    QWidget* rowWidget() const;

    // The menu item has been chosen; act on it.
    void act() const;

protected:
    QString m_title;
    ActionFunction m_func;

private:
    void setDefaults();
};
