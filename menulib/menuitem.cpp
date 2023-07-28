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

// #define DEBUG_VERBOSE

#include "menuitem.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScopedPointer>
#include <QSize>
#include <QUrl>
#include "core/app.h"
#include "menulib/menuproxy.h"
#include "menulib/menuwindow.h"


MenuItem::MenuItem(const QString& title, const MenuItem::ActionFunction& func,
                   const QString& icon, const QString& subtitle) :
    m_title(title)
{
    setDefaults();
    m_func = func;
    m_icon = icon;
    m_subtitle = subtitle;
}


MenuItem::MenuItem(const QString& title,
                   const MenuItem::OpenableWidgetMaker& func,
                   const QString& icon, const QString& subtitle) :
    m_title(title)
{
    setDefaults();
    m_openable_widget_maker = func;
    m_icon = icon;
    m_subtitle = subtitle;
}

MenuItem::MenuItem(MenuProxyPtr p_menuproxy, App& app)
    // m_title: below
{
    setDefaults();
    m_p_menuproxy = p_menuproxy;

    QScopedPointer<MenuWindow> mw(m_p_menuproxy->create(app));
    m_title = mw->title();
    m_subtitle = mw->subtitle();
    m_icon = mw->icon();
}


void MenuItem::setDefaults()
{
    // Not the most efficient, but saves lots of duplication

    // not m_title
    m_subtitle = "";
    m_icon = "";

    m_arrow_on_right = false;
    m_chain = false;
    m_copyright_details_pending = false;
    m_implemented = true;
    m_label_only = false;
    m_needs_privilege = false;
    m_not_if_locked = false;
    m_unsupported = false;

    m_func = nullptr;
    m_openable_widget_maker = nullptr;
    m_p_menuproxy.clear();
}


QString MenuItem::title() const
{
    return m_title;
}


QString MenuItem::subtitle() const
{
    return m_subtitle;
}


QWidget* MenuItem::rowWidget(App& app) const
{
    Q_UNUSED(app)
    Qt::Alignment text_align = Qt::AlignLeft | Qt::AlignVCenter;

    auto row = new BaseWidget();
    auto rowlayout = new QHBoxLayout();
    row->setLayout(rowlayout);

    // --------------------------------------------------------------------
    // Conventional menu item
    // --------------------------------------------------------------------
    //
    // ICON | - Title                                           | childicon
    // ICON | - Subtitle                                        | childicon
    // ICON |                                                   | childicon

    // Title/subtitle
    auto textlayout = new QVBoxLayout();

    auto title = new QLabel(m_title);
    title->setAlignment(text_align);
    textlayout->addWidget(title);
    rowlayout->addLayout(textlayout);
    rowlayout->addStretch();



    // Size policy
    // ... NO; now done automatically by HeightForWidthLayoutContainer
    // QSizePolicy size_policy(QSizePolicy::MinimumExpanding,  // horizontal
    //                         QSizePolicy::Fixed);  // vertical
    // row->setSizePolicy(size_policy);

    return row;
}


void MenuItem::act(App& app) const
{
    // ========================================================================
    // Reasons to refuse
    // ========================================================================
    if (m_label_only) {
        // qDebug() << "Label-only row touched; ignored";
        return;
    }
    // ========================================================================
    // Ways to act
    // ========================================================================
    if (m_p_menuproxy) {
        MenuWindow* pWindow = m_p_menuproxy->create(app);
        app.openSubWindow(pWindow);
        return;
    }
    if (m_func) {
        m_func();
        return;
    }
    if (m_openable_widget_maker) {
        OpenableWidget* widget = m_openable_widget_maker(app);
        app.openSubWindow(widget);
        return;
    }

    qWarning() << "Menu item selected but no action specified:"
               << m_title;
}


bool MenuItem::isImplemented() const
{
    return m_implemented;
}


MenuItem& MenuItem::setImplemented(const bool implemented)
{
    m_implemented = implemented;
    return *this;
}


MenuItem& MenuItem::setLabelOnly(const bool label_only)
{
    m_label_only = label_only;
    return *this;
}


MenuItem& MenuItem::setNeedsPrivilege(const bool needs_privilege)
{
    m_needs_privilege = needs_privilege;
    if (needs_privilege) {
        m_not_if_locked = true;  // just for safety!
    }
    return *this;
}


MenuItem& MenuItem::setNotIfLocked(const bool not_if_locked)
{
    m_not_if_locked = not_if_locked;
    return *this;
}


MenuItem& MenuItem::setUnsupported(const bool unsupported)
{
    m_unsupported = unsupported;
    return *this;
}


MenuItem& MenuItem::setIcon(const QString& icon)
{
    m_icon = icon;
    return *this;
}


QString MenuItem::info() const
{
    return "";
}


bool MenuItem::matchesSearch(const QString& search_text_lower) const
{
    return title().toLower().contains(search_text_lower) ||
            subtitle().toLower().contains(search_text_lower);
}
