#include "gui/component_label.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        ComponentLabel::ComponentLabel(QWidget *parent): QLabel{parent}
        {
        }

        // TODO: moving when added (before release)

        void ComponentLabel::mouseMoveEvent(QMouseEvent *ev)
        {
            if (!_select_mode)
            {
                return;
            }
            ev->accept();
            emit moved(this, ev->x() - _press_x, ev->y() - _press_y);
        }

        void ComponentLabel::mousePressEvent(QMouseEvent *ev)
        {
            if (_select_mode)
            {
                ev->accept();
            }
            else
            {
                ev->ignore();
            }
            _press_x = ev->x();
            _press_y = ev->y();
            emit selected(this);
        }

        void ComponentLabel::mouseReleaseEvent(QMouseEvent *ev)
        {
        }

        void ComponentLabel::selectMode(bool enabled)
        {
            _select_mode = enabled;
        }
    }
}
