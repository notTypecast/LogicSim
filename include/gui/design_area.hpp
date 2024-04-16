#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QScrollArea>
#include <QMouseEvent>

#include "gui/main_window.hpp"
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

        protected:
            void _unselect();

            TOOL _selected_tool;
            COMPONENT _insert_component;

            ComponentLabel *_selected_component;
            QPixmap _border_img;
            QLabel *_selected_border = nullptr;

        protected slots:
            void setSelectMode();
            void setInsertMode();

            void setSelected(ComponentLabel *component);
            void moveComponent(ComponentLabel *component, int, int);

        signals:
            void selectMode(bool enabled);
        };
    }
}

#endif // DESIGN_AREA_HPP
