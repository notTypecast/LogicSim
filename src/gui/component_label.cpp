#include "gui/component_label.hpp"
#include "gui/undo_commands.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        ComponentLabel::ComponentLabel(COMPONENT comp_type, int resource_idx, QUndoStack *stack, QWidget *parent): QLabel{parent}, _undo_stack(stack)
        {
            _comp_type = comp_type;
            _input_wires = std::vector<Wire *>(resources::comp_io_rel_pos[comp_type].first.size(), nullptr);
            _output_wires = std::vector<std::vector<Wire *>>(resources::comp_io_rel_pos[comp_type].second.size(), std::vector<Wire *>());

            setResourceByIdx(resource_idx);

            _component_model = model::ctype_map.at(comp_type_to_ctype.at(comp_type))();

            switch (comp_type)
            {
            case CONSTANT:
            case SWITCH:
                _component_model->set_params(std::to_string(static_cast<int>(resource_idx)));
                break;
            default:
                break;
            }
        }

        ComponentLabel::~ComponentLabel()
        {
            for (const auto &wire: _input_wires)
            {
                if (wire != nullptr)
                {
                    wire->removeFromOppositeComponent(this);
                    delete wire;
                }
            }

            for (const auto &output: _output_wires)
            {
                for (const auto &wire: output)
                {
                    if (wire != nullptr)
                    {
                        wire->removeFromOppositeComponent(this);
                        delete wire;
                    }
                }
            }

            delete _border;

            delete _component_model;
        }

        void ComponentLabel::removeFromCircuit()
        {
            for (const auto &wire: _input_wires)
            {
                if (wire != nullptr)
                {
                    wire->removeFromOppositeComponent(this);
                    wire->hide();
                }
            }

            for (const auto &output: _output_wires)
            {
                for (const auto &wire: output)
                {
                    if (wire != nullptr)
                    {
                        wire->removeFromOppositeComponent(this);
                        wire->hide();
                    }
                }
            }

            if (_border != nullptr)
            {
                delete _border;
                _border = nullptr;
            }
        }

        void ComponentLabel::bringBackToCircuit()
        {
            for (const auto &wire: _input_wires)
            {
                if (wire != nullptr)
                {
                    wire->saveInOppositeComponent(this);
                    wire->show();
                }
            }

            for (const auto &output: _output_wires)
            {
                for (const auto &wire: output)
                {
                    if (wire != nullptr)
                    {
                        wire->saveInOppositeComponent(this);
                        wire->show();
                    }
                }
            }
        }

        COMPONENT ComponentLabel::comp_type() const
        {
            return _comp_type;
        }

        model::component::Component *ComponentLabel::component_model() const
        {
            return _component_model;
        }

        void ComponentLabel::setBorder(QLabel *border)
        {
            if (_border != nullptr)
            {
                delete _border;
            }
            _border = border;
        }

        QLabel *ComponentLabel::border() const
        {
            return _border;
        }

        void ComponentLabel::setResourceByIdx(int idx)
        {
            _resource_idx = idx;
            QPixmap &res = resources::comp_images.at(_comp_type)[_resource_idx];
            setPixmap(res);
            resize(res.width(), res.height());
        }

        int ComponentLabel::resourceIdx() const
        {
            return _resource_idx;
        }

        void ComponentLabel::setParams(QString param_string)
        {
            _component_model->set_params(param_string.toStdString());
        }

        bool ComponentLabel::saveWire(Wire *wire, bool is_input, int idx)
        {
            if (is_input)
            {
                if (_input_wires[idx] != nullptr)
                {
                    return false;
                }
                _input_wires[idx] = wire;

                std::pair<ComponentLabel *, int> output_info = wire->outputComponentInfo();
                dynamic_cast<model::component::NInputComponent *>(_component_model)->set_input(idx, *(output_info.first->component_model()), output_info.second);
            }
            else
            {
                _output_wires[idx].push_back(wire);
            }

            return true;
        }

        void ComponentLabel::removeWire(Wire *wire, bool is_input, int idx)
        {
            if (is_input)
            {
                _input_wires[idx] = nullptr;
                dynamic_cast<model::component::NInputComponent *>(_component_model)->remove_input(idx);
            }
            else
            {
                _output_wires[idx].erase(std::remove(_output_wires[idx].begin(), _output_wires[idx].end(), wire), _output_wires[idx].end());
            }
        }

        void ComponentLabel::moveWires()
        {
            for (const auto &wire: _input_wires)
            {
                if (wire == nullptr)
                {
                    continue;
                }
                wire->reposition();
            }

            for (const auto &output: _output_wires)
            {
                for (const auto &wire: output)
                {
                    wire->reposition();
                }
            }
        }

        void ComponentLabel::mouseMoveEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case INSERT:
                ev->ignore();
                break;
            case SELECT:
                ev->accept();
                emit moved(ev->x() - _press_x, ev->y() - _press_y);
                break;
            case WIRE:
                ev->accept();
                emit wireMoved(ev->x(), ev->y());
                break;
            default:
                break;
            }
        }

        void ComponentLabel::mousePressEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case INSERT:
                ev->ignore();
                break;
            case SELECT:
                ev->accept();
                _press_x = ev->x();
                _press_y = ev->y();
                emit selected(this, ev->modifiers() & Qt::ControlModifier);
                break;
            case WIRE:
                ev->accept();
                emit wireSource(this, ev->x(), ev->y());
                break;
            case SIMULATE:
                ev->accept();
                switch (_comp_type)
                {
                case SWITCH:
                    setResourceByIdx(!_resource_idx);
                    _component_model->set_params(std::to_string(_resource_idx));
                    break;
                case KEYPAD:
                {
                    int row = -1, col = -1;
                    int i = 0;
                    for (const auto &p : resources::keypad_rel_pos_range)
                    {
                        if (width()*p.first <= ev->x() && ev->x() <= width()*p.second)
                        {
                            col = i;
                        }
                        ++i;
                    }
                    if (col == -1)
                    {
                        break;
                    }
                    i = 0;
                    for (const auto &p : resources::keypad_rel_pos_range)
                    {
                        if (height()*p.first <= ev->y() && ev->y() <= height()*p.second)
                        {
                            row = i;
                        }
                        ++i;
                    }
                    if (row == -1)
                    {
                        break;
                    }
                    unsigned int key = 4*row + col;
                    dynamic_cast<model::input::Keypad *>(_component_model)->set_key(key);
                    setPixmap(resources::comp_images.at(_comp_type)[key + 1]);
                    break;
                }
                default:
                    break;
                }

            default:
                break;
            }
        }

        void ComponentLabel::mouseReleaseEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case SELECT:
                ev->accept();
                emit moveFinished();
                break;
            case INSERT:
                ev->ignore();
                break;
            case WIRE:
                ev->accept();
                emit wireReleased();
                break;
            case SIMULATE:
                ev->accept();
                switch (_comp_type)
                {
                case KEYPAD:
                    setPixmap(resources::comp_images.at(_comp_type)[0]);
                    break;
                default:
                    break;
                }

            default:
                break;
            }
        }

        void ComponentLabel::mouseDoubleClickEvent(QMouseEvent *ev)
        {
            switch (_current_tool)
            {
            case SELECT:
                if (resources::components_with_properties.find(_comp_type) == resources::components_with_properties.end())
                {
                    break;
                }
                _setupProperties();
                break;
            case SIMULATE:
                mousePressEvent(ev);
                break;
            default:
                break;
            }
        }

        void ComponentLabel::_setupProperties()
        {
            switch (_comp_type)
            {
            case CONSTANT:
                _properties_popup = new Properties("Constant Properties", parentWidget());
                _properties_popup->addExclusiveGroup("Type", {{"0", "0"}, {"1", "1"}}, _resource_idx);
                break;
            case SWITCH:
                _properties_popup = new Properties("Switch Properties", parentWidget());
                _properties_popup->addExclusiveGroup("State", {{"Off", "0"}, {"On", "1"}}, _resource_idx);
                break;
            case OSCILLATOR:
            {
                _properties_popup = new Properties("Oscillator Properties", parentWidget());

                utils::StringSplitter splitter(_component_model->param_string(), ',');
                QString low = QString::fromStdString(splitter.next());
                QString high = QString::fromStdString(splitter.next());
                high = QString::number(high.toInt() - low.toInt());
                QString phase;

                if (splitter.has_next())
                {
                    phase = QString::fromStdString(splitter.next());
                }
                else
                {
                    phase = "0";
                }

                _properties_popup->addValueGroup("Cycle", {{"Low", low}, {"High", high}, {"Phase", phase}},
                [](std::vector<QLineEdit *> params) {
                    bool ok1, ok2, ok3;
                    int low = params[0]->text().toInt(&ok1), high = params[1]->text().toInt(&ok2);
                    params[2]->text().toInt(&ok3);
                    if (!(ok1 && ok2 && ok3))
                    {
                        return QString();
                    }
                    return params[0]->text() + ',' + QString::number(low + high) + ',' + params[2]->text();
                });
                break;
            }
            default:
                break;
            }

            QObject::connect(_properties_popup, &Properties::optionIndex, this, [this](int idx, int /*group*/)
            {
                _property_command = new ChangeComponentPropertyCommand(this, _resource_idx, idx);
            });
            QObject::connect(_properties_popup, &Properties::optionValue, this, [this](QString params, int /*group*/)
            {
                if (_property_command == nullptr)
                {
                    _property_command = new ChangeComponentPropertyCommand(this);
                }
                QString prev_params = QString::fromStdString(_component_model->param_string());
                if (prev_params != params)
                {
                    _property_command->setParams(prev_params, params);
                    _undo_stack->push(_property_command);
                }
                else
                {
                    delete _property_command;
                }
                _property_command = nullptr;
                emit performPropertyUndoAction();
            });

            QPoint w_pos = parentWidget()->mapToGlobal(pos());
            _properties_popup->move(w_pos.x() + width(), w_pos.y());
            _properties_popup->show();
        }

        void ComponentLabel::changeMode(TOOL tool)
        {
            if (_current_tool == TOOL::SIMULATE && tool != TOOL::SIMULATE)
            {
                _component_model->set_params(_base_params);
                setResourceByIdx(_base_resource_idx);
            }

            _current_tool = tool;

            if (_current_tool == TOOL::SIMULATE)
            {
                _base_params = _component_model->param_string();
                _base_resource_idx = _resource_idx;
            }
        }

        void ComponentLabel::checkRangeQuery(int min_x, int min_y, int max_x, int max_y)
        {
            int center_x = x() + width()/2;
            int center_y = y() + height()/2;
            if (min_x <= center_x && min_y <= center_y && center_x <= max_x && center_y <= max_y)
            {
                emit selected_nocheck(this);
            }
        }

        void ComponentLabel::wireSnap(ComponentLabel *wire_source, int x, int y)
        {
            if (this == wire_source)
            {
                return;
            }
            int obj_x = this->x();
            int obj_y = this->y();
            if (!(obj_x <= x && obj_y <= y && x <= obj_x + width() && y <= obj_y + height()))
            {
                return;
            }

            int dx = x - obj_x;
            int dy = y - obj_y;

            int target_x, target_y;
            bool is_input;
            int io_idx;

            if (!Wire::calculateWireTargetPos(this, dx, dy, target_x, target_y, is_input, io_idx))
            {
                return;
            }

            emit wireSnapFound(this, target_x, target_y);
        }

        void ComponentLabel::evaluate()
        {
            try
            {
                switch (_comp_type)
                {
                case LED:
                    setPixmap(resources::comp_images.at(_comp_type)[_component_model->evaluate()]);
                    break;
                case _7SEG_5IN:
                {
                    std::vector<bool> evals(8);
                    for (int i = 0; i < 8; ++i)
                    {
                        evals[(7 + i) % 8] = _component_model->evaluate(i);
                    }

                    setPixmap(resources::comp_images.at(_comp_type)[resources::_7seg_5in_res_map.at(utils::get_int_from_bools(evals))]);
                    break;
                }
                case _7SEG_8IN:
                {
                    std::vector<bool> evals(8);
                    for (int i = 0; i < 8; ++i)
                    {
                        evals[(7 + i) % 8] = _component_model->evaluate(i);
                    }
                    setPixmap(resources::comp_images.at(_comp_type)[utils::get_int_from_bools(evals)]);
                    break;
                }
                default:
                    break;
                }
            }
            catch (model::component::null_input)
            {
                return;
            }
        }

        void ComponentLabel::resetResource()
        {
            setResourceByIdx(_base_resource_idx);
        }

        void ComponentLabel::writeComponent(std::ofstream &file)
        {
            file << _component_model->id() << ';' << _component_model->ctype() << ';' << _component_model->param_string() << ';' << std::to_string(x()) << ',' << std::to_string(y()) << ';';

            std::vector<std::pair<unsigned int, unsigned int>> input_ids = _component_model->input_ids();
            for (size_t i = 0; i < input_ids.size(); ++i)
            {
                if (input_ids[i].first == std::numeric_limits<unsigned int>::max() && input_ids[i].second == std::numeric_limits<unsigned int>::max())
                {
                    file << "NULL";
                }
                else
                {
                    file << input_ids[i].first << ":" << input_ids[i].second;
                }
                if (i < input_ids.size() - 1)
                {
                    file << ",";
                }
            }

            file << "\n";
        }
    }
}
