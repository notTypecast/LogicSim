#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <stdexcept>
#include "component.hpp"

namespace logicsim
{
    namespace circuit
    {
        class Circuit
        {
        public:
            // Components: components only evaluated recursively when needed by active components
            void add_component(component::Component &component);
            void remove_component(component::Component &component);

            // Active components: components to explicitly evaluate/check
            void add_active_component(component::Component &component);
            void remove_active_component(component::Component &component);

            void tick();
            void check() const;

            // Write circuit to file
            // Only writes components that have been added to the circuit
            // Components that are inputs to existing components must also be in the circuit
            bool write(const std::string &filename) const;

        private:
            std::vector<component::Component *> _components;
            std::vector<component::Component *> _active_components;
            std::unordered_set<unsigned int> _component_ids;
        };
    }
}

#endif // CIRCUIT_HPP
