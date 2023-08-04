#include "app.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QLibraryInfo>
#include <QMainWindow>
#include <QMetaType>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QScreen>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextStream>
#include <QTranslator>
#include <QUrl>
#include <QUuid>
#include "lib/errorfunc.h"
#include "menu/mainmenu.h"

App::App(int& argc, char* argv[]) :
    QApplication(argc, argv),
    m_p_main_window(nullptr),
    m_p_window_stack(nullptr),
    m_p_hidden_stack(nullptr),
    m_maximized_before_fullscreen(true)  // true because openMainWindow() goes maximized
{
}


App::~App()
{
    delete m_p_main_window;
}


int App::run()
{
    openMainWindow();  // uses HelpMenu etc. and so must be AFTER TASK REGISTRATION

    return exec();  // Main Qt event loop
}


void App::openMainWindow()
{
    m_p_main_window = new QMainWindow();
    m_p_window_stack = new QStackedWidget(m_p_main_window);
    m_p_hidden_stack = QSharedPointer<QStackedWidget>(new QStackedWidget());
    OpenableWidget* menu = new MainMenu(*this);
    menu->build();
    m_p_main_window->setCentralWidget(menu);

    //recreateMainMenu();

    m_p_main_window->showMaximized();
}


void App::recreateMainMenu()
{
    closeAnyOpenSubWindows();

    return openSubWindow(new MainMenu(*this));
}


void App::closeAnyOpenSubWindows()
{
    // Scope for optimisation here as we're tearing down everything
    bool last_window;

    do {
        last_window = m_info_stack.isEmpty();

        if (!last_window) {
            m_info_stack.pop();
        }

        QWidget* top = m_p_window_stack->currentWidget();
        if (top) {
            m_p_window_stack->removeWidget(top);
            top->deleteLater();

            if (m_p_hidden_stack->count() > 0) {
                QWidget* w = m_p_hidden_stack->widget(m_p_hidden_stack->count() - 1);
                m_p_hidden_stack->removeWidget(w);
                const int index = m_p_window_stack->addWidget(w);
                m_p_window_stack->setCurrentIndex(index);
            }
        }
    } while (!last_window);
}


// ============================================================================
// Core
// ============================================================================

void App::openSubWindow(OpenableWidget* widget)
{
    if (!widget) {
        qCritical() << Q_FUNC_INFO << "- attempt to open nullptr";
        return;
    }

    Qt::WindowStates prev_window_state = m_p_main_window->windowState();
    QPointer<OpenableWidget> guarded_widget = widget;


    // ------------------------------------------------------------------------
    // Transfer any visible items (should be 0 or 1!) to hidden stack
    // ------------------------------------------------------------------------
    while (m_p_window_stack->count() > 0) {
        QWidget* w = m_p_window_stack->widget(m_p_window_stack->count() - 1);
        if (w) {
            m_p_window_stack->removeWidget(w);  // m_p_window_stack still owns w
            m_p_hidden_stack->addWidget(w);  // m_p_hidden_stack now owns w
        }
    }

    // ------------------------------------------------------------------------
    // Set the fullscreen state (before we build, for efficiency)
    // ------------------------------------------------------------------------
    bool wants_fullscreen = widget->wantsFullscreen();
    if (wants_fullscreen) {
        enterFullscreen();
    }

    // ------------------------------------------------------------------------
    // Add new thing to visible (one-item) "stack"
    // ------------------------------------------------------------------------
    int index = m_p_window_stack->addWidget(widget);  // will show the widget
    // The stack takes over ownership.

    // ------------------------------------------------------------------------
    // Build, if the OpenableWidget wants to be built
    // ------------------------------------------------------------------------
    {
        // qDebug() << Q_FUNC_INFO << "About to build";
        widget->build();
        // qDebug() << Q_FUNC_INFO << "Build complete, about to show";
    }

    // ------------------------------------------------------------------------
    // Make it visible
    // ------------------------------------------------------------------------
    m_p_window_stack->setCurrentIndex(index);

    // ------------------------------------------------------------------------
    // Signals
    // ------------------------------------------------------------------------
    connect(widget, &OpenableWidget::enterFullscreen,
            this, &App::enterFullscreen);
    connect(widget, &OpenableWidget::leaveFullscreen,
            this, &App::leaveFullscreen);
    connect(widget, &OpenableWidget::finished,
            this, &App::closeSubWindow);

    // ------------------------------------------------------------------------
    // Save information and manage ownership of associated things
    // ------------------------------------------------------------------------
    m_info_stack.push(OpenableInfo(guarded_widget,
                                   prev_window_state, wants_fullscreen));
}


