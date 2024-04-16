#include "model/gates.hpp"

namespace logicsim
{
    namespace model
    {
        namespace gate
        {
            bool AND::_evaluate(unsigned int)
            {
                return _inputs[0]->evaluate(_inputs_out[0]) && _inputs[1]->evaluate(_inputs_out[1]);
            }

            std::string AND::ctype() const
            {
                return "AND";
            }

            bool OR::_evaluate(unsigned int)
            {
                return _inputs[0]->evaluate(_inputs_out[0]) || _inputs[1]->evaluate(_inputs_out[1]);
            }

            std::string OR::ctype() const
            {
                return "OR";
            }

            bool XOR::_evaluate(unsigned int)
            {
                return _inputs[0]->evaluate(_inputs_out[0]) ^ _inputs[1]->evaluate(_inputs_out[1]);
            }

            std::string XOR::ctype() const
            {
                return "XOR";
            }

            bool NAND::_evaluate(unsigned int)
            {
                return !(_inputs[0]->evaluate(_inputs_out[0]) && _inputs[1]->evaluate(_inputs_out[1]));
            }

            std::string NAND::ctype() const
            {
                return "NAND";
            }

            bool NOR::_evaluate(unsigned int)
            {
                return !(_inputs[0]->evaluate(_inputs_out[0]) || _inputs[1]->evaluate(_inputs_out[1]));
            }

            std::string NOR::ctype() const
            {
                return "NOR";
            }

            bool XNOR::_evaluate(unsigned int)
            {
                return !(_inputs[0]->evaluate(_inputs_out[0]) ^ _inputs[1]->evaluate(_inputs_out[1]));
            }

            std::string XNOR::ctype() const
            {
                return "XNOR";
            }

            bool NOT::_evaluate(unsigned int)
            {
                return !_inputs[0]->evaluate(_inputs_out[0]);
            }

            std::string NOT::ctype() const
            {
                return "NOT";
            }
        }
    }
}
