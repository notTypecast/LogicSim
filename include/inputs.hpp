#ifndef INPUTS_HPP
#define INPUTS_HPP

#include "component.hpp"

namespace logicsim
{
    namespace input
    {
        class Constant : public component::Component
        {
        public:
            Constant(bool value);
            Constant(const std::string &param_string);

            std::string ctype() const override;
            std::string param_string() const override;

        protected:
            bool _value;
            bool _evaluate(unsigned int = 0) override;
        };

        class Switch : public component::Component
        {
        public:
            Switch(bool value);
            Switch(const std::string &param_string);

            void toggle();

            std::string ctype() const override;
            std::string param_string() const override;

        protected:
            bool _value;
            bool _evaluate(unsigned int = 0) override;
        };

        class Oscillator : public component::TimeComponent
        {
        public:
            Oscillator(unsigned int low_ticks = 1, unsigned int high_ticks = 1);
            Oscillator(const std::string &param_string);

            std::string ctype() const override;
            std::string param_string() const override;

            void reset() override;

        protected:
            unsigned int _low_ticks;
            unsigned int _period;
            bool _evaluate(unsigned int = 0) override;
        };

        class Keypad : public component::Component
        {
        public:
            Keypad();

            void set_key(unsigned int key);

            std::string ctype() const override;

        protected:
            unsigned int _key;
            bool _evaluate(unsigned int out = 0) override;
        };
    }
}

#endif // INPUTS_HPP
