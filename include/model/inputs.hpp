#ifndef LOGICSIM_MODEL_INPUTS_HPP
#define LOGICSIM_MODEL_INPUTS_HPP

#include "model/component.hpp"

namespace logicsim
{
    namespace model
    {
        namespace input
        {
            class Constant : public component::Component
            {
            public:
                Constant();
                Constant(bool value);

                std::string ctype() const override;
                std::string param_string() const override;
                void set_params(const std::string &param_string) override;

            protected:
                bool _value = false;
                bool _evaluate(unsigned int = 0) override;
            };

            class Switch : public component::Component
            {
            public:
                Switch();
                Switch(bool value);

                void toggle();

                std::string ctype() const override;
                std::string param_string() const override;
                void set_params(const std::string &param_string) override;

            protected:
                bool _value = false;
                bool _evaluate(unsigned int = 0) override;
            };

            class Oscillator : public component::TimeComponent
            {
            public:
                Oscillator();
                Oscillator(unsigned int low_ticks, unsigned int high_ticks);

                std::string ctype() const override;
                std::string param_string() const override;
                void set_params(const std::string &param_string) override;

                void reset() override;

            protected:
                unsigned int _low_ticks = 10;
                unsigned int _period = 20;
                bool _evaluate(unsigned int = 0) override;
            };

            class Keypad : public component::Component
            {
            public:
                Keypad();

                void set_key(unsigned int key);

                unsigned int n_outputs() const override;

                std::string ctype() const override;

            protected:
                unsigned int _key;
                bool _evaluate(unsigned int out = 0) override;
            };
        }
    }
}

#endif // INPUTS_HPP
