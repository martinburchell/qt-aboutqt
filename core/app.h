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
#include <QApplication>
#include <QMetaType>
#include <QPointer>
#include <QSharedPointer>
#include <QStack>
class IdPolicy;
class OpenableWidget;
class QMainWindow;
class QStackedWidget;
class QTextStream;
class QTranslator;
class Version;


// The main application object.
class App : public QApplication
{
    Q_OBJECT

public:
    struct OpenableInfo {
    public:
        OpenableInfo()
        {}

        OpenableInfo(const QPointer<OpenableWidget>& widget,
                     Qt::WindowStates prev_window_state, bool wants_fullscreen) :
            widget(widget),
            prev_window_state(prev_window_state),
            wants_fullscreen(wants_fullscreen)
        {}
    public:
        QPointer<OpenableWidget> widget;
        Qt::WindowStates prev_window_state;
        bool wants_fullscreen;
    };

    // ------------------------------------------------------------------------
    // Core
    // ------------------------------------------------------------------------
public:
    // Create the app (with command-line arguments).
    App(int& argc, char* argv[]);

    // Destructor.
    ~App();

    // Run the app.
    int run();

protected:
    // Open the CamCOPS main window.
    void openMainWindow();

    // Add/replace the main/single user menu attached to the main window
    void recreateMainMenu();

    // ------------------------------------------------------------------------
    // Opening/closing windows
    // ------------------------------------------------------------------------
private:
    void closeAnyOpenSubWindows();

public:
    void openSubWindow(OpenableWidget* widget);

public slots:
    // "Close the topmost sub-window."
    void closeSubWindow();

    // "Enter fullscreen mode."
    void enterFullscreen();

    // "Leave fullscreen mode."
    void leaveFullscreen();

protected:
    // The main menu (first, bottom-most) window.
    QPointer<QMainWindow> m_p_main_window;

    // The stack of visible windows.
    QPointer<QStackedWidget> m_p_window_stack;

    // The stack of hidden windows.
    QSharedPointer<QStackedWidget> m_p_hidden_stack;  // we own it entirely, so QSharedPointer

    // Before we went fullscreen, were we maximized?
    bool m_maximized_before_fullscreen;

    // Information about windows we've opened in the stack.
    QStack<OpenableInfo> m_info_stack;

};
