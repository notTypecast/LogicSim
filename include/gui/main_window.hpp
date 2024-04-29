#ifndef LOGICSIM_GUI_MAIN_WINDOW_HPP
#define LOGICSIM_GUI_MAIN_WINDOW_HPP

#include <QApplication>
#include <QMainWindow>
#include <QActionGroup>
#include <QString>

#include "gui/properties.hpp"
#include "gui/resource_loader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// TODO: keep design
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

            void closeEvent(QCloseEvent *ev);

        protected:
            Ui::MainWindow *_ui;
            QActionGroup *_tool_group;
            std::vector<std::tuple<QAction *, COMPONENT, int>> _insert_actions;

            void _setSimulationMenu(bool enabled);
            bool _sim_paused;

            TOOL _last_design_tool = TOOL::SELECT;
            COMPONENT _last_insert_comp = COMPONENT::NONE;

        protected slots:
            void addDesignArea();

            void setSimulationMenu(bool running = true);
            void setDesignMenu();
            // TODO: if component is constant, 1 switches to 0 (same enum type)
            void setLastDesignTool(TOOL tool, COMPONENT comp_type = COMPONENT::NONE);

            void enableContinue();
            void enablePause();

            void simulationProperties();
            void setSimulationFrequency(QString freq);

            void setUndoActionState(bool undo_enabled, bool redo_enabled);
            void setSelectActionState(bool have_select, bool have_clipboard);

            void quit();
        };

    }
}
#endif // MAIN_WINDOW_HPP
