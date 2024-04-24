#ifndef LOGICSIM_MODEL_GATES_HPP
#define LOGICSIM_MODEL_GATES_HPP

#include "model/component.hpp"

namespace logicsim
{
    namespace model
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
            DEFINE_1_INPUT_COMPONENT(BUFFER)
        }
    }
}

#endif // GATES_HPP
