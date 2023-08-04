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


MenuItem::MenuItem(const QString& title, const MenuItem::ActionFunction& func) :
    m_title(title)
{
    m_func = func;
}


QString MenuItem::title() const
{
    return m_title;
}


QWidget* MenuItem::rowWidget(App& app) const
{
    Q_UNUSED(app)
    Qt::Alignment text_align = Qt::AlignLeft | Qt::AlignVCenter;

    auto row = new BaseWidget();
    auto rowlayout = new QHBoxLayout();
    row->setLayout(rowlayout);

    auto textlayout = new QVBoxLayout();

    auto title = new QLabel(m_title);
    title->setAlignment(text_align);
    textlayout->addWidget(title);
    rowlayout->addLayout(textlayout);
    rowlayout->addStretch();

    return row;
}


void MenuItem::act(App& app) const
{
    Q_UNUSED(app)

    if (m_func) {
        m_func();
        return;
    }
}
