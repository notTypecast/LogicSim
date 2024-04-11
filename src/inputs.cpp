#include "inputs.hpp"

namespace logicsim
{
    namespace input
    {
        // Switch
        Switch::Switch(bool value)
        {
            _value = value;
        }

        bool Switch::_evaluate(unsigned int)
        {
            return _value;
        }

        void Switch::toggle()
        {
            _value = !_value;
        }

        // Oscillator
        Oscillator::Oscillator(unsigned int low_ticks, unsigned int high_ticks) : _low_ticks(low_ticks), _period(low_ticks + high_ticks)
        {
        }

        bool Oscillator::_evaluate(unsigned int)
        {
            _ticks %= _period;
            return _ticks >= _low_ticks;
        }
    }
}
