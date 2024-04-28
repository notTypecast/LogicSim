#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QStatusBar>
#include <QFileDialog>
#include <QUndoStack>

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <fstream>

#include "gui/resource_loader.hpp"
#include "gui/component_label.hpp"

#include "model/circuit.hpp"
#include "model/component.hpp"

namespace logicsim
{
    namespace gui
    {
        class InsertComponentCommand;
        class DeleteComponentsCommand;
        class ChangeSimulationPropertiesCommand;

        class DesignArea : public QWidget
        {
            Q_OBJECT
        public:
            friend class InsertComponentCommand;
            friend class DeleteComponentsCommand;
            friend class ChangeSimulationPropertiesCommand;

            explicit DesignArea(QWidget *parent = nullptr);

            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);
            void mouseMoveEvent(QMouseEvent *ev);

            void keyPressEvent(QKeyEvent *ev);

            TOOL mode() const;
            bool setMode(TOOL tool, COMPONENT comp_type = COMPONENT::NONE, int res_idx = 0);
            void stopSimulationMode();
            void pauseSimulation();
            void stepSimulation();
            void continueSimulation();
            void resetSimulation();

            void setStatusBar(QStatusBar *status_bar);

            unsigned int frequency() const;
            void setFrequency(unsigned int freq);

            // called when tab changes from/to this
            void pauseState();
            // returns true if there was a running simulation before
            bool continueState();

            // whether circuit is empty
            bool empty() const;

            QString filepath() const;
            // writes current circuit to file
            bool writeToFile(bool new_file = false);
            void readFromFile(QString filepath);

            void undoAction();
            void redoAction();

        protected:
            // removes all components from selected
            void _unselectAll();

            // current tool
            TOOL _selected_tool = TOOL::SELECT;
            // current component to insert (applies to insert tool)
            COMPONENT _insert_component;
            int _insert_resource_idx;

            // vector of selected components: (Component, border) pairs
            std::vector<ComponentLabel *> _selected_components;

            // initial selection point for selection border
            int _select_x, _select_y;
            // labels for selection lines
            // in order, array holds two vertical (left to right),
            // then two horizontal (top to bottom)
            QLabel *_selection_border[4] = {nullptr};

            // current wire being drawn
            Wire *_wire = nullptr;
            // positions to snap destination to
            // wire snaps to closest
            std::tuple<ComponentLabel *, int, int> _wire_snap_closest;
            std::vector<std::tuple<ComponentLabel *, int, int>> _wire_snap_positions;

            // Circuit model
            model::circuit::Circuit _circuit_model;

            unsigned int _freq = 100;
            QTimer *_timer = nullptr;

            QStatusBar *_status_bar;
            QLabel *_ticks_label;

            // State information
            // Saved when tab changes, to reset when reselected
            bool _paused_state = false;
            QString _ticks_label_text;
            bool _state_sim_paused;

            QString _filepath;

            void _connect_component(ComponentLabel *label, bool first_time = true);
            void _disconnect_component(ComponentLabel *label);
            void _delete_components(std::unordered_map<std::string, ComponentLabel *> components);

            QUndoStack *_undo_stack;

            bool _moved_components = false;
            std::vector<QPoint> _init_comp_positions;

            /* Signals and slots often transmit position information
             * Such information is referred to as global, if its frame of reference
             * is the origin point of DesignArea; in that case, it is denoted (x, y)
             * Such information is referred to as relative, if its frame of reference
             * is the origin point of a different widget (usually a ComponentLabel);
             * in that case, it is denoted as (dx, dy)
             */
        protected slots:
            // Design Slots

            // triggered by selected of ComponentLabel
            // adds component to selected (or removes, based on ctrl)
            void addSelected(ComponentLabel *component, bool ctrl = false);
            // triggered by selected_nocheck of ComponentLabel
            // adds component to selected without performing any checks
            void addSelected_nocheck(ComponentLabel *component);
            // triggered by moved of ComponentLabel
            // moves selected components by dx, dy
            void moveSelectedComponents(int dx, int dy);
            // triggered by moveFinished of ComponentLabel
            // adds move to undo stack
            void finishMove();
            // triggered by wireSource of ComponentLabel
            // sets component as source of wire, with source position dx,dy
            void getWireSource(ComponentLabel *component, int dx, int dy);
            // triggered by wireMoved of ComponentLabel
            // moves wire destionation by dx, dy from wire source position
            void moveWireDest(int dx, int dy);
            // triggered by wireSnapFound of ComponentLabel
            // sets _wire_snap_x and _wire_snap_y to x and y
            void getWireSnapPos(ComponentLabel *component, int x, int y);
            // triggered by wireReleased of ComponentLabel
            // sets final position of wire target as dx, dy away from the wire source position
            void setWireDest();

            // Simulation
            // triggered by timer
            void executeTick();

            void propertyUndoActionPerformed();

        signals:
            // emitted when the mode is changed
            void modeChanged(TOOL tool);
            // emitted when a selection area is completed, to find components inside it
            void rangeQuery(int x_min, int y_min, int x_max, int y_max);
            // emitted during wire creation, when destination is moved
            // used to find components whose inputs/outputs are close to wire destination for snapping
            void wireSnap(ComponentLabel *wire_source, int x, int y);
            // emitted during simulation, after every tick
            // components receiving this signal will evaluate based on their component model
            void evaluate();
            // emitted when simulation is reset
            void resetResource();
            // emitted when writing to file
            void writeComponent(std::ofstream &file) const;
            // emitted when an undo action is redone/undone
            void newUndoActionPerformed(bool was_undo, bool undo_enabled, bool redo_enabled);
        };
    }
}

#endif // DESIGN_AREA_HPP
