#include "model/control.hpp"

namespace logicsim
{
namespace model
{
namespace control
{
// Multiplexer
Multiplexer::Multiplexer(unsigned int bits)
  : NInputComponent(bits + std::pow(2, bits) + 1, 5, 1)
  , _bits(bits)
  , _S(std::vector<Component **>(bits))
  , _S_out(std::vector<unsigned int *>(bits))
  , _D(std::vector<Component **>(n_inputs() - bits - 1))
  , _D_out(std::vector<unsigned int *>(n_inputs() - bits - 1))
{
    _E     = &_inputs[0];
    _E_out = &_inputs_out[0];

    const unsigned int ds = _D.size();
    for (unsigned int i = 0; i < ds; ++i)
    {
        _D[i]     = &_inputs[i + 1];
        _D_out[i] = &_inputs_out[i + 1];
    }

    for (unsigned int i = 0; i < bits; ++i)
    {
        _S[i]     = &_inputs[ds + i + 1];
        _S_out[i] = &_inputs_out[ds + i + 1];
    }
}

State Multiplexer::_evaluate(unsigned int)
{
    switch ((*_E)->evaluate(*_E_out))
    {
    case State::ONE:
        return State::ZERO;
    case State::HiZ:
        return State::HiZ;
    default:
        break;
    }

    unsigned int idx = 0;
    for (unsigned int i = 0; i < _bits; ++i)
    {
        State s = (*_S[i])->evaluate(*_S_out[i]);
        if (s == State::HiZ)
        {
            return State::HiZ;
        }
        idx += s * std::pow(2, _bits - 1 - i);
    }

    return (*_D[idx])->evaluate(*_D_out[idx]);
}

std::string Multiplexer::ctype() const
{
    return "MUX-" + std::to_string(_bits);
}

// Decoder
Decoder::Decoder(unsigned int bits)
  : NInputComponent(bits + 1, 5, std::pow(2, bits))
  , _bits(bits)
  , _outputs(std::pow(2, _bits))
  , _B(std::vector<Component **>(_bits))
  , _B_out(std::vector<unsigned int *>(_bits))
{
    _E     = &_inputs[_inputs.size() - 1];
    _E_out = &_inputs_out[_inputs.size() - 1];

    for (unsigned int i = 0; i < bits; ++i)
    {
        _B[i]     = &_inputs[i];
        _B_out[i] = &_inputs_out[i];
    }
}

State Decoder::_evaluate(unsigned int out)
{
    switch ((*_E)->evaluate(*_E_out))
    {
    case State::ONE:
        return State::ZERO;
    case State::HiZ:
        return State::HiZ;
    default:
        break;
    }

    unsigned int idx = 0;
    for (unsigned int i = 0; i < _bits; ++i)
    {
        State s = (*_B[i])->evaluate(*_B_out[i]);
        if (s == State::HiZ)
        {
            return State::HiZ;
        }
        idx += s * std::pow(2, i);
    }

    return static_cast<State>(out == idx);
}

unsigned int Decoder::n_outputs() const
{
    return _outputs;
}

std::string Decoder::ctype() const
{
    return "DEC-" + std::to_string(_bits);
}
}
}
}
