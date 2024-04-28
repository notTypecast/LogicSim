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

            bool saveAndCloseAll();

        protected:
            DesignArea *_designArea(int idx) const;
            bool _removeDesignArea(DesignArea *design_area);
            void _setupTab(DesignArea *design_area);

            QStatusBar *_status_bar;

            int _selected_idx = 0;

            void _saveFile(bool new_file);

            // maps each tab to its undo state
            // state consists of 4 values: {is saved, undo counter, undo enabled, redo enabled}
            std::unordered_map<DesignArea *, std::tuple<bool, int, bool, bool>> _file_undo_state;
            // maps full file directory to design area
            std::unordered_map<QString, DesignArea *> _open_files;
            // maps file name to design areas (tabs) with that file name
            std::unordered_map<QString, std::vector<DesignArea *>> _open_filenames;

            void _addUnsavedIcon();
            void _removeUnsavedIcon();

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

            // triggered by a design area when an action which changes the undo stack is performed
            // any action that changes the circuit counts as a redo
            void performUndoAction(bool was_undo, bool undo_enabled, bool redo_enabled);

        signals:
            void designToolChanged(TOOL tool, COMPONENT comp_type = COMPONENT::NONE);
            void designTabChosen();
            void simulationTabChosen(bool running_sim);

            // signal to inform main window of undo/redo actions, to update edit menu
            void undoActionPerformed(bool undo_enabled, bool redo_enabled);
        };
    }
}

#endif // LOGICSIM_GUI_TAB_HANDLER_HPP
