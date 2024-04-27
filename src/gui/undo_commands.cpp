#include "gui/undo_commands.hpp"
#include <iostream>
#include <unistd.h>
namespace logicsim
{
    namespace gui
    {
        // InsertComponentCommand
        InsertComponentCommand::InsertComponentCommand(DesignArea *design_area, QMouseEvent *ev) : _design_area(design_area), _component(new ComponentLabel(_design_area->_insert_component, _design_area->_insert_resource_idx, _design_area))
        {
            _component->move(ev->x() - _component->width()/2, ev->y() - _component->height()/2);
        }

        InsertComponentCommand::~InsertComponentCommand()
        {
            delete _component;
        }

        void InsertComponentCommand::redo()
        {
            _design_area->_connect_component(_component, _first_time);
            if (_first_time)
            {
                _first_time = false;
            }
            _component->show();
        }

        void InsertComponentCommand::undo()
        {
            _design_area->_disconnect_component(_component);
            _component->hide();
            // removing from circuit is not needed, since component cannot have wires

            auto comp_iter = std::find(_design_area->_selected_components.begin(),_design_area->_selected_components.end(), _component);
            if (comp_iter != _design_area->_selected_components.end())
            {
                _component->setBorder(nullptr);
                _design_area->_selected_components.erase(comp_iter);
            }
        }

        ComponentLabel *InsertComponentCommand::component() const
        {
            return _component;
        }

        // DeleteComponentsCommand
        DeleteComponentsCommand::DeleteComponentsCommand(DesignArea *design_area, std::vector<ComponentLabel *> &deleted_components) : _design_area(design_area), _deleted_components(deleted_components)
        {
        }

        void DeleteComponentsCommand::redo()
        {
            for (const auto &component: _deleted_components)
            {
                _design_area->_disconnect_component(component);
                component->hide();
                component->removeFromCircuit();
            }
        }

        void DeleteComponentsCommand::undo()
        {
            for (const auto &component: _deleted_components)
            {
                _design_area->_connect_component(component);
                component->show();
                component->bringBackToCircuit();
            }
        }

        // InsertWireCommand
        InsertWireCommand::InsertWireCommand(Wire *wire) : _wire(wire)
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
        }

        void InsertWireCommand::undo()
        {
            _wire->removeFromComponents();
            _wire->hide();
            _disconnected = true;
        }

        bool InsertWireCommand::connected() const
        {
            return _conn_success;
        }

        // MoveComponentsCommand
        MoveComponentsCommand::MoveComponentsCommand(std::vector<ComponentLabel *> &moved_components, std::vector<QPoint> init_positions, std::vector<QPoint> final_positions) : _moved_components(moved_components), _init_positions(init_positions), _final_positions(final_positions)
        {
        }

        void MoveComponentsCommand::redo()
        {
            for (size_t i = 0; i < _moved_components.size(); ++i)
            {
                _moved_components[i]->move(_final_positions[i].x(), _final_positions[i].y());
                if (_moved_components[i]->border() != nullptr)
                {
                    _moved_components[i]->border()->move(_final_positions[i].x(), _final_positions[i].y());
                }
                _moved_components[i]->moveWires();
            }
        }

        void MoveComponentsCommand::undo()
        {
            for (size_t i = 0; i < _moved_components.size(); ++i)
            {
                _moved_components[i]->move(_init_positions[i].x(), _init_positions[i].y());
                if (_moved_components[i]->border() != nullptr)
                {
                    _moved_components[i]->border()->move(_init_positions[i].x(), _init_positions[i].y());
                }
                _moved_components[i]->moveWires();
            }
        }
    }
}
