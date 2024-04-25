#include "gui/design_area.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        DesignArea::DesignArea(QWidget *parent): QWidget(parent)
        {
        }

        void DesignArea::keyPressEvent(QKeyEvent *ev)
        {
            if (ev->key() == Qt::Key_Delete)
            {
                for (const auto & component: _selected_components)
                {
                    _circuit_model.remove_component(*(component->component_model()));
                    delete component;
                }
                _selected_components.clear();
            }
        }

        void DesignArea::mousePressEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
                _unselectAll();
                _select_x = ev->x();
                _select_y = ev->y();
                for (int i = 0; i < 4; ++i)
                {
                    _selection_border[i] = new QLabel(this);
                    _selection_border[i]->show();
                }
                break;

            case WIRE:
                break;

            case INSERT:
            {
                ComponentLabel *label = new ComponentLabel(_insert_component, _insert_resource_idx, this);

                model::component::Component *component_model = label->component_model();
                _circuit_model.add_component(*component_model);

                QObject::connect(label, SIGNAL (selected(ComponentLabel *, bool)), this, SLOT (addSelected(ComponentLabel *, bool)));
                QObject::connect(label, SIGNAL (moved(int, int)), this, SLOT (moveSelectedComponents(int, int)));
                QObject::connect(this, SIGNAL (modeChanged(TOOL)), label, SLOT (changeMode(TOOL)));
                QObject::connect(this, SIGNAL (rangeQuery(int, int, int, int)), label, SLOT (checkRangeQuery(int, int, int, int)));
                QObject::connect(label, SIGNAL (selected_nocheck(ComponentLabel *)), this, SLOT (addSelected_nocheck(ComponentLabel *)));
                QObject::connect(label, SIGNAL (wireSource(ComponentLabel *, int, int)), this, SLOT (getWireSource(ComponentLabel *, int, int)));
                QObject::connect(label, SIGNAL (wireMoved(int, int)), this, SLOT (moveWireDest(int, int)));
                QObject::connect(this, SIGNAL (wireSnap(ComponentLabel *, int, int)), label, SLOT (wireSnap(ComponentLabel *, int, int)));
                QObject::connect(label, SIGNAL (wireSnapFound(ComponentLabel *, int, int)), this, SLOT (getWireSnapPos(ComponentLabel *, int, int)));
                QObject::connect(label, SIGNAL (wireReleased()), this, SLOT (setWireDest()));
                QObject::connect(this, SIGNAL (evaluate()), label, SLOT (evaluate()));
                QObject::connect(this, SIGNAL (resetResource()), label, SLOT (resetResource()));

                label->move(ev->x() - label->width()/2, ev->y() - label->height()/2);
                label->show();

                addSelected(label);
                break;
            }
            default:
                break;
            }
        }

        void DesignArea::mouseReleaseEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
            {
                for (int i = 0; i < 4; ++i)
                {
                    delete _selection_border[i];
                    _selection_border[i] = nullptr;
                }
                int min_x = std::min(_select_x, ev->x()), min_y = std::min(_select_y, ev->y());
                int max_x = std::max(_select_x, ev->x()), max_y = std::max(_select_y, ev->y());
                emit rangeQuery(min_x, min_y, max_x, max_y);
                break;
            }

            case INSERT:
                _unselectAll();
                break;
            default:
                break;
            }
        }

        void DesignArea::mouseMoveEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
            {
                if (_selection_border[0] == nullptr)
                {
                    return;
                }
                int width = std::abs(_select_x - ev->x()) + resources::LINE_THICKNESS, height = std::abs(_select_y - ev->y());
                QPixmap hline = resources::getLine(resources::HORIZONTAL, width);
                QPixmap vline = resources::getLine(resources::VERTICAL, height);

                int min_x = std::min(_select_x, ev->x()), min_y = std::min(_select_y, ev->y());

                _selection_border[0]->resize(resources::LINE_THICKNESS, height);
                _selection_border[0]->setPixmap(vline);
                _selection_border[0]->move(min_x, min_y);

                _selection_border[1]->resize(resources::LINE_THICKNESS, height);
                _selection_border[1]->setPixmap(vline);
                _selection_border[1]->move(std::max(_select_x, ev->x()), min_y);

                _selection_border[2]->resize(width, resources::LINE_THICKNESS);
                _selection_border[2]->setPixmap(hline);
                _selection_border[2]->move(min_x, min_y);

                _selection_border[3]->resize(width, resources::LINE_THICKNESS);
                _selection_border[3]->setPixmap(hline);
                _selection_border[3]->move(min_x, std::max(_select_y, ev->y()));

                break;
            }

            case INSERT:
            {
                int x = ev->x() - _selected_components[0]->width()/2;
                int y = ev->y() - _selected_components[0]->height()/2;
                _selected_components[0]->move(x, y);
                _selected_components[0]->border()->move(x, y);
                break;
            }
            default:
                break;
            }
        }

        void DesignArea::setStatusBar(QStatusBar *status_bar)
        {
            _status_bar = status_bar;
            _ticks_label = status_bar->findChild<QLabel *>("perm-label");
        }

        void DesignArea::setFrequency(unsigned int freq)
        {
            _freq = freq;
            if (_timer != nullptr)
            {
                _timer->setInterval(1000 / freq);
            }
        }

        unsigned int DesignArea::frequency() const
        {
            return _freq;
        }

        void DesignArea::pauseState()
        {
            _ticks_label_text = _ticks_label->isHidden() ? "" : _ticks_label->text();
            if (_selected_tool == TOOL::SIMULATE)
            {
                _state_sim_paused = !_timer->isActive();
                pauseSimulation();
            }

        }

        bool DesignArea::continueState()
        {
            if (!_ticks_label_text.isEmpty())
            {
                _ticks_label->setText(_ticks_label_text);
                _ticks_label->show();
            }
            else
            {
                _ticks_label->hide();
            }
            if (_selected_tool == TOOL::SIMULATE)
            {
                if (!_state_sim_paused)
                {
                    continueSimulation();
                    return true;
                }
            }

            return false;
        }

        void DesignArea::addSelected(ComponentLabel *component, bool ctrl)
        {
            auto comp_iter = std::find(_selected_components.begin(), _selected_components.end(), component);
            if (comp_iter != _selected_components.end())
            {
                if (ctrl)
                {
                    component->setBorder(nullptr);
                    _selected_components.erase(comp_iter);
                }

                return;
            }

            if (!ctrl)
            {
                if (!_selected_components.empty())
                {
                    _unselectAll();
                }
            }

            addSelected_nocheck(component);
        }

        void DesignArea::addSelected_nocheck(ComponentLabel *component)
        {
            _selected_components.push_back(component);

            QLabel *border = new QLabel(this);
            border->setPixmap(resources::getBorder(component->width(), component->height()));
            border->setAttribute(Qt::WA_TransparentForMouseEvents);

            border->move(component->x(), component->y());
            border->show();

            component->setBorder(border);

        }

        void DesignArea::_unselectAll()
        {
            for (const auto &component: _selected_components)
            {
                component->setBorder(nullptr);
            }
            _selected_components.clear();
        }

        void DesignArea::moveSelectedComponents(int dx, int dy)
        {
            for (const auto &component : _selected_components)
            {
                component->move(dx + component->x(), dy + component->y());
                component->border()->move(component->x(), component->y());
                component->moveWires();
            }
        }

        void DesignArea::getWireSource(ComponentLabel *component, int dx, int dy)
        {
            std::get<0>(_wire_snap_closest) = nullptr;
            _wire = new Wire(this);
            bool set = _wire->setComponent1(component, dx, dy);
            if (!set)
            {
                delete _wire;
                _wire = nullptr;
            }
        }

        void DesignArea::moveWireDest(int dx, int dy)
        {
            if (_wire == nullptr)
            {
                return;
            }
            _wire_snap_positions.clear();
            int x = dx + _wire->getComponent1x();
            int y = dy + _wire->getComponent1y();
            emit wireSnap(_wire->component1(), x, y);
            if (_wire_snap_positions.empty())
            {
                std::get<0>(_wire_snap_closest) = nullptr;
                _wire->repositionDest(x, y);
            }
            else
            {
                double distance = std::numeric_limits<double>::max();

                for (const auto & triplet : _wire_snap_positions)
                {
                    int new_distance = std::pow(x - std::get<1>(triplet), 2) + std::pow(y - std::get<2>(triplet), 2);
                    if (distance > new_distance)
                    {
                        distance = new_distance;
                        _wire_snap_closest = triplet;
                    }
                }

                _wire->repositionDest(std::get<1>(_wire_snap_closest), std::get<2>(_wire_snap_closest));
            }
        }

        void DesignArea::getWireSnapPos(ComponentLabel *component, int x, int y)
        {
            _wire_snap_positions.push_back({component, x, y});
        }

        void DesignArea::setWireDest()
        {
            ComponentLabel *dest_component = std::get<0>(_wire_snap_closest);
            if (_wire == nullptr || dest_component == nullptr)
            {
                delete _wire;
                _wire = nullptr;
                return;
            }

            _wire->setComponent2(dest_component, std::get<1>(_wire_snap_closest) - dest_component->x(), std::get<2>(_wire_snap_closest) - dest_component->y());
            if (!_wire->saveInComponents())
            {
                delete _wire;
                _wire = nullptr;
                return;
            }

            std::tuple<ComponentLabel *, int, int> info = _wire->input_component_info();
            // component 1 is input
            if (dest_component != std::get<0>(info))
            {
                model::component::NInputComponent *ninput_component = dynamic_cast<model::component::NInputComponent *>(_wire->component1()->component_model());
                ninput_component->set_input(std::get<1>(info), *(dest_component->component_model()), std::get<2>(info));
            }
            // component 2 is input
            else
            {
                model::component::NInputComponent *ninput_component = dynamic_cast<model::component::NInputComponent *>(dest_component->component_model());
                ninput_component->set_input(std::get<1>(info), *(_wire->component1()->component_model()), std::get<2>(info));
            }
        }

        void DesignArea::executeTick()
        {
            _circuit_model.tick();
            _ticks_label->setText("Ticks: " + QString::number(_circuit_model.total_ticks()));
            emit evaluate();
        }

        TOOL DesignArea::mode() const
        {
            return _selected_tool;
        }

        void DesignArea::setMode(TOOL tool, COMPONENT comp_type, int res_idx)
        {
            _selected_tool = tool;

            switch (tool)
            {
            case TOOL::INSERT:
            {
                _insert_component = comp_type;
                _insert_resource_idx = res_idx;
                break;
            }
            case TOOL::SIMULATE:
                for (const auto &component : _selected_components)
                {
                    component->border()->hide();
                }

                try
                {
                    _circuit_model.check();
                }
                catch (model::component::null_input)
                {
                    // invalid circuit popup
                    std::cout << "Invalid circuit" << std::endl;
                    return;
                }
                _ticks_label->show();
                _ticks_label->setText("Ticks: 0");

                _timer = new QTimer(this);
                QObject::connect(_timer, SIGNAL (timeout()), this, SLOT (executeTick()));
                _timer->start(1000 / _freq);
                break;
            default:
                break;
            }

            emit modeChanged(tool);
        }

        void DesignArea::stopSimulationMode()
        {
            delete _timer;
            _timer = nullptr;
            _circuit_model.reset();
            _ticks_label->hide();

            _selected_tool = TOOL::SELECT;
            emit setMode(_selected_tool);

            for (const auto &component : _selected_components)
            {
                component->border()->show();
            }
        }

        void DesignArea::pauseSimulation()
        {
            _timer->stop();
        }

        void DesignArea::stepSimulation()
        {
            executeTick();
        }

        void DesignArea::continueSimulation()
        {
            _timer->start();
        }

        void DesignArea::resetSimulation()
        {
            _circuit_model.reset();
            _ticks_label->setText("Ticks: 0");
            emit resetResource();
        }

        bool DesignArea::_writeToFile(bool new_file) const
        {
            /*
            std::ofstream file;
            if (_filenames[currentIndex()] != "Untitled*" && !new_file)
            {
                file.open(_filenames[currentIndex()]);
            }
            else
            {
                // create new file
            }

            emit writeComponent(file);

            file.close();

            return !file.fail();*/
        }
    }
}
