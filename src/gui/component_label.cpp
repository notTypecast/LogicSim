#include "gui/component_label.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        ComponentLabel::ComponentLabel(QWidget *parent): QLabel{parent}
        {
        }

        void ComponentLabel::setBorder(QLabel *border)
        {
            if (_border != nullptr)
            {
                delete _border;
            }
            _border = border;
        }

        QLabel *ComponentLabel::border() const
        {
            return _border;
        }

        void ComponentLabel::mouseMoveEvent(QMouseEvent *ev)
        {
            if (!_select_mode)
            {
                return;
            }
            ev->accept();
            emit moved(ev->x() - _press_x, ev->y() - _press_y);
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
            emit selected(this, ev->modifiers() & Qt::ControlModifier);
        }

        void ComponentLabel::mouseReleaseEvent(QMouseEvent *ev)
        {
        }

        void ComponentLabel::selectMode(bool enabled)
        {
            _select_mode = enabled;
        }

        void ComponentLabel::checkRangeQuery(int min_x, int min_y, int max_x, int max_y)
        {
            int center_x = x() + width()/2;
            int center_y = y() + height()/2;
            if (min_x <= center_x && min_y <= center_y && center_x <= max_x && center_y <= max_y)
            {
                emit selected_nocheck(this);
            }
        }
    }
}
