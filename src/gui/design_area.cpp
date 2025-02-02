#include "gui/design_area.hpp"
#include "gui/clipboard.hpp"
#include "gui/undo_commands.hpp"

namespace logicsim
{
namespace gui
{
DesignArea::DesignArea(QWidget *parent) : QWidget(parent)
{
    _undo_stack = new QUndoStack(this);
    _clipboard  = new Clipboard(this);
    QSettings settings("notTypecast", "LogicSim");
    _color_wires = settings.value("wire-color").toBool();
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

    case MOVE:
    case SIMULATE:
        _init_move_x = ev->x();
        _init_move_y = ev->y();
        break;

    case WIRE_REMOVE:
    {
        if (_marked_wire == nullptr)
        {
            break;
        }
        DeleteWireCommand *delete_wire =
          new DeleteWireCommand(this, _marked_wire);
        _undo_stack->push(delete_wire);
        emit newUndoActionPerformed(false,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
        break;
    }

    case INSERT:
    {
        InsertComponentCommand *insert_command =
          new InsertComponentCommand(this, ev);
        _undo_stack->push(insert_command);
        emit newUndoActionPerformed(false,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
        addSelected(insert_command->component());
        break;
    }
    default:
        break;
    }
}

void DesignArea::mouseDoubleClickEvent(QMouseEvent *ev)
{
    switch (_selected_tool)
    {
    case MOVE:
    case SIMULATE:
        mousePressEvent(ev);
        break;
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
        int min_x = std::min(_select_x, ev->x()),
            min_y = std::min(_select_y, ev->y());
        int max_x = std::max(_select_x, ev->x()),
            max_y = std::max(_select_y, ev->y());
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
        int width  = std::abs(_select_x - ev->x()) + resources::LINE_THICKNESS,
            height = std::abs(_select_y - ev->y());
        QPixmap hline = resources::getLine(resources::HORIZONTAL, width);
        QPixmap vline = resources::getLine(resources::VERTICAL, height);

        int min_x = std::min(_select_x, ev->x()),
            min_y = std::min(_select_y, ev->y());

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

    case MOVE:
    case SIMULATE:
    {
        double inv_scale_factor =
          std::pow(INV_BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);
        int dx                  = ev->x() - _init_move_x;
        int dy                  = ev->y() - _init_move_y;
        _transformation.tx     += dx;
        _transformation.ty     += dy;
        _transformation.tx_inv -= inv_scale_factor * dx;
        _transformation.ty_inv -= inv_scale_factor * dy;
        emit transformPosition(dx, dy);
        _init_move_x = ev->x();
        _init_move_y = ev->y();
        break;
    }

    case INSERT:
    {
        int x = ev->x() - _selected_components[0]->width() / 2;
        int y = ev->y() - _selected_components[0]->height() / 2;
        _selected_components[0]->move(x, y);
        break;
    }

    case WIRE_REMOVE:
        emit wireProximityCheck(ev->x(), ev->y());
        if (!_proximity_wire_dist.empty())
        {
            Wire *closest_wire = _proximity_wire_dist[0].first;
            int   min_distance = _proximity_wire_dist[0].second;
            if (_proximity_wire_dist.size() > 1)
            {
                for (auto it = _proximity_wire_dist.begin() + 1;
                     it != _proximity_wire_dist.end();
                     ++it)
                {
                    if ((*it).second < min_distance)
                    {
                        closest_wire = (*it).first;
                        min_distance = (*it).second;
                    }
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

void DesignArea::wheelEvent(QWheelEvent *ev)
{
    if (!(ev->modifiers() & Qt::ControlModifier))
    {
        return;
    }

    QPoint cursor_pos = mapFromGlobal(QCursor::pos());

    if (ev->angleDelta().y() < 0)
    {
        zoomOut(cursor_pos.x(), cursor_pos.y());
    }
    else
    {
        zoomIn(cursor_pos.x(), cursor_pos.y());
    }
}

void DesignArea::setStatusBar(QStatusBar *status_bar)
{
    _status_bar  = status_bar;
    _ticks_label = status_bar->findChild<QLabel *>("perm-label");
    _ticks_label->setText("");
    _ticks_label->hide();
}

void DesignArea::setFrequency(unsigned int freq)
{
    ChangeSimulationPropertiesCommand *sim_prop_command =
      new ChangeSimulationPropertiesCommand(this, _freq, freq);
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

void DesignArea::_connectComponent(ComponentLabel *label, bool first_time)
{
    _circuit_model.add_component(*(label->component_model()));
    _components.push_back(label);

    if (first_time)
    {
        // connections that aren't disconnected by _disconnect_component
        connect(this,
                &DesignArea::modeChanged,
                label,
                &ComponentLabel::changeMode);
        connect(this,
                &DesignArea::resetResource,
                label,
                &ComponentLabel::resetResource);
        connect(this,
                &DesignArea::transformPosition,
                label,
                &ComponentLabel::positionTransformationApplied);
        connect(this,
                &DesignArea::transformScale,
                label,
                &ComponentLabel::scaleTransformationApplied);
        connect(this,
                &DesignArea::cancelMove,
                label,
                &ComponentLabel::cancelMove);
    }

    connect(label, &ComponentLabel::selected, this, &DesignArea::addSelected);
    connect(label,
            &ComponentLabel::moved,
            this,
            &DesignArea::moveSelectedComponents);
    connect(label,
            &ComponentLabel::moveFinished,
            this,
            &DesignArea::finishMove);
    connect(this,
            &DesignArea::rangeQuery,
            label,
            &ComponentLabel::checkRangeQuery);
    connect(label,
            &ComponentLabel::selected_nocheck,
            this,
            &DesignArea::addSelected_nocheck);
    connect(label,
            &ComponentLabel::wireSource,
            this,
            &DesignArea::getWireSource);
    connect(label, &ComponentLabel::wireMoved, this, &DesignArea::moveWireDest);
    connect(this, &DesignArea::wireSnap, label, &ComponentLabel::wireSnap);
    connect(label,
            &ComponentLabel::wireSnapFound,
            this,
            &DesignArea::getWireSnapPos);
    connect(label,
            &ComponentLabel::wireReleased,
            this,
            &DesignArea::setWireDest);
    connect(this, &DesignArea::evaluate, label, &ComponentLabel::evaluate);
    connect(this,
            &DesignArea::writeComponent,
            label,
            &ComponentLabel::writeComponent);
    connect(label,
            &ComponentLabel::performPropertyUndoAction,
            this,
            &DesignArea::propertyUndoActionPerformed);
}

void DesignArea::_disconnectComponent(ComponentLabel *label)
{
    _circuit_model.remove_component(*(label->component_model()));
    _components.erase(std::find(_components.begin(), _components.end(), label));

    disconnect(label,
               &ComponentLabel::selected,
               this,
               &DesignArea::addSelected);
    disconnect(label,
               &ComponentLabel::moved,
               this,
               &DesignArea::moveSelectedComponents);
    disconnect(label,
               &ComponentLabel::moveFinished,
               this,
               &DesignArea::finishMove);
    disconnect(this,
               &DesignArea::rangeQuery,
               label,
               &ComponentLabel::checkRangeQuery);
    disconnect(label,
               &ComponentLabel::selected_nocheck,
               this,
               &DesignArea::addSelected_nocheck);
    disconnect(label,
               &ComponentLabel::wireSource,
               this,
               &DesignArea::getWireSource);
    disconnect(label,
               &ComponentLabel::wireMoved,
               this,
               &DesignArea::moveWireDest);
    disconnect(this, &DesignArea::wireSnap, label, &ComponentLabel::wireSnap);
    disconnect(label,
               &ComponentLabel::wireSnapFound,
               this,
               &DesignArea::getWireSnapPos);
    disconnect(label,
               &ComponentLabel::wireReleased,
               this,
               &DesignArea::setWireDest);
    disconnect(this, &DesignArea::evaluate, label, &ComponentLabel::evaluate);
    disconnect(this,
               &DesignArea::writeComponent,
               label,
               &ComponentLabel::writeComponent);
    disconnect(label,
               &ComponentLabel::performPropertyUndoAction,
               this,
               &DesignArea::propertyUndoActionPerformed);
}

void DesignArea::_connectWire(Wire *wire, bool first_time)
{
    if (first_time)
    {
        connect(this, &DesignArea::modeChanged, wire, &Wire::changeMode);
        connect(this,
                &DesignArea::transformScale,
                wire,
                &Wire::scaleTransformationApplied);
    }
    connect(this, &DesignArea::wireProximityCheck, wire, &Wire::checkProximity);
    connect(wire,
            &Wire::proximityConfirmed,
            this,
            &DesignArea::getProximityWireDistance);
    connect(this, &DesignArea::evaluateWire, wire, &Wire::evaluate);
    connect(this, &DesignArea::disableColorWires, wire, &Wire::uncolor);
    connect(this, &DesignArea::resetWireResource, wire, &Wire::updateColorWire);
}

void DesignArea::_disconnectWire(Wire *wire)
{
    disconnect(this,
               &DesignArea::wireProximityCheck,
               wire,
               &Wire::checkProximity);
    disconnect(wire,
               &Wire::proximityConfirmed,
               this,
               &DesignArea::getProximityWireDistance);
    disconnect(this, &DesignArea::evaluateWire, wire, &Wire::evaluate);
    disconnect(this, &DesignArea::disableColorWires, wire, &Wire::uncolor);
    disconnect(this, &DesignArea::resetResource, wire, &Wire::updateColorWire);
}

void DesignArea::addSelected(ComponentLabel *component, bool ctrl)
{
    auto comp_iter = std::find(_selected_components.begin(),
                               _selected_components.end(),
                               component);
    if (comp_iter != _selected_components.end())
    {
        if (ctrl)
        {
            component->hideBorder();
            _selected_components.erase(comp_iter);

            emit cancelMove(component);

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

    component->showBorder();

    emit newSelection(true, !_clipboard->empty());
}

void DesignArea::_unselectAll()
{
    for (const auto &component : _selected_components)
    {
        component->hideBorder();
    }
    _selected_components.clear();
    emit newSelection(false, !_clipboard->empty());
}

void DesignArea::moveSelectedComponents(int dx, int dy)
{
    if (!_moved_components)
    {
        _moved_components    = true;
        _init_comp_positions = std::vector<QPoint>(_selected_components.size());
        for (size_t i = 0; i < _selected_components.size(); ++i)
        {
            _init_comp_positions[i] = _selected_components[i]->pos();
        }
    }
    for (const auto &component : _selected_components)
    {
        component->move(dx + component->x(), dy + component->y());
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

    MoveComponentsCommand *move_command =
      new MoveComponentsCommand(this,
                                _selected_components,
                                _init_comp_positions,
                                final_positions);
    _undo_stack->push(move_command);
    emit newUndoActionPerformed(false,
                                _undo_stack->canUndo(),
                                _undo_stack->canRedo());
}

void DesignArea::getWireSource(ComponentLabel *component, int dx, int dy)
{
    _wire_snap_closest.component = nullptr;
    _wire                        = new Wire(getScale(), this);
    bool set                     = _wire->setComponent1(component, dx, dy);
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
    int  x = dx + _wire->getComponent1x();
    int  y = dy + _wire->getComponent1y();
    emit wireSnap(_wire->component1(), x, y);
    if (_wire_snap_positions.empty())
    {
        _wire_snap_closest.component = nullptr;
        _wire->repositionDest(x, y);
    }
    else
    {
        double distance = std::numeric_limits<double>::max();

        for (const auto &wire_snap_position : _wire_snap_positions)
        {
            int new_distance = std::pow(x - wire_snap_position.x, 2) +
                               std::pow(y - wire_snap_position.y, 2);
            if (distance > new_distance)
            {
                distance           = new_distance;
                _wire_snap_closest = wire_snap_position;
            }
        }

        _wire->repositionDest(_wire_snap_closest.x, _wire_snap_closest.y);
    }
}

void DesignArea::getWireSnapPos(ComponentLabel *component, int x, int y)
{
    _wire_snap_positions.push_back({ component, x, y });
}

void DesignArea::setWireDest()
{
    ComponentLabel *dest_component = _wire_snap_closest.component;
    if (_wire == nullptr || dest_component == nullptr)
    {
        delete _wire;
        _wire = nullptr;
        return;
    }

    _wire->setComponent2(dest_component,
                         _wire_snap_closest.x - dest_component->x(),
                         _wire_snap_closest.y - dest_component->y());

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
        emit newUndoActionPerformed(false,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
    }
}

void DesignArea::executeTick(unsigned int ticks)
{
    for (unsigned int i = 0; i < ticks; ++i)
    {
        _circuit_model.tick();
    }
    _ticks_label_text =
      "Ticks: " + QString::number(_circuit_model.total_ticks());
    _ticks_label->setText(_ticks_label_text);
    emit evaluate();
    if (_color_wires)
    {
        emit evaluateWire();
    }
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
        _insert_component    = comp_type;
        _insert_resource_idx = res_idx;
        break;
    }
    case TOOL::WIRE_REMOVE:
        setMouseTracking(true);
        break;
    case TOOL::SIMULATE:
        for (const auto &component : _selected_components)
        {
            component->hideBorder();
        }

        if (_timer == nullptr)
        {
            _ticks_label_text = "Ticks: 0";
            _timer            = new QTimer(this);
            connect(_timer,
                    &QTimer::timeout,
                    this,
                    [this]()
                    {
                        executeTick();
                    }); // need lambda because of executeTick's default argument
            _timer->start(1000 / _freq);
        }
        _ticks_label->show();
        _ticks_label->setText(_ticks_label_text);
        break;
    default:
        break;
    }

    if (_selected_tool == TOOL::WIRE_REMOVE && tool != TOOL::WIRE_REMOVE)
    {
        setMouseTracking(false);
        if (_marked_wire != nullptr)
        {
            _marked_wire = nullptr;
        }
    }

    _selected_tool = tool;
    emit modeChanged(tool);

    // emitted after modeChanged, so components correctly save base resource
    // index
    if (_selected_tool == TOOL::SIMULATE)
    {
        emit resetResource();

        if (_color_wires)
        {
            emit resetWireResource();
        }
    }

    return true;
}

void DesignArea::stopSimulationMode()
{
    delete _timer;
    _timer = nullptr;
    _circuit_model.reset();
    _ticks_label->setText("");
    _ticks_label->hide();

    setMode(TOOL::SELECT);

    for (const auto &component : _selected_components)
    {
        component->showBorder();
    }
}

void DesignArea::pauseSimulation()
{
    _timer->stop();
}

void DesignArea::stepSimulation()
{
    executeTick(_freq);
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
    if (_color_wires)
    {
        emit resetWireResource();
    }
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
        QString filepath =
          QFileDialog::getSaveFileName(this,
                                       "Save File",
                                       "../LogicSim/saves/untitled.lsc",
                                       "LogicSim Circuit files (*.lsc)");
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

    emit writeComponent(
      file,
      std::pow(INV_BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL),
      _transformation.tx_inv,
      _transformation.ty_inv);

    file.close();

    return !file.fail();
}

void DesignArea::readFromFile(QString filepath)
{
    // TODO: defensive programming for changing file values
    std::ifstream file(filepath.toStdString());

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
    std::vector<std::pair<std::string, std::string>>  component_inputs;

    logicsim::utils::StringSplitter splitter;

    for (size_t i = 1; getline(file, line); ++i)
    {
        splitter.reset(line, ';');

        std::string id_str = splitter.next();
        if (id_str.empty())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " empty");
        }

        std::string ctype;
        if (!splitter.has_next() || (ctype = splitter.next()).empty())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) +
                                        " missing component type");
        }

        std::string params;
        if (!splitter.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) +
                                        " missing parameters");
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

        ComponentLabel *component =
          new ComponentLabel(ctype_to_comp_type.at(ctype),
                             res_idx,
                             getScale(),
                             _undo_stack,
                             this);
        _connectComponent(component, true);
        components[id_str] = component;

        component->setParams(QString::fromStdString(params));

        if (!splitter.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) +
                                        " missing coordinates");
        }

        logicsim::utils::StringSplitter splitter2(splitter.next(), ',');

        int x, y;
        try
        {
            x = std::stoi(splitter2.next());
        }
        catch (const std::invalid_argument &)
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " bad coordinate");
        }

