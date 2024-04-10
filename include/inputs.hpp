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
            Switch();
            Switch(bool value);

            bool evaluate() const override;

            void toggle();

        private:
            bool _value;
        };
    }
}

#endif // INPUTS_HPP
