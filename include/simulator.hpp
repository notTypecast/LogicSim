#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <iostream>
#include <memory>
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
            Simulator(const T &target)
            {
                assert((std::is_base_of<Component, T>::value) && "Target must be a Circuit object");
                _target = std::make_shared<T>(target);
            }

            void tick();

        protected:
            std::shared_ptr<Component> _target;
            int _ticks = 0;
        };
    }
}

#endif // SIMULATOR_HPP
