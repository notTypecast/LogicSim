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

        void Wire::setComponent1(ComponentLabel *component, int x, int y)
        {
            _setComponent(0, component, x, y);
        }

        void Wire::setComponent2(ComponentLabel *component, int x, int y)
        {
            _setComponent(1, component, x, y);
        }

        void Wire::clearComponent2()
        {
            _conns[1].component = nullptr;
        }

        void Wire::repositionDest(int dest_x, int dest_y)
        {
            int src_x = _conns[0].x;
            int src_y = _conns[0].y;

            int hwire_size = std::abs(src_x - dest_x) / 2;
            int vwire_size = std::abs(src_y - dest_y) + resources::WIRE_THICKNESS;

            _hwire1->resize(hwire_size, resources::WIRE_THICKNESS);
            _hwire2->resize(hwire_size, resources::WIRE_THICKNESS);
            _vwire->resize(resources::WIRE_THICKNESS, vwire_size);

            QPixmap hwire = resources::getWire(resources::LINE_TYPE::HORIZONTAL, hwire_size);

            _hwire1->setPixmap(hwire);
            _hwire2->setPixmap(hwire);
            _vwire->setPixmap(resources::getWire(resources::LINE_TYPE::VERTICAL, vwire_size));

            if (src_x > dest_x)
            {
                std::swap(src_x, dest_x);
                std::swap(src_y, dest_y);
            }

            int min_y = std::min(src_y, dest_y);
            int mid_x = src_x + hwire_size;

            _hwire1->move(src_x, src_y);
            _vwire->move(mid_x, min_y);
            _hwire2->move(mid_x, dest_y);
        }

        bool Wire::saveInComponents()
        {
            if (!(_conns[0].is_input ^ _conns[1].is_input))
            {
                return false;
            }
            if (!_conns[0].component->saveWire(this, _conns[0].is_input, _conns[0].idx))
            {
                return false;
            }
            if (!_conns[1].component->saveWire(this, _conns[1].is_input, _conns[1].idx))
            {
                _conns[0].component->removeWire(_conns[0].is_input, _conns[0].idx);
                return false;
            }

            return true;
        }

        void Wire::reposition()
        {
            _calculatePosition(0);
            _calculatePosition(1);
            repositionDest(_conns[1].x, _conns[1].y);
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

        void Wire::_setComponent(int idx, ComponentLabel *component, int x, int y)
        {
            _conns[idx].component = component;

            if (x >= component->width()/2)
            {
                _v[idx] = &resources::comp_io_rel_pos[component->comp_type()].second;
                _conns[idx].is_input = false;
            }
            else
            {
                _v[idx] = &resources::comp_io_rel_pos[component->comp_type()].first;
                _conns[idx].is_input = true;
            }

            _conns[idx].idx = y * _v[idx]->size() / component->height();
            _calculatePosition(idx);
        }

        void Wire::_calculatePosition(int idx)
        {
            _conns[idx].x = static_cast<int>((*_v[idx])[_conns[idx].idx].first * _conns[idx].component->width()) + _conns[idx].component->x();
            _conns[idx].y = static_cast<int>((*_v[idx])[_conns[idx].idx].second * _conns[idx].component->height()) + _conns[idx].component->y();
        }
    }
}
