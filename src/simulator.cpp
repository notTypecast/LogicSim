#include "simulator.hpp"

namespace logicsim
{
    namespace sim
    {
        void Simulator::tick()
        {
            _target->evaluate();
            ++_ticks;
        }
    }
}