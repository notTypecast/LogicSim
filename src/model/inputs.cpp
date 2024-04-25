#include "model/inputs.hpp"

namespace logicsim
{
    namespace model
    {
        namespace input
        {
            // Input
            Input::Input(unsigned int n_evals) : Component(0, n_evals)
            {

            }

            // Constant
            Constant::Constant() : Input(1)
            {
            }

            Constant::Constant(bool value) : Input(1)
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
            Switch::Switch() : Input(1)
            {
            }

            Switch::Switch(bool value) : Input(1)
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
            Oscillator::Oscillator() : TimeComponent(0, 1)
            {
            }

            Oscillator::Oscillator(unsigned int low_ticks, unsigned int high_ticks, unsigned int phase) : TimeComponent(0, 1), _low_ticks(low_ticks), _period(low_ticks + high_ticks), _phase(phase)
            {
                _ticks = phase - 1;
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
                return std::to_string(_low_ticks) + ',' + std::to_string(_period) + ',' + std::to_string(_phase);
            }

            void Oscillator::set_params(const std::string &param_string)
            {
                utils::StringSplitter splitter(param_string, ',');
                int low_ticks = std::stoi(splitter.next());
                int period = std::stoi(splitter.next());
                if (period <= 0)
                {
                    return;
                }
                _low_ticks = low_ticks;
                _period = period;

                if (splitter.has_next())
                {
                    _phase = std::stoi(splitter.next());
                    _ticks = _phase - 1;
                }
            }

            // Keypad
            Keypad::Keypad() : Input(4), _key(0)
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
