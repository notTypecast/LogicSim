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

            void Circuit::tick()
            {
                for (auto &target : _components)
                {
                    target->clear();
                }
                for (auto &target : _components)
                {
                    target->tick();
                }
                ++_total_ticks;
            }

            void Circuit::check() const
            {
                for (const auto &target : _components)
                {
                    target->check();
                }
            }

            void Circuit::reset()
            {
                for (auto &target : _components)
                {
                    target->reset();
                }
                _total_ticks = 0;
            }

            unsigned int Circuit::total_ticks() const
            {
                return _total_ticks;
            }

            bool Circuit::empty() const
            {
                return _components.empty();
            }
        }
    }
}
