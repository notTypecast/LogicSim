#ifndef LOGICSIM_MODEL_GATES_HPP
#define LOGICSIM_MODEL_GATES_HPP

#include "model/component.hpp"

namespace logicsim
{
    namespace model
    {
        namespace gate
        {
            DEFINE_2_INPUT_COMPONENT(AND, 1)
            DEFINE_2_INPUT_COMPONENT(OR, 1)
            DEFINE_2_INPUT_COMPONENT(XOR, 1)
            DEFINE_2_INPUT_COMPONENT(NAND, 1)
            DEFINE_2_INPUT_COMPONENT(NOR, 1)
            DEFINE_2_INPUT_COMPONENT(XNOR, 1)

            DEFINE_1_INPUT_COMPONENT(NOT, 1)
            DEFINE_1_INPUT_COMPONENT(BUFFER, 1)
            DEFINE_1_INPUT_COMPONENT(CONNECTOR, 0)
        }
    }
}

#endif // LOGICSIM_MODEL_GATES_HPP
