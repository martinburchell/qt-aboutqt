#include "app.h"

#include <QApplication>
#include <QMainWindow>
#include <QUuid>

#include "menu/mainmenu.h"

App::App(int& argc, char* argv[]) :
    QApplication(argc, argv),
    m_p_main_window(nullptr)
{
}


App::~App()
{
    delete m_p_main_window;
}


int App::run()
{
    m_p_main_window = new QMainWindow();
    OpenableWidget* menu = new MainMenu();
    menu->build();
    m_p_main_window->setCentralWidget(menu);
    m_p_main_window->showMaximized();

    return exec();
}
