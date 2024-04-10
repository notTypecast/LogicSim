#include "inputs.hpp"

namespace logicsim
{
    namespace input
    {
        Switch::Switch()
        {
            _value = false;
        }

        Switch::Switch(bool value)
        {
            _value = value;
        }

        bool Switch::evaluate() const
        {
            return _value;
        }

        void Switch::toggle()
        {
            _value = !_value;
        }
    }
}
