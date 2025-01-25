#ifndef LOGICSIM_MODEL_CIRCUIT_HPP
#define LOGICSIM_MODEL_CIRCUIT_HPP

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    void tick();
    void check() const;
    void reset();

    unsigned int total_ticks() const;

    bool empty() const;

  protected:
    unsigned int                        _total_ticks = 0;
    std::vector<component::Component *> _components;
    std::unordered_set<unsigned int>    _component_ids;

    // Components created by this object, to be deleted in destructor
    std::vector<component::Component *> _created_components;
};
}
}
}

#endif // LOGICSIM_MODEL_CIRCUIT_HPP