        if (!splitter2.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) +
                                        " missing coordinate 2");
        }

        try
        {
            y = std::stoi(splitter2.next());
        }
        catch (const std::invalid_argument &)
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " bad coordinate");
        }

        component->move(x, y);
        component->show();

        if (splitter2.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " too many fields");
        }

        if (!splitter.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " missing inputs");
        }

        component_inputs.emplace_back(id_str, splitter.next());

        if (splitter.has_next())
        {
            _deleteComponents(components);
            throw std::invalid_argument("Invalid file format: line " +
                                        std::to_string(i) + " too many fields");
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
                _deleteComponents(components);
                throw std::invalid_argument(
                  "Invalid file format: missing inputs for component " +
                  input.first);
            }
            continue;
        }

        splitter.reset(input.second, ',');

        size_t i;
        for (i = 0; splitter.has_next(); ++i)
        {
            if (i >= components[input.first]->component_model()->n_inputs())
            {
                _deleteComponents(components);
                throw std::invalid_argument(
                  "Invalid file format: too many inputs for component " +
                  input.first);
            }
            std::string input_str = splitter.next();
            if (input_str == "NULL")
            {
                continue;
            }
            splitter2.reset(input_str, ':');
            std::string input_id_str = splitter2.next();
            if (input_id_str.empty() ||
                components.find(input_id_str) == components.end() ||
                !splitter2.has_next())
            {
                _deleteComponents(components);
                throw std::invalid_argument(
                  "Invalid file format: invalid input for component " +
                  input.first);
            }
            unsigned int output_idx;
            try
            {
                output_idx = std::stoi(splitter2.next());
                if (output_idx >=
                    components[input_id_str]->component_model()->n_outputs())
                {
                    throw std::invalid_argument("");
                }
            }
            catch (const std::invalid_argument &e)
            {
                _deleteComponents(components);
                throw std::invalid_argument(
                  "Invalid file format: invalid output index for component " +
                  input.first);
            }
            if (splitter2.has_next())
            {
                _deleteComponents(components);
                throw std::invalid_argument(
                  "Invalid file format: too many fields for input of " +
                  input.first);
            }

            Wire *wire = new Wire(getScale(), this);
            _connectWire(wire);
            wire->setComponent1(components[input.first], true, i);
            wire->setComponent2(components[input_id_str], false, output_idx);
            wire->saveInComponents();
            wire->reposition();

            model::component::NInputComponent *n_input_component =
              static_cast<model::component::NInputComponent *>(
                components[input.first]->component_model());
            n_input_component->set_input(
              i,
              *(components[input_id_str]->component_model()),
              output_idx);
        }

        if (i < components[input.first]->component_model()->n_inputs())
        {
            _deleteComponents(components);
            throw std::invalid_argument(
              "Invalid file format: missing inputs for component " +
              input.first);
        }
    }

    setMode(_selected_tool);
    _filepath = filepath;
}

