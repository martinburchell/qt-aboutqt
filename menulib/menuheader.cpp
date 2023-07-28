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

// #define DEBUG_SLOTS

#include "menuheader.h"
#include <QAbstractButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "widgets/basewidget.h"

MenuHeader::MenuHeader(QWidget* parent,
                       App& app,
                       const bool top,
                       const QString& title,
                       const QString& icon_filename,
                       const bool debug_allowed)
    : QWidget(parent),
      m_app(app)
{
    Q_UNUSED(top)
    Q_UNUSED(debug_allowed)

    auto mainlayout = new QVBoxLayout();
    setLayout(mainlayout);

    // ------------------------------------------------------------------------
    // Main row
    // ------------------------------------------------------------------------

    // Left
    m_top_bar = new BaseWidget();
    auto toprowlayout = new QHBoxLayout();
    m_top_bar->setLayout(toprowlayout);
    mainlayout->addWidget(m_top_bar);

    const Qt::Alignment button_align = Qt::AlignHCenter | Qt::AlignTop;
    const Qt::Alignment text_align = Qt::AlignLeft | Qt::AlignVCenter;


    // Spacing
    toprowlayout->addStretch();

    // Centre

    // - Icon for current menu
    m_icon = new QLabel();
    setIcon(icon_filename, false);
    toprowlayout->addWidget(m_icon, 0, button_align);

    // - Title
    m_title_label = new QLabel(title);
    m_title_label->setAlignment(text_align);
    toprowlayout->addWidget(m_title_label, 0, text_align);

    // Spacing
    toprowlayout->addStretch();
}


void MenuHeader::setTitle(const QString& title)
{
    if (!m_title_label) {
        return;
    }
    m_title_label->setText(title);
}


void MenuHeader::setIcon(const QString& icon_filename)
{
    setIcon(icon_filename, true);
}


void MenuHeader::setIcon(const QString& icon_filename, bool call_show_or_hide)
{
    Q_UNUSED(icon_filename)
    Q_UNUSED(call_show_or_hide)
}
