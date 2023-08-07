#include "app.h"

#include <QApplication>
#include <QMainWindow>

#include "mainmenu.h"

App::App(int& argc, char* argv[]) :
    QApplication(argc, argv){
}


int App::run()
{
    auto main_window = new QMainWindow();
    QWidget* menu = new MainMenu();
    main_window->setCentralWidget(menu);
    main_window->showMaximized();

    return exec();
}