void App::closeSubWindow()
{
    // ------------------------------------------------------------------------
    // All done?
    // ------------------------------------------------------------------------
    if (m_info_stack.isEmpty()) {
        errorfunc::fatalError("App::close: No more windows; closing");
    }

    // ------------------------------------------------------------------------
    // Get saved info (and, at the end of this function, release ownerships)
    // ------------------------------------------------------------------------
    OpenableInfo info = m_info_stack.pop();
    bool want_fullscreen = false;
    for (const OpenableInfo& info : m_info_stack) {
        if (info.wants_fullscreen) {
            want_fullscreen = true;
            break;
        }
    }

    // ------------------------------------------------------------------------
    // Get rid of the widget that's closing from the visible stack
    // ------------------------------------------------------------------------
    QWidget* top = m_p_window_stack->currentWidget();
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO << "Popping screen";
#endif
    m_p_window_stack->removeWidget(top);
    // Ownership is returned to the application, so...
    // - AH, NO. OWNERSHIP IS CONFUSING AND THE DOCS ARE DIFFERENT IN QT 4.8
    //   AND 5.9
    // - From http://doc.qt.io/qt-4.8/qstackedwidget.html#removeWidget :
    //      Removes widget from the QStackedWidget. i.e., widget is not deleted
    //      but simply removed from the stacked layout, causing it to be hidden.
    //      Note: Ownership of widget reverts to the application.
    // - From http://doc.qt.io/qt-5/qstackedwidget.html#removeWidget :
    //      Removes widget from the QStackedWidget. i.e., widget is not deleted
    //      but simply removed from the stacked layout, causing it to be hidden.
    //      Note: Parent object and parent widget of widget will remain the
    //      QStackedWidget. If the application wants to reuse the removed
    //      widget, then it is recommended to re-parent it.
    //   ... same for Qt 5.11.
    // - Also:
    //   https://stackoverflow.com/questions/2506625/how-to-delete-a-widget-from-a-stacked-widget-in-qt
    // But this should work regardless:
    top->deleteLater();  // later, in case it was this object that called us

    // ------------------------------------------------------------------------
    // Restore the widget from the top of the hidden stack
    // ------------------------------------------------------------------------
    Q_ASSERT(m_p_hidden_stack->count() > 0);  // the m_info_stack.isEmpty() check should exclude this
    QWidget* w = m_p_hidden_stack->widget(m_p_hidden_stack->count() - 1);
    m_p_hidden_stack->removeWidget(w);  // m_p_hidden_stack still owns w
    const int index = m_p_window_stack->addWidget(w);  // m_p_window_stack now owns w
    m_p_window_stack->setCurrentIndex(index);

    // ------------------------------------------------------------------------
    // Set next fullscreen state
    // ------------------------------------------------------------------------
    if (!want_fullscreen) {
        leaveFullscreen();  // will do nothing if we're not fullscreen now
    }
}

void App::enterFullscreen()
{
    // QWidget::showFullScreen does this:
    //
    // ensurePolished();
    // setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowMaximized))
    //               | Qt::WindowFullScreen);
    // setVisible(true);
    // activateWindow();

    // In other words, it clears the maximized flag. So we want this:
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO << "old windowState():" << m_p_main_window->windowState();
#endif
    Qt::WindowStates old_state = m_p_main_window->windowState();
    if (old_state & Qt::WindowFullScreen) {
        return;  // already fullscreen
    }
    m_maximized_before_fullscreen = old_state & Qt::WindowMaximized;
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO
             << "calling showFullScreen(); m_maximized_before_fullscreen ="
             << m_maximized_before_fullscreen;
#endif
    m_p_main_window->showFullScreen();
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO << "new windowState():" << m_p_main_window->windowState();
#endif
}


void App::leaveFullscreen()
{
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO << "old windowState():" << m_p_main_window->windowState();
#endif
    Qt::WindowStates old_state = m_p_main_window->windowState();
    if (!(old_state & Qt::WindowFullScreen)) {
        return;  // wasn't fullscreen
    }

    // m_p_main_window->showNormal();
    //
    // The docs say: "To return from full-screen mode, call showNormal()."
    // That's true, but incomplete. Both showFullscreen() and showNormal() turn
    // off any maximized state. QWidget::showNormal() does this:
    //
    // ensurePolished();
    // setWindowState(windowState() & ~(Qt::WindowMinimized
    //                                  | Qt::WindowMaximized
    //                                  | Qt::WindowFullScreen));
    // setVisible(true);

    // So, how to return to maximized mode from fullscreen?
    // Under Linux, the method above doesn't; that takes it to normal mode.
    // Under Linux, showMaximized() also takes it to normal mode!
    // But under Linux, calling showNormal() then showMaximized() immediately
    // does work.
    if (m_maximized_before_fullscreen) {
#ifdef DEBUG_SCREEN_STACK
        qDebug() << Q_FUNC_INFO << "calling showMaximized() then showMaximized()";
#endif
        // Under Linux, if you start with a fullscreen window and call
        // showMaximized(), it goes to normal mode. Also if you do this:
        // But this works:
        m_p_main_window->showNormal();
        m_p_main_window->showMaximized();
    } else {
#ifdef DEBUG_SCREEN_STACK
        qDebug() << Q_FUNC_INFO << "calling showNormal()";
#endif
        m_p_main_window->showNormal();
    }

    // Done.
#ifdef DEBUG_SCREEN_STACK
    qDebug() << Q_FUNC_INFO << "new windowState():" << m_p_main_window->windowState();
#endif
}
