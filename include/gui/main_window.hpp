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
            QActionGroup *_tool_group;
            std::vector<std::tuple<QAction *, COMPONENT, int>> _insert_actions;

            void _setSimulationMenu(bool enabled);
            bool _sim_paused;

        protected slots:
            void setSimulationMenu();
            void setDesignMenu();

            void enableContinue();
            void enablePause();
        };

    }
}
#endif // MAIN_WINDOW_HPP
