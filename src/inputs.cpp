#include "inputs.hpp"

namespace logicsim
{
    namespace input
    {
        // Constant
        Constant::Constant(bool value)
        {
            _value = value;
        }

        bool Constant::_evaluate(unsigned int)
        {
            return _value;
        }

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

        // Keypad
        Keypad::Keypad() : _key(0)
        {
        }

        void Keypad::set_key(unsigned int key)
        {
            _key = key;
        }

        bool Keypad::_evaluate(unsigned int out)
        {
            if (out >= 4)
            {
                out = 0;
            }
            return (_key >> out) & 1;
        }
    }
}
