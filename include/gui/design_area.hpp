#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QScrollArea>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QStatusBar>

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cmath>

#include "gui/resource_loader.hpp"
#include "gui/component_label.hpp"

#include "model/circuit.hpp"
#include "model/component.hpp"

namespace logicsim
{
    namespace gui
    {
        class DesignArea : public QScrollArea
        {
            Q_OBJECT
        public:
            explicit DesignArea(QWidget *parent = nullptr);

            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);
            void mouseMoveEvent(QMouseEvent *ev);

            void keyPressEvent(QKeyEvent *ev);

            void setStatusBar(QStatusBar *status_bar);

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

            int _freq = 10;
            QTimer *_timer;

            QStatusBar *_status_bar;
            QLabel *_ticks_label;

            /* Signals and slots often transmit position information
             * Such information is referred to as global, if its frame of reference
             * is the origin point of DesignArea; in that case, it is denoted (x, y)
             * Such information is referred to as relative, if its frame of reference
             * is the origin point of a different widget (usually a ComponentLabel);
             * in that case, it is denoted as (dx, dy)
             */
        protected slots:
            // triggered by triggered of actionSelect
            void setSelectMode();
            // triggered by triggered of any action that corresponds to a component
            void setInsertMode();
            // triggered by triggered of actionWire
            void setWireMode();
            // triggered by triggered of actionStart
            void setSimulationMode();
            // triggered by triggered of actionStop
            void stopSimulationMode();

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
            // triggered by triggered of actionPause
            void pauseSimulation();
            // triggered by triggered of actionContinue
            void continueSimulation();
            // triggered by triggered of actionReset
            void resetSimulation();

        signals:
            // emitted when the mode is changed
            void setMode(TOOL tool);
            // emitted when a selection area is completed, to find components inside it
            void rangeQuery(int x_min, int y_min, int x_max, int y_max);
            // emitted during wire creation, when destination is moved
            // used to find components whose inputs/outputs are close to wire destination for snapping
            void wireSnap(ComponentLabel *wire_source, int x, int y);
            // emitted during simulation, after every tick
            // components receiving this signal will evaluate based on their component model
            void evaluate();
        };
    }
}

#endif // DESIGN_AREA_HPP
