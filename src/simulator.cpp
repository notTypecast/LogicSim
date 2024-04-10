#include "simulator.hpp"

namespace logicsim
{
    namespace sim
    {
        void Simulator::tick()
        {
            for (auto &target : _targets)
            {
                target->clear();
            }
            for (auto &target : _targets)
            {
                target->evaluate();
            }
            _targets[0]->evaluate();
            ++_ticks;
        }

        void Simulator::check_circuit() const
        {
            for (const auto &target : _targets)
            {
                target->check();
            }
        }
    }
}