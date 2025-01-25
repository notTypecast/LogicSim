#include "gui/clipboard.hpp"
#include "gui/undo_commands.hpp"

namespace logicsim
{
namespace gui
{
Clipboard::Clipboard(DesignArea *design_area) : _design_area(design_area) {}

void Clipboard::copy(std::vector<ComponentLabel *> components)
{
    _component_reprs = std::vector<ComponentRepr>(components.size());
    std::unordered_map<ComponentLabel *, ComponentRepr *> component_map;

    for (size_t i = 0; i < components.size(); ++i)
    {
        _component_reprs[i].comp_type = components[i]->comp_type();
        _component_reprs[i].res_idx   = components[i]->resourceIdx();
        _component_reprs[i].params    = components[i]->params();
        _component_reprs[i].x         = components[i]->x();
        _component_reprs[i].y         = components[i]->y();

        component_map[components[i]] = &(_component_reprs[i]);
    }

    for (size_t i = 0; i < components.size(); ++i)
    {
        const std::vector<Wire *> &input_wires = components[i]->inputWires();
        _component_reprs[i].inputs =
          std::vector<std::pair<ComponentRepr *, int>>(input_wires.size());
        for (size_t j = 0; j < input_wires.size(); ++j)
        {
            if (input_wires[j] != nullptr)
            {
                ComponentLabel *opposite =
                  input_wires[j]->oppositeComponent(components[i]);
                if (component_map.find(opposite) != component_map.end())
                {
                    _component_reprs[i].inputs[j] = {
                        component_map[opposite],
                        input_wires[j]->outputComponentInfo().second
                    };
                    continue;
                }
            }
            _component_reprs[i].inputs[j] = { nullptr, -1 };
        }
    }
}

void Clipboard::paste()
{
    std::vector<ComponentLabel *> components(_component_reprs.size());
    std::unordered_map<ComponentRepr *, ComponentLabel *> component_map;

    _design_area->_unselectAll();

    _design_area->_undo_stack->beginMacro("paste");

    for (size_t i = 0; i < _component_reprs.size(); ++i)
    {
        components[i] = new ComponentLabel(_component_reprs[i].comp_type,
                                           _component_reprs[i].res_idx,
                                           _design_area->getScale(),
                                           _design_area->_undo_stack,
                                           _design_area);
        components[i]->setParams(_component_reprs[i].params);
        components[i]->move(_component_reprs[i].x,
                            _component_reprs[i].y + PASTE_OFFSET_Y);

        InsertComponentCommand *insert_component =
          new InsertComponentCommand(_design_area, components[i]);
        _design_area->_undo_stack->push(insert_component);

        _design_area->addSelected_nocheck(components[i]);

        component_map[&(_component_reprs[i])] = components[i];
    }

    for (size_t i = 0; i < _component_reprs.size(); ++i)
    {
        for (size_t j = 0; j < _component_reprs[i].inputs.size(); ++j)
        {
            if (_component_reprs[i].inputs[j].first != nullptr)
            {
                Wire *wire = new Wire(_design_area->getScale(), _design_area);
                wire->setComponent1(components[i], true, j);
                wire->setComponent2(
                  component_map[_component_reprs[i].inputs[j].first],
                  false,
                  _component_reprs[i].inputs[j].second);
                wire->reposition();

                InsertWireCommand *insert_wire =
                  new InsertWireCommand(_design_area, wire);
                _design_area->_undo_stack->push(insert_wire);
            }
        }
    }

    _design_area->_undo_stack->endMacro();
    emit _design_area->newUndoActionPerformed(
      false,
      _design_area->_undo_stack->canUndo(),
      _design_area->_undo_stack->canRedo());
    emit _design_area->newSelection(true, true);
}

bool Clipboard::empty() const
{
    return _component_reprs.empty();
}
}
}