void DesignArea::_deleteComponents(
  std::unordered_map<std::string, ComponentLabel *> components)
{
    for (const auto &pair : components)
    {
        _circuit_model.remove_component(*(pair.second->component_model()));
        delete pair.second;
    }
}

void DesignArea::undoAction()
{
    if (_selected_tool != TOOL::SIMULATE && _undo_stack->canUndo())
    {
        _undo_stack->undo();
        emit newUndoActionPerformed(true,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
    }
}

void DesignArea::redoAction()
{
    if (_selected_tool != TOOL::SIMULATE && _undo_stack->canRedo())
    {
        _undo_stack->redo();
        emit newUndoActionPerformed(false,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
    }
}

void DesignArea::propertyUndoActionPerformed()
{
    emit newUndoActionPerformed(false, true, false);
}

void DesignArea::getProximityWireDistance(Wire *wire, int distance)
{
    _proximity_wire_dist.push_back({ wire, distance });
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
        DeleteComponentsCommand *delete_command =
          new DeleteComponentsCommand(this, _selected_components);
        _undo_stack->push(delete_command);
        _selected_components.clear();
        emit newUndoActionPerformed(false,
                                    _undo_stack->canUndo(),
                                    _undo_stack->canRedo());
        emit newSelection(false, !_clipboard->empty());
        break;
    }
    default:
        break;
    }
}

