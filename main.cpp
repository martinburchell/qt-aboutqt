#include <QApplication>
#include <QStyleFactory>
#include "core/app.h"


int main(int argc, char* argv[])
{
    QStyle* style = QStyleFactory::create("Fusion");
    QApplication::setStyle(style);

    App app(argc, argv);
    return app.run();
}
