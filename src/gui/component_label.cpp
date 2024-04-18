#include "gui/component_label.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        ComponentLabel::ComponentLabel(QWidget *parent): QLabel{parent}
        {
        }

        ComponentLabel::~ComponentLabel()
        {
            for (const auto &wire: _input_wires)
            {
                delete wire;
            }

            for (const auto &wire: _output_wires)
            {
                delete wire;
            }
        }

        void ComponentLabel::setCompType(COMPONENT comp_type)
        {
            _comp_type = comp_type;
            setPixmap(resources::comp_images.at(comp_type));
            _n_inputs = resources::comp_io_rel_pos[comp_type].first.size();
            _input_wires = std::vector<Wire *>(_n_inputs, nullptr);
            _n_outputs = resources::comp_io_rel_pos[comp_type].second.size();
        }

        COMPONENT ComponentLabel::comp_type() const
        {
            return _comp_type;
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

        bool ComponentLabel::saveWire(Wire *wire, bool is_input, int idx)
        {
            if (is_input)
            {
                if (_input_wires[idx] != nullptr)
                {
                    return false;
                }
                _input_wires[idx] = wire;
            }
            else
            {
                _output_wires.push_back(wire);
            }

            return true;
        }

        void ComponentLabel::removeWire(bool is_input, int idx)
        {
            if (is_input)
            {
                _input_wires[idx] = nullptr;
            }
            else
            {
                _output_wires[idx] = nullptr;
            }
        }

        void ComponentLabel::moveWires()
        {
            for (const auto &wire: _input_wires)
            {
                if (wire == nullptr)
                {
                    continue;
                }
                wire->reposition();
            }

            for (const auto &wire: _output_wires)
            {
                wire->reposition();
            }
        }

        void ComponentLabel::mouseMoveEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case INSERT:
                ev->ignore();
                break;
            case SELECT:
                ev->accept();
                emit moved(ev->x() - _press_x, ev->y() - _press_y);
                break;
            case WIRE:
                ev->accept();
                emit wireMoved(ev->x(), ev->y());
                break;
            }
        }

        void ComponentLabel::mousePressEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case INSERT:
                ev->ignore();
                break;
            case SELECT:
                ev->accept();
                _press_x = ev->x();
                _press_y = ev->y();
                emit selected(this, ev->modifiers() & Qt::ControlModifier);
                break;
            case WIRE:
                ev->accept();
                emit wireSource(this, ev->x(), ev->y());
                break;
            }
        }

        void ComponentLabel::mouseReleaseEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case INSERT:
                ev->ignore();
                break;
            case WIRE:
                ev->accept();
                emit wireReleased(ev->x(), ev->y());
                break;
            default:
                break;
            }
        }

        void ComponentLabel::changeMode(TOOL tool)
        {
            _current_tool = tool;
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

        void ComponentLabel::wireSnap(int x, int y)
        {
            int obj_x = this->x();
            int obj_y = this->y();
            if (!(obj_x <= x && obj_y <= y && x <= obj_x + width() && y <= obj_y + height()))
            {
                return;
            }

            x -= obj_x;
            y -= obj_y;

            const std::vector<std::pair<double, double>> *v;
            if (x >= width()/2)
            {
                v = &resources::comp_io_rel_pos[_comp_type].second;
            }
            else
            {
                v = &resources::comp_io_rel_pos[_comp_type].first;
            }

            size_t idx = y * v->size() / height();
            if (idx == v->size())
            {
                --idx;
            }
            int target_x = static_cast<int>((*v)[idx].first * width()) + obj_x;
            int target_y = static_cast<int>((*v)[idx].second * height()) + obj_y;

            emit wireSnapFound(target_x, target_y);
        }

        void ComponentLabel::checkPosition(int x, int y)
        {
            int obj_x = this->x();
            int obj_y = this->y();
            if (obj_x <= x && obj_y <= y && x <= obj_x + width() && y <= obj_y + height())
            {
                emit positionOverlaps(this, x, y);
            }
        }
    }
}
