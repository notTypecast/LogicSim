#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QScrollArea>
#include <QMouseEvent>
#include <QString>

#include <vector>
#include <unordered_set>
#include <algorithm>

#include "gui/resource_loader.hpp"
#include "gui/component_label.hpp"

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
            // position to snap destination to
            int _wire_snap_x, _wire_snap_y;

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
            void getWireSnapPos(int x, int y);
            // triggered by wireReleased of ComponentLabel
            // sets final position of wire target as dx, dy away from the wire source position
            void setWireDest(int dx, int dy);
            // triggered by positionOverlaps of ComponentLabel
            // used to respond to a checkPosition signal sent by setWireDest
            void positionOverlaps(ComponentLabel *component, int x, int y);

        signals:
            // emitted when the mode is changed
            void setMode(TOOL tool);
            // emitted when a selection area is completed, to find components inside it
            void rangeQuery(int x_min, int y_min, int x_max, int y_max);
            // emitted during wire creation, when destination is moved
            // used to find components whose inputs/outputs are close to wire destination for snapping
            void wireSnap(ComponentLabel *wire_source, int x, int y);
            // emitted during wire creation, whend destination is finalized
            // used to find component node that wire should connect to, and create connection
            void checkPosition(int x, int y);
        };
    }
}

#endif // DESIGN_AREA_HPP
