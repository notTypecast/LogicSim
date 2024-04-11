#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <vector>
#include <cassert>
#include "component.hpp"

namespace logicsim
{
    namespace sim
    {

        struct params
        {
            double dt = 0.01;
        };

        class Simulator
        {
        public:
            template <typename T>
            Simulator(T &target)
            {
                assert((std::is_base_of<component::Component, T>::value) && "Target must be a Circuit object");
                _targets.push_back(&target);
            }

            template <typename T>
            Simulator(const std::vector<T> &targets)
            {
                for (const auto &target : targets)
                {
                    assert((std::is_base_of<component::Component, T>::value) && "Targets must be Circuit objects");
                    _targets.push_back(&target);
                }
            }

            void tick();

            void check_circuit() const;

        protected:
            std::vector<component::Component *> _targets;
            unsigned long _ticks = 0;
        };
    }
}

#endif // SIMULATOR_HPP
