#include "model/circuit.hpp"

namespace logicsim
{
    namespace model
    {
        namespace circuit
        {
            Circuit::~Circuit()
            {
                for (auto &component : _created_components)
                {
                    delete component;
                }
            }

            void Circuit::add_component(component::Component &component)
            {
                if (_component_ids.find(component.id()) != _component_ids.end())
                {
                    throw std::invalid_argument("Component already added");
                }

                if (ACTIVE_COMPONENTS.find(component.ctype()) != ACTIVE_COMPONENTS.end())
                {
                    _active_components.push_back(&component);
                }
                else
                {
                    _components.push_back(&component);
                }

                _component_ids.insert(component.id());
            }

            void Circuit::remove_component(component::Component &component)
            {
                if (_component_ids.find(component.id()) == _component_ids.end())
                {
                    throw std::invalid_argument("Component not found");
                }

                if (ACTIVE_COMPONENTS.find(component.ctype()) != ACTIVE_COMPONENTS.end())
                {
                    _active_components.erase(std::remove(_active_components.begin(), _active_components.end(), &component), _active_components.end());
                }
                else
                {
                    _components.erase(std::remove(_components.begin(), _components.end(), &component), _components.end());
                }

                _component_ids.erase(component.id());
            }

            component::Component &Circuit::get_active_component(size_t idx) const
            {
                if (idx >= _active_components.size())
                {
                    throw std::out_of_range("Index out of range");
                }

                return *_active_components[idx];
            }

            void Circuit::tick()
            {
                for (auto &target : _active_components)
                {
                    target->clear();
                }
                for (auto &target : _active_components)
                {
                    for (size_t i = 0; i < target->n_evals(); ++i)
                    {
                        target->evaluate(i);
                    }
                }
            }

            void Circuit::check() const
            {
                for (auto &target : _active_components)
                {
                    target->check();
                }
            }

            bool Circuit::write(const std::string &filename) const
            {
                std::ofstream file("saves/" + filename + ".lsc");

                for (const auto &v : {&_components, &_active_components})
                {
                    for (auto &component : *v)
                    {
                        file << component->id() << ";" << component->ctype() << ";" << component->param_string() << ";";

                        std::vector<std::pair<unsigned int, unsigned int>> input_ids = component->input_ids();
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

                file.close();

                return !file.fail();
            }

            void Circuit::read(const std::string &filename)
            {
                std::unordered_map<std::string, component::Component *> components;
                std::vector<std::pair<std::string, std::string>> component_inputs;
                std::ifstream file(filename);

                if (file.fail())
                {
                    throw std::invalid_argument("File not found");
                }

                std::string line;
                logicsim::utils::StringSplitter splitter;

                for (size_t i = 1; getline(file, line); ++i)
                {
                    splitter.reset(line, ';');

                    std::string id_str = splitter.next();
                    if (id_str.empty())
                    {
                        throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " empty");
                    }

                    std::string ctype;
                    if (!splitter.has_next() || (ctype = splitter.next()).empty())
                    {
                        throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing component type");
                    }

                    component::Component *component = ctype_map.at(ctype)();
                    components[id_str] = component;

                    std::string params;
                    if (!splitter.has_next())
                    {
                        throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing parameters");
                    }
                    params = splitter.next();

                    component->set_params(params);

                    if (!splitter.has_next())
                    {
                        throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " missing inputs");
                    }

                    component_inputs.emplace_back(id_str, splitter.next());

                    if (splitter.has_next())
                    {
                        throw std::invalid_argument("Invalid file format: line " + std::to_string(i) + " too many fields");
                    }
                }

                file.close();

                logicsim::utils::StringSplitter splitter2;

                for (const auto &input : component_inputs)
                {
                    if (input.second.empty())
                    {
                        if (components[input.first]->n_inputs() > 0)
                        {
                            throw std::invalid_argument("Invalid file format: missing inputs for component " + input.first);
                        }
                        continue;
                    }

                    splitter.reset(input.second, ',');

                    size_t i;
                    for (i = 0; splitter.has_next(); ++i)
                    {
                        if (i >= components[input.first]->n_inputs())
                        {
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
                            throw std::invalid_argument("Invalid file format: invalid input for component " + input.first);
                        }
                        unsigned int output_idx;
                        try
                        {
                            output_idx = std::stoi(splitter2.next());
                            if (output_idx >= components[input_id_str]->n_outputs())
                            {
                                throw std::invalid_argument("");
                            }
                        }
                        catch (const std::invalid_argument &e)
                        {
                            throw std::invalid_argument("Invalid file format: invalid output index for component " + input.first);
                        }
                        if (splitter2.has_next())
                        {
                            throw std::invalid_argument("Invalid file format: too many fields for input of " + input.first);
                        }
                        component::NInputComponent *n_input_component = dynamic_cast<component::NInputComponent *>(components[input.first]);
                        n_input_component->set_input(i, *components[input_id_str], output_idx);
                    }

                    if (i < components[input.first]->n_inputs())
                    {
                        throw std::invalid_argument("Invalid file format: missing inputs for component " + input.first);
                    }
                }

                for (const auto &component : components)
                {
                    add_component(*component.second);
                }
            }
        }
    }
}
