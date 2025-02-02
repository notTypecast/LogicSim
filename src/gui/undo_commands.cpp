#include "gui/undo_commands.hpp"
#include "gui/clipboard.hpp"

namespace logicsim
{
namespace gui
{
// InsertComponentCommand
InsertComponentCommand::InsertComponentCommand(DesignArea  *design_area,
                                               QMouseEvent *ev)
  : _design_area(design_area)
  , _component(new ComponentLabel(
      _design_area->_insert_component, _design_area->_insert_resource_idx,
      _design_area->getScale(), _design_area->_undo_stack, _design_area))
{
    _component->move(ev->x() - _component->width() / 2,
                     ev->y() - _component->height() / 2);
}

InsertComponentCommand::InsertComponentCommand(DesignArea     *design_area,
                                               ComponentLabel *component_label)
  : _design_area(design_area)
  , _component(component_label)
{
}

InsertComponentCommand::~InsertComponentCommand()
{
    delete _component;
}

void InsertComponentCommand::redo()
{
    _design_area->_connectComponent(_component, _first_time);
    if (_first_time)
    {
        _first_time = false;
    }
    _component->show();
}

void InsertComponentCommand::undo()
{
    _design_area->_disconnectComponent(_component);
    _component->hide();
    // removing from circuit is not needed, since component cannot have wires

    auto comp_iter = std::find(_design_area->_selected_components.begin(),
                               _design_area->_selected_components.end(),
                               _component);
    if (comp_iter != _design_area->_selected_components.end())
    {
        _component->showBorder();
        _design_area->_selected_components.erase(comp_iter);
        emit _design_area->newSelection(
          !_design_area->_selected_components.empty(),
          !_design_area->_clipboard->empty());
    }
}

ComponentLabel *InsertComponentCommand::component() const
{
    return _component;
}

// DeleteComponentsCommand
DeleteComponentsCommand::DeleteComponentsCommand(
  DesignArea *design_area, std::vector<ComponentLabel *> &deleted_components)
  : _design_area(design_area)
  , _deleted_components(deleted_components)
{
}

void DeleteComponentsCommand::redo()
{
    for (const auto &component : _deleted_components)
    {
        _design_area->_disconnectComponent(component);
        component->hide();
        component->removeFromCircuit();

        for (const auto &wire : component->inputWires())
        {
            if (wire != nullptr)
            {
                _design_area->_disconnectWire(wire);
            }
        }

        for (const auto &output : component->outputWires())
        {
            for (const auto &wire : output)
            {
                _design_area->_disconnectWire(wire);
            }
        }
    }
}

void DeleteComponentsCommand::undo()
{
    for (const auto &component : _deleted_components)
    {
        _design_area->_connectComponent(component);
        component->show();
        component->bringBackToCircuit();

        for (const auto &wire : component->inputWires())
        {
            if (wire != nullptr)
            {
                _design_area->_connectWire(wire);
            }
        }

        for (const auto &output : component->outputWires())
        {
            for (const auto &wire : output)
            {
                _design_area->_connectWire(wire);
            }
        }
    }
}

// InsertWireCommand
InsertWireCommand::InsertWireCommand(DesignArea *design_area, Wire *wire)
  : _design_area(design_area)
  , _wire(wire)
{
}

InsertWireCommand::~InsertWireCommand()
{
    if (_disconnected)
    {
        delete _wire;
    }
}

void InsertWireCommand::redo()
{
    _conn_success = _wire->saveInComponents();
    _wire->show();
    _disconnected = false;
    _design_area->_connectWire(_wire, _first_time);
    if (_first_time)
    {
        _first_time = false;
    }
}

void InsertWireCommand::undo()
{
    _wire->removeFromComponents();
    _wire->hide();
    _disconnected = true;
    _design_area->_disconnectWire(_wire);
}

bool InsertWireCommand::connected() const
{
    return _conn_success;
}

// DeleteWireCommand
DeleteWireCommand::DeleteWireCommand(DesignArea *design_area, Wire *wire)
  : _design_area(design_area)
  , _wire(wire)
{
}

void DeleteWireCommand::redo()
{
    _wire->removeFromComponents();
    _wire->hide();
    _design_area->_disconnectWire(_wire);
}

void DeleteWireCommand::undo()
{
    _wire->saveInComponents();
    _wire->show();
    _design_area->_connectWire(_wire);
}

// MoveComponentsCommand
MoveComponentsCommand::MoveComponentsCommand(
  DesignArea *design_area, std::vector<ComponentLabel *> &moved_components,
  std::vector<QPoint> init_positions, std::vector<QPoint> final_positions)
  : _design_area(design_area)
  , _moved_components(moved_components)
  , _init_positions(std::vector<QPoint>(init_positions.size()))
  , _final_positions(std::vector<QPoint>(final_positions.size()))
{
    // transform to native coordinates
    std::pair<int, int> coords;
    for (size_t i = 0; i < moved_components.size(); ++i)
    {
        // TODO: possibly keep position as double
        coords = _design_area->_toNativeCoordinates(init_positions[i].x(),
                                                    init_positions[i].y());
        _init_positions[i].setX(coords.first);
        _init_positions[i].setY(coords.second);

        coords = _design_area->_toNativeCoordinates(final_positions[i].x(),
                                                    final_positions[i].y());
        _final_positions[i].setX(coords.first);
        _final_positions[i].setY(coords.second);
    }
}

void MoveComponentsCommand::redo()
{
    std::pair<int, int> coords;
    for (size_t i = 0; i < _moved_components.size(); ++i)
    {
        coords = _design_area->_toViewCoordinates(_final_positions[i].x(),
                                                  _final_positions[i].y());
        _moved_components[i]->move(coords.first, coords.second);
    }
}

void MoveComponentsCommand::undo()
{
    std::pair<int, int> coords;
    for (size_t i = 0; i < _moved_components.size(); ++i)
    {
        coords = _design_area->_toViewCoordinates(_init_positions[i].x(),
                                                  _init_positions[i].y());
        _moved_components[i]->move(coords.first, coords.second);
    }
}

// ChangeComponentPropertyCommand
ChangeComponentPropertyCommand::ChangeComponentPropertyCommand(
  ComponentLabel *component, int prev_res_idx, int res_idx)
  : _component(component)
  , _prev_res_idx(prev_res_idx)
  , _res_idx(res_idx)
{
}

void ChangeComponentPropertyCommand::setParams(const QString &prev_params,
                                               const QString &params)
{
    _prev_params = prev_params;
    _params      = params;
}

void ChangeComponentPropertyCommand::redo()
{
    if (_res_idx != -1)
    {
        _component->setResourceByIdx(_res_idx);
    }
    _component->_component_model->set_params(_params.toStdString());
}

void ChangeComponentPropertyCommand::undo()
{
    if (_prev_res_idx != -1)
    {
        _component->setResourceByIdx(_prev_res_idx);
    }

    _component->_component_model->set_params(_prev_params.toStdString());
}

// ChangeSimulationPropertiesCommand
ChangeSimulationPropertiesCommand::ChangeSimulationPropertiesCommand(
  DesignArea *design_area, int prev_freq, int freq)
  : _design_area(design_area)
  , _prev_freq(prev_freq)
  , _freq(freq)
{
}

void ChangeSimulationPropertiesCommand::redo()
{
    _design_area->_freq = _freq;
    if (_design_area->_timer != nullptr)
    {
        _design_area->_timer->setInterval(1000 / _freq);
    }
}

void ChangeSimulationPropertiesCommand::undo()
{
    _design_area->_freq = _prev_freq;
    if (_design_area->_timer != nullptr)
    {
        _design_area->_timer->setInterval(1000 / _prev_freq);
    }
}
}
}
