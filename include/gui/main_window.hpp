#ifndef LOGICSIM_GUI_MAIN_WINDOW_HPP
#define LOGICSIM_GUI_MAIN_WINDOW_HPP

#include <QApplication>
#include <QMainWindow>
#include <QActionGroup>
#include <QString>

#include "gui/resource_loader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace logicsim
{
    namespace gui
    {
        class MainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            explicit MainWindow(QWidget *parent = nullptr);
            ~MainWindow();

        protected:
            Ui::MainWindow *_ui;
        };

    }
}
#endif // MAIN_WINDOW_HPP
