#include "gui/design_area.hpp"
#include "gui/undo_commands.hpp"
#include "gui/clipboard.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        DesignArea::DesignArea(QWidget *parent): QWidget(parent)
        {
            _undo_stack = new QUndoStack(this);
            _clipboard = new Clipboard(this);
        }

        DesignArea::~DesignArea()
        {
            delete _undo_stack;
            delete _clipboard;
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

            case WIRE_REMOVE:
            {
                if (_marked_wire == nullptr)
                {
                    break;
                }
                DeleteWireCommand *delete_wire = new DeleteWireCommand(this, _marked_wire);
                _undo_stack->push(delete_wire);
                emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
                break;
            }

            case INSERT:
            {
                InsertComponentCommand *insert_command = new InsertComponentCommand(this, ev);
                _undo_stack->push(insert_command);
                emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
                addSelected(insert_command->component());
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

            case WIRE_REMOVE:
                emit wireProximityCheck(ev->x(), ev->y());
                if (!_proximity_wire_dist.empty())
                {
                    Wire *closest_wire;
                    int min_distance = std::numeric_limits<int>::max();
                    for (const auto &pair : _proximity_wire_dist)
                    {
                        if (pair.second < min_distance)
                        {
                            closest_wire = pair.first;
                            min_distance = pair.second;
                        }
                    }
                    _proximity_wire_dist.clear();

                    if (closest_wire != _marked_wire)
                    {
                        if (_marked_wire != nullptr)
                        {
                            _marked_wire->unmarkForDeletion();
                        }
                        _marked_wire = closest_wire;
                        closest_wire->markForDeletion();
                    }
                }
                else
                {
                    if (_marked_wire != nullptr)
                    {
                        _marked_wire->unmarkForDeletion();
                        _marked_wire = nullptr;
                    }
                }

                break;

            default:
                break;
            }
        }

        void DesignArea::setStatusBar(QStatusBar *status_bar)
        {
            _status_bar = status_bar;
            _ticks_label = status_bar->findChild<QLabel *>("perm-label");
            _ticks_label->setText("");
            _ticks_label->hide();
        }

        void DesignArea::setFrequency(unsigned int freq)
        {
            ChangeSimulationPropertiesCommand *sim_prop_command = new ChangeSimulationPropertiesCommand(this, _freq, freq);
            _undo_stack->push(sim_prop_command);
            emit newUndoActionPerformed(false, true, false);
        }

        unsigned int DesignArea::frequency() const
        {
            return _freq;
        }

        void DesignArea::pauseState()
        {
            if (_selected_tool == TOOL::SIMULATE)
            {
                _state_sim_paused = !_timer->isActive();
                pauseSimulation();
            }

            _paused_state = true;
        }

        bool DesignArea::continueState()
        {
            if (!_paused_state)
            {
                return false;
            }
            _paused_state = false;

            if (!_ticks_label_text.isEmpty())
            {
                _ticks_label->setText(_ticks_label_text);
                _ticks_label->show();
            }
            else
            {
                _ticks_label->setText("");
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

        void DesignArea::_connect_component(ComponentLabel *label, bool first_time)
        {
            _circuit_model.add_component(*(label->component_model()));

            if (first_time)
            {
                // connections that aren't disconnected by _disconnect_component
                QObject::connect(this, SIGNAL (modeChanged(TOOL)), label, SLOT (changeMode(TOOL)));
                QObject::connect(this, SIGNAL (resetResource()), label, SLOT (resetResource()));
            }

            QObject::connect(label, SIGNAL (selected(ComponentLabel *, bool)), this, SLOT (addSelected(ComponentLabel *, bool)));
            QObject::connect(label, SIGNAL (moved(int, int)), this, SLOT (moveSelectedComponents(int, int)));
            QObject::connect(label, SIGNAL (moveFinished()), this, SLOT (finishMove()));
            QObject::connect(this, SIGNAL (rangeQuery(int, int, int, int)), label, SLOT (checkRangeQuery(int, int, int, int)));
            QObject::connect(label, SIGNAL (selected_nocheck(ComponentLabel *)), this, SLOT (addSelected_nocheck(ComponentLabel *)));
            QObject::connect(label, SIGNAL (wireSource(ComponentLabel *, int, int)), this, SLOT (getWireSource(ComponentLabel *, int, int)));
            QObject::connect(label, SIGNAL (wireMoved(int, int)), this, SLOT (moveWireDest(int, int)));
            QObject::connect(this, SIGNAL (wireSnap(ComponentLabel *, int, int)), label, SLOT (wireSnap(ComponentLabel *, int, int)));
            QObject::connect(label, SIGNAL (wireSnapFound(ComponentLabel *, int, int)), this, SLOT (getWireSnapPos(ComponentLabel *, int, int)));
            QObject::connect(label, SIGNAL (wireReleased()), this, SLOT (setWireDest()));
            QObject::connect(this, SIGNAL (evaluate()), label, SLOT (evaluate()));
            QObject::connect(this, SIGNAL (writeComponent(std::ofstream &)), label, SLOT (writeComponent(std::ofstream &)));
            QObject::connect(label, SIGNAL (performPropertyUndoAction()), this, SLOT (propertyUndoActionPerformed()));
        }

        void DesignArea::_disconnect_component(ComponentLabel *label)
        {
            _circuit_model.remove_component(*(label->component_model()));

            // don't disconnect, so component stays up to date on mode
            // QObject::disconnect(this, SIGNAL (modeChanged(TOOL)), label, SLOT (changeMode(TOOL)));
            // QObject::disconnect(this, SIGNAL (resetResource()), label, SLOT (resetResource()));

            QObject::disconnect(label, SIGNAL (selected(ComponentLabel *, bool)), this, SLOT (addSelected(ComponentLabel *, bool)));
            QObject::disconnect(label, SIGNAL (moved(int, int)), this, SLOT (moveSelectedComponents(int, int)));
            QObject::disconnect(label, SIGNAL (moveFinished()), this, SLOT (finishMove()));
            QObject::disconnect(this, SIGNAL (rangeQuery(int, int, int, int)), label, SLOT (checkRangeQuery(int, int, int, int)));
            QObject::disconnect(label, SIGNAL (selected_nocheck(ComponentLabel *)), this, SLOT (addSelected_nocheck(ComponentLabel *)));
            QObject::disconnect(label, SIGNAL (wireSource(ComponentLabel *, int, int)), this, SLOT (getWireSource(ComponentLabel *, int, int)));
            QObject::disconnect(label, SIGNAL (wireMoved(int, int)), this, SLOT (moveWireDest(int, int)));
            QObject::disconnect(this, SIGNAL (wireSnap(ComponentLabel *, int, int)), label, SLOT (wireSnap(ComponentLabel *, int, int)));
            QObject::disconnect(label, SIGNAL (wireSnapFound(ComponentLabel *, int, int)), this, SLOT (getWireSnapPos(ComponentLabel *, int, int)));
            QObject::disconnect(label, SIGNAL (wireReleased()), this, SLOT (setWireDest()));
            QObject::disconnect(this, SIGNAL (evaluate()), label, SLOT (evaluate()));
            QObject::disconnect(this, SIGNAL (writeComponent(std::ofstream &)), label, SLOT (writeComponent(std::ofstream &)));
            QObject::disconnect(label, SIGNAL (performPropertyUndoAction()), this, SLOT (propertyUndoActionPerformed()));
        }

        void DesignArea::_connect_wire(Wire *wire, bool first_time)
        {
            if (first_time)
            {
                QObject::connect(this, SIGNAL (modeChanged(TOOL)), wire, SLOT (changeMode(TOOL)));
            }
            QObject::connect(this, SIGNAL (wireProximityCheck(int, int)), wire, SLOT (checkProximity(int, int)));
            QObject::connect(wire, SIGNAL (proximityConfirmed(Wire *, int)), this, SLOT (getProximityWireDistance(Wire *, int)));
        }

        void DesignArea::_disconnect_wire(Wire *wire)
        {
            QObject::disconnect(this, SIGNAL (wireProximityCheck(int, int)), wire, SLOT (checkProximity(int, int)));
            QObject::disconnect(wire, SIGNAL (proximityConfirmed(Wire *, int)), this, SLOT (getProximityWireDistance(Wire *, int)));
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

                    if (_selected_components.empty())
                    {
                        emit newSelection(false, !_clipboard->empty());
                    }
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

            emit newSelection(true, !_clipboard->empty());
        }

        void DesignArea::_unselectAll()
        {
            for (const auto &component: _selected_components)
            {
                component->setBorder(nullptr);
            }
            _selected_components.clear();
            emit newSelection(false, !_clipboard->empty());
        }

        void DesignArea::moveSelectedComponents(int dx, int dy)
        {
            if (!_moved_components)
            {
                _moved_components = true;
                _init_comp_positions = std::vector<QPoint>(_selected_components.size());
                for (size_t i = 0; i < _selected_components.size(); ++i)
                {
                    _init_comp_positions[i] = _selected_components[i]->pos();
                }
            }
            for (const auto &component : _selected_components)
            {
                component->move(dx + component->x(), dy + component->y());
                component->border()->move(component->x(), component->y());
                component->moveWires();
            }
        }

        void DesignArea::finishMove()
        {
            if (!_moved_components)
            {
                return;
            }
            _moved_components = false;
            std::vector<QPoint> final_positions(_selected_components.size());

            for (size_t i = 0; i < _selected_components.size(); ++i)
            {
                final_positions[i] = _selected_components[i]->pos();
            }

            MoveComponentsCommand *move_command = new MoveComponentsCommand(_selected_components, _init_comp_positions, final_positions);
            _undo_stack->push(move_command);
            emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
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

            InsertWireCommand *wire_command = new InsertWireCommand(this, _wire);
            _undo_stack->push(wire_command);

            if (!wire_command->connected())
            {
                _wire = nullptr;
                _undo_stack->undo();
                wire_command->setObsolete(true);
            }
            else
            {
                emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
            }
        }

        void DesignArea::executeTick()
        {
            _circuit_model.tick();
            _ticks_label_text = "Ticks: " + QString::number(_circuit_model.total_ticks());
            _ticks_label->setText(_ticks_label_text);
            emit evaluate();
        }

        TOOL DesignArea::mode() const
        {
            return _selected_tool;
        }

        bool DesignArea::setMode(TOOL tool, COMPONENT comp_type, int res_idx)
        {
            switch (tool)
            {
            case TOOL::INSERT:
            {
                _insert_component = comp_type;
                _insert_resource_idx = res_idx;
                break;
            }
            case TOOL::WIRE_REMOVE:
                setMouseTracking(true);
                break;
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
                    return false;
                }
                _ticks_label->show();
                _ticks_label_text = "Ticks: 0";
                _ticks_label->setText(_ticks_label_text);

                _timer = new QTimer(this);
                QObject::connect(_timer, SIGNAL (timeout()), this, SLOT (executeTick()));
                _timer->start(1000 / _freq);
                break;
            default:
                break;
            }

            if (tool != TOOL::WIRE_REMOVE)
            {
                setMouseTracking(false);
                if (_marked_wire != nullptr)
                {
                    _marked_wire = nullptr;
                }
            }

            _selected_tool = tool;
            emit modeChanged(tool);
            return true;
        }

        void DesignArea::stopSimulationMode()
        {
            delete _timer;
            _timer = nullptr;
            _circuit_model.reset();
            _ticks_label->setText("");
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

        bool DesignArea::empty() const
        {
            return _circuit_model.empty();
        }

        QString DesignArea::filepath() const
        {
            return _filepath;
        }

        bool DesignArea::writeToFile(bool new_file)
        {
            std::ofstream file;

            if (_filepath.isEmpty() || new_file)
            {
                QString filepath = QFileDialog::getSaveFileName(this, "Save File", "../LogicSim/saves/untitled.lsc", "LogicSim Circuit files (*.lsc)");
                if (filepath.isEmpty())
                {
                    throw std::invalid_argument("");
                }
                _filepath = filepath;
                file.open(_filepath.toStdString());
            }
            else
            {
                file.open(_filepath.toStdString());
            }

            file << std::to_string(_freq) << "\n";

            emit writeComponent(file);

            file.close();

            return !file.fail();
        }

        void DesignArea::readFromFile(QString filepath)
        {
            // TODO: defensive programming for changing file values
            _filepath = filepath;
            std::ifstream file(_filepath.toStdString());

            if (file.fail())
            {
                throw std::invalid_argument("File not found");
            }

            std::string line;

            getline(file, line);

            if (!utils::is_positive_int(line))
            {
                throw std::invalid_argument("Invalid file format: invalid frequency");
            }

            _freq = std::stoi(line);

            std::unordered_map<std::string, ComponentLabel *> components;
            std::vector<std::pair<std::string, std::string>> component_inputs;

            logicsim::utils::StringSplitter splitter;

            for (size_t i = 1; getline(file, line); ++i)
            {
                splitter.reset(line, ';');

                std::string id_str = splitter.next();
                if (id_str.empty())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " empty");
                }

                std::string ctype;
                if (!splitter.has_next() || (ctype = splitter.next()).empty())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing component type");
                }

                std::string params;
                if (!splitter.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing parameters");
                }
                params = splitter.next();

                int res_idx;

                if (utils::is_positive_int(params))
                {
                    res_idx = std::stoi(params);
                }
                else
                {
                    res_idx = 0;
                }

                ComponentLabel *component = new ComponentLabel(resources::ctype_to_component_t.at(ctype), res_idx, _undo_stack, this);
                _connect_component(component);
                components[id_str] = component;

                component->setParams(QString::fromStdString(params));

                if (!splitter.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing coordinates");
                }

                logicsim::utils::StringSplitter splitter2(splitter.next(), ',');

                int x, y;
                try {
                    x = std::stoi(splitter2.next());
                }
                catch (std::invalid_argument) {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " bad coordinate");
                }

                if (!splitter2.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing coordinate 2");
                }

                try {
                    y = std::stoi(splitter2.next());
                }
                catch (std::invalid_argument) {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " bad coordinate");
                }

                component->move(x, y);
                component->show();

                if (splitter2.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " too many fields");
                }

                if (!splitter.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing inputs");
                }

                component_inputs.emplace_back(id_str, splitter.next());

                if (splitter.has_next())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " too many fields");
                }
            }

            file.close();

            logicsim::utils::StringSplitter splitter2;

            for (const auto &input : component_inputs)
            {
                if (input.second.empty())
                {
                    if (components[input.first]->component_model()->n_inputs() > 0)
                    {
                        _delete_components(components);
                        throw std::invalid_argument("Invalid file format: missing inputs for component " + input.first);
                    }
                    continue;
                }

                splitter.reset(input.second, ',');

                size_t i;
                for (i = 0; splitter.has_next(); ++i)
                {
                    if (i >= components[input.first]->component_model()->n_inputs())
                    {
                        _delete_components(components);
                        throw std::invalid_argument("Invalid file format: too many inputs for component " + input.first);
                    }
                    std::string input_str = splitter.next();
                    if (input_str == "NULL")
                    {
                        continue;
                    }
                    splitter2.reset(input_str, ':');
                    std::string input_id_str = splitter2.next();
                    if (input_id_str.empty() || components.find(input_id_str) == components.end() || !splitter2.has_next())
                    {
                        _delete_components(components);
                        throw std::invalid_argument("Invalid file format: invalid input for component " + input.first);
                    }
                    unsigned int output_idx;
                    try
                    {
                        output_idx = std::stoi(splitter2.next());
                        if (output_idx >= components[input_id_str]->component_model()->n_outputs())
                        {
                            _delete_components(components);
                            throw std::invalid_argument("");
                        }
                    }
                    catch (const std::invalid_argument &e)
                    {
                        _delete_components(components);
                        throw std::invalid_argument("Invalid file format: invalid output index for component " + input.first);
                    }
                    if (splitter2.has_next())
                    {
                        _delete_components(components);
                        throw std::invalid_argument("Invalid file format: too many fields for input of " + input.first);
                    }

                    Wire *wire = new Wire(this);
                    _connect_wire(wire);
                    wire->setComponent1(components[input.first], true, i);
                    wire->setComponent2(components[input_id_str], false, output_idx);
                    wire->saveInComponents();
                    wire->reposition();

                    model::component::NInputComponent *n_input_component = dynamic_cast<model::component::NInputComponent *>(components[input.first]->component_model());
                    n_input_component->set_input(i, *(components[input_id_str]->component_model()), output_idx);
                }

                if (i < components[input.first]->component_model()->n_inputs())
                {
                    _delete_components(components);
                    throw std::invalid_argument("Invalid file format: missing inputs for component " + input.first);
                }
            }

            setMode(_selected_tool);
        }

        void DesignArea::_delete_components(std::unordered_map<std::string, ComponentLabel *> components)
        {
            for (const auto &pair : components)
            {
                delete pair.second;
            }
        }

        void DesignArea::undoAction()
        {
            if (_selected_tool != TOOL::SIMULATE && _undo_stack->canUndo())
            {
                _undo_stack->undo();
                emit newUndoActionPerformed(true, _undo_stack->canUndo(), _undo_stack->canRedo());
            }
        }

        void DesignArea::redoAction()
        {
            if (_selected_tool != TOOL::SIMULATE && _undo_stack->canRedo())
            {
                _undo_stack->redo();
                emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
            }
        }

        void DesignArea::propertyUndoActionPerformed()
        {
            emit newUndoActionPerformed(false, true, false);
        }

        void DesignArea::getProximityWireDistance(Wire *wire, int distance)
        {
            _proximity_wire_dist.push_back({wire, distance});
        }

        void DesignArea::cutAction()
        {
            switch (_selected_tool)
            {
            case SELECT:
                if (!_selected_components.empty())
                {
                    _clipboard->copy(_selected_components);
                    deleteAction();
                }
                break;
            default:
                break;
            }
        }
        void DesignArea::copyAction()
        {
            switch (_selected_tool)
            {
            case SELECT:
                if (!_selected_components.empty())
                {
                    _clipboard->copy(_selected_components);
                    emit newSelection(true, !_clipboard->empty());
                }
                break;
            default:
                break;
            }
        }
        void DesignArea::pasteAction()
        {
            switch (_selected_tool)
            {
            case SELECT:
                if (!_clipboard->empty())
                {
                    _clipboard->paste();
                    emit setMode(TOOL::SELECT);
                }
                break;
            default:
                break;
            }
        }
        void DesignArea::deleteAction()
        {
            switch (_selected_tool)
            {
            case SELECT:
            {
                DeleteComponentsCommand *delete_command = new DeleteComponentsCommand(this, _selected_components);
                _undo_stack->push(delete_command);
                _selected_components.clear();
                emit newUndoActionPerformed(false, _undo_stack->canUndo(), _undo_stack->canRedo());
                emit newSelection(false, !_clipboard->empty());
                break;
            }
            default:
                break;
            }
        }
    }
}
