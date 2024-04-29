#include "gui/undo_commands.hpp"
#include <iostream>
#include <unistd.h>
namespace logicsim
{
    namespace gui
    {
        // InsertComponentCommand
        InsertComponentCommand::InsertComponentCommand(DesignArea *design_area, QMouseEvent *ev) : _design_area(design_area), _component(new ComponentLabel(_design_area->_insert_component, _design_area->_insert_resource_idx, _design_area->_undo_stack, _design_area))
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

        // DeleteWireCommand
        DeleteWireCommand::DeleteWireCommand(Wire *wire) : _wire(wire)
        {
        }

        void DeleteWireCommand::redo()
        {
            _wire->removeFromComponents();
            _wire->hide();
        }

        void DeleteWireCommand::undo()
        {
            _wire->saveInComponents();
            _wire->show();
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

        // ChangeComponentPropertyCommand
        ChangeComponentPropertyCommand::ChangeComponentPropertyCommand(ComponentLabel *component, int prev_res_idx, int res_idx) : _component(component), _prev_res_idx(prev_res_idx), _res_idx(res_idx)
        {
        }

        void ChangeComponentPropertyCommand::setParams(const QString &prev_params, const QString &params)
        {
            _prev_params = prev_params;
            _params = params;
        }

        void ChangeComponentPropertyCommand::redo()
        {
            if (_res_idx != -1)
            {
                _component->_resource_idx = _res_idx;
                QPixmap &res = resources::comp_images.at(_component->_comp_type)[_res_idx];
                _component->setPixmap(res);
                _component->resize(res.width(), res.height());
            }
            _component->_component_model->set_params(_params.toStdString());
        }

        void ChangeComponentPropertyCommand::undo()
        {
            if (_prev_res_idx != -1)
            {
                _component->_resource_idx = _prev_res_idx;
                QPixmap &res = resources::comp_images.at(_component->_comp_type)[_prev_res_idx];
                _component->setPixmap(res);
                _component->resize(res.width(), res.height());
            }

            _component->_component_model->set_params(_prev_params.toStdString());
        }

        // ChangeSimulationPropertiesCommand
        ChangeSimulationPropertiesCommand::ChangeSimulationPropertiesCommand(DesignArea *design_area, int prev_freq, int freq) : _design_area(design_area), _prev_freq(prev_freq), _freq(freq)
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
