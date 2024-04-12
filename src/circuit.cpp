#include "circuit.hpp"
#include <iostream>
namespace logicsim
{
    namespace circuit
    {
        void Circuit::add_component(component::Component &component)
        {
            if (_component_ids.find(component.id()) != _component_ids.end())
            {
                throw std::invalid_argument("Component already added");
            }
            _components.push_back(&component);
            _component_ids.insert(component.id());
        }

        void Circuit::remove_component(component::Component &component)
        {
            if (_component_ids.find(component.id()) == _component_ids.end())
            {
                throw std::invalid_argument("Component not found");
            }
            _components.erase(std::remove(_components.begin(), _components.end(), &component), _components.end());
            _component_ids.erase(component.id());
        }

        void Circuit::add_active_component(component::Component &component)
        {
            if (_component_ids.find(component.id()) != _component_ids.end())
            {
                throw std::invalid_argument("Component already added");
            }
            _active_components.push_back(&component);
            _component_ids.insert(component.id());
        }

        void Circuit::remove_active_component(component::Component &component)
        {
            if (_component_ids.find(component.id()) == _component_ids.end())
            {
                throw std::invalid_argument("Component not found");
            }
            _active_components.erase(std::remove(_active_components.begin(), _active_components.end(), &component), _active_components.end());
            _component_ids.erase(component.id());
        }

        void Circuit::tick()
        {
            for (auto &target : _active_components)
            {
                target->clear();
            }
            std::cout << "clear" << std::endl;
            for (auto &target : _active_components)
            {
                target->evaluate();
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
    }
}