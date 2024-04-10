#ifndef GATES_HPP
#define GATES_HPP

#include "component.hpp"

namespace logicsim
{
    namespace gate
    {
        DEFINE_2_INPUT_COMPONENT(AND)
        DEFINE_2_INPUT_COMPONENT(OR)
        DEFINE_2_INPUT_COMPONENT(XOR)
        DEFINE_2_INPUT_COMPONENT(NAND)
        DEFINE_2_INPUT_COMPONENT(NOR)
        DEFINE_2_INPUT_COMPONENT(XNOR)

        DEFINE_1_INPUT_COMPONENT(NOT)
    }
}

#endif // GATES_HPP
