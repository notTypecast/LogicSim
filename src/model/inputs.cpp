#include "model/inputs.hpp"

namespace logicsim
{
    namespace model
    {
        namespace input
        {
            // Constant
            Constant::Constant()
            {
            }

            Constant::Constant(bool value)
            {
                _value = value;
            }

            bool Constant::_evaluate(unsigned int)
            {
                return _value;
            }

            std::string Constant::ctype() const
            {
                return "CONSTANT";
            }

            std::string Constant::param_string() const
            {
                return std::to_string(_value);
            }

            void Constant::set_params(const std::string &param_string)
            {
                _value = std::stoi(param_string);
            }

            // Switch
            Switch::Switch()
            {
            }

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

            std::string Switch::ctype() const
            {
                return "SWITCH";
            }

            std::string Switch::param_string() const
            {
                return std::to_string(_value);
            }

            void Switch::set_params(const std::string &param_string)
            {
                _value = std::stoi(param_string);
            }

            // Oscillator
            Oscillator::Oscillator()
            {
            }

            Oscillator::Oscillator(unsigned int low_ticks, unsigned int high_ticks) : _low_ticks(low_ticks), _period(low_ticks + high_ticks)
            {
            }

            void Oscillator::reset()
            {
                _ticks = 0;
            }

            bool Oscillator::_evaluate(unsigned int)
            {
                _ticks %= _period;
                return _ticks >= _low_ticks;
            }

            std::string Oscillator::ctype() const
            {
                return "OSCILLATOR";
            }

            std::string Oscillator::param_string() const
            {
                return std::to_string(_low_ticks) + "," + std::to_string(_period);
            }

            void Oscillator::set_params(const std::string &param_string)
            {

                int period = std::stoi(param_string.substr(param_string.find(',') + 1));
                if (period <= 0)
                {
                    return;
                }

                _period = period;
                _low_ticks = std::stoi(param_string.substr(0, param_string.find(',')));
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
                unsigned int outputs = n_outputs();
                if (out >= outputs)
                {
                    out = 0;
                }
                return (_key >> (outputs - 1 - out)) & 1;
            }

            std::string Keypad::ctype() const
            {
                return "KEYPAD";
            }

            unsigned int Keypad::n_outputs() const
            {
                return 4;
            }
        }
    }
}
