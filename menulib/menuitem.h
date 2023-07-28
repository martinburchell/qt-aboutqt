/*
    Copyright (C) 2012, University of Cambridge, Department of Psychiatry.
    Created by Rudolf Cardinal (rnc1001@cam.ac.uk).

    This file is part of CamCOPS.

    CamCOPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CamCOPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CamCOPS. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <functional>  // for std::function
#include <QCoreApplication>  // for Q_DECLARE_TR_FUNCTIONS
#include <QSharedPointer>
#include <QString>
#include "menulib/menuproxy.h"

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
    using OpenableWidgetMaker = std::function<OpenableWidget*(App&)>;
    // http://stackoverflow.com/questions/14189440

public:
    // Multiple ways of constructing:
    MenuItem(const QString& title, const ActionFunction& func,
             const QString& icon = "", const QString& subtitle = "");
    MenuItem(const QString& title, const OpenableWidgetMaker& func,
             const QString& icon = "", const QString& subtitle = "");
    MenuItem(MenuProxyPtr p_menuproxy, App& app);

    // Item title.
    QString title() const;

    // Item subtitle.
    QString subtitle() const;

    // Set various options...
    // Return *this (https://en.wikipedia.org/wiki/Method_chaining).

    // Way to indicate "not implemented yet".
    MenuItem& setImplemented(bool implemented);

    // Text only.
    MenuItem& setLabelOnly(bool label_only = true);

    // Menu item can only be launched in privileged mode.
    MenuItem& setNeedsPrivilege(bool needs_privilege = true);

    // Menu item cannot be launched if app is locked.
    MenuItem& setNotIfLocked(bool not_if_locked = true);

    // Way to indicate "unsupported".
    MenuItem& setUnsupported(bool unsupported = true);

    // Set the icon filename.
    MenuItem& setIcon(const QString& icon);

    // Creates and returns an (unowned) widget representing the row.
    QWidget* rowWidget(App& app) const;

    // The menu item has been chosen; act on it.
    void act(App& app) const;

    // Is this item implemented?
    bool isImplemented() const;

    // Debugging description.
    QString info() const;

    // Do the title or subtitle contain the search text?
    // (Case-insensitive search.)
    bool matchesSearch(const QString& search_text_lower) const;

protected:
    QString m_title;
    QString m_subtitle;
    QString m_icon;  // icon filename
    bool m_arrow_on_right;
    bool m_chain;
    bool m_copyright_details_pending;
    bool m_implemented;
    bool m_label_only;
    bool m_needs_privilege;
    bool m_not_if_locked;
    bool m_unsupported;
    ActionFunction m_func;
    OpenableWidgetMaker m_openable_widget_maker;
    MenuProxyPtr m_p_menuproxy;

private:
    void setDefaults();

};
