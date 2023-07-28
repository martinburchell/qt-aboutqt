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

class MenuHeader;
class Questionnaire;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

// A CamCOPS menu.

class MenuWindow : public OpenableWidget
{
    Q_OBJECT

public:
    MenuWindow(App& app, bool offer_search = false);
    void setIcon(const QString& icon);
    QString icon() const;

    // Menu title. Dynamic, so that the language can be changed dynamically.
    virtual QString title() const = 0;

    // Menu subtitle.
    virtual QString subtitle() const;

    // Returns the zero-based index of the currently selected item.
    int currentIndex() const;

    // Returns the patient instance represented by the currently selected item.
    PatientPtr currentPatient() const;

    // Catch generic events
    virtual bool event(QEvent* e) override;


protected:
    // Ensures items are recreated in full
    void rebuild(bool rebuild_header = true);

    // Make the Qt widget layout. Calls extraLayoutCreation().
    void makeLayout();

    // Additional function that subclasses can override to specialize layout.
    virtual void extraLayoutCreation() {}

    // Called by the default implementation of build(), for simplicity
    virtual void makeItems() {}

    // Create widgets. Called by the OpenableWidget framework prior to opening.
    void build() override;

    // Called by build() as it finishes. Allows subclasses to do extra
    // processing, e.g. emitting signals.
    virtual void afterBuild() {}

    // Load or reload the stylesheet on our widget.
    void reloadStyleSheet();
    void loadStyleSheet();


public slots:
    // "A menu item has been clicked."
    void menuItemClicked(QListWidgetItem* item);


protected:
    App& m_app;
    QString m_icon;
    bool m_top;
    bool m_offer_search;
    QVector<MenuItem> m_items;
    QPointer<QVBoxLayout> m_mainlayout;
    QPointer<MenuHeader> m_p_header;
    QPointer<QLineEdit> m_search_box;
    QPointer<QListWidget> m_p_listwidget;
};
