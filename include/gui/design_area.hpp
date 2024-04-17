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
            bool _just_inserted = false;

            int _select_x, _select_y;
            QLabel *_selection_border[4] = {nullptr};

        protected slots:
            void setSelectMode();
            void setInsertMode();

            void addSelected(ComponentLabel *component, bool ctrl = false);
            void addSelected_nocheck(ComponentLabel *component);
            void moveSelectedComponents(int, int);

        signals:
            void selectMode(bool enabled);
            void rangeQuery(int x_min, int y_min, int x_max, int y_max);
        };
    }
}

#endif // DESIGN_AREA_HPP
