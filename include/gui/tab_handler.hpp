#ifndef LOGICSIM_GUI_TAB_HANDLER_HPP
#define LOGICSIM_GUI_TAB_HANDLER_HPP

#include <QTabWidget>
#include <QMessageBox>

#include <vector>
#include <unordered_map>

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

            void keyPressEvent(QKeyEvent *ev);

            bool setSimulationMode();

        protected:
            DesignArea *_designArea(int idx) const;
            bool _removeDesignArea(DesignArea *design_area);
            void _setupTab(DesignArea *design_area);

            QStatusBar *_status_bar;

            int _selected_idx = 0;

            void _saveFile(bool new_file);

            // maps full file directory to design area
            std::unordered_map<QString, DesignArea *> _open_files;
            // maps file name to design areas (tabs) with that file name
            std::unordered_map<QString, std::vector<DesignArea *>> _open_filenames;

        public slots:
            void addDesignArea();
            bool removeDesignArea();
            void openFile();
            void saveFile();
            void saveFileAs();
            void undoAction();
            void redoAction();

        protected slots:
            void changeArea(int idx);
            bool removeDesignArea(int idx);

            void setSelectMode();
            void setWireMode();
            void setInsertMode();
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
