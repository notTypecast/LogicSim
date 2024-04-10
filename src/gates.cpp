#include "gates.hpp"

namespace logicsim
{
    namespace gate
    {
        bool AND::evaluate() const
        {
            _check_inputs();
            return _input1->evaluate() && _input2->evaluate();
        }

        bool OR::evaluate() const
        {
            _check_inputs();
            return _input1->evaluate() || _input2->evaluate();
        }

        bool XOR::evaluate() const
        {
            _check_inputs();
            return _input1->evaluate() ^ _input2->evaluate();
        }

        bool NAND::evaluate() const
        {
            _check_inputs();
            return !(_input1->evaluate() && _input2->evaluate());
        }

        bool NOR::evaluate() const
        {
            _check_inputs();
            return !(_input1->evaluate() || _input2->evaluate());
        }

        bool XNOR::evaluate() const
        {
            _check_inputs();
            return !(_input1->evaluate() ^ _input2->evaluate());
        }

        bool NOT::evaluate() const
        {
            _check_input();
            return !_input->evaluate();
        }
    }
}