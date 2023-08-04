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

#define MENUWINDOW_USE_HFW_LISTWIDGET  // good
// #define MENUWINDOW_USE_HFW_LAYOUT  // bad; window contains scroll area, which gets too short

#if defined(MENUWINDOW_USE_HFW_LISTWIDGET) == defined(MENUWINDOW_USE_HFW_LAYOUT)
#error Define MENUWINDOW_USE_HFW_LISTWIDGET xor MENUWINDOW_USE_HFW_LAYOUT
#endif

#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <QVBoxLayout>
#include "core/app.h"  // for LockState
#include "menulib/menuitem.h"
#include "widgets/openablewidget.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;

class MenuWindow : public OpenableWidget
{
    Q_OBJECT

public:
    MenuWindow(App& app);
    virtual QString title() const = 0;

    int currentIndex() const;

    virtual bool event(QEvent* e) override;


protected:
    void makeLayout();

    virtual void makeItems() {}

    void build() override;


public slots:
    // "A menu item has been clicked."
    void menuItemClicked(QListWidgetItem* item);


protected:
    App& m_app;
    bool m_top;
    QVector<MenuItem> m_items;
    QPointer<QVBoxLayout> m_mainlayout;
    QPointer<QListWidget> m_p_listwidget;
};
