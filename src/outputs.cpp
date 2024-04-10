#include "outputs.hpp"

namespace logicsim
{
    namespace output
    {
        bool BaseOutput::_evaluate()
        {
            return _input->evaluate();
        }
    }
}