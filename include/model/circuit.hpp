#ifndef LOGICSIM_MODEL_CIRCUIT_HPP
#define LOGICSIM_MODEL_CIRCUIT_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <stdexcept>

#include "model/component.hpp"
#include "model/mapped_data.hpp"

#include "utils.hpp"

namespace logicsim
{
    namespace model
    {
        namespace circuit
        {
            class Circuit
            {
            public:
                ~Circuit();

                void add_component(component::Component &component);
                void remove_component(component::Component &component);

                // TODO: Change
                component::Component &get_active_component(size_t idx) const;

                void tick();
                void check() const;
                void reset();

                unsigned int total_ticks() const;

                // Write circuit to file
                // Only writes components that have been added to the circuit
                // Components that are inputs to existing components must also be in the circuit
                bool write(const std::string &filename) const;

                // Read circuit from file
                void read(const std::string &filename);

            protected:
                unsigned int _total_ticks = 0;
                std::vector<component::Component *> _components;
                std::vector<component::Component *> _active_components;
                std::unordered_set<unsigned int> _component_ids;

                // Components created by this object, to be deleted in destructor
                std::vector<component::Component *> _created_components;
            };
        }
    }
}

#endif // CIRCUIT_HPP
