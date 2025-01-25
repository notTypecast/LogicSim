#include "model/gates.hpp"

namespace logicsim
{
namespace model
{
namespace gate
{
State AND::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]) &&
           _inputs[1]->evaluate(_inputs_out[1]);
}

std::string AND::ctype() const
{
    return "AND";
}

State OR::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]) ||
           _inputs[1]->evaluate(_inputs_out[1]);
}

std::string OR::ctype() const
{
    return "OR";
}

State XOR::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]) ^
           _inputs[1]->evaluate(_inputs_out[1]);
}

std::string XOR::ctype() const
{
    return "XOR";
}

State NAND::_evaluate(unsigned int)
{
    return !(_inputs[0]->evaluate(_inputs_out[0]) &&
             _inputs[1]->evaluate(_inputs_out[1]));
}

std::string NAND::ctype() const
{
    return "NAND";
}

State NOR::_evaluate(unsigned int)
{
    return !(_inputs[0]->evaluate(_inputs_out[0]) ||
             _inputs[1]->evaluate(_inputs_out[1]));
}

std::string NOR::ctype() const
{
    return "NOR";
}

State XNOR::_evaluate(unsigned int)
{
    return !(_inputs[0]->evaluate(_inputs_out[0]) ^
             _inputs[1]->evaluate(_inputs_out[1]));
}

std::string XNOR::ctype() const
{
    return "XNOR";
}

State NOT::_evaluate(unsigned int)
{
    return !_inputs[0]->evaluate(_inputs_out[0]);
}

std::string NOT::ctype() const
{
    return "NOT";
}

State BUFFER::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]);
}

std::string BUFFER::ctype() const
{
    return "BUFFER";
}

State CONNECTOR::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]);
}

std::string CONNECTOR::ctype() const
{
    return "CONNECTOR";
}
}
}
}