void DesignArea::setColorWires(bool enabled)
{
    _color_wires = enabled;
    if (!_color_wires)
    {
        emit disableColorWires();
        return;
    }

    emit resetWireResource();
}

void DesignArea::zoomIn(int origin_x, int origin_y)
{
    if (_zoom_level < 15)
    {
        _zoom(origin_x, origin_y, _zoom_level + 1);
    }
}

void DesignArea::zoomOut(int origin_x, int origin_y)
{
    if (_zoom_level > 2)
    {
        _zoom(origin_x, origin_y, _zoom_level - 1);
    }
}

void DesignArea::resetZoom()
{
    _zoom(width() / 2, height() / 2, BASE_ZOOM_LEVEL);
}

void DesignArea::_zoom(int origin_x, int origin_y, int new_zoom_level)
{
    double size_scale_factor =
      std::pow(BASE_SCALE_FACTOR, new_zoom_level - BASE_ZOOM_LEVEL);
    double scale_factor =
      std::pow(BASE_SCALE_FACTOR, new_zoom_level - _zoom_level);
    double prev_inv_scale_factor =
      std::pow(INV_BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);

    _transformation.tx =
      scale_factor * _transformation.tx - scale_factor * origin_x + origin_x;
    _transformation.ty =
      scale_factor * _transformation.ty - scale_factor * origin_y + origin_y;

    _transformation.tx_inv +=
      prev_inv_scale_factor * (origin_x - origin_x / scale_factor);
    _transformation.ty_inv +=
      prev_inv_scale_factor * (origin_y - origin_y / scale_factor);

    emit transformScale(size_scale_factor,
                        scale_factor,
                        -scale_factor * origin_x + origin_x,
                        -scale_factor * origin_y + origin_y);

    _zoom_level = new_zoom_level;
}

