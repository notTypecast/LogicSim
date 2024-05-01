#include "gui/wire.hpp"
#include "gui/component_label.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
    Wire::Wire(double scale, QWidget *parent) : QWidget{parent}
        {
            _scale = SCALE_SCALING_FACTOR * scale;

            _hwire1 = new QLabel(parent);
            _vwire = new QLabel(parent);
            _hwire2 = new QLabel(parent);

            _hwire1->show();
            _vwire->show();
            _hwire2->show();

            _hwire1_on = new QLabel(parent);
            _vwire_on = new QLabel(parent);
            _hwire2_on = new QLabel(parent);

            _hwire1_on->hide();
            _vwire_on->hide();
            _hwire2_on->hide();

            _hwire1_up = new QLabel(parent);
            _hwire1_down = new QLabel(parent);
            _vwire_left = new QLabel(parent);
            _vwire_right = new QLabel(parent);
            _hwire2_up = new QLabel(parent);
            _hwire2_down = new QLabel(parent);

            _hwire1_up->hide();
            _hwire1_down->hide();
            _vwire_left->hide();
            _vwire_right->hide();
            _hwire2_up->hide();
            _hwire2_down->hide();
        }

        Wire::~Wire()
        {
            delete _hwire1;
            delete _vwire;
            delete _hwire2;

            delete _hwire1_on;
            delete _vwire_on;
            delete _hwire2_on;

            delete _hwire1_up;
            delete _hwire1_down;
            delete _vwire_left;
            delete _vwire_right;
            delete _hwire2_up;
            delete _hwire2_down;
        }

        bool Wire::setComponent1(ComponentLabel *component, int dx, int dy)
        {
            return _setComponent(0, component, dx, dy);
        }

        bool Wire::setComponent2(ComponentLabel *component, int dx, int dy)
        {
            return _setComponent(1, component, dx, dy);
        }

        bool Wire::setComponent1(ComponentLabel *component, bool is_input, int io_idx)
        {
            return _setComponent(0, component, is_input, io_idx);
        }

        bool Wire::setComponent2(ComponentLabel *component, bool is_input, int io_idx)
        {
            return _setComponent(1, component, is_input, io_idx);
        }

        void Wire::repositionDest(int dest_x, int dest_y)
        {
            // refresh potential loaded colored wires
            _wire_on = false;

            int src_x = _conns[0].x;
            int src_y = _conns[0].y;

            int hwire_size = std::abs(src_x - dest_x) / 2;
            int vwire_size = std::abs(src_y - dest_y) + static_cast<int>(_scale * resources::WIRE_THICKNESS);

            QPixmap hwire = resources::getWire(resources::LINE_TYPE::HORIZONTAL, hwire_size, _scale);
            QPixmap vwire = resources::getWire(resources::LINE_TYPE::VERTICAL, vwire_size, _scale);

            _hwire1->resize(hwire.width(), hwire.height());
            _hwire2->resize(hwire.width(), hwire.height());
            _vwire->resize(vwire.width(), vwire.height());

            _hwire1->setPixmap(hwire);
            _hwire2->setPixmap(hwire);
            _vwire->setPixmap(vwire);

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
            if (_current_tool == TOOL::SIMULATE)
            {
                _wire_on = false;
                _createColorWire();
            }
        }

        void Wire::hide()
        {
            _hwire1->hide();
            _vwire->hide();
            _hwire2->hide();

            _hwire1_up->hide();
            _hwire1_down->hide();
            _vwire_left->hide();
            _vwire_right->hide();
            _hwire2_up->hide();
            _hwire2_down->hide();
        }

        void Wire::show()
        {
            _hwire1->show();
            _vwire->show();
            _hwire2->show();
        }

        std::pair<ComponentLabel *, int> Wire::outputComponentInfo() const
        {
            int comp_idx = _conns[0].is_input;
            return {_conns[comp_idx].component, _conns[comp_idx].idx};
        }

        ComponentLabel *Wire::component1() const
        {
            return _conns[0].component;
        }

        ComponentLabel *Wire::component2() const
        {
            return _conns[1].component;
        }

        ComponentLabel *Wire::oppositeComponent(ComponentLabel *component) const
        {
            return _conns[component == _conns[0].component].component;
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

        void Wire::saveInOppositeComponent(ComponentLabel *component)
        {
            bool idx = _conns[0].component == component;
            _conns[idx].component->saveWire(this, _conns[idx].is_input, _conns[idx].idx);
        }

        void Wire::removeFromOppositeComponent(ComponentLabel *component)
        {
            bool idx = _conns[0].component == component;
            _conns[idx].component->removeWire(this, _conns[idx].is_input, _conns[idx].idx);
        }

        void Wire::removeFromComponents()
        {
            _conns[0].component->removeWire(this, _conns[0].is_input, _conns[0].idx);
            _conns[1].component->removeWire(this, _conns[1].is_input, _conns[1].idx);
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

        bool Wire::_setComponent(int idx, ComponentLabel *component, bool is_input, int io_idx)
        {
            if (static_cast<size_t>(io_idx) >= resources::getComponentIOPositionVector(component->comp_type(), is_input).size())
            {
                return false;
            }

            _conns[idx].component = component;
            _conns[idx].is_input = is_input;
            _conns[idx].idx = io_idx;
            std::tie(_conns[idx].x, _conns[idx].y) = resources::getComponentIORelativePos(component, is_input, io_idx);
            _conns[idx].x += component->x();
            _conns[idx].y += component->y();

            return true;
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
                is_input = !is_input;
            }

            int distance = std::numeric_limits<int>::max();
            io_idx = 0;

            const std::vector<std::pair<double, double>> &vec = resources::getComponentIOPositionVector(component->comp_type(), is_input);
            int i = -1;
            for (auto const & p : vec)
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

        void Wire::checkProximity(int x, int y)
        {
            int middle = (_conns[0].x + _conns[1].x)/2;
            QPoint p(x, y);
            QPoint points[4] = {
                {_conns[0].x, _conns[0].y},
                {_conns[1].x, _conns[1].y},
                {middle, _conns[0].y},
                {middle, _conns[1].y}
            };

            // find closest point
            int min_dist = std::numeric_limits<int>::max(), min_idx = -1;
            int dist[4];

            for (int i = 0; i < 4; ++i)
            {
                dist[i] = (p - points[i]).manhattanLength();
                if (dist[i] < min_dist)
                {
                    min_dist = dist[i];
                    min_idx = i;
                }
            }

            // project to closest wire segment
            QPoint proj;
            int other_vert_idx = min_idx == 2 ? 3 : 2;
            if (min_idx <= 1)
            {
                proj.setX(x);
                proj.setY(points[min_idx].y());

                bool left = std::min(points[min_idx].x(), middle) > x;
                bool right = std::max(points[min_idx].x(), middle) < x;
                if (left && min_idx == 0 || right && min_idx == 1)
                {
                    proj = points[min_idx];
                }
                else if (left && min_idx == 1 || right && min_idx == 0)
                {
                    proj = points[min_idx + 2];
                }

            }
            else if (dist[min_idx - 2] < dist[other_vert_idx])
            {
                proj.setX(x);
                proj.setY(points[min_idx].y());

                bool left = std::min(points[min_idx - 2].x(), middle) > x;
                bool right = std::max(points[min_idx - 2].x(), middle) < x;
                if (left && min_idx == 2 || right && min_idx == 3)
                {
                    proj = points[min_idx - 2];
                }
                else if (left && min_idx == 3 || right && min_idx == 2)
                {
                    proj = points[min_idx];
                }
            }
            else
            {
                proj.setX(middle);
                proj.setY(y);

                bool up = std::min(points[min_idx].y(), points[other_vert_idx].y()) > y;
                bool down = std::max(points[min_idx].y(), points[other_vert_idx].y()) < y;
                if (up && min_idx == 3 || down && min_idx == 2)
                {
                    proj = points[min_idx];
                }
                else if (up && min_idx == 2 || down && min_idx == 3)
                {
                    proj = points[other_vert_idx];
                }
            }

            // calculate distance from projection
            int distance = (p - proj).manhattanLength();

            if (distance <= WIRE_REMOVE_DISTANCE_THRESHOLD)
            {
                emit proximityConfirmed(this, distance);
            }
        }

        void Wire::changeMode(TOOL tool)
        {
            if (_current_tool == TOOL::SIMULATE && tool != TOOL::SIMULATE)
            {
                uncolor();
            }

            switch (tool)
            {
            case WIRE_REMOVE:
                if (!_updated_markings)
                {
                    QPixmap hup = resources::getWireMarking(resources::LINE_TYPE::HORIZONTAL, _hwire1->width());
                    QPixmap hdown = resources::getWireMarking(resources::LINE_TYPE::HORIZONTAL, _hwire1->width(), false);
                    QPixmap &hup_ext = hup, &hdown_ext = hdown;
                    int ext_size = 0;
                    int move_size = 0;

                    bool dir = _hwire1->y() < _hwire2->y();
                    bool eq = _hwire1->y() == _hwire2->y();

                    if (!eq)
                    {
                        ext_size = resources::WIRE_MARKING_THICKNESS + resources::WIRE_THICKNESS;
                        move_size = resources::WIRE_MARKING_THICKNESS;
                        hup_ext = resources::getWireMarking(resources::LINE_TYPE::HORIZONTAL, _hwire1->width() + ext_size);
                        hdown_ext = resources::getWireMarking(resources::LINE_TYPE::HORIZONTAL, _hwire1->width() + ext_size, false);
                    }

                    _hwire1_up->setPixmap(dir ? hup_ext : hup);
                    _hwire1_up->resize(_hwire1->width() + (dir ? ext_size : 0), resources::WIRE_MARKING_THICKNESS);
                    _hwire1_up->move(_hwire1->x(), _hwire1->y() - resources::WIRE_MARKING_THICKNESS);
                    _hwire1_down->setPixmap(!dir ? hdown_ext : hdown);
                    _hwire1_down->resize(_hwire1->width() + (!dir ? ext_size : 0), resources::WIRE_MARKING_THICKNESS);
                    _hwire1_down->move(_hwire1->x(), _hwire1->y() + resources::WIRE_THICKNESS);
                    _vwire_left->setPixmap(resources::getWireMarking(resources::LINE_TYPE::VERTICAL, _vwire->height()));
                    _vwire_left->resize(resources::WIRE_MARKING_THICKNESS, _vwire->height());
                    _vwire_left->move(_vwire->x() - resources::WIRE_MARKING_THICKNESS, _vwire->y());
                    _vwire_right->setPixmap(resources::getWireMarking(resources::LINE_TYPE::VERTICAL, _vwire->height(), false));
                    _vwire_right->resize(resources::WIRE_MARKING_THICKNESS, _vwire->height());
                    _vwire_right->move(_vwire->x() + resources::WIRE_THICKNESS, _vwire->y());
                    _hwire2_up->setPixmap(!dir ? hup_ext : hup);
                    _hwire2_up->resize(_hwire2->width() + (!dir ? ext_size : 0), resources::WIRE_MARKING_THICKNESS);
                    _hwire2_up->move(_hwire2->x() - (!dir ? move_size : 0), _hwire2->y() - resources::WIRE_MARKING_THICKNESS);
                    _hwire2_down->setPixmap(dir ? hdown_ext : hdown);
                    _hwire2_down->resize(_hwire2->width() + (dir ? ext_size : 0), resources::WIRE_MARKING_THICKNESS);
                    _hwire2_down->move(_hwire2->x() - (dir ? move_size : 0), _hwire2->y() + resources::WIRE_THICKNESS);

                    _updated_markings = true;
                }
                break;
            default:
                _updated_markings = false;
                break;
            }

            _current_tool = tool;
        }

        void Wire::markForDeletion()
        {
            _hwire1_up->show();
            _hwire1_down->show();
            _vwire_left->show();
            _vwire_right->show();
            _hwire2_up->show();
            _hwire2_down->show();
        }

        void Wire::unmarkForDeletion()
        {
            _hwire1_up->hide();
            _hwire1_down->hide();
            _vwire_left->hide();
            _vwire_right->hide();
            _hwire2_up->hide();
            _hwire2_down->hide();
        }

        void Wire::evaluate()
        {
            _createColorWire();

            int conn_idx = _conns[0].is_input;
            if (_conns[conn_idx].component->getValue(_conns[conn_idx].idx))
            {
                _hwire1->hide();
                _vwire->hide();
                _hwire2->hide();

                _hwire1_on->show();
                _vwire_on->show();
                _hwire2_on->show();
            }
            else
            {
                uncolor();
            }
        }

        void Wire::_createColorWire()
        {
            if (!_wire_on)
            {
                QPixmap hwire_on = resources::getWire(resources::LINE_TYPE::HORIZONTAL, _hwire1->width(), _scale, true);

                _hwire1_on->setPixmap(hwire_on);
                _hwire1_on->resize(_hwire1->width(), _hwire1->height());
                _hwire1_on->move(_hwire1->x(), _hwire1->y());

                _vwire_on->setPixmap(resources::getWire(resources::LINE_TYPE::VERTICAL, _vwire->height(), _scale, true));
                _vwire_on->resize(_vwire->width(), _vwire->height());
                _vwire_on->move(_vwire->x(), _vwire->y());

                _hwire2_on->setPixmap(hwire_on);
                _hwire2_on->resize(_hwire2->width(), _hwire2->height());
                _hwire2_on->move(_hwire2->x(), _hwire2->y());

                _wire_on = true;
            }
        }

        void Wire::uncolor()
        {
            _hwire1->show();
            _vwire->show();
            _hwire2->show();

            _hwire1_on->hide();
            _vwire_on->hide();
            _hwire2_on->hide();
        }

        void Wire::scaleTransformationApplied(double size_scale)
        {
            _scale = SCALE_SCALING_FACTOR * size_scale;
            reposition();
        }
    }
}
