#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QScrollArea>
#include <QMouseEvent>

#include <vector>
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

        protected:
            void _unselectAll();

            TOOL _selected_tool = TOOL::SELECT;
            COMPONENT _insert_component;

            // vector of selected components: (Component, border) pairs
            std::vector<ComponentLabel *> _selected_components;

            int _select_x, _select_y;
            QLabel *_selection_border[4] = {nullptr};

            Wire *_wire = nullptr;
            int _wire_snap_x, _wire_snap_y;

        protected slots:
            void setSelectMode();
            void setInsertMode();
            void setWireMode();

            void addSelected(ComponentLabel *component, bool ctrl = false);
            void addSelected_nocheck(ComponentLabel *component);
            void moveSelectedComponents(int, int);

            void getWireSource(ComponentLabel *component, int x, int y);
            void moveWireDest(int x, int y);
            void getWireSnapPos(int x, int y);
            void setWireDest(int x, int y);
            void positionOverlaps(ComponentLabel *component, int x, int y);

        signals:
            void setMode(TOOL tool);
            void rangeQuery(int x_min, int y_min, int x_max, int y_max);
            void wireSnap(int x, int y);
            void checkPosition(int x, int y);
        };
    }
}

#endif // DESIGN_AREA_HPP
