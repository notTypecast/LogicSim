#include "inputs.hpp"

namespace logicsim
{
    namespace input
    {
        Switch::Switch(bool value)
        {
            _value = value;
        }

        bool Switch::_evaluate()
        {
            return _value;
        }

        void Switch::toggle()
        {
            _value = !_value;
        }

        Oscillator::Oscillator(unsigned int low_ticks, unsigned int high_ticks) : _low_ticks(low_ticks), _period(low_ticks + high_ticks)
        {
        }

        bool Oscillator::_evaluate()
        {
            _ticks %= _period;
            return _ticks >= _low_ticks;
        }
    }
}
