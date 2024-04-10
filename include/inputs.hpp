#ifndef INPUTS_HPP
#define INPUTS_HPP

#include "component.hpp"

namespace logicsim
{
    namespace input
    {
        class Switch : public Component
        {
        public:
            Switch(bool value);

            void toggle();

        protected:
            bool _value;
            bool _evaluate() override;
        };

        class Oscillator : public TimeComponent
        {
        public:
            Oscillator(unsigned int low_ticks = 1, unsigned int high_ticks = 1);

        protected:
            unsigned int _low_ticks;
            unsigned int _period;
            bool _evaluate() override;
        };
    }
}

#endif // INPUTS_HPP
