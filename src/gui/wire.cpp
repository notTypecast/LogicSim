#include "gui/wire.hpp"
#include "gui/component_label.hpp"
#include <iostream>

namespace logicsim
{
    namespace gui
    {
        Wire::Wire(QWidget *parent)
        {
            _parent = parent;
            _hwire1 = new QLabel(parent);
            _vwire = new QLabel(parent);
            _hwire2 = new QLabel(parent);

            _hwire1->show();
            _vwire->show();
            _hwire2->show();
        }

        Wire::~Wire()
        {
            delete _hwire1;
            delete _vwire;
            delete _hwire2;
        }

        bool Wire::setComponent1(ComponentLabel *component, int dx, int dy)
        {
            return _setComponent(0, component, dx, dy);
        }

        bool Wire::setComponent2(ComponentLabel *component, int dx, int dy)
        {
            return _setComponent(1, component, dx, dy);
        }

        void Wire::clearComponent2()
        {
            _conns[1].component = nullptr;
        }

        void Wire::repositionDest(int dest_dx, int dest_dy)
        {
            int src_x = _conns[0].x;
            int src_y = _conns[0].y;

            int hwire_size = std::abs(src_x - dest_dx) / 2;
            int vwire_size = std::abs(src_y - dest_dy) + resources::WIRE_THICKNESS;

            _hwire1->resize(hwire_size, resources::WIRE_THICKNESS);
            _hwire2->resize(hwire_size, resources::WIRE_THICKNESS);
            _vwire->resize(resources::WIRE_THICKNESS, vwire_size);

            QPixmap hwire = resources::getWire(resources::LINE_TYPE::HORIZONTAL, hwire_size);

            _hwire1->setPixmap(hwire);
            _hwire2->setPixmap(hwire);
            _vwire->setPixmap(resources::getWire(resources::LINE_TYPE::VERTICAL, vwire_size));

            if (src_x > dest_dx)
            {
                std::swap(src_x, dest_dx);
                std::swap(src_y, dest_dy);
            }

            int min_y = std::min(src_y, dest_dy);
            int mid_x = src_x + hwire_size;

            _hwire1->move(src_x, src_y);
            _vwire->move(mid_x, min_y);
            _hwire2->move(mid_x, dest_dy);
        }

        bool Wire::saveInComponents()
        {
            if (!(_conns[0].is_input ^ _conns[1].is_input) || _conns[0].component == _conns[1].component)
            {
                return false;
            }
            if (!_conns[0].component->saveWire(this, _conns[0].is_input, _conns[0].idx))
            {
                return false;
            }
            if (!_conns[1].component->saveWire(this, _conns[1].is_input, _conns[1].idx))
            {
                _conns[0].component->removeWire(this, _conns[0].is_input, _conns[0].idx);
                return false;
            }

            return true;
        }

        void Wire::reposition()
        {
            _updatePosition(0);
            _updatePosition(1);
            repositionDest(_conns[1].x, _conns[1].y);
        }

        ComponentLabel *Wire::component1() const
        {
            return _conns[0].component;
        }

        int Wire::getComponent1x() const
        {
            return _conns[0].component->x();
        }

        int Wire::getComponent1y() const
        {
            return _conns[0].component->y();
        }

        bool Wire::finalized() const
        {
            return _conns[1].component != nullptr;
        }

        void Wire::removeFromOppositeComponent(ComponentLabel *component)
        {
            bool idx = _conns[0].component == component;
            _conns[idx].component->removeWire(this, _conns[idx].is_input, _conns[idx].idx);
        }

        bool Wire::_setComponent(int idx, ComponentLabel *component, int dx, int dy)
        {
            bool result = Wire::calculateWireTargetPos(component, dx, dy, _conns[idx].x, _conns[idx].y, _conns[idx].is_input, _conns[idx].idx);
            if (result)
            {
                _conns[idx].component = component;
            }

            return result;
        }

        void Wire::_updatePosition(int idx)
        {
            int rel_x, rel_y;
            std::tie(rel_x, rel_y) = resources::getComponentIORelativePos(_conns[idx].component, _conns[idx].is_input, _conns[idx].idx);
            _conns[idx].x = rel_x + _conns[idx].component->x();
            _conns[idx].y = rel_y + _conns[idx].component->y();
        }

        bool Wire::calculateWireTargetPos(ComponentLabel *component, int dx, int dy, int &x, int &y, bool &is_input, int &io_idx)
        {
            if (dx >= component->width()/2)
            {
                is_input = false;
            }
            else
            {
                is_input = true;
            }

            if (resources::getComponentIOPositionVector(component->comp_type(), is_input).empty())
            {
                return false;
            }

            int distance = std::numeric_limits<int>::max();
            io_idx = 0;

            int i = -1;
            for (auto const & p : resources::getComponentIOPositionVector(component->comp_type(), is_input))
            {
                ++i;
                int new_distance = std::abs(dy - p.second * component->height());
                if (new_distance < distance)
                {
                    distance = new_distance;
                    io_idx = i;
                    continue;
                }
                break;
            }

            int rel_x, rel_y;
            std::tie(rel_x, rel_y) = resources::getComponentIORelativePos(component, is_input, io_idx);
            x = rel_x + component->x();
            y = rel_y + component->y();

            return true;
        }
    }
}
