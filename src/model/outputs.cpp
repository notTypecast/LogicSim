#include "model/outputs.hpp"

namespace logicsim
{
namespace model
{
namespace output
{
// Output
Output::Output(unsigned int n_inputs, unsigned int n_evals)
  : NInputComponent(n_inputs, 0, n_evals)
{
}

unsigned int Output::n_outputs() const
{
    return 0;
}

// BaseOutput
BaseOutput::BaseOutput() : Output(1, 1) {}

State BaseOutput::_evaluate(unsigned int)
{
    return _inputs[0]->evaluate(_inputs_out[0]);
}

std::string BaseOutput::ctype() const
{
    return "OUTPUT";
}

// _5in_7SegmentDisplay
_5in_7SegmentDisplay::_5in_7SegmentDisplay() : Output(5, 8) {}

void _5in_7SegmentDisplay::tick()
{
    _ticked = false;
    Component::tick();
}

State _5in_7SegmentDisplay::_evaluate(unsigned int out)
{
    if (!_ticked)
    {
        for (size_t i = 0; i < 5; ++i)
        {
            _input_cache[i] = _inputs[i]->evaluate(_inputs_out[i]);
        }
    }

    State(&x)[5] = _input_cache;
    switch (out)
    {
    case 1:
        return !((x[0] && x[1] && x[2]) || (x[0] && x[1] && !x[2] && !x[3]) ||
                 (!x[0] && !x[1] && x[2] && !x[3]));
    case 2:
        return !((x[1] && x[2] && x[3]) || (!x[0] && x[1] && !x[2] && !x[3]) ||
                 (!x[0] && !x[1] && !x[2] && x[3]) ||
                 (x[0] && !x[1] && x[2] && !x[3]));
    case 3:
        return !((!x[0] && x[3]) || (!x[0] && x[1] && !x[2] && !x[3]) ||
                 (x[0] && !x[1] && !x[2] && x[3]));
    case 4:
        return !((!x[0] && !x[1] && !x[2]) || (!x[0] && x[1] && x[2] && x[3]) ||
                 (x[0] && x[1] && !x[2] && !x[3]));
    case 5:
        return !((!x[0] && !x[1] && x[2]) || (!x[0] && x[2] && x[3]) ||
                 (!x[0] && !x[1] && !x[2] && x[3]) ||
                 (x[0] && x[1] && !x[2] && x[3]));
    case 6:
        return !((!x[0] && !x[1] && !x[2] && x[3]) ||
                 (!x[0] && x[1] && !x[2] && !x[3]) ||
                 (x[0] && x[1] && !x[2] && x[3]) ||
                 (x[0] && !x[1] && x[2] && x[3]));
    case 7:
        return !((x[1] && x[2] && !x[3]) || (x[0] && x[1] && x[2]) ||
                 (x[0] && x[2] && x[3]) || (x[0] && x[1] && !x[3]) ||
                 (!x[0] && x[1] && !x[2] && x[3]));
    case 0:
    default:
        return x[4];
    }
}

std::string _5in_7SegmentDisplay::ctype() const
{
    return "5IN_7SEGMENT";
}

// _8in_7SegmentDisplay
_8in_7SegmentDisplay::_8in_7SegmentDisplay() : Output(8, 8) {}

void _8in_7SegmentDisplay::tick()
{
    _ticked = false;
    Component::tick();
}

State _8in_7SegmentDisplay::_evaluate(unsigned int out)
{
    if (!_ticked)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            _input_cache[i] = _inputs[i]->evaluate(_inputs_out[i]);
        }
    }

    return out < 8 ? _input_cache[7 - out] : _input_cache[0];
}

std::string _8in_7SegmentDisplay::ctype() const
{
    return "8IN_7SEGMENT";
}
}
}
}
