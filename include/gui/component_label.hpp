#ifndef LOGICSIM_GUI_COMPONENT_HPP
#define LOGICSIM_GUI_COMPONENT_HPP

#include <QLabel>
#include <QMouseEvent>

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel : public QLabel
        {
            Q_OBJECT
        public:
            explicit ComponentLabel(QWidget *parent = nullptr);

            void mouseMoveEvent(QMouseEvent *ev);
            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);

        protected:
            int _press_x, _press_y;
            bool _select_mode = true;

        signals:
            void selected(ComponentLabel *component);
            void moved(ComponentLabel *component, int x, int y);

        protected slots:
            void selectMode(bool enabled);

        };
    }
}

#endif // GUI_COMPONENT_HPP
