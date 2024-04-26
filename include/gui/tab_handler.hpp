#ifndef LOGICSIM_GUI_TAB_HANDLER_HPP
#define LOGICSIM_GUI_TAB_HANDLER_HPP

#include <QTabWidget>
#include <QMessageBox>

#include <vector>

#include "gui/design_area.hpp"
#include "gui/resource_loader.hpp"

namespace logicsim
{
    namespace gui
    {
        class TabHandler : public QTabWidget
        {
            Q_OBJECT
        public:
            explicit TabHandler(QWidget *parent = nullptr);

            void setStatusBar(QStatusBar *status_bar);

            DesignArea *currentDesignArea() const;

        protected:
            std::vector<DesignArea *> _design_areas;

            QStatusBar *_status_bar;

            int _selected_idx = 0;

            void _saveFile(bool new_file);

        public slots:
            void addDesignArea();
            void removeDesignArea();
            void openFile();
            void saveFile();
            void saveFileAs();

        protected slots:
            void changeArea(int idx);
            bool removeDesignArea(int idx);

            void setSelectMode();
            void setWireMode();
            void setInsertMode();
            void setSimulationMode();
            void stopSimulationMode();
            void pauseSimulation();
            void stepSimulation();
            void continueSimulation();
            void resetSimulation();

        signals:
            void designToolChanged(TOOL tool, COMPONENT comp_type = COMPONENT::NONE);
            void designTabChosen();
            void simulationTabChosen(bool running_sim);
        };
    }
}

#endif // LOGICSIM_GUI_TAB_HANDLER_HPP