double DesignArea::getScale()
{
    return std::pow(BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);
}

void DesignArea::clearUndoStack()
{
    _undo_stack->clear();
}

bool DesignArea::isDrawingWire()
{
    return _wire != nullptr;
}

void DesignArea::nextComponent()
{
    if (_components.empty())
    {
        return;
    }
    _component_idx = (_component_idx + 1) % _components.size();
    _moveToComponent();
}

void DesignArea::previousComponent()
{
    if (_components.empty())
    {
        return;
    }
    _component_idx =
      _component_idx - 1 < 0 ? _components.size() - 1 : _component_idx - 1;
    _moveToComponent();
}

void DesignArea::_moveToComponent()
{
    // native component coordinates
    int native_x, native_y;
    std::tie(native_x, native_y) =
      _toNativeCoordinates(_components[_component_idx]->x(),
                           _components[_component_idx]->y());

    // new view position of component
    int new_view_x = width() / 3;
    int new_view_y = height() / 3;

    // amount to move from current position to get to new position (offset)
    int offset_x = new_view_x - _components[_component_idx]->x();
    int offset_y = new_view_y - _components[_component_idx]->y();

    double scale_factor =
      std::pow(BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);
    double inv_scale_factor = 1 / scale_factor;

    // since view moves to entirely different position, overwrite these with new
    // position data
    _transformation.tx = new_view_x - scale_factor * native_x;
    _transformation.ty = new_view_y - scale_factor * native_y;

    _transformation.tx_inv = native_x - inv_scale_factor * new_view_x;
    _transformation.ty_inv = native_y - inv_scale_factor * new_view_y;

    emit transformPosition(offset_x, offset_y);
}

std::pair<int, int> DesignArea::_toNativeCoordinates(int x, int y)
{
    double inv_scale_factor =
      std::pow(INV_BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);

    return { std::round(inv_scale_factor * x + _transformation.tx_inv),
             std::round(inv_scale_factor * y + _transformation.ty_inv) };
}

std::pair<int, int> DesignArea::_toViewCoordinates(int x, int y)
{
    double scale_factor =
      std::pow(BASE_SCALE_FACTOR, _zoom_level - BASE_ZOOM_LEVEL);
    return { std::round(scale_factor * x + _transformation.tx),
             std::round(scale_factor * y + _transformation.ty) };
}

} // namespace gui
} // namespace logicsim
