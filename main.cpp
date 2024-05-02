#include <QApplication>
#include "gui/main_window.hpp"

#define LOGICSIM_VERSION 1.0

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    logicsim::gui::MainWindow window;

    window.show();

    return app.exec();
}
