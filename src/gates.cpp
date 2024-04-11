#include "gates.hpp"

namespace logicsim
{
    namespace gate
    {
        bool AND::_evaluate(unsigned int)
        {
            return _inputs[0]->evaluate(_inputs_out[0]) && _inputs[1]->evaluate(_inputs_out[1]);
        }

        bool OR::_evaluate(unsigned int)
        {
            return _inputs[0]->evaluate(_inputs_out[0]) || _inputs[1]->evaluate(_inputs_out[1]);
        }

        bool XOR::_evaluate(unsigned int)
        {
            return _inputs[0]->evaluate(_inputs_out[0]) ^ _inputs[1]->evaluate(_inputs_out[1]);
        }

        bool NAND::_evaluate(unsigned int)
        {
            return !(_inputs[0]->evaluate(_inputs_out[0]) && _inputs[1]->evaluate(_inputs_out[1]));
        }

        bool NOR::_evaluate(unsigned int)
        {
            return !(_inputs[0]->evaluate(_inputs_out[0]) || _inputs[1]->evaluate(_inputs_out[1]));
        }

        bool XNOR::_evaluate(unsigned int)
        {
            return !(_inputs[0]->evaluate(_inputs_out[0]) ^ _inputs[1]->evaluate(_inputs_out[1]));
        }

        bool NOT::_evaluate(unsigned int)
        {
            return !_inputs[0]->evaluate(_inputs_out[0]);
        }
    }
}