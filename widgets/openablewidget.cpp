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

#include "openablewidget.h"
#include <QGraphicsView>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QVBoxLayout>


OpenableWidget::OpenableWidget(QWidget* parent) :
    QWidget(parent),
    m_subwidget(nullptr),
    m_wants_fullscreen(false),
    m_escape_key_can_abort(true),
    m_escape_aborts_without_confirmation(false)
{
}


void OpenableWidget::build()
{
    QWidget* widget = m_subwidget.data();
    auto openable_widget = dynamic_cast<OpenableWidget*>(widget);
    if (openable_widget) {
        openable_widget->build();
    }
}


bool OpenableWidget::wantsFullscreen() const
{
    return m_wants_fullscreen;
}


void OpenableWidget::setWantsFullscreen(const bool fullscreen)
{
    m_wants_fullscreen = fullscreen;
}


void OpenableWidget::setWidgetAsOnlyContents(QWidget* widget,
                                             const int margin,
                                             const bool fullscreen,
                                             const bool esc_can_abort)
{
    // Remove any existing layout
    m_subwidget = nullptr;
    QLayout* l = layout();
    delete l;
    // https://stackoverflow.com/questions/7528680/how-to-delete-an-already-existing-layout-on-a-widget
    // https://stackoverflow.com/questions/6731331/is-it-still-safe-to-delete-nullptr-in-c0x

    auto vl = new QVBoxLayout(this);
    setLayout(vl);
    vl->setContentsMargins(margin, margin, margin, margin);
    vl->addWidget(widget);

    m_subwidget = widget;

    // Full screen?
    setWantsFullscreen(fullscreen);  // in case we're not shown yet
    // In case we're already showing:
    if (fullscreen) {
        emit enterFullscreen();
    } else {
        emit leaveFullscreen();
    }

    // Escape key behaviour?
    // - Note that one reason not to have multiple widgets active but not
    //   shown is performance; another is defining which grabs the Escape key
    //   (or other keys); a third is connecting up all the signals correctly.
    setEscapeKeyCanAbort(esc_can_abort, false);
}


bool OpenableWidget::escapeKeyCanAbort() const
{
    return m_escape_key_can_abort;
}


void OpenableWidget::setEscapeKeyCanAbort(const bool esc_can_abort,
                                          const bool without_confirmation)
{
    m_escape_key_can_abort = esc_can_abort;
    m_escape_aborts_without_confirmation = without_confirmation;
}


void OpenableWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
}


void OpenableWidget::keyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event)
}
