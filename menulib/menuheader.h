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
#include <QPointer>
#include <QWidget>
#include "core/app.h"  // for LockState

class BaseWidget;
class ClickableLabelWordWrapWide;
class LabelWordWrapWide;
class QAbstractButton;
class QLabel;
class QPushButton;


// A widget for the top part of a CamCOPS menu (with title and control
// buttons).

class MenuHeader : public QWidget  // breaks as BaseWidget
{
    Q_OBJECT
public:
    MenuHeader(QWidget* parent,
               App& app,
               bool top,
               const QString& title,
               const QString& icon_filename = "",
               bool debug_allowed = false);

    // Set the title.
    void setTitle(const QString& title);

    // Set the icon.
    // The parameter is a CamCOPS icon filename stub.
    void setIcon(const QString& icon_filename);

protected:
    // Internal version of setIcon().
    void setIcon(const QString& icon_filename, bool call_show_or_hide);

protected:
    App& m_app;
    QPointer<BaseWidget> m_top_bar;
    QPointer<QLabel> m_title_label;
    QPointer<QLabel> m_icon;
    QPointer<QLabel> m_mode;
};
