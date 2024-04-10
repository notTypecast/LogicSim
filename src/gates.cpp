#include "gates.hpp"

namespace logicsim
{
    namespace gate
    {
        bool AND::_evaluate()
        {
            return _input1->evaluate() && _input2->evaluate();
        }

        bool OR::_evaluate()
        {
            return _input1->evaluate() || _input2->evaluate();
        }

        bool XOR::_evaluate()
        {
            return _input1->evaluate() ^ _input2->evaluate();
        }

        bool NAND::_evaluate()
        {
            return !(_input1->evaluate() && _input2->evaluate());
        }

        bool NOR::_evaluate()
        {
            return !(_input1->evaluate() || _input2->evaluate());
        }

        bool XNOR::_evaluate()
        {
            return !(_input1->evaluate() ^ _input2->evaluate());
        }

        bool NOT::_evaluate()
        {
            return !_input->evaluate();
        }
    }
}